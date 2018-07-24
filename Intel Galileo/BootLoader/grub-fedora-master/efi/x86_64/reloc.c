/* reloc_x86_64.c - position independent x86_64 ELF shared object relocator
   Copyright (C) 1999 Hewlett-Packard Co.
	Contributed by David Mosberger <davidm@hpl.hp.com>.
   Copyright (C) 2005 Intel Co.
	Contributed by Fenghua Yu <fenghua.yu@intel.com>.

   This file is part of GNU-EFI, the GNU EFI development environment.

   GNU EFI is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU EFI is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU EFI; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA. */

#include <grub/efi/efi.h>
#include <grub/efi/api.h>
#include <link.h>

grub_efi_status_t _relocate (long ldbase, ElfW(Dyn) *dyn,
			     grub_efi_handle_t image_handle,
			     grub_efi_system_table_t *sys_tab)
{
	//extern EFI_STATUS efi_main (EFI_HANDLE, EFI_SYSTEM_TABLE *);
	long relsz = 0, relent = 0;
	ElfW(Rel) *rel = 0;
	unsigned long *addr;
	int i;

	for (i = 0; dyn[i].d_tag != DT_NULL; ++i) {
		switch (dyn[i].d_tag) {
		      case DT_RELA:
			rel = (ElfW(Rel)*) ((long) dyn[i].d_un.d_ptr + ldbase);
			break;

		      case DT_RELASZ:
			relsz = dyn[i].d_un.d_val;
			break;

		      case DT_RELAENT:
			relent = dyn[i].d_un.d_val;
			break;

		      default:
			break;
		}
	}

 	if (!rel || relent == 0){
 		return GRUB_EFI_LOAD_ERROR;
	}
	while (relsz > 0) {
		/* apply the relocs */
		switch (ELF64_R_TYPE (rel->r_info)) {
		      case R_X86_64_NONE:
			break;

		      case R_X86_64_RELATIVE:
			 addr = (unsigned long *) (ldbase + rel->r_offset);
			 *addr += ldbase;
			      break;

		      default:
			break;
		}
		rel = (ElfW(Rel)*) ((char *) rel + relent);
		relsz -= relent;
	}
	return GRUB_EFI_SUCCESS;
}

