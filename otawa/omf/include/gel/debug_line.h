/*
 * $Id$
 * Copyright (c) 2011, IRIT- UPS
 * 
 * GEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GEL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GEL; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GEL_DEBUG_LINE_H
#define GEL_DEBUG_LINE_H

#include <stdint.h>
#include <gel/common.h>
#include <gel/util.h>
#include <gel/gel.h>

__BEGIN_DECLS

/* flags */
#define GEL_IS_STMT			0x0001
#define GEL_BASIC_BLOCK		0x0002
#define GEL_PROLOGUE_END	0x0004
#define GEL_EPILOGUE_BEGIN	0x0008
#define GEL_END_SEQUENCE	0x8000

/* gel_line_map_t type */
typedef struct gel_line_map_t gel_line_map_t;
typedef struct unit_t gel_unit_t;

/* gel_line_iter_t type */
typedef struct gel_line_iter_t {
	gel_hash_enum_t hen;
	struct unit_t *unit;
	int line, loc;
	gel_line_map_t *map;
} gel_line_iter_t;

/* gel_addr_iter_t type */
typedef struct gel_addr_iter_t {
	gel_line_map_t *map;
	struct segment_t *seg;
	int loc;
} gel_addr_iter_t;

/* gel_location_t type */
typedef struct gel_location_t {
	const char *file;
	uint32_t line;
	vaddr_t low_addr, high_addr;
	uint16_t flags;
	uint16_t isa;
} gel_location_t;

/* unit iterator */
typedef struct gel_unit_iter_t {
	gel_hash_enum_t hen;
	struct unit_t *unit;
	gel_line_map_t *map;
} gel_unit_iter_t;

/* management */
gel_line_map_t *gel_new_line_map(gel_file_t *file);
void gel_delete_line_map(gel_line_map_t *map);
int gel_line_from_address(gel_line_map_t *map, vaddr_t addr, const char **file, int *line);
vaddr_t gel_address_from_line(gel_line_map_t *map, const char *file, int line);

/* unit accessor */
gel_unit_t *gel_unit_first(gel_line_map_t *map, gel_unit_iter_t *iter);
gel_unit_t *gel_unit_next(gel_unit_iter_t *iter);
const char *gel_unit_name(gel_unit_t *unit);
gel_location_t gel_unit_to_line_iter(gel_unit_iter_t *unit, gel_line_iter_t *line);

/* location accessor */
#define gel_isa(l)				(l.isa)
#define gel_is_stmt(l)			(l.flags & GEL_IS_STMT)
#define gel_basic_block(l)		(l.flags & GEL_BASIC_BLOCK)
#define gel_prologue_end(l)		(l.flags & GEL_PROLOGUE_END)
#define gel_epilogue_begin(l)	(l.flags & GEL_EPILOGUE_BEGIN)

/* Iterators */
gel_location_t gel_first_line(gel_line_iter_t *iter, gel_line_map_t *map);
gel_location_t gel_next_line(gel_line_iter_t *iter);
gel_location_t gel_first_addr(gel_addr_iter_t *iter, gel_line_map_t *map);
gel_location_t gel_next_addr(gel_addr_iter_t *iter);

__END_DECLS

#endif	/* GEL_DEBUG_LINE_H */
