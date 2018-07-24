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

#include <grub/misc.h>
#include <grub/types.h>
#include <grub/cpu/linux.h>
#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/efi/misc.h>

#include "switch.h"

#include <shared.h>

#include "graphics.h"

#define grub_file_size()    filemax

#define NEXT_MEMORY_DESCRIPTOR(desc, size)      \
  ((grub_efi_memory_descriptor_t *) ((char *) (desc) + (size)))

#define PTR_HI(x) ((grub_uint32_t) ((unsigned long long)((unsigned long)(x)) >> 32))

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 0x200
#endif /* defined(SECTOR_SIZE) */
#ifndef SECTOR_BITS
#define SECTOR_BITS 9
#endif /* defined(SECTOR_BITS) */

static unsigned long linux_mem_size;
static int loaded;
static void *real_mode_mem;
static void *prot_mode_mem;
static grub_size_t prot_kernel_size;
static void *initrd_mem;
static grub_efi_uintn_t real_mode_pages;
static grub_efi_uintn_t prot_mode_pages;
static grub_efi_uintn_t initrd_pages;
static grub_efi_guid_t graphics_output_guid = GRUB_EFI_GRAPHICS_OUTPUT_GUID;

static inline grub_size_t
page_align (grub_size_t size)
{
  return (size + (1 << 12) - 1) & (~((1 << 12) - 1));
}

static void
free_pages (void)
{
  if (real_mode_mem)
    {
      grub_efi_free_pages ((grub_addr_t) real_mode_mem, real_mode_pages);
      real_mode_mem = 0;
    }

  if (initrd_mem)
    {
      grub_efi_free_pages ((grub_addr_t) initrd_mem, initrd_pages);
      initrd_mem = 0;
    }

  if (mmap_buf)
    {
      grub_efi_free_pages ((grub_addr_t) mmap_buf, mmap_pages);
      mmap_buf = 0;
    }
}

/* Allocate pages for the real mode code and the protected mode code
   for linux as well as a memory map buffer.  */
static int
allocate_pages (grub_size_t real_size, grub_size_t prot_size)
{
  grub_efi_uintn_t desc_size;
  grub_efi_memory_descriptor_t *mmap_end;
  grub_efi_memory_descriptor_t *desc;
  grub_efi_physical_address_t addr;

  /* Make sure that each size is aligned to a page boundary.  */
  real_size = page_align (real_size + SECTOR_SIZE);
  prot_size = page_align (prot_size);

  grub_dprintf ("linux", "real_size = %x, prot_size = %x, mmap_size = %x\n",
		(unsigned int) real_size, (unsigned int) prot_size,
		(unsigned int) mmap_size);

  /* Calculate the number of pages; Combine the real mode code with
     the memory map buffer for simplicity.  */
  real_mode_pages = (real_size >> 12);
  prot_mode_pages = (prot_size >> 12);

  /* Initialize the memory pointers with NULL for convenience.  */
  real_mode_mem = 0;
  prot_mode_mem = 0;

  if (grub_efi_get_memory_map (0, &desc_size, 0) <= 0)
    grub_fatal ("cannot get memory map");

  addr = 0;
  mmap_end = NEXT_MEMORY_DESCRIPTOR (mmap_buf, mmap_size);
  /* First, find free pages for the real mode code
     and the memory map buffer.  */
  for (desc = mmap_buf;
       desc < mmap_end;
       desc = NEXT_MEMORY_DESCRIPTOR (desc, desc_size))
    {
      if (desc->type == GRUB_EFI_CONVENTIONAL_MEMORY
	  && desc->num_pages >= real_mode_pages)
	{
	  grub_efi_physical_address_t physical_end;

          physical_end = desc->physical_start + (desc->num_pages << 12);

          grub_dprintf ("linux", "physical_start = %x, physical_end = %x\n",
                        (unsigned) desc->physical_start,
                        (unsigned) physical_end);
          addr = physical_end - real_size;
          if (addr < 0x100000)
            continue;

          /* the kernel wants this address to be under 1 gig.*/
          if (desc->physical_start > 0x40000000 - real_size)
            continue;

          if (addr > 0x40000000 - real_size)
            addr = 0x40000000 - real_size;

          grub_dprintf ("linux", "trying to allocate %u pages at %x\n",
                        (unsigned) real_mode_pages, (unsigned) addr);
          real_mode_mem = grub_efi_allocate_pages (addr, real_mode_pages);
          if (! real_mode_mem)
            grub_fatal ("cannot allocate pages");

          desc->num_pages -= real_mode_pages;
          break;
	}
    }

  if (! real_mode_mem)
    {
      grub_printf ("cannot allocate real mode pages");
      errnum = ERR_WONT_FIT;
      goto fail;
    }

  grub_printf("Trying to allocate %u pages for VMLINUZ\n",
		(unsigned) prot_mode_pages);
  prot_mode_mem = grub_efi_allocate_anypages(prot_mode_pages);

  if (!prot_mode_mem)
	grub_fatal("Cannot allocate pages for VMLINUZ");
  grub_printf("Got pages at %p\n", prot_mode_mem);
 
  return 1;

 fail:
  free_pages ();
  return 0;
}

/* do some funky stuff, then boot linux */
void
linux_boot (void)
{
  grub_printf ("zImage is not supported under EFI.\n");
  for (;;);
}

static void
grub_efi_disable_network (void)
{
  grub_efi_guid_t pci_io_guid = GRUB_EFI_PCI_IO_GUID;
  grub_efi_pci_io_t *pci_proto;
  grub_efi_uintn_t num_handles;
  grub_efi_handle_t *handle, *handles;

  handles = grub_efi_locate_handle (GRUB_EFI_BY_PROTOCOL,
					  &pci_io_guid,
					  NULL, &num_handles);
  if (!handles || !num_handles)
    return;

  for (handle = handles; num_handles--; handle++)
    {
      grub_efi_uint8_t class, pos, id, pm = 0;
      grub_efi_uint16_t pm_state, vendor;
      int ttl = 48;

      pci_proto = grub_efi_open_protocol (*handle, &pci_io_guid,
					  GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);

      if (!pci_proto)
	continue;

      Call_Service_5 (pci_proto->pci.read, pci_proto,
		      grub_efi_pci_io_width_uint8, 0x0b, 1, &class);

      /* Not a network device */
      if (class != 0x02)
	continue;

      Call_Service_5 (pci_proto->pci.read, pci_proto,
		      grub_efi_pci_io_width_uint16, 0x00, 1, &vendor);

      /* Not a Broadcom */
      if (vendor != 0x14e4)
	continue;

      Call_Service_5 (pci_proto->pci.read, pci_proto,
		      grub_efi_pci_io_width_uint16, 0x2c, 1, &vendor);

      /* Not an Apple */
      if (vendor != 0x106b)
	continue;

      pos = 0x34;

      /* Find the power management registers */
      while (ttl--)
	{
	  Call_Service_5 (pci_proto->pci.read, pci_proto,
			  grub_efi_pci_io_width_uint8, pos, 1, &pos);

	  if (pos < 0x40)
	    break;

	  pos &= ~3;

	  Call_Service_5 (pci_proto->pci.read, pci_proto,
			  grub_efi_pci_io_width_uint8, pos, 1, &id);

	  if (id == 0xff)
	    break;

	  if (id == 0x01)
	    {
	      pm = pos;
	      break;
	    }

	  pos += 1;
	}

      if (pm)
	{
	  Call_Service_5 (pci_proto->pci.read, pci_proto,
			  grub_efi_pci_io_width_uint16, pm + 4, 1, &pm_state);

	  pm_state &= ~0x03;
	  pm_state |= 0x03;

	  /* Set to D3 */

	  Call_Service_5 (pci_proto->pci.write, pci_proto,
			  grub_efi_pci_io_width_uint16, pm + 4, 1, &pm_state);

	  Call_Service_5 (pci_proto->pci.read, pci_proto,
			  grub_efi_pci_io_width_uint16, pm + 4, 1,
			  &pm_state);
	}
    }
}

void
big_linux_boot (void)
{
  struct linux_kernel_params *params;
  struct grub_linux_kernel_header *lh;
  grub_efi_uintn_t map_key;
  grub_efi_uintn_t desc_size;
  grub_efi_uint32_t desc_version;
  int e820_nr_map;

  params = real_mode_mem;

  graphics_set_kernel_params (params);

  if (grub_efi_get_memory_map (&map_key, &desc_size, &desc_version) <= 0)
    grub_fatal ("cannot get memory map");

  /* Pass e820 memmap. */
  e820_map_from_efi_map ((struct e820_entry *) params->e820_map, &e820_nr_map,
			 mmap_buf, desc_size, mmap_size);
  params->e820_nr_map = e820_nr_map;

  grub_efi_disable_network();

  if (! grub_efi_exit_boot_services (map_key))
    grub_fatal ("cannot exit boot services");

  /* Note that no boot services are available from here.  */

  /* copy vmlinuz image to hdr.code32_start */
  memcpy ((char *)(unsigned long)(params->hdr.code32_start), (char *)prot_mode_mem,
	  prot_kernel_size);
  /* copy switch image */
  memcpy ((void *) 0x700, switch_image, switch_size);

  lh = &params->hdr;
  /* Pass EFI parameters.  */
  if (grub_le_to_cpu16 (lh->version) >= 0x0206) {
    params->version_0206.efi_mem_desc_size = desc_size;
    params->version_0206.efi_mem_desc_version = desc_version;
    params->version_0206.efi_mmap = (grub_uint32_t) (unsigned long) mmap_buf;
    params->version_0206.efi_mmap_size = mmap_size;
  } else if (grub_le_to_cpu16 (lh->version) >= 0x0204) {
    params->version_0204.efi_mem_desc_size = desc_size;
    params->version_0204.efi_mem_desc_version = desc_version;
    params->version_0204.efi_mmap = (grub_uint32_t) (unsigned long) mmap_buf;
    params->version_0204.efi_mmap_size = mmap_size;
  } else /* dunno */ {
    params->dunno.efi_mem_desc_size = desc_size;
    params->dunno.efi_mem_desc_version = desc_version;
    params->dunno.efi_mmap = (grub_uint32_t) (unsigned long) mmap_buf;
    params->dunno.efi_mmap_size = mmap_size;
    params->dunno.efi_mmap_hi = PTR_HI(mmap_buf);
  }

#ifdef __x86_64__
  /* Pass parameters.  */
  asm volatile ("mov %0, %%rsi" : : "m" (real_mode_mem));
  asm volatile ("movl %0, %%ebx" : : "m" (params->hdr.code32_start));

  /* Enter Linux, switch from 64-bit long mode
   * to 32-bit protect mode, this code end address
   * must not exceed 0x1000, because linux kernel bootstrap
   * code will flush this area
   */
  asm volatile ( "mov $0x700, %%rdi" : :);
  asm volatile ( "jmp *%%rdi" : :);
#else
  /* Pass parameters.  */
  asm volatile ("mov %0, %%esi" : : "m" (real_mode_mem));
  asm volatile ("movl %0, %%ebx" : : "m" (params->hdr.code32_start));

  /* Enter Linux, switch from 64-bit long mode
   * to 32-bit protect mode, this code end address
   * must not exceed 0x1000, because linux kernel bootstrap
   * code will flush this area
   */
  asm volatile ( "mov $0x700, %%edi" : :);
  asm volatile ( "jmp *%%edi" : :);
#endif

  /* Never reach here.  */
  for (;;);
}

int
grub_load_linux (char *kernel, char *arg)
{
  struct grub_linux_kernel_header *lh;
  struct linux_kernel_params *params;
  static struct linux_kernel_params params_buf;
  grub_uint8_t setup_sects;
  grub_size_t real_size, prot_size;
  grub_uint64_t kernel_base, kernel_length, kernel_pages;
  grub_ssize_t len;
  char *dest;
  int align, min_alignment;
  int relocatable = 0;

  if (kernel == NULL)
    {
      errnum = ERR_BAD_FILENAME;
      grub_printf ("no kernel specified");
      goto fail1;
    }

  if (! grub_open (kernel))
    goto fail1;

  if (grub_read ((char *) &params_buf, sizeof (params_buf))
      != sizeof (params_buf))
    {
      errnum = ERR_EXEC_FORMAT;
      grub_printf ("cannot read the linux header");
      goto fail;
    }

  lh = &params_buf.hdr;

  if (lh->boot_flag != grub_cpu_to_le16 (0xaa55))
    {
      errnum = ERR_EXEC_FORMAT;
      grub_printf ("invalid magic number: %x", lh->boot_flag);
      goto fail;
    }

  /* EFI support is quite new, so reject old versions.  */
  if (lh->header != grub_cpu_to_le32 (GRUB_LINUX_MAGIC_SIGNATURE)
      || grub_le_to_cpu16 (lh->version) < 0x0203)
    {
      errnum = ERR_EXEC_FORMAT;
      grub_printf ("too old version");
      goto fail;
    }

  /* I'm not sure how to support zImage on EFI.  */
  if (! (lh->loadflags & GRUB_LINUX_FLAG_BIG_KERNEL))
    {
      errnum = ERR_EXEC_FORMAT;
      grub_printf ("zImage is not supported");
      goto fail;
    }

  setup_sects = lh->setup_sects;

  real_size = 0x1000 + grub_strlen(arg);
  prot_size = grub_file_size () - (setup_sects << SECTOR_BITS) - SECTOR_SIZE;
  prot_kernel_size = prot_size;

  if (! allocate_pages (real_size, prot_size))
    goto fail;

  /* XXX Linux assumes that only elilo can boot Linux on EFI!!!  */
  lh->type_of_loader = 0x50;

  lh->cmd_line_ptr = (grub_uint32_t) (unsigned long) real_mode_mem + 0x1000;
  lh->ramdisk_image = 0;
  lh->ramdisk_size = 0;

  grub_memset(real_mode_mem, 0, real_size);

  params = (struct linux_kernel_params *) real_mode_mem;

  grub_memmove(&params->hdr, lh, 0x202 + lh->jump_off - 0x1f1);

  params->cl_magic = GRUB_LINUX_CL_MAGIC;
  params->cl_offset = 0x1000;

  /* These are not needed to be precise, because Linux uses these values
     only to raise an error when the decompression code cannot find good
     space.  */
  params->ext_mem = ((32 * 0x100000) >> 10);
  params->alt_mem = ((32 * 0x100000) >> 10);

  /* No APM on EFI.  */
  params->apm_version = 0;
  params->apm_code_segment = 0;
  params->apm_entry = 0;
  params->apm_16bit_code_segment = 0;
  params->apm_data_segment = 0;
  params->apm_flags = 0;
  params->apm_code_len = 0;
  params->apm_data_len = 0;

  /* XXX is there any way to use SpeedStep on EFI?  */
  params->ist_signature = 0;
  params->ist_command = 0;
  params->ist_event = 0;
  params->ist_perf_level = 0;

  /* Let the kernel probe the information.  */
  grub_memset (params->hd0_drive_info, 0, sizeof (params->hd0_drive_info));
  grub_memset (params->hd1_drive_info, 0, sizeof (params->hd1_drive_info));

  /* No MCA on EFI.  */
  params->rom_config_len = 0;

  if (grub_le_to_cpu16 (lh->version) >= 0x0206) {
    grub_memcpy(&params->version_0204.efi_signature, "EL64", 4);
    params->version_0206.efi_system_table = \
                        (grub_uint32_t) (unsigned long) grub_efi_system_table;
  } else if (grub_le_to_cpu16 (lh->version) >= 0x0204) {
    grub_memcpy(&params->version_0204.efi_signature, "EFIL", 4);
    params->version_0204.efi_system_table = \
                        (grub_uint32_t) (unsigned long) grub_efi_system_table;
  } else /* dunno */ {
    params->dunno.efi_signature = GRUB_LINUX_EFI_SIGNATURE_X64;
    params->dunno.efi_system_table = \
                        (grub_uint32_t) (unsigned long) grub_efi_system_table;
    params->dunno.efi_system_table_hi = PTR_HI(grub_efi_system_table);
  }
  /* The other EFI parameters are filled when booting.  */

  /* No EDD */
  params->eddbuf_entries = 0;
  params->edd_mbr_sig_buf_entries = 0;

  /* XXX there is no way to know if the kernel really supports EFI.  */
  grub_printf ("[Linux-EFI, setup=0x%x, size=0x%x]\n", (unsigned int)real_size,
	       (unsigned int)prot_size);

  /* Check the mem= option to limit memory used for initrd.  */
  {
    char *mem;

    mem = grub_strstr (arg, "mem=");
    if (mem)
      {
	char *value = mem + 4;

	safe_parse_maxulong (&value, &linux_mem_size);
	switch (errnum)
	  {
	  case ERR_NUMBER_OVERFLOW:
	    /* If an overflow occurs, use the maximum address for
	       initrd instead. This is good, because MAXINT is
	       greater than LINUX_INITRD_MAX_ADDRESS.  */
	    linux_mem_size = LINUX_INITRD_MAX_ADDRESS;
	    errnum = ERR_NONE;
	    break;

	  case ERR_NONE:
	    {
	      int shift = 0;

	      switch (grub_tolower (*value))
		{
		case 'g':
		  shift += 10;
		case 'm':
		  shift += 10;
		case 'k':
		  shift += 10;
		default:
		  break;
		}

	      /* Check an overflow.  */
	      if (linux_mem_size > (~0UL >> shift))
		linux_mem_size = 0;
	      else
		linux_mem_size <<= shift;
	    }
	    break;

	  default:
	    linux_mem_size = 0;
	    errnum = ERR_NONE;
	    break;
	  }
      }
    else
      linux_mem_size = 0;
  }

  dest = grub_stpcpy ((char *) real_mode_mem + 0x1000, skip_to(0, arg));

  grub_seek ((setup_sects << SECTOR_BITS) + SECTOR_SIZE);
  len = prot_size;
  if (grub_read ((char *)prot_mode_mem, len) != len)
    grub_printf ("Couldn't read file");

  if (lh->version >= 0x205) {
    for (align = lh->min_alignment; align < 32; align++) {
      if (lh->kernel_alignment & (1 << align)) {
	break;
      }
    }
    relocatable = lh->relocatable_kernel;
  }

  if (lh->version >= 0x20a) {
    kernel_base = lh->pref_address;
    kernel_length = lh->init_size;
    min_alignment = lh->min_alignment;
  } else {
    kernel_base = lh->code32_start;
    kernel_length = prot_kernel_size;
    min_alignment = 0;
  }

  kernel_pages = (kernel_length + 4095) >> 12;

  /* Attempt to allocate address space for the kernel */
  kernel_base = (grub_uint64_t)grub_efi_allocate_pages(kernel_base, kernel_pages);

  if (!kernel_base && relocatable) {
    grub_efi_memory_descriptor_t *desc;
    grub_efi_memory_descriptor_t tdesc;
    grub_efi_uintn_t desc_size;

    if (grub_efi_get_memory_map (0, &desc_size, 0) <= 0)
      grub_fatal ("cannot get memory map");

    while (align >= min_alignment) {
      for (desc = mmap_buf;
	   desc < NEXT_MEMORY_DESCRIPTOR (mmap_buf, mmap_size);
	   desc = NEXT_MEMORY_DESCRIPTOR (desc, desc_size))
	{
	  grub_uint64_t addr;
	  grub_uint64_t alignval = (1 << align) - 1;

	  if (desc->type != GRUB_EFI_CONVENTIONAL_MEMORY)
	    continue;

	  memcpy(&tdesc, desc, sizeof(tdesc));

	  addr = (tdesc.physical_start + alignval) & ~(alignval);

	  if ((addr + kernel_length) >
	      (tdesc.physical_start + (tdesc.num_pages << 12)))
	    continue;

	  kernel_base = (grub_uint64_t)grub_efi_allocate_pages(addr, kernel_pages);

	  if (kernel_base) {
	    lh->kernel_alignment = 1 << align;
	    break;
	  }
	}
      align--;
      if (kernel_base)
	break;
    }
  }

  if (!kernel_base) {
    grub_printf("Failed to allocate kernel memory");
    errnum = ERR_UNRECOGNIZED;
    goto fail;
  }

  lh->code32_start = kernel_base;

  if (errnum == ERR_NONE)
    {
      loaded = 1;
    }

 fail:

  grub_close ();

 fail1:

  if (errnum != ERR_NONE)
    {
      loaded = 0;
    }
  return errnum ? KERNEL_TYPE_NONE : KERNEL_TYPE_BIG_LINUX;
}

int
grub_load_initrd (char *initrd)
{
  grub_ssize_t size;
  grub_addr_t addr_min, addr_max;
  grub_addr_t addr;
  grub_efi_memory_descriptor_t *desc;
  grub_efi_memory_descriptor_t tdesc;
  grub_efi_uintn_t desc_size;
  struct linux_kernel_params *params;

  if (initrd == NULL)
    {
      errnum = ERR_BAD_FILENAME;
      grub_printf ("No module specified");
      goto fail1;
    }

  if (! loaded)
    {
      errnum = ERR_UNRECOGNIZED;
      grub_printf ("You need to load the kernel first.");
      goto fail1;
    }

  if (! grub_open (initrd))
    goto fail1;

  size = grub_file_size ();
  initrd_pages = (page_align (size) >> 12);

  params = (struct linux_kernel_params *) real_mode_mem;
  grub_dprintf(__func__, "initrd_pages: %lu\n", initrd_pages);

  addr_max = grub_cpu_to_le32 (params->hdr.initrd_addr_max);
  if (linux_mem_size != 0 && linux_mem_size < addr_max)
    addr_max = linux_mem_size;
  addr_max &= ~((1 << 12)-1);

  /* Usually, the compression ratio is about 50%.  */
  addr_min = 0;

  /* Find the highest address to put the initrd.  */
  grub_dprintf(__func__, "addr_min: 0x%lx addr_max: 0x%lx mmap_size: %lu\n", addr_min, addr_max, mmap_size);
  if (grub_efi_get_memory_map (0, &desc_size, 0) <= 0)
    grub_fatal ("cannot get memory map");

  addr = 0;
  for (desc = mmap_buf;
       desc < NEXT_MEMORY_DESCRIPTOR (mmap_buf, mmap_size);
       desc = NEXT_MEMORY_DESCRIPTOR (desc, desc_size))
    {
      if (desc->type != GRUB_EFI_CONVENTIONAL_MEMORY)
        continue;
      memcpy(&tdesc, desc, sizeof (tdesc));
      if (tdesc.physical_start < addr_min
              && tdesc.num_pages > ((addr_min - tdesc.physical_start) >> 12))
        {
          tdesc.num_pages -= ((addr_min - tdesc.physical_start) >> 12);
          tdesc.physical_start = addr_min;
        }

      grub_dprintf(__func__, "desc = {type=%d,ps=0x%llx,vs=0x%llx,sz=%llu,attr=%llu}\n", desc->type, (unsigned long long)desc->physical_start, (unsigned long long)desc->virtual_start, (unsigned long long)desc->num_pages, (unsigned long long)desc->attribute);
      if (tdesc.physical_start >= addr_min
	  && tdesc.physical_start + page_align (size) <= addr_max
	  && tdesc.num_pages >= initrd_pages)
	{
	  grub_efi_physical_address_t physical_end;

	  physical_end = tdesc.physical_start + (tdesc.num_pages << 12);
	  if (physical_end > addr_max)
	    physical_end = addr_max;

	  if (physical_end <= 0x7fffffffUL && physical_end > addr)
	    addr = physical_end - page_align (size);
	}
    }

  if (addr == 0)
    {
      errnum = ERR_UNRECOGNIZED;
      grub_printf ("no free pages available");
      goto fail;
    }

  initrd_mem = grub_efi_allocate_pages (addr, initrd_pages);
  if (! initrd_mem)
    grub_fatal ("cannot allocate pages: %x@%x", (unsigned)initrd_pages,
		(unsigned)addr);

  if (grub_read (initrd_mem, size) != size)
    {
      grub_printf ("Couldn't read file");
      goto fail;
    }

  grub_printf ("   [Initrd, addr=0x%x, size=0x%x]\n", (unsigned int) addr,
	       (unsigned int) size);

  params->hdr.ramdisk_image = addr;
  params->hdr.ramdisk_size = size;
  params->hdr.root_dev = 0x0100; /* XXX */

 fail:
  grub_close ();
 fail1:
  return !errnum;
}
