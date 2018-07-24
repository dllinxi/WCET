/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-07, IRIT- UPS
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <gel/prog.h>
#include <gel/gel_elf.h>
#include <gel/error.h>
#include "gel_types_private.h"
#include "gel_mem.h"

#define TRACE(cmd)	//cmd


/**
 * @defgroup phdr Program Headers
 *
 * This module provides functions to handle program headers. In ELF, the
 * programs headers provide a way to build quickly the memory image of a
 * program. Although there is different types of programs headers, the main
 * type is @ref PT_LOAD that provides information to load code or data from
 * the ELF file to the memory. There is several other types for dynamic linking,
 * compiler notes and so on.
 *
 * The program headers may be handled using the @ref gel_prog_t pointers and
 * information may be obtained into a @ref gel_prog_info_t structure. A simple
 * way to get a program header is the use of index with the function
 * @ref gel_getprogbyidx().
 *
 * The @ref PT_NOTE program header may be scanned using the functions:
 * gel_note_init(), gel_note_next(), gel_note_destroy(), gel_note_name(),
 * gel_note_type(), gel_note_size() and gel_note_desc().
 */

/**
 * Initialize a note cursor.
 * @param phdr	Program header to use.
 * @param curs	Used cursor.
 * @return		0 for success, -1 else.
 * @ingroup		phdr
 */
int gel_note_init(gel_prog_t *phdr, gel_note_t *curs) {
	assert(phdr);
	assert(curs);
	assert(phdr->type == PT_NOTE);

	/* Initialization */
	if(gel_prog2cursor(phdr, &curs->cur) == -1)
		return -1;
	curs->name = 0;
	curs->size = 0;
	curs->type = 0;
	curs->aligned = 1;

	/* Testing alignment */
	/* !!WARNING!!
	 * This is a trick to detect unaligned notes and scan them right. */
	while(!gel_cursor_at_end(curs->cur)) {
		Elf32_Word namesz, descsz;
		if(gel_cursor_avail(curs->cur) < 3 * sizeof(Elf32_Word)) {
			curs->aligned = 0;
			break;
		}
		namesz = GEL_ALIGN(gel_read_u32(curs->cur), 4);
		descsz = GEL_ALIGN(gel_read_u32(curs->cur), 4);
		gel_move_rel(&curs->cur, sizeof(curs->cur));
		if(gel_cursor_avail(curs->cur) < namesz + descsz) {
			curs->aligned = 0;
			break;
		}
		gel_move_rel(&curs->cur, namesz + descsz);
	}

	/* All is fine ! */
	gel_cursor_reset(curs->cur);
	return 0;
}


/**
 * Free a note cursor.
 * @param curs	Note cursor.
 * @ingroup		phdr
 */
void gel_note_destroy(gel_note_t *curs) {
	if(curs->name)
		delete(curs->name);
}


/**
 * Get the next note.
 * @param curs	Used not cursor.
 * @return		0 for success. -1 if en is reahced ((gel_errno == 0) or if
 * 				there is an error (gel_errno != 0).
 * @ingroup		phdr
 */
int gel_note_next(gel_note_t *curs) {
	Elf32_Word namesz, descsz;

	/* End reached ? */
	if(gel_cursor_at_end(curs->cur)) {
		gel_errno = 0;
		return -1;
	}

	/* Read the note header */
	if(gel_cursor_avail(curs->cur) < 3 * sizeof(Elf32_Word)) {
		TRACE(fprintf(stderr, "TRACE: not enough place for sizes !\n"));
		gel_errno = GEL_EFORMAT;
		return -1;
	}
	namesz = gel_read_s32(curs->cur);
	descsz = gel_read_s32(curs->cur);
	curs->type = gel_read_s32(curs->cur);
	if(gel_cursor_avail(curs->cur) < namesz + descsz) {
		TRACE(fprintf(stderr, "TRACE: not enough place for values %d + %d > %d\n",
			namesz, descsz, gel_cursor_avail(curs->cur)));
		gel_errno = GEL_EFORMAT;
		return -1;
	}

	/* Read the name */
	if(curs->size < namesz) {
		if(curs->name)
			delete(curs->name);
		curs->size = namesz;
		curs->name = newv(char, namesz);
		if(!curs->name) {
			gel_errno = GEL_ERESOURCE;
			return -1;
		}
	}
	gel_read_data(curs->cur, curs->name, namesz);
	if(curs->aligned)
		gel_cursor_align(curs->cur, sizeof(Elf32_Word));

	/* Set the descriptor cursor */
	gel_subcursor(&curs->cur, descsz, &curs->desc);
	gel_move_rel(&curs->cur, descsz);
	if(curs->aligned)
		gel_cursor_align(curs->cur, sizeof(Elf32_Word));

	/* Return result */
	return 0;
}


/**
 * @def gel_note_name(c)
 * Get the name of the current note.
 * @param c	Note cursor.
 * @return	Name of the note.
 * @ingroup		phdr
 */


/**
 * @def gel_note_type(c)
 * Get the type of the current note.
 * @param c	Note cursor.
 * @return	Type of the note.
 * @ingroup		phdr
 */


/**
 * @def gel_note_desc(c)
 * Get a cursor on the description of the note.
 * @param c	Note cursor.
 * @return	Cursor on the description of the note.
 * @ingroup		phdr
 */


/**
 * @def gel_note_size(c)
 * Get the size of the description part of the note.
 * @param c	Cursor on the current note.
 * @ingroup		phdr
 */

/**
 * @def PF_X
 * Program header flag: segment executable.
 * @ingroup		phdr
 */

/**
 * @def PF_W
 * Program header flag: segment writable.
 * @ingroup		phdr
 */

/**
 * @def PF_R
 * Program header flag: segment readable.
 * @ingroup		phdr
 */


/**
 * @def PF_MASKOS
 * Program header flag: OS-specific mask.
 * @ingroup		phdr
 */


/**
 * @def PF_MASKPROC
 * Program header flag: processor specific mask.
 * @ingroup		phdr
 */


/**
 * @def PT_NULL
 * Program header type: null.
 * @ingroup		phdr
 */


/**
 * @def PT_LOAD
 * Program header type: loadable segment.
 * @ingroup		phdr
 */


/**
 * @def PT_DYNAMIC
 * Program header type: information for dynamic linking.
 * @ingroup		phdr
 */


/**
 * @def PT_INTERP
 * Program header type: information about the program interpreter.
 * @ingroup		phdr
 */


/**
 * @def PT_NOTE
 * Program header type: generic note.
 * @ingroup		phdr
 */


/**
 * @def PT_SHLIB
 * Program header type: shared library information.
 * @ingroup		phdr
 */


/**
 * @def PT_PHDR
 * Program header type: position in memory of the program header table.
 * @ingroup		phdr
 */


/**
 * @def PT_LOPROC
 * Program header type: low index for processor specific program header.
 * @ingroup		phdr
 */


/**
 * @def PT_HIPROC
 * Program header type: hig index for processor specific program header.
 * @ingroup		phdr
 */

 /**
 * Load the rough content of a loadable segment at the given address
 * or, if null, to a memory allocated by GEL.
 * @param p		Program header handle to load.
 * @param addr	Address to load to or null for automatic allocation.
 * @return 		0 for success, -1 for error.
 * @ingroup		phdr
 */
int gel_prog_load(gel_prog_t *p, raddr_t addr)  {
	gel_prog_s *pp = (gel_prog_s *)p;
	int size;
	assert(p);

	/* Already done ? */
	if ((pp->data != NULL) && (addr == NULL))
		return 0;

	/* Compute the file size */
	size = p->filesz;

	/* Allocate the memory if required */
	if(addr == NULL) {
		addr = newv(char, p->memsz);
    	if(addr == NULL)  {
			gel_errno = GEL_ERESOURCE;
			return -1;
		}
		/* fprintf(stderr, "DEBUG: allocate %p to %p\n", addr, addr + p->memsz); */
		pp->data = addr;
	}

	/* !!HKC!! Not only set to 0 an empty program entry
	 * We must complement with 0 initialized data when memsz > filesz
	 * Too lame ! */
	/* Juste BSS ? */
	/*if(size == 0) {
		memset(addr, 0, p->memsz);
		*fprintf(stderr, "GEL CLEAR %p at %p (%08x)\n", p->vaddr, addr, p->size);*
	}*/

	/* Load from file */
	/*else {*/
	if(size != 0) {
		gel_file_s *filep = (gel_file_s *)p->container;
		if(lseek(filep->fd, p->offset, SEEK_SET) == -1
		|| read(filep->fd, addr, size) == -1)  {
			delete(addr);
			pp->data = NULL;
			gel_errno = GEL_EIO;
			return -1;
		}
		/*if(p->vaddr >= 0x3000 && p->vaddr < 0x02000000)
			fprintf(stderr, "GEL LOAD %p (%08x) at %p [%08x]\n",
				p->vaddr, p->size, addr, *(int32_t *)addr);*/
	}

	/* Fill with 0 remaining */
	if(size < p->memsz)
		memset(addr + p->filesz, 0, p->memsz - p->filesz);

	/* All is fine ! */
	return 0;
}


/**
 * Delete a PHDR.
 * @param p	PHDR to delete.
 */
void gel_kill_prog(gel_prog_t *p) {
	gel_prog_s *pp = (gel_prog_s *)p;
	if (p == NULL)
		return;
	if(pp->data)
		delete(pp->data);
	delete(pp);
}


/**
 * Get a program header handler from its index.
 * The allocated is automatically managed by the file handler and does not
 * any to be fried by the caller.
 * @param e		File descriptor.
 * @param idx	Index of the required program header.
 * @return 		Program header handle or null if there is an error.
 * @ingroup		phdr
 */
gel_prog_t *gel_getprogbyidx(gel_file_t *e, u16_t idx) {
	Elf32_Phdr phdr;
	gel_prog_s *prog;
	gel_file_s *ep = (gel_file_s *)e;
  assert (!(idx >= e->prognum));

  /* verifie si on a deja le segment, le cas echeant on retourne directement
   * le segment */
  if (ep->progs[idx] != NULL)
  {
    return(ep->progs[idx]);
  }

  /* se positionne sur l'entree de correspondante de la segment header table */
  if (lseek(ep->fd, (ep->progoff) + idx*sizeof(phdr), SEEK_SET) == -1)
  {
    gel_errno = GEL_EIO;
    return NULL;
  }

  /* lit l'entree corespondante */
  if (read(ep->fd, &phdr, sizeof(phdr)) == -1)
  {
    gel_errno = GEL_EIO;
    return NULL;
  }

  /* Alloue l'espace necessaire pour la structure a retourner*/
  prog = new(gel_prog_s);
  if (prog == NULL)
  {
    gel_errno = GEL_ERESOURCE;
    return NULL;
  }

  /* initialise la structure et la remplit */
  prog->index = idx;
  prog->data = 0;
  prog->_.container = e;
  prog->_.type = ENDIAN4(ep->plat->endianness, phdr.p_type);
  prog->_.vaddr = ENDIAN4(ep->plat->endianness, phdr.p_vaddr);
  prog->_.paddr = ENDIAN4(ep->plat->endianness, phdr.p_paddr);
  prog->_.memsz = ENDIAN4(ep->plat->endianness,  phdr.p_memsz);
  prog->_.filesz = ENDIAN4(ep->plat->endianness, phdr.p_filesz);
  prog->_.align = ENDIAN4(ep->plat->endianness, phdr.p_align);
  prog->_.flags = ENDIAN4(ep->plat->endianness, phdr.p_flags);
  prog->_.offset = ENDIAN4(ep->plat->endianness, phdr.p_offset);

  /* ajoute les donnees sur le segment dans le descripteur de fichier ELF */
  ep->progs[idx] = &prog->_;

  return(&prog->_);
}


/**
 * Get public information for the program header handle.
 * Notice that the got information will become inconsistant as soon as
 * the program header is destroyed.
 * @param e 	Program header handle.
 * @param pi	Structure receiving information.
 * @return 		0 for success, -1 for error.
 * @ingroup		phdr
 * @deprecated	Public information are now straight-forward reachable from the
 * 				handle.
 */
int gel_prog_infos(gel_prog_t *e, gel_prog_info_t *pi) {
	*pi = *e;
	return 0;
}
