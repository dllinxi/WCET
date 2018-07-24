/*
 * $Id$
 * Copyright (c) 2005-07, IRIT- UPS
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
 
#ifndef GEL_DWARF_LINE_H
#define GEL_DWARF_LINE_H

/**
 * @file
 * Provide access to the Dwarf line section.
 */

#include <gel/common.h>
#include <gel/gel.h>
#include <gel/debug_line.h>


__BEGIN_DECLS

/* Strings */
#define DWARF_DEBUG_LINE_NAME	".debug_line"


/* Line types (for compatibility with older versions) */
typedef struct gel_line_map_t	dwarf_line_map_t;
typedef struct gel_line_iter_t	dwarf_line_iter_t;
typedef struct gel_addr_iter_t	dwarf_addr_iter_t;
typedef struct gel_location_t	dwarf_location_t;


/* Line primitives */
dwarf_line_map_t *dwarf_new_line_map(gel_file_t *file, gel_sect_t *section);
#define dwarf_delete_line_map(map)						gel_delete_line_map(map)
#define dwarf_line_from_address(map, addr, file, line)	gel_line_from_address(map, addr, file, line)
#define dwarf_address_from_line(map, file, line)		gel_address_from_line(map, file, line)

/* Iterators */
#define dwarf_first_line(iter, map)		gel_first_line(iter, map)
#define dwarf_next_line(iter)			gel_next_line(iter)
#define dwarf_first_addr(iter, map)		gel_first_addr(iter, map)
#define dwarf_next_addr(iter)			gel_next_addr(iter)

__END_DECLS

#endif // GEL_DWARF_LINE_H
