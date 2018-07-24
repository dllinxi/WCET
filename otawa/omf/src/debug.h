/*
 * $Id$
 * Copyright (c) 2011, IRIT- UPS
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
#ifndef GEL_DEBUG_H
#define GEL_DEBUG_H

#include <gel/common.h>
#include <gel/gel.h>
#include <gel/util.h> 
#include <stdint.h>

__BEGIN_DECLS

/* Internal types */
typedef int32_t word;
typedef uint32_t uword;
typedef int16_t half;
typedef uint16_t uhalf;
#if !defined(__WIN32) && !defined(__WIN64)
	typedef int8_t byte;
#endif
typedef uint8_t ubyte;

/* unit_line_t type */
typedef struct unit_line_t {
	struct unit_t *unit;
	struct segment_t *seg;
	vaddr_t low, high;
	uhalf flags;
	uhalf isa;
} unit_line_t;


/* unit_t type */
typedef struct unit_t {
	const char *name;
	uword length;
	uword low, high;
	unit_line_t *lines;
} unit_t;


/* segment_t type  */
typedef struct segment_t {
	struct segment_t *next;
	vaddr_t low, high;
	unit_line_t **locs;
} segment_t;


/* dwarf_line_map_t type */
struct gel_line_map_t {
	gel_hash_t units;
	vaddr_t base;
	uword step;
	segment_t *segs;
};

/* functions */
int gel_init_line_map_segs(gel_file_t *file, gel_line_map_t *map, int step);

__END_DECLS

#endif	/* GEL_DEBUG_H */
