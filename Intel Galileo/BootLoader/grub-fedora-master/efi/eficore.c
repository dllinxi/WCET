/* eficore.c - generic EFI support */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2006  Free Software Foundation, Inc.
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

#include <config.h>
#include <grub/misc.h>
#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/efi/eficall.h>
#include <grub/efi/console_control.h>
#include <grub/efi/time.h>

#include <shared.h>

/* The handle of GRUB itself. Filled in by the startup code.  */
grub_efi_handle_t grub_efi_image_handle;

/* The pointer to a system table. Filled in by the startup code.  */
grub_efi_system_table_t *grub_efi_system_table;

static grub_efi_guid_t console_control_guid = GRUB_EFI_CONSOLE_CONTROL_GUID;
static grub_efi_guid_t loaded_image_guid = GRUB_EFI_LOADED_IMAGE_GUID;
static grub_efi_guid_t device_path_guid = GRUB_EFI_DEVICE_PATH_GUID;

/* temporary, until we're using gnu-efi's include files --pjones */
extern int setjmp(grub_jmp_buf env);
int grub_setjmp(grub_jmp_buf env)
{
	return setjmp(env);
}

extern void longjmp(grub_jmp_buf env, int val);
void grub_longjmp(grub_jmp_buf env, int val)
{
	longjmp(env, val);
}

grub_efi_status_t
grub_efi_locate_device_path (grub_efi_guid_t *protocol,
			     grub_efi_device_path_t **dp,
			     grub_efi_handle_t *handle)
{
	return Call_Service_3(
		grub_efi_system_table->boot_services->locate_device_path,
		protocol, dp, handle);
}

grub_efi_status_t
grub_efi_locate_handle_buffer (grub_efi_locate_search_type_t search_type,
				grub_efi_guid_t *protocol,
				void *search_key,
				grub_efi_uintn_t *no_handles,
				grub_efi_handle_t **buffer)
{
	return Call_Service_5(
		grub_efi_system_table->boot_services->locate_handle_buffer,
		search_type, protocol, search_key, no_handles, buffer);
}

void *
grub_efi_locate_protocol (grub_efi_guid_t *protocol, void *registration)
{
  void *interface;
  grub_efi_status_t status;

  status = Call_Service_3 (grub_efi_system_table->boot_services->locate_protocol,
				protocol,
				registration,
				&interface);
  if (status != GRUB_EFI_SUCCESS)
    return 0;

  return interface;
}

/* Return the array of handles which meet the requirement. If successful,
   the number of handles is stored in NUM_HANDLES. The array is allocated
   from the heap.  */
grub_efi_handle_t *
grub_efi_locate_handle (grub_efi_locate_search_type_t search_type,
			grub_efi_guid_t *protocol,
			void *search_key,
			grub_efi_uintn_t *num_handles)
{
  grub_efi_boot_services_t *b;
  grub_efi_status_t status;
  grub_efi_handle_t *buffer;
  grub_efi_uintn_t buffer_size = 8 * sizeof (grub_efi_handle_t);

  buffer = grub_malloc (buffer_size);
  if (! buffer)
    return 0;

  b = grub_efi_system_table->boot_services;
  status = Call_Service_5 (b->locate_handle, search_type, protocol,
				 search_key, &buffer_size, buffer);
  if (status == GRUB_EFI_BUFFER_TOO_SMALL)
    {
      grub_free (buffer);
      buffer = grub_malloc (buffer_size);
      if (! buffer)
	return 0;

      status = Call_Service_5 (b->locate_handle, search_type, protocol,
				 search_key, &buffer_size, buffer);
    }

  if (status != GRUB_EFI_SUCCESS)
    {
      grub_free (buffer);
      return 0;
    }

  *num_handles = buffer_size / sizeof (grub_efi_handle_t);
  return buffer;
}

void *
grub_efi_open_protocol (grub_efi_handle_t handle,
			grub_efi_guid_t *protocol,
			grub_efi_uint32_t attributes)
{
  grub_efi_boot_services_t *b;
  grub_efi_status_t status;
  void *interface;

  b = grub_efi_system_table->boot_services;
  status = Call_Service_6 ( b->open_protocol,
			     handle,
			     protocol,
			     &interface,
			     grub_efi_image_handle,
			     0,
			     attributes);
  if (status != GRUB_EFI_SUCCESS)
    return 0;

  return interface;
}

int
grub_efi_set_text_mode (int on)
{
  grub_efi_console_control_protocol_t *c;
  grub_efi_screen_mode_t mode, new_mode;

  c = grub_efi_locate_protocol (&console_control_guid, 0);
  if (! c)
    /* No console control protocol instance available, assume it is
       already in text mode. */
    return 1;

  if (Call_Service_4 (c->get_mode , c, &mode, 0, 0) != GRUB_EFI_SUCCESS)
    return 0;

  new_mode = on ? GRUB_EFI_SCREEN_TEXT : GRUB_EFI_SCREEN_GRAPHICS;
  if (mode != new_mode)
    if (Call_Service_2 (c->set_mode , c, new_mode) != GRUB_EFI_SUCCESS)
      return 0;

  return 1;
}

void
grub_efi_stall (grub_efi_uintn_t microseconds)
{
  Call_Service_1 (grub_efi_system_table->boot_services->stall , microseconds);
}

grub_efi_loaded_image_t *
grub_efi_get_loaded_image (grub_efi_handle_t image_handle)
{
  return grub_efi_open_protocol (image_handle,
			 &loaded_image_guid,
			 GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
}

void
grub_exit (void)
{
  grub_efi_fini ();
  Call_Service_4(grub_efi_system_table->boot_services->exit,
		 grub_efi_image_handle,
		 GRUB_EFI_SUCCESS,
		 0,
		 0 );
  for (;;);
}

int
grub_efi_exit_boot_services (grub_efi_uintn_t map_key)
{
  grub_efi_boot_services_t *b;
  grub_efi_status_t status;

  b = grub_efi_system_table->boot_services;
  status = Call_Service_2 (b->exit_boot_services ,
				grub_efi_image_handle,
				map_key);
  return status == GRUB_EFI_SUCCESS;
}

grub_uint32_t
grub_get_rtc (void)
{
  grub_efi_time_t time;
  grub_efi_runtime_services_t *r;

  r = grub_efi_system_table->runtime_services;
  if (Call_Service_2(r->get_time , &time, 0) != GRUB_EFI_SUCCESS)
    /* What is possible in this case?  */
    return 0;

  return (((time.minute * 60 + time.second) * 1000
	   + time.nanosecond / 1000000)
	  * GRUB_TICKS_PER_SECOND / 1000);
}

grub_efi_device_path_t *
grub_efi_get_device_path (grub_efi_handle_t handle)
{
  return grub_efi_open_protocol (handle, &device_path_guid,
				 GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
}

