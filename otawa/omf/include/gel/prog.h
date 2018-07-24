/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-07, IRIT- UPS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GEL_PHDR_H
#define GEL_PHDR_H

#include <gel/common.h>
#include <gel/types.h>
#include <gel/file.h>
#include <gel/cursor.h>

__BEGIN_DECLS

/**
 * Structure used to get public information from a program header.
 * They may be obtained using the gel_prog_infos() function.
 * @ingroup		phdr
 */
typedef struct {
	gel_file_t *container;	/**< Container ELF file. */
	u32_t type;				/**< Type: one of PT_xxx values. */
	u32_t offset;			/**< Offset in the file. */
	vaddr_t vaddr;			/**< Address in virtual memory. */
	vaddr_t paddr;			/**< Address in physical memory. */
	u32_t memsz;			/**< Size in host memory. */
	u32_t flags;			/**< Flags as a combination of PF_xxx. */
	u32_t filesz;			/**< Segment size in the file. */
	u32_t align;			/**< Alignment of the segment. */
} gel_prog_info_t;


/**
 * Handle for a program header. The program headers may obtained by
 * calling the function @ref gel_getprogbyidx().
 * @ingroup		phdr
 */
typedef gel_prog_info_t gel_prog_t;


/**
 * Cursor for the content of @ref PT_NOTE program header.
 * @ingroup		phdr
 */
typedef struct gel_note_t {
	gel_cursor_t cur;
	char *name;
	u32_t type;
	gel_cursor_t desc;
	size_t size;
	int aligned;
} gel_note_t;


/* Primitives */
gel_prog_t *gel_getprogbyidx(gel_file_t *e, u16_t idx);
void gel_kill_prog(gel_prog_t *s);
#define gel_note_desc(c)	((c)->desc)
void gel_note_destroy(gel_note_t *curs);
int gel_note_init(gel_prog_t *phdr, gel_note_t *curs);
#define gel_note_name(c)	((c)->name)
int gel_note_next(gel_note_t *curs);
#define gel_note_size(c)	((c)->desc.size)
#define gel_note_type(c)	((c)->type)
int gel_prog_infos(gel_prog_t *s, gel_prog_info_t *pi);
int gel_prog_load(gel_prog_t *p, raddr_t addr);
int gel_prog2cursor(gel_prog_t *p, gel_cursor_t *c);

__END_DECLS

#endif /* PHDR_H */
