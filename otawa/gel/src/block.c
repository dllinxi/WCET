/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2010, IRIT- UPS
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

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <gel/image.h>
#include <gel/prog.h>
#include <gel/gel_elf.h>
#include <gel/error.h>
#include <gel/image.h>
#include "gel_private.h"
#include "gel_base_private.h"


/**
 * @defgroup block Block Management
 *
 * 
 * @code
 * #include <gel/image.h>
 * @endcode
 * 
 * This functions allows to access blocks, that is, the program bits including code and data
 * part of the execution built image.
 * 

 */


/**
 * Get back public information about an image block.
 * Got information remains valid until the image object or the matching file object
 * is removed.
 *
 * @param b		Block to get information for.
 * @param bi 	Data to store information in.
 * @return 		Error code (0 for success, -1 for error -- error code in @ref gel_errno)
 *
 * @ingroup block
 */
int gel_block_infos(gel_block_t *b, gel_block_info_t *bi) {
	bi->base_vaddr = b->base_vaddr;
	bi->base_raddr = b->base_raddr;

	bi->container = b->container;
	bi->vreloc = RADDR2VRELOC(b, b->data);
	return 0;
}

/**
 * @typedef gel_block_info_t
 * Public information of a block. Provided by the function
 * gel_block_infos() .
 * @ingroup block
 */

/**
 * @var gel_block_info_t::container;
 * ELF file owner of the block.
 */

/**
 * @var gel_block_info_t::base_vaddr;
 * Virtual base address (0 it not loaded).
 */

/**
 * @var gel_block_info_t::base_raddr;
 * Real base address (0 if not loaded).
 */

/**
 * @var gel_block_info_t::vreloc;
 * Relocated virtual address.
 */




