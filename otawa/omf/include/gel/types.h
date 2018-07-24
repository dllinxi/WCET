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
#ifndef GEL_TYPES_H_
#define GEL_TYPES_H_

#include <gel/common.h>
#include <inttypes.h>

__BEGIN_DECLS

/* base types */
typedef uint16_t  u16_t; /**< Non signe 16 bits */
typedef int16_t   s16_t; /**< Signe 16 bits */
typedef uint32_t  u32_t; /**< Non signe 32 bits */
typedef int32_t   s32_t; /**< Signe 32 bits */
typedef uint8_t   u8_t; /**< Non signe 8 bits */
typedef int8_t    s8_t; /**< Signe 8 bits */
typedef uint32_t  vaddr_t; /**< Adresse virtuelle */
typedef void*     raddr_t; /**< Adresse reelle */


/* opaque types */
typedef struct gel_block_s gel_block_t;
typedef struct gel_enum_s gel_enum_t;
typedef struct gel_file_info_t gel_file_t;
typedef struct gel_platform_s gel_platform_t;
typedef struct gel_sym_s gel_sym_t;

__END_DECLS

#endif /* GEL_TYPES_H_ */
