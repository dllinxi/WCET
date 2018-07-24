/*
 * $Id$
 *
 * GEL is part of the GEL library.
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

/*
 * Documentation
 * http://sourceware.org/gdb/onlinedocs/stabs.html
 * http://www.google.com/url?sa=t&source=web&cd=11&ved=0CBgQFjAAOAo&url=http%3A%2F%2Fdevelopers.sun.com%2Fsunstudio%2Fdocumentation%2Fss11%2Fstabs.pdf&rct=j&q=stabs%20documentation&ei=j7akTaHPJIag8QOG_c25Dw&usg=AFQjCNEsgOIHxuGAHtRbjgOqWkGo42KFdA&cad=rja
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <gel/stabs.h>
#include <gel/debug_line.h>
#include <gel/stabs.h>
#include "../src/debug.h"
#include "gel_mem.h"

/* STABS handler */
struct stabs_t {
	int cnt;
	stab_t *items;
	char *str;
};

/* STAB structure in ELF file */
typedef struct elf_stab_t {
	uint32_t	n_strx;		/* file String table index */
	uint8_t		n_type;		/* Stab type */
	int8_t		n_other;	/* used by N_SLINE stab */
	int16_t		n_desc;		/* Desc value */
	uint32_t	n_value;	/* Offset or value */
} elf_stab_t;


/* source entry */
typedef struct linemap_entry_t {
	struct linemap_entry_t *next;
	const char *file;
	int fst_line, lst_line;
	uint32_t low_addr, high_addr;
} linemap_entry_t;


/**
 * Open the STABS section of the given file.
 * @param file	File to look in.
 * @return		STABS handler or null if there is an error.
 */
stabs_t *stabs_new(gel_file_t *file) {
	assert(file);
	gel_sect_t *sect, *str;
	gel_sect_info_t sect_si, str_si;
	stabs_t *stabs;
	gel_cursor_t curs;
	stab_t *stab;

	/* look for STABS section */
	sect = gel_getsectbyname(file, STABS_SECT_STAB);
	if(sect == NULL) {
		gel_errno = STABS_ERR_NONE;
		return NULL;
	}
	if(gel_sect_infos(sect, &sect_si) < 0)
		return NULL;

	/* look for STABS string section */
	str = gel_getsectbyidx(file, sect_si.link);
	if(str == NULL) {
		gel_errno = STABS_ERR_NOSTR;
		return NULL;
	}
	if(gel_sect_infos(str, &str_si) < 0)
		return NULL;

	/* allocate and initialize STABS handler */
	stabs = (stabs_t *)newv(char, sizeof(stabs_t) + str_si.size + sect_si.size / sizeof(elf_stab_t) * sizeof(stab_t));
	if(stabs == NULL) {
		gel_errno = STABS_ERR_NORES;
		return NULL;
	}
	stabs->cnt = sect_si.size / sizeof(elf_stab_t);
	stabs->items = (stab_t *)(stabs + 1);
	stabs->str = (char *)(stabs->items + stabs->cnt);

	/* load string section */
	if(gel_sect_load(str, stabs->str) < 0) {
		delete(stabs);
		return NULL;
	}

	/* load the STABs */
	stab = stabs->items;
	if(gel_sect2cursor(sect, &curs) < 0) {
		delete(stabs);
		return NULL;
	}
	while(!gel_cursor_at_end(curs)) {
		stab->n_strx = stabs->str + gel_read_u32(curs);
		stab->n_type = gel_read_u8(curs);
		stab->n_other = gel_read_s8(curs);
		stab->n_desc = gel_read_u16(curs);
		stab->n_value = gel_read_u32(curs);
		stab++;
	}

	/* success */
	return stabs;
}

/**
 * Delete a STABS handler.
 * @param stabs	STABS handler to delete.
 */
void stabs_delete(stabs_t *stabs) {
	assert(stabs);
	delete(stabs);
}


/**
 * Get the count of items in STABS.
 * @param stabs		STABS handle.
 * @return			Count of items.
 */
int stabs_count(stabs_t *stabs) {
	assert(stabs);
	return stabs->cnt;
}

/**
 * Get the item at the given index.
 * @param stabs		STABS handle.
 * @param i			Index of the item.
 * @return			Item matching the index.
 */
stab_t *stabs_item(stabs_t *stabs, int i) {
	assert(stabs);
	assert(i >= 0);
	assert(i < stabs->cnt);
	return &stabs->items[i];
}


/**
 * Compute quickly a GCD.
 * @param a		First value.
 * @param b		Second value.
 * @return		GCD of a and b
 */
static int fast_gcd(int a, int b){
	int i,j;
	assert(a > 0);
	assert(b > 0);
	for(i = 0; !(a & 1); i++)
		a >>= 1;
	for(j = 0; !(b & 1); j++)
		b >>= 1;

	while(b != a){
        b -= a;
        if(b < 0){
            a += b;
            b = -b;
        }
        while(!(b & 1))
            b >>= 1;
    }
    return a << ((i < j) ? i : j);
}


/**
 * Build a line map that allows to map source:lines
 * with addresses.
 * @param file		File to get linemap from.
 * @param stabs		Current stabs.
 * @return			Created linemap or NULL if there is a problem (error in gel_errno).
 * @warning			Stabs must not be deleted while the returned line map exists.
 */
gel_line_map_t *stabs_make_linemap(gel_file_t *file, stabs_t *stabs) {
	gel_line_map_t *map;
	stab_t *last;
	int i;
	vaddr_t base_addr = 0, addr;
	unit_t *unit = 0;
	assert(stabs);
	uword base_line = 0, line;
	gel_hash_enum_t unit_enum;
	segment_t *seg;

	/* allocate and initialize */
	map =new(gel_line_map_t);
	if(map == NULL) {
		gel_errno = STABS_ERR_NORES;
		return NULL;
	}
	map->units = gel_hash_new(63);
	map->step = 0;

	/* compute steps and line size */
	last = NULL;
	for(i = 0; i < stabs->cnt; i++) {
		/*fprintf(stderr, "DEBUG: %s, type=%02x, other=%d, desc=%d, value=%08x\n",
			stabs->items[i].n_strx,
			stabs->items[i].n_type,
			stabs->items[i].n_other,
			stabs->items[i].n_desc,
			stabs->items[i].n_value);*/
		switch(stabs->items[i].n_type) {

		/* new file */
		case N_SO:
		case N_SOL:
			unit = hash_get(map->units, (char *)stabs->items[i].n_strx);
			if(!unit) {
				unit = newc(unit_t);
				unit->low = (uword)-1;
				unit->high = 0;
				unit->name = strdup(stabs->items[i].n_strx);
				hash_put(map->units, (char *)unit->name, unit);
			}
			base_addr = 0;
			/*fprintf(stderr, "=> UNIT: %s (base = %d)\n", unit->name, stabs->items[i].n_desc);*/
			base_line = stabs->items[i].n_desc;
			/*fprintf(stderr, "BASE LINE=%d\n", base_line);*/
			last = NULL;
			break;

		/* new function */
		case N_FUN:
			if(stabs->items[i].n_strx[0]) {
				base_addr = stabs->items[i].n_value;
				/*fprintf(stderr, "FUNCTION %s\n", stabs->items[i].n_strx);*/
				last = NULL;
				break;
			}
			else
				goto compute_step;

		/* new line */
		case N_SLINE:
			if(unit) {
				line = base_line + stabs->items[i].n_desc;
				if(line < unit->low)
					unit->low = line;
				if(line > unit->high)
					unit->high = line;
				/*fprintf(stderr, " => LINE %d => [%d, %d]\n", line, unit->low, unit->high);*/
			}
		
		/* compute step */
		compute_step:
			if(last) {
				uword new_step = stabs->items[i].n_value - last->n_value;
				if(!map->step)
					map->step = new_step;
				else if(new_step)
					map->step = fast_gcd(map->step, new_step);
			}
			last = &stabs->items[i];
			break;
		}
	}

	/* allocate the segments */
	if(gel_init_line_map_segs(file, map, map->step) < 0) {
		gel_delete_line_map(map);
		return 0;
	}
	
	/* allocate memory in units */
	unit = (unit_t *)gel_hash_enum_init(&unit_enum, map->units);
	while(unit) {
		unit->lines = newvc(unit_line_t, unit->high - unit->low + 2);
		unit = (unit_t *)gel_hash_enum_next(&unit_enum);
	}

	/* build the entries */
	last = NULL;
	base_addr = 0;
	base_line = 0;
	unit = NULL;
	for(i = 0; i < stabs->cnt; i++) {
		switch(stabs->items[i].n_type) {

		/* new file */
		case N_SO:
		case N_SOL:
			unit = hash_get(map->units, (char *)stabs->items[i].n_strx);
			assert(unit);
			/*fprintf(stderr, "UNIT: %s\n", unit->name);*/
			base_addr = 0;
			base_line = stabs->items[i].n_desc;
			last = NULL;
			break;

		/* new function */
		case N_FUN:
			assert(stabs->items[i].n_strx);
			if(stabs->items[i].n_strx[0]) {
				/*fprintf(stderr, "FUN: %s (%d, %08x)\n", stabs->items[i].n_strx, base_line, base_addr);*/
				base_addr = stabs->items[i].n_value;
				last = NULL;
			}
			break;

		case N_SLINE:
		last_line:
			if(unit && last) {
				vaddr_t low, high, addr;
				line = base_line + last->n_desc;
				low = base_addr + last->n_value;
				high = base_addr + stabs->items[i].n_value;
				/*fprintf(stderr, "LINE: %d [%08x-%08X]\n", line, low, high);*/
				assert(unit->low <= line && line <= unit->high);
				unit->lines[line - unit->low].low = low;
				unit->lines[line - unit->low].high = high;
				unit->lines[line - unit->low].unit = unit;
				for(seg = map->segs; seg ; seg = seg->next) {
					if(seg->low <= low && low < seg->high) {
						vaddr_t top = high;
						if(high > seg->high)
							top = seg->high;
						for(addr = low; addr < top; addr += map->step) {
							seg->locs[(addr - seg->low) / map->step] = &unit->lines[line - unit->low];
							unit->lines[line - unit->low].seg = seg;
						}
					}
				}
			}
			last = &stabs->items[i];
			break;
		}
	}

	/* return result */
	return map;
}


/**
 * Build the line information.
 * @param file	File containing the section.
 * @return		Read line map or null for an error.
 * gel_errno errors include:
 * @li @ref GEL_ENOTFOUND -- no .debug_line section available.
 * @li @ref GEL_EFORMAT -- bad format.
 * @li @ref GEL_ERESOURCE -- not enough memory.
 * @ingroup		stabs
 */
gel_line_map_t *stabs_new_line_map(gel_file_t *file) {
	stabs_t *stabs;
	gel_line_map_t *map;
	
	/* build the stabs array */
	stabs = stabs_new(file);
	if(stabs == NULL)
		return NULL;

	/* build the map */
	map = stabs_make_linemap(file, stabs);

	/* finally, return the map */
	stabs_delete(stabs);
	return map;
}



