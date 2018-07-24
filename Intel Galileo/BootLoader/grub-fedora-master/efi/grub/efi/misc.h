/* misc.h - prototypes for misc EFI functions */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2003,2005,2006  Free Software Foundation, Inc.
 *
 *  GRUB is free software; you can redistribute it and/or modify
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
 *  along with GRUB; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GRUB_EFI_MISC_HEADER
#define GRUB_EFI_MISC_HEADER	1

struct e820_entry;
void e820_map_from_efi_map (struct e820_entry *e820_map,
			    int *e820_nr_map,
			    grub_efi_memory_descriptor_t *memory_map,
			    grub_efi_uintn_t desc_size,
			    grub_efi_uintn_t memory_map_size);

/* Initialize the console system.  */
void grub_console_init (void);
/* Finish the console system.  */
void grub_console_fini (void);

void grub_efidisk_init (void);
void grub_efidisk_fini (void);
grub_efi_handle_t grub_efidisk_get_current_bdev_handle (void);
int grub_get_drive_partition_from_bdev_handle (grub_efi_handle_t handle,
					       unsigned long *drive,
					       unsigned long *partition);

char *grub_efi_file_path_to_path_name (grub_efi_device_path_t *file_path);
void grub_load_saved_default (grub_efi_handle_t dev_handle);

grub_efi_device_path_t *
find_last_device_path (const grub_efi_device_path_t *dp);
grub_efi_device_path_t *
find_parent_device_path (const grub_efi_device_path_t *dp);
grub_efi_device_path_t *
duplicate_device_path (const grub_efi_device_path_t *dp);
int
compare_device_paths (const grub_efi_device_path_t *dp1,
		      const grub_efi_device_path_t *dp2);
grub_efi_device_path_t *
device_path_from_utf8 (const char *device);

extern grub_efi_guid_t simple_file_system_guid;

#endif /* ! GRUB_EFI_MISC_HEADER */
