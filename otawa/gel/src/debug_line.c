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

#include <stdio.h>

#include <stdlib.h>
#if defined(__unix) || defined(__APPLE__)
#include <dlfcn.h>
#elif defined(__WIN32) || defined(__WIN64)
#include <windows.h>
#endif
#include <gel/dwarf_line.h>
#include <gel/stabs.h>
#include <gel/gel_elf.h>
#include "debug.h"
#include "gel_mem.h"

#define DWARF_DEBUG_LINE_NAME	".debug_line"

/**
 * @defgroup debug Generic Debugging Information
 * 
 * @code
 * #include <gel/debug_line.h>
 * @endcode
 *
 * This module provide access to the debugging information of an executable.
 * It replaces and complement the old DWARF module allowing transparent
 * access to any supported debugging format. For now, it only provides
 * DWARF access but will soon support STABS debugging information.
 * 
 * To use debugging line information one has first to build a linemap
 * once the executable has been opened:
 * @code
 * gel_line_map_t *map = gel_new_line_map(file);
 * if(map == NULL)
 * 		/* handle error here * /
 * @endcode
 * 
 * This function will automatically look for the available debugging
 * information and invoke the matching manager. If no debugging information
 * is found or for any error, it returns NULL (and error in @ref gel_errno).
 * 
 * Then, you can retrieve the address matching a source / line pair with:
 * @code
 * vaddr_t 	address = gel_address_from_line(map, "my_source.c", 666);
 * @endcode
 * 
 * Or do the inverse, getting source and line from an address (if any).
 * @code
 * const char *source;
 * int line;
 * if(gel_line_from_address(map, address, &source, &line) < 0)
 * 		/* there is an error ! * /
 * @endcode
 * 
 * You can also traverse the list of sorted by source / line information:
 * @code
 * dwarf_line_iter_t iter;
 * dwarf_location_t loc;
 * for(loc = dwarf_first_line(&iter, map); loc.file; loc = dwarf_next_line(&iter))
 * 		printf("low address=%08x, high address=%08x, line=%8d, file=%s\n",
 * 			loc.low_addr, loc.high_addr, loc.line, loc.file);
 * @endcode
 * Notice that, in this case, if the debugging information contains aliases
 * (same file name for different instructions blocks), ony one will be visible.
 * 
 * Or the list of available sorted by memory addresses:
 * @code
 * dwarf_addr_iter_t iter;
 * dwarf_location_t loc;
 * for(loc = dwarf_first_addr(&iter, map); loc.file; loc = dwarf_next_addr(&iter))
 * 		printf("low address=%08x, high address=%08x, line=%8d, file=%s\n",
 * 			loc.low_addr, loc.high_addr, loc.line, loc.file);
 * @endcode
 * 
 * Finally, the linemap may be fried with:
 * @code
 * 	gel_delete_line_map(map);
 * @endcode
 */

/**
 * Delete a line map.
 * @param map	Line map to delete.
 * @ingroup		debug
 */
void gel_delete_line_map(gel_line_map_t *map) {
	gel_hash_enum_t unit_enum;
	unit_t *unit;
	segment_t *seg, *next;
	assert(map);

	/* Free unit line tables */
	unit = (unit_t *)gel_hash_enum_init(&unit_enum, map->units);
	while(unit) {
		if(unit->lines)
			delete(unit->lines);
		unit = (unit_t *)gel_hash_enum_next(&unit_enum);
	}
	
	/* free the segments ? */
	for(seg = map->segs; seg; seg = next) {
		next = seg->next;
		delete(seg->locs);
		delete(seg);
	}

	/* Free the line map */
	hash_free(map->units);
	delete(map);
}


/**
 * Find the next available line.
 * @param iter	Iterator to work on.
 * @return		Found location.
 * @ingroup		debug
 */
static gel_location_t gel_find_line(gel_line_iter_t *iter) {
	int top;
	unit_line_t *line;
	gel_location_t loc = { 0 };
	while(iter->unit) {
		while(iter->line <= iter->unit->high) {
			line = &iter->unit->lines[iter->line - iter->unit->low];
			if(line->unit != NULL && line->seg != NULL) {

				// compute top position
				// source line of macros may span several segments: avoid this
				top = (line->high - line->seg->low) / iter->map->step;
				int max_top = (line->seg->high - line->seg->low) / iter->map->step;
				if(top > max_top)
					top = max_top;

				// prepare location
				loc.file = iter->unit->name;
				loc.line = iter->line;
				
				// find the low address
				if(iter->loc < 0)
					iter->loc = (line->low - line->seg->low) / iter->map->step;
				else {
					while(iter->loc < top && line->seg->locs[iter->loc] != line) iter->loc++;
					if(iter->loc >= top) {
						iter->line++;
						iter->loc = -1;
						continue;
					}
				}
				loc.low_addr = iter->loc * iter->map->step + line->seg->low;
				
				// find the high address
				while(iter->loc < top && line->seg->locs[iter->loc] == line) iter->loc++;
				if(iter->loc >= top)
					loc.high_addr = line->high;
				else
					loc.high_addr = iter->loc * iter->map->step + line->seg->low;
				
				return loc;
			}
			iter->line++;
		}
		iter->unit = (unit_t *)gel_hash_enum_next(&iter->hen);
		if(iter->unit)
			iter->line = iter->unit->low;
	}
	return loc;
}


/**
 * Initialize the iterator on the lines.
 * @param iter	Iterator to use.
 * @param map	Map to iterate on.
 * @return		First location or an empty file location if there is no line.
 * @ingroup		debug
 */
gel_location_t gel_first_line(
	gel_line_iter_t *iter,
	gel_line_map_t *map)
{
	iter->map = map;
	iter->unit = (unit_t *)gel_hash_enum_init(&iter->hen, map->units);
	if(iter->unit) {
		iter->line = iter->unit->low;
		iter->loc = -1;
	}
	return gel_find_line(iter);
}


/**
 * Go to the next line.
 * @param iter	Used iterator.
 * @return		Next location or an empty file location if there is no more line.
 * @ingroup		debug
 */
gel_location_t gel_next_line(gel_line_iter_t *iter) {
	return gel_find_line(iter);
}


/**
 * Start an iteration on the debug units.
 * @param map	Line map to debug with.
 * @param iter	Unit iterator (intialized by this call).
 * @return		Found first unit or null (if there is no unit).
 */
gel_unit_t *gel_unit_first(gel_line_map_t *map, gel_unit_iter_t *iter) {
	iter->map = map;
	iter->unit = (unit_t *)gel_hash_enum_init(&iter->hen, map->units);
	return iter->unit;
}


/**
 * Get the next unit of the iterator.
 * @param iter		Unit iterator.
 * @return			Next unit or null for end of iteration.
 */
gel_unit_t *gel_unit_next(gel_unit_iter_t *iter) {
	iter->unit = (unit_t *)gel_hash_enum_next(&iter->hen);
	return iter->unit;
}


/**
 * Get the name of a unit.
 * @param unit	Unit to get name for.
 * @return		Unit name.
 */
const char *gel_unit_name(gel_unit_t *unit) {
	return unit->name;
}


/**
 * Start an iteration on the lines of the unit.
 * @param unit	Unit iterator.
 * @param iter	Line iterator.
 * @return		First found location.
 */
gel_location_t gel_unit_to_line_iter(gel_unit_iter_t *unit, gel_line_iter_t *iter) {
	gel_hash_enum_end(&iter->hen, unit->hen.htab);
	iter->unit = unit->unit;
	iter->line = iter->unit->low;
	iter->loc = -1;
	return gel_find_line(iter);
}


/**
 * Find the nearest address.
 * @param iter	Current iteration.
 * @return		Next address or an empty file location at end.
 */
static gel_location_t gel_find_addr(gel_addr_iter_t *iter) {
	unit_line_t *line;
	gel_location_t loc = { 0 };
	int cnt;
	assert(iter);
	while(iter->seg) {
		cnt = (iter->seg->high - iter->seg->low) / iter->map->step;
		while(iter->loc < cnt) {
			if(iter->seg->locs[iter->loc]) {
				line = iter->seg->locs[iter->loc];
				assert(line->unit);
				
				/* build the location */
				loc.file = line->unit->name;
				loc.line = line - line->unit->lines + line->unit->low;
				loc.low_addr = iter->seg->low + iter->loc * iter->map->step;
				loc.high_addr = iter->seg->low + (iter->loc + 1) * iter->map->step;
				
				/* find the high address */
				for(; iter->loc + 1 < cnt && iter->seg->locs[iter->loc + 1] == line; iter->loc++);
				if(iter->loc + 1 >= cnt)
					loc.high_addr = iter->seg->high;
				else
					loc.high_addr = iter->seg->low + (iter->loc + 1) * iter->map->step;
	
				return loc;
			}
			else
				iter->loc++;
		}
		iter->seg = iter->seg->next;
		iter->loc = 0;
	}
	return loc;
}


/**
 * Initialize the given iterator on the first address.
 * @param iter	Iterator to initialize.
 * @param map	Line map to use.
 * @return		Return the first location or an empty file location if there
 * 				is no address.
 * @ingroup		debug
 */
gel_location_t gel_first_addr(
	gel_addr_iter_t *iter,
	gel_line_map_t *map)
{
	iter->map = map;
	iter->seg = map->segs;
	iter->loc = 0;
	return gel_find_addr(iter);
}


/**
 * Get the next location.
 * @param iter	Current address iterator.
 * @return		Next address or empty file location.
 * @ingroup		debug
 */
gel_location_t gel_next_addr(gel_addr_iter_t *iter) {
	iter->loc++;
	return gel_find_addr(iter);
}


/**
 * Get the actual address of a source line.
 * @param map	Line map to use.
 * @param file	File name.
 * @param line	Line number.
 * @return		Matching address or null if no address is found.
 * @ingroup		debug
 */
vaddr_t gel_address_from_line(
	gel_line_map_t *map,
	const char *file,
	int line)
{
	unit_t *unit;
	assert(map);
	assert(file);

	/* Find the unit */
	unit = hash_get(map->units, (char *)file);
	if(!unit)
		return 0;

	/* check line interval */
	if(line < unit->low || line > unit->high)
				return 0;

	/* Find the address */
	return unit->lines[line - unit->low].low;
}


/**
 * Get the file and line matching an actual binary address.
 * @param map	Used map.
 * @param addr	Looked address.
 * @param file	Returned file.
 * @param line	Returned line.
 * @return		0 for success, -1 if not found.
 * @ingroup		debug
 */
int gel_line_from_address(
	gel_line_map_t *map,
	vaddr_t addr,
	const char **file,
	int *line)
{
	int i;
	segment_t *seg;
	assert(map);
	assert(file);
	assert(line);
	for(seg = map->segs; seg; seg = seg->next)
			if(addr >= seg->low && addr < seg->high) {
				i = (addr - seg->low) / map->step;
				if(!seg->locs[i])
					return -1;
				*file = seg->locs[i]->unit->name;
				*line = seg->locs[i] - seg->locs[i]->unit->lines + seg->locs[i]->unit->low;
				return 0;
			}
	return -1;
}


/**
 * Initialize the segments part of line map.
 * @param file	GEL file to work with.
 * @param map	Line map to initialize.
 * @param step	Step in bytes between each line.
 * @return		0 for success, -1 else (error in gel_errno).
 */
int gel_init_line_map_segs(gel_file_t *file, gel_line_map_t *map, int step) {
	int i;
	segment_t *seg;
	map->segs = NULL;

	// Make list of segments
	for(i = 0; i < file->prognum; i++) {
		gel_prog_t *prog = gel_getprogbyidx(file, i);
		if(prog->type == PT_LOAD
		&& prog->flags & PF_X) {

			/* look a concatenable segment */
			for(seg = map->segs; seg; seg = seg->next)
				if(seg->high == prog->vaddr) {
					seg->high = prog->vaddr + prog->memsz;
					break;
				}
			if(seg)
				continue;

			/* create a new one */
			seg = new(segment_t);
			if(!seg) {
				gel_errno = GEL_ERESOURCE;
				return -1;
			}
			seg->next = map->segs;
			map->segs = seg;
			seg->low = prog->vaddr;
			seg->high = prog->vaddr + prog->memsz;
			seg->locs = NULL;
		}
	}

	// Allocate locations
	for(seg = map->segs; seg; seg = seg->next) {
		int n = (seg->high - seg->low) / step;
		seg->locs = newvc(unit_line_t *, n);
		if(!seg->locs) {
			gel_errno = GEL_ERESOURCE;
			return -1;
		}
	}
	
	// it works !
	return 0;
}


typedef gel_line_map_t *(*cons_t)(gel_file_t *, gel_sect_t *);


/**
 * Open the debug source line information.
 * @param file	File to work on.
 * @return		Created line map for file or NULL (error in gel_errno).
 * @ingroup		debug
 */
gel_line_map_t *gel_new_line_map(gel_file_t *file) {
	gel_sect_t *section;
	cons_t cons;
	gel_line_map_t *map;
	
	/* look for DWARF */
	section = gel_getsectbyname(file, DWARF_DEBUG_LINE_NAME);
	if(section)
		return dwarf_new_line_map(file, section);

	/* look for STABS */
	else {
		map = stabs_new_line_map(file);
		if(map)
			return map;
	}
	
	/* not found */
	gel_errno = GEL_ENOTFOUND;
	return NULL;
}


/**
 * @typedef gel_location_t
 * This structure is used to return a debugging line information
 * made of a source file, a line, a starting and an ending address.
 * @ingroup debug
 */


/**
 * @var gel_location_t::file
 * Source file path of the current line information.
 * This is the path has found in the line block,
 * relative or absolute, possibly from another file system:
 * GEL does not do any processing on it.
 */

/**
 * @var gel_location_t::line
 * Source line of the current line block.
 */

/**
 * @var gel_location_t::low_addr
 * Low addres of the current line block.
 */

/**
 * @var gel_location_t::high_addr
 * High address of the current line block, that is, one more than
 * the address of the last block byte.
 */

/**
 * @macro gel_isa(l)
 * Get the ISA of the current line location (meaning of this code
 * depends on the architecture ABI).
 * @param l	Line location.
 * @return	ISA.
 * @ingroup		debug
 */

/**
 * @macro gel_is_stmt(l)
 * Test if the current line location is the start of a statement.
 * @param l		Line location.
 * @return		True if it is statement begin, false else.
 * @ingroup		debug
 */
 
/**
 * @macro gel_basic_block(l)
 * Test if the current line location is the start of a basic block.
 * @param l		Line location.
 * @return		True if it is basic block begin, false else.
 * @ingroup		debug
 */

/**
 * @macro gel_prologue_end(l)
 * Test if the current line location is the end of a function prologue.
 * @param l		Line location.
 * @return		True if it is a prologue end, false else.
 * @ingroup		debug
 */

/**
 * @macro gel_epilogue_begin(l)
 * Test if the current line location is the start of a function epilogue.
 * @param l		Line location.
 * @return		True if it is a end of epilogue, false else.
 * @ingroup		debug
 */

