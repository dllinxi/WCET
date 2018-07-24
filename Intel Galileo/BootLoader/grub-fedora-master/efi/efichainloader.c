/* efichainloader.c - boot another boot loader */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2004,2006  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <grub/misc.h>
#include <grub/types.h>
#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/efi/misc.h>

#include <shared.h>

#define grub_file_size()    filemax

static grub_efi_physical_address_t address;
static grub_efi_uintn_t pages;
static grub_efi_device_path_t *file_path;
static grub_efi_handle_t image_handle;

static int
grub_chainloader_unload (void)
{
  grub_efi_boot_services_t *b;

  b = grub_efi_system_table->boot_services;
  Call_Service_1 (b->unload_image, image_handle);
  Call_Service_2 (b->free_pages, address, pages);
  grub_free (file_path);

  return 0;
}

static int
grub_chainloader_boot (void)
{
  grub_efi_boot_services_t *b;
  grub_efi_status_t status;
  grub_efi_uintn_t exit_data_size = 0;
  grub_efi_char16_t *exit_data = NULL;

  b = grub_efi_system_table->boot_services;
  status = Call_Service_3 (b->start_image, image_handle,
			   &exit_data_size, &exit_data);
  if (status != GRUB_EFI_SUCCESS)
    {
      if (exit_data)
	{
	  char *buf;

	  buf = grub_malloc (exit_data_size * 4 + 1);
	  if (buf)
	    {
	      *grub_utf16_to_utf8 ((grub_uint8_t *) buf,
				   exit_data, exit_data_size) = 0;

	      grub_printf (buf);
	      grub_free (buf);
	    }
	  else
	    grub_printf ("unknown error\n");
	}
      errnum = ERR_UNRECOGNIZED;
    }

  if (exit_data)
    Call_Service_1 (b->free_pool, exit_data);

  grub_chainloader_unload ();

  return 0;
}

static void
copy_file_path (grub_efi_file_path_device_path_t *fp,
		const char *str, grub_efi_uint16_t len)
{
  grub_efi_char16_t *p;
  grub_efi_uint16_t size;

  fp->header.type = GRUB_EFI_MEDIA_DEVICE_PATH_TYPE;
  fp->header.subtype = GRUB_EFI_FILE_PATH_DEVICE_PATH_SUBTYPE;
  size = len * sizeof (grub_efi_char16_t) + sizeof (*fp);
  fp->header.length[0] = (grub_efi_uint8_t) (size & 0xff);
  fp->header.length[1] = (grub_efi_uint8_t) (size >> 8);
  for (p = fp->path_name; len > 0; len--, p++, str++)
    {
      *p = (grub_efi_char16_t) (*str == '/' ? '\\' : *str);
    }
}

static grub_efi_device_path_t *
make_file_path (const char *filename)
{
  char *dir_start;
  char *dir_end;
  grub_size_t size;
  grub_efi_device_path_t *d;

  dir_start = grub_strchr (filename, ')');
  if (! dir_start)
    dir_start = (char *) filename;
  else
    dir_start++;

  dir_end = grub_strrchr (dir_start, '/');
  if (! dir_end)
    {
      errnum = ERR_BAD_FILENAME;
      grub_printf ("invalid EFI file path");
      return 0;
    }

  size = 0;
  file_path = grub_malloc ((grub_strlen (dir_start) + 1)
			   * sizeof (grub_efi_char16_t)
			   + sizeof (grub_efi_file_path_device_path_t) * 2);
  if (! file_path)
    return 0;

  /* Fill the file path for the directory.  */
  d = (grub_efi_device_path_t *) file_path;

  if (dir_end - dir_start)
    {
      copy_file_path ((grub_efi_file_path_device_path_t *) d,
		      dir_start, dir_end - dir_start);
      d = GRUB_EFI_NEXT_DEVICE_PATH (d);
      /* Fill the file path for the file.  */
      copy_file_path ((grub_efi_file_path_device_path_t *) d,
		      dir_end + 1, grub_strlen (dir_end + 1));
    }
  else
    {
      /* Fill the file path for the file.  */
      copy_file_path ((grub_efi_file_path_device_path_t *) d,
		      dir_end, grub_strlen (dir_end));
    }

  /* Fill the end of device path nodes.  */
  d = GRUB_EFI_NEXT_DEVICE_PATH (d);
  d->type = GRUB_EFI_END_DEVICE_PATH_TYPE;
  d->subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE;
  d->length[0] = sizeof (*d);
  d->length[1] = 0;

  return file_path;
}

int
grub_chainloader (char *filename)
{
  grub_ssize_t size;
  grub_efi_status_t status;
  grub_efi_boot_services_t *b;
  grub_efi_handle_t dev_handle = 0;
  grub_efi_loaded_image_t *loaded_image;

  /* Initialize some global variables.  */
  address = 0xffffffff;
  image_handle = 0;

  b = grub_efi_system_table->boot_services;

  if (! grub_open (filename))
    goto fail1;

  dev_handle = grub_efidisk_get_current_bdev_handle ();
  if (! dev_handle)
    {
      errnum = ERR_UNRECOGNIZED;
      grub_printf ("not a valid root device\n");
      goto fail;
    }

  file_path = make_file_path (filename);
  if (! file_path)
    goto fail;

  grub_printf ("file path: ");
  grub_efi_print_device_path (file_path);

  size = grub_file_size ();
  pages = (((grub_efi_uintn_t) size + ((1 << 12) - 1)) >> 12);

  status = Call_Service_4 (b->allocate_pages, GRUB_EFI_ALLOCATE_ANY_PAGES,
			   GRUB_EFI_LOADER_CODE, pages, &address);
  if (status != GRUB_EFI_SUCCESS)
    {
      errnum = ERR_UNRECOGNIZED;
      grub_printf ("cannot allocate %u pages", pages);
      goto fail;
    }

  if (grub_read ((void *) ((grub_addr_t) address), size) != size)
    {
      if (errnum == ERR_NONE)
	{
	  errnum = ERR_UNRECOGNIZED;
	  grub_printf ("too small");
	}
      goto fail;
    }

  status = Call_Service_6 (b->load_image, 0, grub_efi_image_handle, file_path,
			   (void *) ((grub_addr_t) address), size,
			   &image_handle);
  if (status != GRUB_EFI_SUCCESS)
    {
      errnum = ERR_UNRECOGNIZED;
      if (status == GRUB_EFI_OUT_OF_RESOURCES)
	grub_printf ("out of resources");
      else
	grub_printf ("cannot load image");

      goto fail;
    }

  /* LoadImage does not set a device handler when the image is
     loaded from memory, so it is necessary to set it explicitly here.
     This is a mess.  */
  loaded_image = grub_efi_get_loaded_image (image_handle);
  if (! loaded_image)
    {
      errnum = ERR_UNRECOGNIZED;
      grub_printf ("no loaded image available");
      goto fail;
    }
  loaded_image->device_handle = dev_handle;

  grub_close ();
  return KERNEL_TYPE_CHAINLOADER;

 fail:
  grub_close ();
 fail1:
  if (address)
    b->free_pages (address, pages);

  return KERNEL_TYPE_NONE;
}

/* calls for direct boot-loader chaining */
void
chain_stage1 (unsigned long segment, unsigned long offset,
	      unsigned long part_table_addr)
{
  grub_chainloader_boot ();
  for (;;);
}
