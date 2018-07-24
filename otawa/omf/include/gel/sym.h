/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-09, IRIT- UPS
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
#ifndef GEL_SYM_H_
#define GEL_SYM_H_

#include <gel/common.h>
#include <gel/types.h>

__BEGIN_DECLS

typedef struct gel_sym_info_t {
	gel_file_t *container;
	gel_block_t *blockcont;
	u32_t type;
	vaddr_t vaddr;
	raddr_t raddr;
	u16_t sect;
	char *name;
	u8_t info;
	u32_t size;
} gel_sym_info_t;


// functions
int gel_sym_infos(gel_sym_t *s, gel_sym_info_t *smi);

__END_DECLS

#endif /* GEL_SYM_H_ */
