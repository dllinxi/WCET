/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-08, IRIT- UPS
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
#ifndef GEL_FILE_H
#define GEL_FILE_H

#include <gel/common.h>
#include <gel/types.h>
#include <gel/util.h>

__BEGIN_DECLS

/* types */
typedef struct gel_file_info_t {
	char ident[16];
	char *filename;
	u16_t type;
	u16_t machine;
	u32_t version;
	vaddr_t entry;
	u16_t sectnum;
	u16_t prognum;
	u16_t blknum;
	gel_block_t **blocks;
} gel_file_info_t;


/* flags */
#define GEL_OPEN_NOPLUGINS      0x00000001
#define GEL_OPEN_QUIET			0x00000002

/* Externals */
extern int gel_errno;


/* Primitives */
void gel_close(gel_file_t *e);
int gel_file_endianness(gel_file_t *file);
int gel_file_infos(gel_file_t *e, gel_file_info_t *fi);
gel_file_t *gel_open(const char *filename, const char *plugpath, int flags);
char *gel_strtab(gel_file_t *f, int sect, u32_t off);

/* Symbol iterator */
typedef gel_hash_enum_t gel_sym_iter_t;
gel_sym_t *gel_sym_first(gel_sym_iter_t *iter, gel_file_t *file);
gel_sym_t *gel_sym_next(gel_sym_iter_t *iter);

__END_DECLS

#endif /* GEL_FILE_H */
