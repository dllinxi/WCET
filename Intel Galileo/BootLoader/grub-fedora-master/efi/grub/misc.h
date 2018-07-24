/* misc.h - prototypes for misc functions */
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

#ifndef GRUB_MISC_HEADER
#define GRUB_MISC_HEADER	1

#include <stdarg.h>
#include <grub/types.h>

#define grub_dprintf(condition, fmt, args...) \
	grub_real_dprintf(__FILE__, __LINE__, condition, fmt, ## args)

char *grub_stpcpy (char *dest, const char *src);
void grub_real_dprintf (const char *file,
			const int line,
			const char *condition,
			const char *fmt, ...)
  __attribute__ ((format (printf, 4, 5)));
void grub_exit (void) __attribute__ ((noreturn));
void grub_abort (void) __attribute__ ((noreturn));
void grub_fatal (const char *fmt, ...) __attribute__ ((noreturn));
grub_size_t grub_utf8_char_len(grub_uint8_t ch);
grub_uint32_t grub_utf8_to_utf32(const grub_uint8_t *src, grub_size_t length);
void grub_utf8_to_utf16(const grub_uint8_t *src, grub_size_t srclen,
			grub_uint16_t *dst, grub_size_t dstlen);
grub_uint8_t *grub_utf16_to_utf8 (grub_uint8_t * dest,
				  grub_uint16_t * src, grub_size_t size);

void *grub_malloc (grub_size_t size);
void grub_free (void *ptr);

char *grub_strndup (const char *s, int n);
#define strndup grub_strndup

int safe_parse_maxulong (char **str_ptr, unsigned long *myulong_ptr);

#define E820_RAM        1
#define E820_RESERVED   2
#define E820_ACPI       3
#define E820_NVS        4
#define E820_EXEC_CODE  5
#define E820_MAX        128

struct e820_entry
{
  grub_uint64_t addr;    /* start of memory segment */
  grub_uint64_t size;    /* size of memory segment */
  grub_uint32_t type;    /* type of memory segment */
} __attribute__ ((packed));

int grub_load_linux (char *kernel, char *arg);
int grub_load_initrd (char *initrd);

#endif /* ! GRUB_MISC_HEADER */
