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

#include <gel/sym.h>
#include <gel_types_private.h>
#include <gel/gel_plugin.h>

/**
 * @defgroup sym	File Symbols
 * 
 * @code
 * #include <gel/sym.h>
 * @endcode
 * 
 * This module allows to traverse the symbols of an executable file.
 * 
 * You can also obtains the symbol of the file as:
 * @code
 * gel_sym_iter_t iter;
 * gel_sym_t *sym;
 * for(sym = gel_sym_first(&iter, file); sym; sym = gel_sym_next(&iter)) {
 *	gel_sym_info_t info;
 *		gel_sym_infos(sym, &info);
 * }
 * @endcode

 */

/**
 * Get public information about a symbol. This information
 * remains valid until the container file or image is deleted.
 *
 * @param s		Symbol to get information for.
 * @param smi	Structure to store symbol information in.
 * @return		0 for success, -1 for failure (details in @ref gel_errno)
 * @ingroup		sym
 */
int gel_sym_infos(gel_sym_t *s, gel_sym_info_t *smi)
{
	int i;
  smi->vaddr = s->vaddr;
  smi->sect = s->sect;
  smi->name = s->name;

  if (s->ct == GEL_SYMCONT_FILE)
  {
	gel_file_t *file = s->cont.file;
    smi->container = file;
    smi->blockcont = 0;
    for(i = 0; i < file->blknum; i++)
    if(smi->vaddr >= file->blocks[i]->exec_begin
    && smi->vaddr < file->blocks[i]->exec_begin + file->blocks[i]->exec_size)
    	smi->blockcont = file->blocks[i];
  }
  else
  {
    smi->blockcont = s->cont.block;
    smi->container = s->cont.block->container;
  }

  smi->raddr = ((smi->blockcont == NULL) || (s->vaddr == 0 )) ? 0 : VADDR2RADDR(smi->blockcont, s->vaddr);
  smi->info = s->info;
  smi->size = s->size;
  return 0;
}


/**
 * Get the first symbol of the file.
 * @param iter	Iterator on file symbol.
 * @param file	File to iterate on symbols.
 * @ingroup sym
 */
gel_sym_t *gel_sym_first(gel_sym_iter_t *iter, gel_file_t *file) {
	struct gel_file_s *rec = (struct gel_file_s *)file;
	if(gel_hash_enum_init(iter, rec->syms))
		return gel_hash_value(iter);
	else
		return NULL;
}


/**
 * Go to the next symbol.
 * @param iter	Symbol iterator.
 * @ingroup sym
 */
gel_sym_t *gel_sym_next(gel_sym_iter_t *iter) {
	if(gel_hash_enum_next(iter))
		return gel_hash_value(iter);
	else
		return NULL;
}


/**
 * @typedef gel_sym_info_t
 * Structure to store information available for a symbol.
 * Returned by gel_sym_infos().
 * @ingroup sym
 */

/**
 * @var gel_sym_info_t::container
 * Contained file.
 */

/**
 * @var gel_sym_info_t::blockcont
 * Image block the symbol is in.
 */

/**
 * @var gel_sym_info_t::type
 * Type of the symbol. Constants ELF32_STT_XXX in the ELF specification.
 */

/**
 * @var gel_sym_info_t::vaddr
 * Value of the symbol. If the symbolis an object or code, this is the
 * virtual address of the symbol.
 */

/**
 * @var gel_sym_info_t::raddr
 * Real addres of the symbol (in the current memory).
 */

/**
 * @var gel_sym_info_t::sect
 * Number of section containing the symbol.
 */

/**
 * @var gel_sym_info_t::name
 * Name of the symbol.
 */

/**
 * @var gel_sym_info_t::info
 * Symbol information as in the ELF specification, constants ELF32_ST_XXX.
 */

/**
 * @var gel_sym_info_t::size
 * Size of the symbol.
 */
