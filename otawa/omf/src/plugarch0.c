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


/*
 * \file Ce fichier est le plug-in d'archi par defaut.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gel/gel_elf.h>
#include <gel/error.h>
#include <gel/gel_plugin.h>


/**
 * @struct arch_plugin_s
 * Plugin structure for architecture-dependent features.
 */

/**
 * @var arch_plugin_s arch_plugin_s::machine
 * Architecture identifier.
 */

/**
 * @var arch_plugin_s arch_plugin_s::version
 * Version of the plug-in.
 */

/**
 * @var arch_plugin_s arch_plugin_s::stack_align
 * Stack alignment constraint (raise this value to power 2 to get byte alignment).
 */

/**
 * @var arch_plugin_s arch_plugin_s::dlh
 * DL-handle of the plug-in.
 * */

/**
 * @var arch_plugin_s arch_plugin_s::plug_do_reloc
 * Function performing the relocation of image.
 * @param r 	Image to relocate.
 * @param flags Flags (one of GEL_IMAGE_PLTBLOCK_NOW, GEL_IMAGE_PLTBLOCK_LAZY).
 * @return 		Error code.
 */

/**
 * @var arch_plugin_s arch_plugin_s::align
 * Undocumented.
 */

/**
 * @var arch_plugin_s arch_plugin_s::psize
 * Page size of the system.
 */


/* Externals */
extern int gel_errno;


/* Default relocation function causing error. */
static int do_reloc(gel_image_t *im, int flags) {
	gel_image_info_t ii;
	gel_block_info_t bi;
	gel_file_info_t fi;
	gel_sect_info_t si;
	int i, j;
	gel_sect_t *s;

	/* Look each file... */
	gel_image_infos(im, &ii);
	for (i = 0; i < ii.membersnum; i++) {
		gel_block_infos(ii.members[i], &bi);
		if (bi.container == NULL)
      		continue; /* Non-file blocks can't have a relocation section */
		gel_file_infos(bi.container, &fi);

		/* ... for a relocation section */
		for (j = 0; j < fi.sectnum; j++) {
			s = gel_getsectbyidx(bi.container, j);
			assert(s);
			gel_sect_infos(s, &si);
			if (si.type == SHT_RELA) {
				gel_errno = GEL_ENOTSUPP;
				return -1;
			}
		}
	}

	/* No relocation. It is ok ! */
	return 0;
}


/**
 * Null plugin for architecture.
 */
arch_plugin_t null_plugin_arch = {
  0,
  1,
  2,		/* stack alignment */
  NULL,
  do_reloc,
  1,
  4096
};
