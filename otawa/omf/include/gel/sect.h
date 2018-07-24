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
#ifndef GEL_SECT_H_
#define GEL_SECT_H_

#include <gel/common.h>
#include <gel/types.h>
#include <gel/file.h>

__BEGIN_DECLS

/* types */
typedef struct gel_sect_s gel_sect_t;
typedef struct {
  gel_file_t *container;
  char *name;
  u32_t type;
  vaddr_t vaddr;
  u32_t offset;
  u32_t size;
  u32_t flags;
  u32_t link;
  u32_t info;
} gel_sect_info_t;


/* primitives */
gel_sect_t *gel_getsectbyidx(gel_file_t *e, u16_t idx);
gel_sect_t *gel_getsectbyname(gel_file_t *e, char *sectname);
int gel_sect_infos(gel_sect_t *s, gel_sect_info_t *si);
int gel_sect_load(gel_sect_t *s, raddr_t addr);

__END_DECLS

#endif /* GEL_SECT_H_ */
