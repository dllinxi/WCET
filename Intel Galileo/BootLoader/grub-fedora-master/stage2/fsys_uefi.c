/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2001   Free Software Foundation, Inc.
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

#include "shared.h"
#include "filesys.h"

#ifdef PLATFORM_EFI
#undef GRUB_UTIL

#include <grub/types.h>
#include <grub/misc.h>
#include <grub/efi/efi.h>
#include <grub/efi/misc.h>
#include <grub/efi/api.h>

static grub_efi_simple_file_system_t *file_system;
static grub_efi_guid_t fsinfo_guid = GRUB_EFI_DEVICE_PATH_GUID;
grub_efi_file_t *root = NULL;
grub_efi_file_t *file = NULL;

typedef struct {
  grub_efi_uint64_t size;
  grub_efi_uint64_t filesize;
  grub_efi_uint64_t physicalsize;
  grub_efi_time_t createtime;
  grub_efi_time_t accesstime;
  grub_efi_time_t modifytime;
  grub_efi_uint64_t attribute;
  grub_efi_char16_t filename[];
} grub_efi_file_info_t;

int 
uefi_mount (void)
{
  grub_efi_handle_t dev_handle = grub_efidisk_get_current_bdev_handle();
  grub_efi_status_t status;

  if (!dev_handle)
    return 0;
  
  file_system = grub_efi_open_protocol (dev_handle, &simple_file_system_guid,
					GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);

  if (!file_system)
    return 0;

  status = Call_Service_2 (file_system->open_volume, file_system, &root);

  if (status != GRUB_EFI_SUCCESS)
    return 0;

  return 1;
}

int 
uefi_dir (char *dirname)
{
  grub_efi_status_t status;
  grub_efi_char16_t *file_name_w = NULL;
  grub_efi_char16_t *dir_name_w = NULL;
  grub_efi_file_info_t *fileinfo = NULL;
  grub_efi_uintn_t buffersize = 0;  
  grub_efi_file_t *directory = NULL;
  int i, dirlen = 0, ret = 0;

  file_name_w = grub_malloc (2 * strlen(dirname) + 2);
  if (!file_name_w)
    goto done;

  for (i=0; i<strlen(dirname); i++) {
    file_name_w[i] = dirname[i];
    if (file_name_w[i] == '/') {
      file_name_w[i] = '\\';
      dirlen = i;
    }
  }

  file_name_w[i] = '\0';

  status = Call_Service_5 (root->open, root, &file, file_name_w,
			   GRUB_EFI_FILE_MODE_READ, 0);

  if (status != GRUB_EFI_SUCCESS)
    goto done;

  if (dirname[i-1] == '/') {
    if (print_possibilities)
      grub_printf("\n");

    while (1) {
      int filenamelen;

      status = Call_Service_3 (file->read, file, &buffersize, fileinfo);

      if (status == GRUB_EFI_BUFFER_TOO_SMALL) {
	fileinfo = grub_malloc(buffersize);
	continue;
      } else if (status) {
	goto done;
      } else if (buffersize == 0) {
	ret = 1;
	if (print_possibilities)
	  grub_printf("\n");
	goto done;
      }

      filenamelen = fileinfo->size - sizeof(*fileinfo);

      if (print_possibilities) {
	for (i=0; i<filenamelen/2; i++)
	  grub_printf("%c", (char)fileinfo->filename[i]);
      }
      grub_printf(" ");
    }
  } else {
    char *data = NULL;
    dir_name_w = grub_malloc (2 * dirlen + 2);
    if (!dir_name_w)
      goto done;

    for (i=0; i<dirlen; i++)
      dir_name_w[i] = file_name_w[i];

    dir_name_w[i] = '\0';

    status = Call_Service_5 (root->open, root, &directory, dir_name_w,
			     GRUB_EFI_FILE_MODE_READ, 0);

    while (1) {
      int filenamelen;
      int invalid = 0;

      status = Call_Service_3 (directory->read, directory, &buffersize, fileinfo);

      if (status == GRUB_EFI_BUFFER_TOO_SMALL) {
	fileinfo = grub_malloc(buffersize);
	continue;
      } else if (status) {
	goto done;
      } else if (buffersize == 0) {
	goto done;
      }

      filenamelen = fileinfo->size - sizeof(*fileinfo);

      if (filenamelen != ((strlen(dirname) - dirlen) * 2))
	continue;

      for (i=0; i<filenamelen/2; i++) {
	if (fileinfo->filename[i] != file_name_w[i + dirlen + 1]) {
	  if (fileinfo->filename[i] > 0xff ||
	      (grub_tolower(fileinfo->filename[i]) !=
	       grub_tolower(file_name_w[i + dirlen + 1])))
	    invalid = 1;
	}
      }

      if (!invalid)
	break;
    }

    if (fileinfo->filesize < 256 && fileinfo->filesize > 3)
      {
	data = grub_malloc(fileinfo->filesize);
	filepos = 0;

	uefi_read(data, fileinfo->filesize);

	/*
	 * UEFI doesn't really support symlinks. Apple's UEFI driver exposes
	 * them as files containing the path of the target. This provides
	 * hacky support in the absence of a real driver.
	 */
	if (data[0] == '.' && data[1] == '.' && data[2] == '/')
	  {
	    int j;
	    char *tmpdir = grub_malloc(dirlen + fileinfo->filesize);

	    for (j=0; j<dirlen+1; j++)
	      tmpdir[j] = dirname[j];
	    for (j=0; j<fileinfo->filesize; j++)
	      tmpdir[j+dirlen+1] = data[j];
	    tmpdir[j+dirlen+1] = '\0';

	    /* Open the new file */
	    ret = uefi_dir(tmpdir);
	    grub_free (tmpdir);
	    grub_free (data);
	    goto done;
	  }
      }

    if (data)
      grub_free(data);
    ret = 1;
    filemax = fileinfo->filesize;
    filepos = 0;
  }

 done:
  if (fileinfo)
    grub_free (fileinfo);
  if (dir_name_w)
    grub_free (dir_name_w);
  if (file_name_w)
    grub_free (file_name_w);

  return ret;
}

void
uefi_close (void)
{
  grub_efi_status_t status = 0;

  if (file)
    status = Call_Service_1 (file->close, file);

  file = NULL;
}

int 
uefi_read (char *addr, int len)
{
  grub_efi_status_t status;
  grub_efi_uintn_t length = len;

  errnum = 0;

  status = Call_Service_2 (file->set_position, file, filepos);

  if (status != GRUB_EFI_SUCCESS)
    goto out;

  status = Call_Service_3 (file->read, file, &length, addr);
  
  len = length;

  if (status == GRUB_EFI_SUCCESS) {
    filepos += len;
    return len;
  }

 out:
  errnum = ERR_FILE_NOT_FOUND;

  return 0;
}
#endif
