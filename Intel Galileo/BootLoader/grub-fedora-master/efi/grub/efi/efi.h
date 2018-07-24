/* efi.h - declare variables and functions for EFI support */
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

#ifndef GRUB_EFI_EFI_HEADER
#define GRUB_EFI_EFI_HEADER	1

#include <grub/types.h>
#include <grub/efi/api.h>

/* Functions.  */
grub_efi_status_t
grub_efi_locate_device_path (grub_efi_guid_t *protocol,
                             grub_efi_device_path_t **dp,
                             grub_efi_handle_t *handle);
grub_efi_status_t
grub_efi_locate_handle_buffer (grub_efi_locate_search_type_t search_type,
				grub_efi_guid_t *protocol,
				void *search_key,
				grub_efi_uintn_t *no_handles,
				grub_efi_handle_t **buffer);
void *grub_efi_locate_protocol (grub_efi_guid_t * protocol,
				void *registration);
grub_efi_handle_t *grub_efi_locate_handle (grub_efi_locate_search_type_t
					   search_type,
					   grub_efi_guid_t * protocol,
					   void *search_key,
					   grub_efi_uintn_t * num_handles);
void *grub_efi_open_protocol (grub_efi_handle_t handle,
			      grub_efi_guid_t * protocol,
			      grub_efi_uint32_t attributes);
int grub_efi_set_text_mode (int on);
void grub_efi_stall (grub_efi_uintn_t microseconds);
void *grub_efi_allocate_pool (grub_efi_uintn_t size);
void grub_efi_free_pool (void *buffer);
void *grub_efi_allocate_anypages (grub_efi_uintn_t pages);
void *grub_efi_allocate_pages (grub_efi_physical_address_t address,
			       grub_efi_uintn_t pages);
void *grub_efi_allocate_runtime_pages (grub_efi_physical_address_t address,
				       grub_efi_uintn_t pages);
void
grub_efi_free_pages (grub_efi_physical_address_t address,
		     grub_efi_uintn_t pages);
int
grub_efi_get_memory_map (grub_efi_uintn_t * map_key,
			 grub_efi_uintn_t * descriptor_size,
			 grub_efi_uint32_t * descriptor_version);
grub_efi_loaded_image_t *grub_efi_get_loaded_image (grub_efi_handle_t
						    image_handle);
void grub_efi_print_device_path (grub_efi_device_path_t * dp);
char *grub_efi_get_filename (grub_efi_device_path_t * dp);
grub_efi_device_path_t *grub_efi_get_device_path (grub_efi_handle_t handle);
int grub_efi_exit_boot_services (grub_efi_uintn_t map_key);

void grub_efi_mm_init (void);
void grub_efi_mm_fini (void);
void grub_efi_init (void);
void grub_efi_fini (void);
void grub_efi_set_prefix (void);

/* Variables.  */
extern void *mmap_buf;
extern grub_efi_uintn_t mmap_size;
extern grub_efi_uintn_t mmap_pages;

extern grub_efi_system_table_t *grub_efi_system_table;
extern grub_efi_handle_t grub_efi_image_handle;

#endif /* ! GRUB_EFI_EFI_HEADER */
