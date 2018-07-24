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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <gel/file.h>
#include <gel/gel_elf.h>
#include <gel/error.h>
#include "gel_types_private.h"
#include "gel_base_private.h"
#include "gel_mem.h"

/**
 * @defgroup authors		Authors
 *
 * @li C. Ballabriga <ballabri@irit.fr>
 * @li H. Cassé <casse@irit.fr>
 * @li S. Lemouzy
 */


/**
 * @defgroup file	ELF File
 *
 * @code
 * #include <gel/gel.h>
 * @endcode
 *
 * This module provides basic functions to access an ELF file.
 * @li gel_open() - open a file,
 * @li gel_close() - close a file.
 *
 * And some generic facilities:
 * @li gel_strtab() - to access a string table.
 *
 * This module contains also facilities to manager errors and warnings:
 * @li @ref gel_errno - global to return error codes,
 * @li gel_strerror() - function to get human-readable error messages.
 *
 */


/**
 * @struct gel_file_info_t
 * Provides public fields of a file handle.
 * @ingroup	file
 */

/**
 * @var gel_file_info_t::ident
 * Identification of the header. Includes the magic, the file class and
 * endianness.
 */

/**
 * @var gel_file_info_t::filename
 * File name.
 */

/**
 * @var gel_file_info_t::type
 * File type (one of ET_xxx definition).
 */

/**
 * @var gel_file_info_t::machine
 * Machine identification.
 */

/**
 * @var gel_file_info_t::version
 * ELF version.
 */

/**
 * @var gel_file_info_t::entry;
 * Virtual address of the program entry point.
 */

/**
 * @var gel_file_info_t::sectnum
 * Number of section headers.
 */

/**
 * @var gel_file_info_t::prognum
 * Number of program headers.
 */


/**
 * @typedef gel_file_t
 * Handle to an opened ELF file and pointer to the public fields.
 * It is usually obtained by a call to the function gel_open().
 * @ingroup	file
 */


/**
 * @var GEL_OPEN_NOPLUGINS
 * If passed in the gel_open() flags, informs that no plug-in must be opened.
 * @ingroup	file
 */


/**
 * Remplit le champ "needed" de gel_file_t avec les noms des librairies
 * requises.
 *
 * Fill the "needed" file of gel_file_t with required libraries names.
 * @param f 	GEL file to process.
 */
static void gel_get_needed(gel_file_t *f) {
	gel_file_s *fp = (gel_file_s *)f;
  int i,j,k, stop, str;
  gel_sect_t *sect, *old;
  Elf32_Dyn *edyn;
  int needednum;

  /* recherche la section avec les informations sur les liaisons dynamiques */
  for (i = 0; i < f->sectnum; i++)
  {
  	 old = fp->sects[i];

    sect = gel_getsectbyidx(f, i);

    if (sect == NULL)
    {
      return;
    }

    if (sect->type == SHT_DYNAMIC)
    {
      if (gel_sect_load(sect, NULL) == -1)
      {
        gel_errno = GEL_WDYNAMIC;
        return;
      }
      /* remplir tmp->needed */

      str = sect->link;
      if (sect->link >= f->sectnum) {
        gel_errno = GEL_WDYNAMIC;
        return;
      }

      stop = 0;
      needednum = 0;
      for (j = 0; (j < sect->size) && (!stop); j += sizeof(Elf32_Dyn)) {
        edyn = (void*) ((char*)sect->data + j);
        switch(ENDIAN4(fp->plat->endianness, edyn->d_tag)) {
          case DT_NULL:
            stop = 1;
            break;
          case DT_NEEDED:
            needednum++;
            break;
          default:
            break;
        }
      }

      fp->needed = newv(char*, needednum);
      if (fp->needed == NULL) {
        gel_errno = GEL_WDYNAMIC;
        return;
      }
      fp->needednum = needednum;
      stop = 0;
      k = 0;
      for (j = 0; (j < sect->size) && (!stop); j += sizeof(Elf32_Dyn)) {
        edyn = (void*) ((char*)sect->data + j);
        switch(ENDIAN4(fp->plat->endianness, edyn->d_tag)) {
          case DT_NULL:
            stop = 1;
            break;
          case DT_NEEDED:
            fp->needed[k] = gel_strtab(f, str, ENDIAN4(fp->plat->endianness, edyn->d_un.d_val));
            if (fp->needed[k] == NULL) {
              gel_errno = GEL_WNONFATAL;
            } else k++;
            break;
          default:
            break;
        }
      }

      return;
    }
    if (old == NULL) {
      fp->sects[i] = NULL;
      gel_kill_sect(sect);
    }
  }
}


/**
 * Build a platform structure.
 * @param m Machine.
 * @param e Endianness.
 * @param f Type of float values.
 */
static gel_platform_t *gel_make_platform(u16_t m, int e, int abi, int f)
{
  gel_platform_t *plat;

  plat = new(gel_platform_t);
  if (plat == NULL) {
    gel_errno = GEL_ERESOURCE;
    return(NULL);
  }
  memset(plat, 0, sizeof(gel_platform_t));
  plat->machine = m;
  plat->endianness = e;
  plat->float_type = f;
  plat->system = abi;
  plat->refcount = 0;
  return plat;
}

extern arch_plugin_t null_plugin_arch;
extern sys_plugin_t null_plugin_sys;


/**
 * Open an ELF file.
 * @param filename	Path to the file.
 * @param plugpath	Alternate GEL plugin path (NULL for default plugins).
 * @param flags		Flags (see @ref GEL_OPEN_NOPLUGINS).
 * @return			File handle or null if there is an error.
 * @ingroup	file
 */
gel_file_t *gel_open(const char *filename, const char *plugpath, int flags) {
	gel_file_s *tmp;
	int fd;
	Elf32_Ehdr ehdr;
	int r;

	/* open the file */
#	if defined(__WIN32) || defined(__WIN64)
#		define FLAGS	O_RDONLY | O_BINARY
#	else
#		define FLAGS	O_RDONLY
#	endif
	fd = open(filename, FLAGS);
	if (fd == -1) {
		gel_errno = GEL_EIO;
		return NULL;
	}

	/* allocate the file descritor*/
	tmp = new(gel_file_s);
	if (tmp == NULL) {
		gel_errno = GEL_ERESOURCE;
		close(fd);
		return NULL;
	}

	/* initialize the file descriptor and copy the file name */
	memset(tmp, 0, sizeof(gel_file_s));
	tmp->fd = fd;
	tmp->_.filename = newv(char, strlen(filename) + 1);
	if (tmp->_.filename == NULL)  {
		gel_errno = GEL_ERESOURCE;
		gel_close(&tmp->_);
		return NULL;
	}
	strcpy(tmp->_.filename, filename);

	/* read the ELF header */
	if(read(fd, &ehdr, sizeof(ehdr)) == -1) {
		gel_errno = GEL_EIO;
		gel_close(&tmp->_);
		return NULL;
	}

	/* is it ELF and 32-bits ? */
	if(ehdr.e_ident[EI_MAG0] != ELFMAG0
	|| ehdr.e_ident[EI_MAG1] != ELFMAG1
	|| ehdr.e_ident[EI_MAG2] != ELFMAG2
	|| ehdr.e_ident[EI_MAG2] != ELFMAG2) {
		gel_set_error(GEL_EFORMAT, "not ELF file");
		gel_close(&tmp->_);
		return NULL;
	}
	if(ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
		gel_set_error(GEL_EFORMAT, "only 32-bits class supported");
		gel_close(&tmp->_);
		return NULL;
	}

	/* copy the identification part */
	memcpy(tmp->_.ident , ehdr.e_ident, sizeof(tmp->_.ident));

	/* get the platform */
	tmp->plat = gel_make_platform( ENDIAN2(ehdr.e_ident[EI_DATA], ehdr.e_machine),
		ehdr.e_ident[EI_DATA],
		ehdr.e_ident[EI_OSABI],
		0);
	if (tmp->plat == NULL) {
		gel_errno = GEL_ERESOURCE;
		gel_close(&tmp->_);
		return NULL;
	}

	/* fill the file desctiptor with ELF header data */
	tmp->plat->refcount = 1;
	tmp->_.type = ENDIAN2(tmp->plat->endianness, ehdr.e_type);
	tmp->_.version = ENDIAN4(tmp->plat->endianness, ehdr.e_version);
	tmp->_.entry = ENDIAN4(tmp->plat->endianness, ehdr.e_entry);
	tmp->_.sectnum = ENDIAN2(tmp->plat->endianness, ehdr.e_shnum);
	tmp->_.prognum = ENDIAN2(tmp->plat->endianness, ehdr.e_phnum);
	tmp->str = ENDIAN2(tmp->plat->endianness, ehdr.e_shstrndx);

	/* check validity of .strtab index */
	if(tmp->str >= tmp->_.sectnum)  {
		gel_errno = GEL_EFORMAT;
		gel_close(&tmp->_);
		return(NULL);
	}

	/* endianness fix */
	tmp->sectoff = ENDIAN4(tmp->plat->endianness, ehdr.e_shoff);
	tmp->progoff = ENDIAN4(tmp->plat->endianness, ehdr.e_phoff);
	tmp->sects = newv(gel_sect_t *, tmp->_.sectnum);
	tmp->progs = newv(gel_prog_t *, tmp->_.prognum);
	if ((tmp->sects == NULL) || (tmp->progs == NULL))  {
		gel_errno = GEL_ERESOURCE;
		gel_close(&tmp->_);
		return(NULL);
	}

	/* initialize section and program tables */
	memset(tmp->sects, 0, sizeof(void*) * tmp->_.sectnum);
	memset(tmp->progs, 0, sizeof(void*) * tmp->_.prognum);

	/* open the dependent files  */
	gel_get_needed(&tmp->_);

	/* load plugins as needed */
	if(!(flags & GEL_OPEN_NOPLUGINS)) {
		gel_load_plugins(tmp->plat, plugpath, flags & GEL_OPEN_QUIET);
		if (tmp->plat->arch == NULL || tmp->plat->sys == NULL) {
			gel_errno = GEL_ENOTSUPP;
			gel_close(&tmp->_);
			return NULL;
		}
		assert(tmp->plat->arch);
		assert(tmp->plat->sys);
	}

	/* build the hash table */
	tmp->syms = gel_hash_new(211);
	r = -1;
	if (tmp->syms != NULL)
		r = gel_hash_syms(&tmp->_, tmp->syms, 0);

	/* cleanup in case of failure */
	if (r == -1) {
		if (tmp->syms != NULL)
			hash_free(tmp->syms);
		else
			gel_errno = GEL_ERESOURCE;

		delete(tmp->sects);
		delete(tmp->progs);
		delete(tmp->plat);
		delete(tmp->_.filename);
		delete(tmp);
		close(fd);
		return(NULL);
	}

	/* return the result */
	return &tmp->_;
}


/**
 * Close an opened file and relase of the allocated resources.
 * @param file File descriptor.
 * @ingroup	file
 */
void gel_close(gel_file_t *file) {
	gel_file_s *f = (gel_file_s *)file;
	int i;
	if(file == NULL)
		return;

	/* free blocks */
	if(f->blocks != NULL) {
		for(i = 0; i < f->_.blknum; i++)
			if(f->blocks[i] != NULL)
				delete(f->blocks[i]);
		delete(f->blocks);
	}

	/* free sections */
	if(f->sects != NULL) {
		for(i = 0; i < f->_.sectnum; i++)
			gel_kill_sect(f->sects[i]);
		delete(f->sects);
	}

	/* free program headers */
	if(f->progs != NULL) {
		for(i = 0; i < f->_.prognum; i++)
			gel_kill_prog(f->progs[i]);
		delete(f->progs);
	}

	/* free symbols */
	if(f->syms != NULL)
		hash_free(f->syms);

	/* free file name */
	delete(f->_.filename);

	/* free platform */
	if(f->plat != NULL) {
		f->plat->refcount--;
		if(f->plat->refcount == 0) {
			gel_unload_plugins(f->plat);
			delete(f->plat);
		}
	}

	/* close the FD */
	if(f->fd != 0)
		close(f->fd);

	/* cleanup remaining */
	delete(f->needed);
	delete(f);
	return;
}


/**
 * Get a string from a string table.
 * @param f		Current ELF file.
 * @param sect	String table index.
 * @param off	String offset.
 * @return 		String pointer or null (offset out of the string table).
 * @ingroup		file
 */
char *gel_strtab(gel_file_t *f, int sect, u32_t off) {
  gel_sect_t *s;
  s  = gel_getsectbyidx(f, sect);
  if (s == NULL)
  {
    return NULL;
  }

  if (off >= s->size) {
    gel_errno = GEL_EFORMAT;
    return NULL;
  }

  if (gel_sect_load(s, NULL) == -1)
  {
    return NULL;
  }

  return (((char*)(s->data)) + (off));
}


/**
 * Get the endianness of the given file.
 * @param file	File to look endianness in.
 * @return		One of GEL_BIG_ENDIAN and GEL_LITTLE_ENDIAN.
 */
int gel_file_endianness(gel_file_t *file) {
	return ((gel_file_s *)file)->plat->endianness;
}
