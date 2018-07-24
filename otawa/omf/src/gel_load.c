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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <gel/image.h>
#include <stdarg.h>

#include <gel/error.h>
#include "gel_private.h"
#include <gel/gel_elf.h>
#include "gel_mem.h"

/**
 * Fonction servant a entrer les symboles du fichier f dans la table de
 * hashage.
 *
 * @param f Le fichier
 * @param hash La table de hashage
 * @param no_local Si vrai, le container est un bloc d'image et on ne regarde pas les symboles locaux. Sinon le container est un fichier et on regarde les symboles locaux.
 * @return Code d'erreur (0 pour succes ou -1 pour erreur)
 */
int gel_hash_syms(gel_file_t *f, gel_hash_t hash, int no_local)
{
  gel_sect_t *symtab; /* pointeur vers la table des symboles */
  gel_sym_t *sym;     /* pointeur vers un symbole */
  Elf32_Sym *esym;    /* pointeur vers une entree de la table des symboles */
  char *symname;
  int i,p;
  int add;
  gel_file_s *fp = (gel_file_s *)f;

  /* If .dynsym is present we load it instead of .symtab, because
   * we are only interested in dynamic symbols */
  symtab = gel_getsectbyname(f, GEL_SPECIAL_DYNSYM);
  if (symtab == NULL)
  {
    symtab = gel_getsectbyname(f, GEL_SPECIAL_SYMTAB);
    if (symtab == NULL)
    {
      return 0;
    }
  }

  if (gel_sect_load(symtab, NULL) == -1)
  {
    return -1;
  }

  if (symtab->link >= f->sectnum)
  {
    return -1;
  }

  p = 0;
  for (i = 0; i < symtab->size; i += sizeof(Elf32_Sym), p++)
  {
    esym = ((Elf32_Sym *)symtab->data) + p;

    /*fprintf(stderr, "Symbol value=%08x\n", esym->st_value);*/

    symname = gel_strtab(f, symtab->link,
    	ENDIAN4(fp->plat->endianness, esym->st_name));
    if (symname == NULL)
    {
      gel_errno = GEL_WNONFATAL;
      continue;
    }

    /*fprintf(stderr, "Symbol %s, value=%08x\n", symname, esym->st_value);*/

    add = 0;
    if ((esym->st_shndx == SHN_UNDEF) && (no_local))
    {
      /*
       * Undefined symbols can't be in an image
       * Unresolved symbols will be detected during relocation
       */
      continue;
    }

    switch(ELF32_ST_BIND(esym->st_info)) {
      case STB_GLOBAL:
        /* global symbol takes precedence over anything */
        hash_del(hash, symname);
        add = 1;
        break;
      case STB_WEAK:
        /* weak symbol added only if there is no global symbol with same name */
        add = (hash_get(hash, symname) == NULL);

        break;
      case STB_LOCAL:
        /* local symbols are added only if no_local is false */
        add = !no_local;
        break;
      default:
#ifndef NDEBUG
        printf("DEBUG: alien symbol: %s\n", symname);
#endif
        gel_errno = GEL_EFORMAT;
        return -1;
        break;
    }

    if (add)
    {
        sym = new(gel_sym_t);
        if (sym == NULL)
        {
          gel_errno = GEL_ERESOURCE;
          return -1;
        }
        memset(sym, 0, sizeof(gel_sym_t));
        if(no_local &&
        (ELF32_ST_TYPE(esym->st_info) == STT_OBJECT || ELF32_ST_TYPE(esym->st_info) == STT_FUNC)) {
        	int j;
        	vaddr_t addr = ENDIAN4(fp->plat->endianness, esym->st_value);
          sym->ct = GEL_SYMCONT_BLOCK;
          for(j = 0; j < f->blknum; j++)
          	if(addr >= f->blocks[j]->exec_begin
          	&& addr < f->blocks[j]->exec_begin + f->blocks[j]->exec_size) {
          		assert(f->blocks[j]);
          		sym->cont.block = f->blocks[j];
          		break;
          	}
        }
        else
        {
          sym->ct = GEL_SYMCONT_FILE;
          sym->cont.file = f;
        }
        sym->vaddr = ENDIAN4(fp->plat->endianness, esym->st_value);
        sym->sect = ENDIAN2(fp->plat->endianness, esym->st_shndx);
        if ((sym->sect >= f->sectnum) && (sym->sect < SHN_LORESERVE))
        {
          gel_errno = GEL_WNONFATAL;
          delete(sym);
          continue;
        }
        sym->info = esym->st_info;
        sym->size = ENDIAN4(fp->plat->endianness, esym->st_size);
        sym->name = newv(char, strlen(symname) + 1);
        if (sym->name == NULL)
        {
          delete(sym);
          gel_errno = GEL_ERESOURCE;
          return -1;
        }
        strcpy(sym->name, symname);

        if(hash_put(hash, sym->name, sym) == -1)
        {
          delete(sym);
          return -1;
        }

    } /* if (add) */
  }
  return 0;
}


/**
 * Get a global symbol in the image from its name.
 * @param im	Image descriptor..
 * @param name	Symbol name.
 * @return 		Symbol descriptor or null if it is not found..
 */
gel_sym_t *gel_find_glob_symbol(gel_image_t *im, char *name) {
  return (hash_get(im->syms,name));
}


/**
 * Return an enumeration of the global symbols of the image.
 * Free the returned enumeration with enu_free().
 * @param im	Image to work on.
 * @return		Enumeration or null if there is an error (error code in
 * 				@ref gel_errno).
 */
gel_enum_t *gel_enum_glob_symbol(gel_image_t *im) {
	return (hash_enum(im->syms));
}


