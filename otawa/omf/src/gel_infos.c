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

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include "gel_private.h"

/**
 * Extract public data from the GEL file descriptor. These data becomes
 * invalid if the GEL file is closed.
 * @param e 	File descriptor.
 * @param fi	Result of the call, that is, GEL file information.
 * @return 		0 for success, -1 for failure (error code in @ref gel_errno).
 * @deprecated	Information is already accessible from the gel_file_t descriptor.
 */
int gel_file_infos(gel_file_t *e, gel_file_info_t *fi) {
	memcpy(fi->ident, e->ident, sizeof(fi->ident));
	fi->type = e->type;
	fi->filename = e->filename;
	fi->machine = ((gel_file_s *)e)->plat->machine;
	fi->version = e->version;
	fi->entry = e->entry;
	fi->sectnum = e->sectnum;
	fi->prognum = e->prognum;
	fi->blknum = e->blknum;
	fi->blocks = e->blocks;
	return 0;
}



