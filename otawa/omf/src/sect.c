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

#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>

#include <gel/sect.h>
#include <gel/gel_elf.h>
#include <gel/error.h>
#include "gel_types_private.h"
#include "gel_base_private.h"
#include "gel_mem.h"

/**
 * @defgroup sect	Sections
 * Sections are part of ELF files giving a logical division
 * of a program. They are usually obtained from the ELF file
 * using functions gel_getsectbyidx() and gel_getsectbyname().
 */


/**
 * @typedef gel_sect_t
 * Opaque type to describe a section in the ELF file.
 * This kind of data is returned by getsectbyidx() et getsectbyname().
 * @ingroup sect
 */



/**
 * @struct gel_sect_info_t
 * This is used to get back information about a section using
 * gel_sect_info() function.
 * @ingroup sect
 */

/**
 * @var gel_sect_info_t::container
 * File containing the section.
 */

/**
 * @var gel_sect_info_t::name
 * Section name.
 */

/**
 * @var gel_sect_info_t::type
 * Section type as defined in ELF specification (one of SHT_XXX constants).
 */

/**
 * @var gel_sect_info_t::vaddr
 * Section virtual address as defined in ELF specification.
 */

/**
 * @var gel_sect_info_t::offset
 * Offset in the ELF file to retrieve content.
 */

/**
 * @var gel_sect_info_t::size
 * Size of the content of the section.
 */

/**
 * @var gel_sect_info_t::flags
 * Flags about the section (a combination of SHF_XX).
 */

/**
 * @var gel_sect_info_t::link
 * Link to another section (st_link in ELF specification).
 */

/**
 * @var gel_sect_info_t::info
 * Extra information depending on the section type (st_info in ELF specification).
 */


/**
 * Load the content of a section at the given address or, if null,
 * allocate some memory to put content in.
 *
 * @param s 	Section descriptor.
 * @param addr 	Memory to load content in.
 * @return 		0 for success, -1 for error (see gel_errno for details)
 * @ingroup		sect
 */
int gel_sect_load(gel_sect_t *s, raddr_t addr)
{
  raddr_t addr2 = addr;
  /* verifie si la section n'est pas deja chargee */
  if ((s->data != NULL) && (addr == NULL))
  {
	return 0;
  }

  if (addr == NULL) {
    addr2 = newv(char, s->size);
    if (addr2 == NULL) {
      gel_errno = GEL_ERESOURCE;
      return -1;
    }
  }

  if (s->type == SHT_NOBITS) {
    memset(addr2, 0, s->size);
  } else {
  	gel_file_s *fp = (gel_file_s *)s->container;
    if (lseek(fp->fd, s->offset, SEEK_SET) == -1) {
      gel_errno = GEL_EIO;
      delete(addr2);
      return -1;
    }
    if (read(fp->fd, addr2, s->size) == -1) {
      gel_errno = GEL_EIO;
      delete(addr2);
      return -1;
    }
  }
  if (addr == NULL)
    s->data = addr2;
  return 0;
}


/**
 * Delete a section.
 * @param s		Section to delete.
 */
void gel_kill_sect(gel_sect_t *s) {
  if (s == NULL)
    return;
  delete(s->data);
  delete(s);
}


/**
 * Get a section descriptor from its index in the ELF file.
 * The section will be automatically fried as soon the ELF file descriptor
 * is closed.
 *
 * @param e 	Descriptor of the ELF file containing the section.
 * @param idx	Section index.
 * @return 		Section descriptor pointer or NULL for failure (see gel_errno fro details).
 * @ingroup		sect
 */
gel_sect_t *gel_getsectbyidx(gel_file_t *e, u16_t idx) {
	gel_file_s *ep = (gel_file_s *)e;
  Elf32_Shdr shdr;
  gel_sect_t *sect;

  assert (!(idx >= e->sectnum));

  /* verifier si on a deja la section, le cas echeant on retourne direct la
   * section */
  if (ep->sects[idx] != NULL)
  {
    return(ep->sects[idx]);
  }

  /* se positionne sur l'entree de correspondante de la section header table */
  if (lseek(ep->fd, (ep->sectoff) + idx*sizeof(shdr), SEEK_SET) == -1)
  {
    gel_errno = GEL_EIO;
    return NULL;
  }
  /* recupere cette entree*/
  if (read(ep->fd, &shdr, sizeof(shdr)) == -1)
  {
    gel_errno = GEL_EIO;
    return NULL;
  }

  /* alloue la place necessaire pour le gel_sect_t */
  sect = new(gel_sect_t);
  if (sect == NULL)
  {
    gel_errno = GEL_ERESOURCE;
    return NULL;
  }
  /* initialise et remplit la structure */
  memset(sect, 0, sizeof(gel_sect_t));
  sect->container = e;
  sect->sect_index = idx;

  sect->link = ENDIAN4(ep->plat->endianness, shdr.sh_link);
  /* verifie le format des donnees */
  if ((sect->link >= e->sectnum) && (sect->link < SHN_LORESERVE)
       && (sect->link > SHN_HIRESERVE))
  {
    delete(sect);
    gel_errno = GEL_EFORMAT;
    return NULL;
  }

  sect->info = ENDIAN4(ep->plat->endianness, shdr.sh_info);
  sect->type = ENDIAN4(ep->plat->endianness, shdr.sh_type);
  sect->vaddr = ENDIAN4(ep->plat->endianness, shdr.sh_addr);
  sect->size = ENDIAN4(ep->plat->endianness, shdr.sh_size);
  sect->flags = ENDIAN4(ep->plat->endianness, shdr.sh_flags);
  sect->offset = ENDIAN4(ep->plat->endianness, shdr.sh_offset);
  /* ajoute les donnees sur la section dans le descripteur de fichier ELF */
  ep->sects[idx] = sect;

  sect->name = gel_strtab(e, ep->str, ENDIAN4(ep->plat->endianness,shdr.sh_name));
  if (sect->name == NULL)
  {
    delete(sect);
    gel_errno = GEL_EFORMAT;
    return NULL;
  }

   return(sect);
}

/**
 * Get a section descriptor from its name. If two sections have
 * the same name, only the first one in index order is returned.
 * The section will be automatically fried as soon as the ELF
 * file descriptor is closed.
 *
 * @param e			Descriptor of the ELF file containing the sectio.
 * @param sectname	Name of the looked section.
 * @return 			Description pointer of the found section or NULL (see gel_errno for details).
 * @ingroup			sect
 */
gel_sect_t *gel_getsectbyname(gel_file_t *e, char *sectname) {
	gel_file_s *ep = (gel_file_s *)e;
  int i;
  gel_sect_t *sect, *old;

  for (i = 0; i < e->sectnum; i++) {
    old = ep->sects[i];
    sect = gel_getsectbyidx(e, i);
    if (sect == NULL)
      return NULL;
    if ((sect->name != NULL) && (!strcmp(sectname, sect->name))) {
      return sect;
    }
    if (old == NULL) {
      ep->sects[i] = NULL;
      gel_kill_sect(sect);
    }
  }
  gel_errno = GEL_ENOTFOUND;
  return NULL;
}


/**
 * Get public information of a section. Note that the returned information
 * remains only valid until the secton is removed.
 *
 * @param e 	Section descriptor.
 * @param si 	Structure to get back section information.
 * @return 		0 for success, -1 for failure (details in gel_errno).
 * @ingroup		sect
 */
int gel_sect_infos(gel_sect_t *e, gel_sect_info_t *si)
{
  si->name = e->name;
  si->type = e->type;
  si->vaddr = e->vaddr;
  si->offset = e->offset;
  si->size = e->size;
  si->info = e->info;
  si->link = e->link;
  si->flags = e->flags;
  si->container = e->container;
  return 0;
}

