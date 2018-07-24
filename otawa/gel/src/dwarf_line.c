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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gel/util.h>
#include <gel/prog.h>
#include "gel/dwarf_line.h"
#include <gel/gel_elf.h>
#include  <gel/error.h>
#include "debug.h"
#include "gel_mem.h"
#if defined(__WIN32) || defined(__WIN64)
#	include <windows.h>
	typedef unsigned int uint;
#elif defined(__APPLE__)
	typedef unsigned int uint;
#endif

/**
 * @defgroup dwarf Dwarf Debugging Information
 *
 * This module provide access to the DWARF debugging information.
 * Currently, only access to the map between the binary and the sources
 * is provided.
 *
 * To get such an information, you have first to get the line map :
 * @code
 * #include <gel/dwarf_line.h>
 * dwarf_line_map_t *map = dwarf_new_line_map(GEL_file, containing_section);
 * ...
 * dwarf_delete_line_map(map);
 * @endcode
 * If no containing section is given, the default section called ".debug_line"
 * is used. dwarf_delete_line_map is used to release memory occupied by the
 * line map.
 *
 * If you have an address or a source file / line pair, you may get the
 * corresponding information using the following functions :
 * @code
 * int dwarf_line_from_address(dwarf_line_map_t *map, vaddr_t addr,
 *	const char **file, int *line);
 * vaddr_t dwarf_address_from_line(dwarf_line_map_t *map, const char *file, int line);
 * @endcode
 * The first one gives the start address of the code matching the given file
 * name and line. The second one is used to get the line and the source file
 * matching the given address.
 *
 * You may also traverse all available lines :
 * @code
 * dwarf_line_iter_t iter;
 * dwarf_location_t loc;
 * for(loc = dwarf_first_line(&iter, map); loc.file;
 *	loc = dwarf_next_line(&iter))
 *		...
 * @endcode
 *
 * Or all available code blocks :
 * @code
 * dwarf_addr_iter_t iter;
 * dwarf_location_t loc;
 * for(loc = dwarf_first_addr(&iter, map); loc.file;
 * loc = dwarf_next_addr(&iter))
 *		...
 * @endcode
 *
 * The dwarf_location_t type provides usual information :
 * @li file -- source file,
 * @li line -- line,
 * @li low_addr -- low address of the block,
 * @li high_addr -- high address of the block.
 */

/**
 * @typedef dwarf_line_map_t;
 * Type representing a code / line map.
 * @ingroup dwarf
 */

/**
 * @struct dwarf_line_iter_t;
 * An iterator used to traverse the source / line pairs of the map.
 * @ingroup dwarf
 */

/**
 * @struct dwarf_addr_iter_t;
 * An iterator used to traverse the code blocks of the map.
 * @ingroup dwarf
 */

/**
 * @struct dwarf_location_t;
 * A location provides an entry in the map.
 * @ingroup dwarf
 */


/* Trace configuration */
/*#define TRACE_LINES*/
/*#define TRACE_LOCAL*/

/* Trace macros */
#if defined(NDEBUG) || !defined(TRACE_LINES)
#	define TLINE(c)
#	define ITRACE(c)
#	define OTRACE(c)
#else
#	define TLINE(c) c
#	define ITRACE(c)	TLINE(printf("\t%08x:%02d:%03d: ", address, file, line); c)
#	define OTRACE(c)	TLINE(printf("%08x: ", cur.cur_addr - 1 - cur.base_addr); c)
#endif
#if defined(NDEBUG) || !defined(TRACE_LOCAL)
#	define LTRACE(c)
#else
#	define LTRACE(c)	c
#endif


// Standard opcodes
#define DW_LNS_copy					1
#define DW_LNS_advance_pc			2
#define DW_LNS_advance_line			3
#define DW_LNS_set_file				4
#define DW_LNS_set_column			5
#define DW_LNS_negate_stmt			6
#define DW_LNS_set_basic_block		7
#define DW_LNS_const_add_pc			8
#define DW_LNS_fixed_advance_pc		9
#define DW_LNS_set_prologue_end		10		/* DWARF-3 */
#define DW_LNS_set_epilogue_begin	11		/* DWARF-3 */
#define DW_LNS_set_isa				12		/* DWARF-3 */


// Extended opcodes
#define DW_LNE_end_sequence			1
#define DW_LNE_set_address			2
#define DW_LNE_define_file			3
#define DW_LNE_set_discriminator	4		/* DWARF-4 */

/*
 * DATASTRUCTURE DESCRIPTION
 * 
 * line map
 * 	- units: hashtable of units (indexed by file name)
 * 	- segs: null-ended list of segment of the executable
 * 	- step: address of items in segments
 * 
 * segment
 * 	- min, max: minimal and maximal address of the segment
 * 	- locs: array of entries matching a line
 * 		index of entry matching a = (a - min) / ste
 * 		each points to a line descriptor
 * 
 * unit:
 * 	- low, high: low and high line numbers
 *  - lines: array of unit lines
 * 		index of descriptor of line l = l - low
 * 
 * line descriptor:
 * 	- unit: back pointer to owner unit
 * 	- seg: owner segment
 * 	- low, high: low and high address of code matching the line
 * 		This interval may be convex (interleft with other line according to
 * 		the compiler transformations).
 * 	finding back the line number: (descriptor address - descriptor array address) / descriptor_size + unit low number
 * 
 * DEVELOPMENT NOTE
 * 		Segment entry array should be reduced by determining the code step
 * 		from the minimal step in the read line matrix (instead of using the minimimum_instruction_size
 * 		from the .debug_line header).
 */

/*** internal read structure ***/

/* address_tree_t structure */
typedef struct chunk_t {
	struct chunk_t *next;
	char *cur;
	char buffer[0];
} chunk_t;
typedef struct pool_t {
	int size;
	chunk_t *head, *tail;
} pool_t;


/* line_t structure */
typedef struct line_t {
	unit_t *file;
	int line;
	vaddr_t addr;
	uhalf flags;
	uhalf isa;
} line_t;


/* expand_t type */
typedef struct expand_t {
	int cnt, max, size;
	void *tab;
} expand_t;

#define EXPAND_CNT(exp) ((exp).cnt)
#define EXPAND_GET(t, exp, i) (((t *)((exp).tab))[i])
#define EXPAND_ADD(t, exp, v) \
	{ \
		if((exp).cnt >= (exp).max && expand_expand(&(exp), (exp).max * 2)) \
			return GEL_ERESOURCE; \
		EXPAND_GET(t, exp, (exp).cnt) = v; \
		(exp).cnt++; \
	}


/* local_t type */
typedef struct local_t {
	uword total_length;
	uhalf version;
	uword header_length;
	ubyte minimum_instruction_length;
	ubyte maximum_operations_per_instruction;		/* DWARF-4 */
	ubyte default_is_stmt;
	byte line_base;
	ubyte line_range;
	ubyte opcode_base;
	ubyte *standard_opcode_lengths;
	expand_t include_directories;
	expand_t file_names;
	pool_t lines;
	vaddr_t min_addr, max_addr, addr_set;
	gel_hash_t file_htab;
} local_t;


/**
 * Init an expandable array.
 * @param expand	Expandable array to init.
 * @param size		Size of items in the array.
 * @param max		Maximal initial size.
 * @return			0 success, -1 failure.
 */
static int expand_init(expand_t *expand, int size, int max) {
	expand->cnt = 0;
	expand->max = max;
	expand->size = size;
	expand->tab = newv(char, size * max);
	return expand->tab ? 0 : -1;
}


/**
 * Expand the size of the expandable array to the given new maximum.
 * @param expand	Expandable array.
 * @param new_max	New maximal size.
 * @return			0 for success, -1 if there is no more memory.
 */
static int expand_expand(expand_t *expand, int new_max) {
	void *new_mem = realloc(expand->tab, new_max * expand->size);
	if(!new_mem)
		return -1;
	else {
		expand->max = new_max;
		expand->tab = new_mem;
		return 0;
	}
}


/**
 * Free an expandable table.
 * @param expand	Expandle array to free.
 */
static void expand_free(expand_t *expand) {
	delete(expand->tab);
}


/**
 * Reset the expandable table to an empty one.
 * @param expand	Expandable table to reset.
 */
static void expand_reset(expand_t *expand) {
	expand->cnt = 0;
}


/**
 * Add a chunk to a pool.
 * @param pool	Pool to work on.
 * @return		0 for success, -1 for an error.
 */
static int pool_add(pool_t *pool) {
	chunk_t *chunk;

	/* allocate it */
	chunk = (chunk_t *)newv(char, sizeof(chunk_t) + pool->size);
	if(!chunk)
		return -1;

	/* initialize it */
	chunk->next = NULL;
	chunk->cur = chunk->buffer + pool->size;

	/* link it */
	if(!pool->tail)
		pool->head = pool->tail = chunk;
	else {
		pool->tail->next = chunk;
		pool->tail = chunk;
	}
	return 0;
}


/**
 * Initialize a pool.
 * @param pool	Pool to initialize.
 * @param size	Size of chunks.
 * @return		0 for success, -1 for error.
 */
static int pool_init(pool_t *pool, int size) {
	pool->size = size;
	pool->head = 0;
	pool->tail = 0;
	return pool_add(pool);
}


/**
 * Allocate memory in a pool.
 * @param pool	Pool to allocate in.
 * @param size	Allocatted block size.
 * @return		Allocated block or null if there is no more memory.
 */
static void *pool_alloc(pool_t *pool, int size) {
	char *result = pool->tail->cur - size;
	if(result < pool->tail->buffer) {
		if(pool_add(pool))
			return 0;
		result = pool->tail->cur - size;
	}
	pool->tail->cur = result;
	return result;
}


/**
 * Free memory allocated by a pool. Do not use it after this call.
 * @param pool	Pool to destroy.
 */
static void pool_destroy(pool_t *pool) {
	chunk_t *cur, *next;
	for(cur= pool->head; cur; cur = next) {
		next = cur->next;
		delete(cur);
	}
}


/**
 * Decode a LEB128 unsigned integer. On return, set the @ref gel_errno to
 * @ref GEL_ESUCCESS or to @ref GEL_EFORMAT if the integer is truncated.
 * @param cur	Cursor on the read bytes.
 * @return		Unsigned integer read.
 */
static uword leb128_read_unsigned(gel_cursor_t *cur) {
	gel_errno = GEL_ESUCCESS;
	uword result = 0;
	ubyte abyte;
	int shift = 0;
	while(1) {
		if(gel_cursor_at_end(*cur)) {
			gel_errno = GEL_EFORMAT;
			return 0;
		}
		abyte = gel_read_u8(*cur);
		result |= (abyte & 0x7f) << shift;
		if((abyte & 0x80) == 0)
			break;
		shift += 7;
	}
	return result;
}



/**
 * Decode a LEB128 signed integer. On return, set the @ref gel_errno to
 * @ref GEL_ESUCCESS or to @ref GEL_EFORMAT if the integer is truncated.
 * @param cur	Cursor on the read bytes.
 * @return		Signed integer read.
 */
static uword leb128_read_signed(gel_cursor_t *cur) {
	gel_errno = GEL_ESUCCESS;
	word result = 0;
	int shift = 0;
	int size = sizeof(word) * 8;
	ubyte abyte;
	//TLINE(printf("leb128_read_signed(%p) {\n", cur));
	while(1) {
		if(gel_cursor_at_end(*cur)) {
			gel_errno = GEL_EFORMAT;
			return 0;
		}
		abyte = gel_read_u8(*cur);
		//TLINE(printf("\tgel_read_u8() = %02x\n", byte));
		result |= (abyte & 0x7f) << shift;
		shift += 7;
		/* sign bit of byte is 2nd high order bit (0x40) */
		if((abyte & 0x80) == 0)
			break;
	}
	if((shift < size) && (abyte & 0x40))
		result |= -(1 << shift);
	//TLINE(printf("} = %ld // leb128_read_signed\n", result));
	return result;
}


/**
 * Initialize the local data to the module.
 * @param local	Local data to initialize.
 * @return	Error code.
 */
static int local_init(local_t *local) {
	local->addr_set = 0;
	gel_errno = GEL_ESUCCESS;
	local->standard_opcode_lengths = 0;
	if(pool_init(&local->lines, 1<<15))
		gel_errno = GEL_ERESOURCE;
	if(expand_init(&local->include_directories, sizeof(char *), 4))
		gel_errno = GEL_ERESOURCE;
	if(expand_init(&local->file_names, sizeof(unit_t *), 32))
		gel_errno = GEL_ERESOURCE;
	local->file_htab = gel_hash_new(63);
	return gel_errno;
}


/**
 * Clean the local for a new block of lines.
 * @param local	Local to clean.
 */
static void local_clean(local_t *local) {
	local->standard_opcode_lengths = 0;
	expand_reset(&local->include_directories);
	expand_reset(&local->file_names);
}


/**
 * Destroy the current local data.
 * @param local	Local data to destroy.
 */
static void local_destroy(local_t *local) {
	expand_free(&local->include_directories);
	expand_free(&local->file_names);
	pool_destroy(&local->lines);
	if(local->file_htab)
		hash_free(local->file_htab);
}


/**
 * Read a file encoding.
 * @param local	Local data.
 * @param name	Name of the file.
 * @param cur	Cursor on the buffer to read.
 * @return		Allocated file.
 */
static unit_t *read_file(
	local_t *local,
	const char *name,
	gel_cursor_t *cur)
{
	assert(local);
	assert(name);
	assert(cur);
	unit_t *file;
	uword dir, mod, len;
	int size;
	char *file_name;
	const char *p;
	char *q;

	/* Read the data */
	gel_errno = GEL_EFORMAT;
	do {
		if(gel_cursor_at_end(*cur))
			return 0;
	} while(gel_read_u8(*cur)) ;
	dir = leb128_read_unsigned(cur);
	if(gel_errno)
		return 0;
	mod = leb128_read_unsigned(cur);
	if(gel_errno)
		return 0;
	len = leb128_read_unsigned(cur);
	if(gel_errno)
		return 0;

	/* Make the full name */
	size = strlen(name) + 1;
	if(dir)
		size += strlen(EXPAND_GET(char *, local->include_directories, dir - 1)) + 1;
	file_name = newv(char, size);
	if(!file_name) {
		gel_errno = GEL_ERESOURCE;
		return 0;
	}
	q = file_name, *p;
	if(dir) {
		for(p = EXPAND_GET(char *, local->include_directories, dir - 1); *p != '\0'; p++) {
			if(*p == '\\')
				*q++ = '/';
			else
				*q++ = *p;
		}
		*q++ = '/';
	}
	for(p = name; *p != '\0'; p++) {
		if(*p == '\\')
			*q++ = '/';
		else
			*q++ = *p;
	}
	*q = '\0';

	/* Look for an exitsing file */
	file = (unit_t *)hash_get(local->file_htab, (char *)name);
	if(file) {
		delete(file_name);
		return file;
	}

	/* Create the file */
	file = new(unit_t);
	if(!file) {
		delete(file_name);
		gel_errno = GEL_ERESOURCE;
		return 0;
	}
	file->name = file_name;
	file->length = len;
	file->low = file->high = 0;
	file->lines = 0;
	hash_put(local->file_htab, (char *)file->name, file);
	return file;
}


/**
 * Read the header of the line storage.
 * @param local		Module local data.
 * @param cursor	Cursor to the line buffer.
 * @return			Error code.
 */
static int read_header(local_t *local, gel_cursor_t *cur) {
	char *name;
	assert(local);
	assert(cur);

	/* First fields */
	local->total_length = gel_read_u32(*cur);
	local->version = gel_read_u16(*cur);
	if(local->version < 2 || local->version > 4)
		return GEL_EFORMAT;
	local->header_length = gel_read_u32(*cur);
	local->minimum_instruction_length = gel_read_u8(*cur);
	if(local->version >= 4)		/* DWARF 4 */
		local->maximum_operations_per_instruction = gel_read_u8(*cur);
	else
		local->maximum_operations_per_instruction = 1;
	local->default_is_stmt = gel_read_u8(*cur);
	local->line_base = gel_read_s8(*cur);
	local->line_range = gel_read_u8(*cur);
	local->opcode_base = gel_read_u8(*cur);
	local->standard_opcode_lengths = (ubyte *)gel_cursor_addr(cur);
	gel_move_rel(cur, local->opcode_base - 1);
	if(gel_cursor_at_end(*cur))
		return GEL_EFORMAT;

	/* Read include directories */
	EXPAND_ADD(char *, local->include_directories,
		(char *)gel_cursor_addr(cur));
	while(gel_read_u8(*cur)) {
		do {
			if(gel_cursor_at_end(*cur))
				return GEL_EFORMAT;
		} while(gel_read_u8(*cur));
		TLINE(printf("INCLUDE %d: \"%s\"\n", EXPAND_CNT(local->include_directories) + 1, (char *)gel_cursor_addr(cur)));
		EXPAND_ADD(char *, local->include_directories,
			(char *)gel_cursor_addr(cur));
		if(gel_cursor_at_end(*cur))
			return GEL_EFORMAT;
	}

	/* Read file names */
	if(gel_cursor_at_end(*cur))
		return GEL_EFORMAT;
	name = (char *)gel_cursor_addr(cur);
	while(gel_read_u8(*cur)) {
		unit_t *file = read_file(local, name, cur);
		if(!file)
			return gel_errno;
		TLINE(printf("FILE %d: \"%s\"\n", EXPAND_CNT(local->file_names) + 1, file->name));
		EXPAND_ADD(unit_t *, local->file_names, file);
		name = (char *)gel_cursor_addr(cur);
		if(gel_cursor_at_end(*cur))
			return GEL_EFORMAT;
	}

	/* Success */
	return GEL_ESUCCESS;
}


/**
 * Record a line in the map.
 * @param local		Local data to use.
 * @param file		Container file index.
 * @param line		Current line (0 for an end sequence).
 * @param address	Current address.
 * @param flags	Line flags.
 * @param isa		ISA.
 * @return			0 success, -1 failure.
 */
static int emit_line(local_t *local, int file, int line, vaddr_t address, uhalf flags, uhalf isa) {
	line_t *line_rec;
	unit_t *file_rec;
	assert(local);

	ITRACE(printf("emit(%d, %d, %08x)\n", file, line, address));

	/* get the file available (some compiler exhibit bad file numbers) */
	if(file - 1 >= EXPAND_CNT(local->file_names)) {
		ITRACE(printf("ERROR: bad file number"));
		return GEL_ESUCCESS;
	}
	else
		file_rec = EXPAND_GET(unit_t *, local->file_names, file - 1);

	/* Get the memory */
	line_rec = (line_t *)pool_alloc(&local->lines, sizeof(line_t));
	if(!line_rec)
		return GEL_ERESOURCE;

	/* Initialize the record */
	line_rec->file = file_rec;
	assert(line_rec->file);
	line_rec->line = line;
	line_rec->addr = address;
	line_rec->flags = flags;
	line_rec->isa = isa;

	/* Address stats */
	if(!local->addr_set) {
		local->addr_set = 1;
		local->min_addr = address;
		local->max_addr = address;
	}
	else if(address < local->min_addr)
		local->min_addr = address;
	else if(address > local->max_addr)
		local->max_addr = address;

	/* File stats */
	if(line) {
		if(!file_rec->low) {
			file_rec->low = line;
			file_rec->high = line;
		}
		else if(line < file_rec->low)
			file_rec->low = line;
		else if(line > file_rec->high)
			file_rec->high = line;
	}

	/*fprintf(stderr, ">>> emit_line(%p, %d(%s), %d, %p)\n",
		local, file, file_rec->name, line, (void *)address);*/
	return GEL_ESUCCESS;
}


/**
 * Read the lines.
 * @param local		Local data.
 * @param base_cur	Cursor on the buffer.
 * @return			Error code.
 */
static int read_lines(local_t *local, gel_cursor_t *base_cur) {
	gel_cursor_t cur;

	/* debug state machine */
	vaddr_t address = 0;
	uint32_t op_index = 0;				/* DWARF 4 */
	int file = 1;
	int line = 1;
	int column = 0;
	uint16_t flags = 0, isa = 0;	/* DWARF-3 */
	uint32_t discriminator = 0;		/* DWARF 4 */

	/* state initialization */
	if(local->default_is_stmt)
		flags |= GEL_IS_STMT;

	/* Initialize the sub-cursor */
	if(gel_subcursor(
		base_cur,
		local->total_length - local->header_length - sizeof(uhalf) - sizeof(uword),
		&cur
	))
		return GEL_EFORMAT;

	/* Scan the lines */
	while(!gel_cursor_at_end(cur) && !(flags & GEL_END_SEQUENCE)) {
		ubyte opcode = gel_read_u8(cur);
		OTRACE(printf("OPCODE: %d\n", opcode));

		// Standard opcode
		switch(opcode) {
		case DW_LNS_copy:
			ITRACE(printf("copy\n"));
			if((gel_errno = emit_line(local, file, line, address, flags, isa)))
				return gel_errno;
			discriminator = 0;	/* DWARF-4 */
			flags &= ~(GEL_BASIC_BLOCK | GEL_PROLOGUE_END | GEL_EPILOGUE_BEGIN);
			break;
		case DW_LNS_advance_pc:
			address += local->minimum_instruction_length
				* leb128_read_unsigned(&cur);
			ITRACE(printf("STD: advance_pc(%p)\n", (void *)address));
			break;
		case DW_LNS_advance_line:
			line += leb128_read_signed(&cur);
			if(gel_errno)
				return gel_errno;
			ITRACE(printf("STD: advance_line(%d)\n", line));
			break;
		case DW_LNS_set_file:
			file = leb128_read_unsigned(&cur);
			ITRACE(printf("STD: set_file(%s)\n",
				EXPAND_GET(unit_t *, local->file_names, file - 1)->name));
			break;
		case DW_LNS_set_column:
			column = leb128_read_unsigned(&cur);
			ITRACE(printf("STD: set_column(%d)\n", column));
			break;
		case DW_LNS_negate_stmt:
			if(flags & GEL_IS_STMT)
				flags &= ~GEL_IS_STMT;
			else
				flags |= GEL_IS_STMT;
			ITRACE(printf("STD: negate_stmt\n"));
			break;
		case DW_LNS_set_basic_block:
			flags |= GEL_BASIC_BLOCK;
			ITRACE(printf("STD: set_basic_block\n"));
			break;
		case DW_LNS_const_add_pc:
			address += local->minimum_instruction_length
				* ((255 - local->opcode_base) / local->line_range);
			ITRACE(printf("STD: const_add_pc(%p)\n", (void *)address));
			break;
		case DW_LNS_fixed_advance_pc:
			address += gel_read_u16(cur);
			ITRACE(printf("STD: fixed_advance_pc(%p)\n", (void *)address));
			break;
		case DW_LNS_set_prologue_end:		/* DWARF 3 */
			flags |= GEL_PROLOGUE_END;
			ITRACE(printf("STD: set_prologue_end\n"));
			break;
		case DW_LNS_set_epilogue_begin:		/* DWARF 3 */
			flags |= GEL_EPILOGUE_BEGIN;
			ITRACE(printf("STD: set_epilogue_begin\n"));
			break;
		case DW_LNS_set_isa:				/* DWARF 3 */
			isa = leb128_read_unsigned(&cur);
			ITRACE(printf("STD: set_isa(%d)\n", isa));
			break;

		case 0:
			/* Extended opcode */
			{
				int size = leb128_read_unsigned(&cur);
				opcode = gel_read_u8(cur);
				/*ITRACE(printf("EXT: %d/%d at %p\n", opcode, 0 *size*,
					(void *)(cur.cur_addr - cur.base_addr - 1)));*/
				OTRACE(printf("OPCODE_EXT: %d (%d)\n", opcode, size));
				switch(opcode) {
				case DW_LNE_end_sequence:
					ITRACE(printf("EXT: end_sequence\n"));
					if(size != 1)
						return GEL_EFORMAT;
					flags |= GEL_END_SEQUENCE;
					if((gel_errno = emit_line(local, file, 0, address, flags, isa)))
						return gel_errno;
					file = 1;
					line = 1;
					column = 0;
					flags = 0;
					if(local->default_is_stmt)
						flags |= GEL_IS_STMT;
					break;
				case DW_LNE_set_address:
					if(size != 5)
						return GEL_EFORMAT;
					OTRACE(printf("u32=%02x%02x%02x%02x\n", cur.cur_addr[0],
						cur.cur_addr[1], cur.cur_addr[2], cur.cur_addr[3]));
					address = gel_read_vaddr(cur);
					ITRACE(printf("EXT: set_address (%p)\n", (void *)address));
					break;
				case DW_LNE_define_file:
					{
						char *name = gel_cursor_addr(&cur);
						/* !!TOFIX!! use the read size */
						unit_t *unit = read_file(local, name, &cur);
						if(!unit)
							return gel_errno;
						EXPAND_ADD(unit_t *, local->file_names, unit);
						ITRACE(printf("EXT: define_file(%s)\n", unit->name));
					}
					break;
				case DW_LNE_set_discriminator:
					discriminator = leb128_read_unsigned(&cur);
					ITRACE(printf("EXT: set_discriminator(%d)\n", discriminator));
					break;
				default:
					return GEL_EFORMAT;
				}
			}
			break;

		default:	/* Special opcode */
			{
				uint adjusted_opcode, operation_advance;
				adjusted_opcode = opcode - local->opcode_base;
				operation_advance = adjusted_opcode / local->line_range;
				
				/* compute address increment */
				address += local->minimum_instruction_length
					* ((op_index + operation_advance) / local->maximum_operations_per_instruction);

				/* new op index -- DWARF-4 */
				op_index = (op_index + operation_advance) % local->maximum_operations_per_instruction;

				/* compute line increment */
				line += local->line_base + (adjusted_opcode % local->line_range);
				
				/* emit line */
				ITRACE(printf("SPEC: %d\n", opcode));
				if((gel_errno = emit_line(local, file, line, address, flags, isa)))
					return gel_errno;

				/* flags & discriminator */
				flags &= ~(GEL_BASIC_BLOCK | GEL_PROLOGUE_END | GEL_EPILOGUE_BEGIN);
				discriminator = 0;	/* DWARF 4 */				
				break;
			}
		}

		/* Check error */
		if(gel_errno)
			return gel_errno;
	}

	// Update base cursor
	gel_move_abs(base_cur, gel_cursor_addr(&cur));
	return GEL_ESUCCESS;
}


/**
 * Allocate and initialize the map.
 * gel_errno errors include:
 * @li @ref GEL_ERESOURCE -- not enough memory.
 * @param file	Opened ELF file.
 * @param local	Current local data.
 * @return		Allocated map or null for failure.
 */
static dwarf_line_map_t *build_line_map(gel_file_t *file, local_t *local) {
	//int count;
	dwarf_line_map_t *map;
	chunk_t *chunk;
	line_t *line, *stop, *next;
	gel_hash_enum_t unit_enum;
	unit_t *unit;
	segment_t *seg;
	assert(local);

	/* Perform allocation */
	map = newc(dwarf_line_map_t);
	if(!map) {
		gel_errno = GEL_ERESOURCE;
		return 0;
	}

	/* initialize the map */
	map->units = local->file_htab;
	local->file_htab = 0;
	map->step = local->minimum_instruction_length;
	map->base = local->min_addr;
	map->segs = 0;

	/* initialize the units */
	unit = (unit_t *)gel_hash_enum_init(&unit_enum, map->units);
	while(unit) {
		unit->lines = newvc(unit_line_t, unit->high - unit->low  + 2);
		if(!unit->lines) {
			dwarf_delete_line_map(map);
			gel_errno = GEL_ERESOURCE;
			return 0;
		}
		unit = (unit_t *)gel_hash_enum_next(&unit_enum);
	}

	/* initialize the segments */
	if(gel_init_line_map_segs(file, map, map->step) < 0) {
		dwarf_delete_line_map(map);
		return 0;
	}

	/* use the stored lines */
	for(chunk = local->lines.head; chunk; chunk = chunk->next) {
		line = (line_t *)(chunk->buffer + local->lines.size) - 1;
		stop = (line_t *)chunk->cur;;
		while(line >= stop) {
			int loc, num;

			// End of sequence ?
			if(!line->line) {
				num = line->file->high - line->file->low + 1;
				if(line->addr >= line->file->lines[num].high)
					line->file->lines[num].high = line->addr;
			}

			// Normal address
			else {
				num = line->line - line->file->low;

				/* compute the next line if any */
				next = NULL;
				if(line -1 >= stop)
					next = line - 1;
				else if(chunk->next)
					next = (line_t *)(chunk->next->buffer + local->lines.size) - 1;

				/* record in the map : address to line */
				if(next) {
					int found = 0;
					vaddr_t addr = addr = line->addr;

					// !!CHECK!!
					vaddr_t top_addr = next->addr;
					if(top_addr <= addr)
						top_addr = addr + map->step;

					for(; addr < top_addr; addr += map->step) {
						for(seg = map->segs; seg; seg = seg->next)
							if(seg->low <= addr && addr < seg->high) {
								loc = (addr - seg->low) / map->step;
								seg->locs[loc] = &line->file->lines[num];
								LTRACE(if(!found) fprintf(stderr, "CODE: %p:%p:%d:%08x:%08x\n", &seg->locs[loc], seg->locs[loc], num, line->addr, top_addr));
								found = 1;
								break;
							}
					}

					/* record in the map : line to address */
					/*fprintf(stderr, "record line %d of file %s with %p\n",
						line->line, line->file->name, (void *)line->addr);*/
					if(found) {
						if(line->file->lines[num].unit == NULL) {
							line->file->lines[num].unit = line->file;
							assert(line->file);
							line->file->lines[num].seg = seg;
							line->file->lines[num].low = line->addr;
							line->file->lines[num].high = top_addr;
							line->file->lines[num].flags = line->flags;
							line->file->lines[num].isa = line->isa;							
						}
						else {
							if(line->addr < line->file->lines[num].low)
								line->file->lines[num].low = line->addr;
							if(top_addr > line->file->lines[num].high)
								line->file->lines[num].high = top_addr;
						}
						LTRACE(fprintf(stderr, "LINE:%p:%08x:%08x:%08x:%08x:%s:%d\n",
							&line->file->lines[num], line->addr, top_addr,
							line->file->lines[num].low, line->file->lines[num].high,
							line->file->name, line->line);)
					}
				}
			}

			// next line
			line--;
		}
	}

	/* success */
	return map;
}


/**
 * @def dwarf_address_from_line(map, file, line)
 * Get the actual address of a source line.
 * @param map	Line map to use.
 * @param file	File name.
 * @param line	Line number.
 * @return		Matching address or null if no address is found.
 * @ingroup		dwarf
 * @deprecated	Use gel_address_from_line().
 */


/**
 * @def dwarf_line_from_address(map, addr, file, line)
 * Get the file and line matching an actual binary address.
 * @param map	Used map.
 * @param addr	Looked address.
 * @param file	Returned file.
 * @param line	Returned line.
 * @return		0 for success, -1 if not found.
 * @ingroup		dwarf
 * @deprecated	Use gel_line_from_address().
 */


/**
 * @def dwarf_delete_line_map(map)
 * Delete a line map.
 * @param map	Line map to delete.
 * @ingroup		dwarf
 * @deprecated	Use gel_delete_line_map().
 */


#ifdef TRACE_LOCAL
/**
 * Dump the header information.
 * @param local	Local information.
 */
static void dump_header(local_t *local) {
	int i;
	printf(
		"total_length=%lu\n"
		"version=%u\n"
		"prologue_length=%u\n"
		"minimum_instruction_length=%u\n"
		"default_is_stmt=%d\n"
		"line_base=%d\n"
		"line_range=%d\n"
		"opcode_base=%u\n",
		local->total_length,
		local->version,
		local->header_length,
		local->minimum_instruction_length,
		local->default_is_stmt,
		local->line_base,
		local->line_range,
		local->opcode_base);
	printf("standard_opcode_lengths\n");
	for(i = 0; i < local->opcode_base - 1; i++)
		printf("\t[%d] = %d\n", i + 1, local->standard_opcode_lengths[i]);
	printf("include_directories\n");
	for(i = 0; i < EXPAND_CNT(local->include_directories) ; i++)
		printf("\t%s\n", EXPAND_GET(char *, local->include_directories, i));
	printf("file_names\n");
	for(i = 0; i < EXPAND_CNT(local->file_names); i++)
		printf("\t%s [%lu]\n",
			EXPAND_GET(unit_t *, local->file_names, i)->name,
			EXPAND_GET(unit_t *, local->file_names, i)->length);
}
#endif	// TRACE_LOCAL


/**
 * Build the line information.
 * @param file		File containing the section.
 * @param section	Section containing the information (null for default .debug_line).
 * @return			Read line map or null for an error.
 * gel_errno errors include:
 * @li @ref GEL_ENOTFOUND -- no .debug_line section available.
 * @li @ref GEL_EFORMAT -- bad format.
 * @li @ref GEL_ERESOURCE -- not enough memory.
 * @ingroup		dwarf
 */
dwarf_line_map_t *dwarf_new_line_map(gel_file_t *file, gel_sect_t *section) {
	gel_cursor_t cur;
	gel_errno = GEL_ESUCCESS;
	dwarf_line_map_t *map = 0;
	local_t local;

	/* Assertion tests */
	assert(file);

	/* Find section */
	if(!section) {
		section = gel_getsectbyname(file, DWARF_DEBUG_LINE_NAME);
		if(!section) {
			gel_errno = GEL_ENOTFOUND;
			return 0;
		}
	}

	/* Read header */
	if(gel_sect2cursor(section, &cur) == -1) {
		gel_errno = GEL_EFORMAT;
		return 0;
	}

	/* Prepare local data */
	gel_errno = local_init(&local);

	/* Read section */
	if(!gel_errno)
		while(!gel_cursor_at_end(cur)) {

			/* Read header */
			if(!gel_errno)
				gel_errno = read_header(&local, &cur);
			#ifdef TRACE_LOCAL
				dump_header(&local);
			#endif

			/* Generate the lines */
			if(!gel_errno)
				gel_errno = read_lines(&local, &cur);

			/* cleanup */
			local_clean(&local);
		}

	/* Build line / address tables */
	if(!gel_errno)
		map = build_line_map(file, &local);

	/* Cleanup */
	local_destroy(&local);
	return map;
}


/**
 * @def dwarf_first_line(iter, map);
 * Initialize the iterator on the lines.
 * @param iter	Iterator to use.
 * @param map	Map to iterate on.
 * @return		First location or an empty file location if there is no line.
 * @ingroup		dwarf
 * @deprecated	Use gel_first_line().
 */


/**
 * @def dwarf_next_line(iter);
 * Go to the next line.
 * @param iter	Used iterator.
 * @return		Next location or an empty file location if there is no more line.
 * @ingroup		dwarf
 * @deprecated	Use gel_next_line().
 */


/**
 * @def dwarf_first_addr(iter, map);
 * Initialize the given iterator on the first address.
 * @param iter	Iterator to initialize.
 * @param map	Line map to use.
 * @return		Return the first location or an empty file location if there
 * 				is no address.
 * @ingroup		dwarf
 * @deprecated	Use gel_first_addr().
 */



/**
 * @def dwarf_next_addr(iter);
 * Get the next location.
 * @param iter	Current address iterator.
 * @return		Next address or empty file location.
 * @ingroup		dwarf
 * @deprecated	Use gel_next_addr().
 */

