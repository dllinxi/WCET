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

#include "../config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

#ifndef CMAKE_FLAG
#	include <ltdl.h>
#endif

#include "gel_private.h"
#include <gel/gel_elf.h>
#include <gel/error.h>
#include "gel_mem.h"

#define TRACE(stat) stat

void gel_load_plugins(gel_platform_t *f, const char *plugpath, int quiet);
void gel_unload_plugins(gel_platform_t *pf);


/**
 * fonction personnalisee de copie de chaines de carracteres
 */
char *mystrdup(char *s) {
  char *tmp;
  tmp = newv(char, strlen(s) + 1);
  if (tmp == NULL)
    return NULL;
  strcpy(tmp, s);
  return tmp;
}

/**
 * Compare deux plateformes
 *
 * @param p1 La plateforme 1
 * @param p2 La plateforme 2
 * @return 0 pour egal, un autre nombre pour different.
 */
int gel_compare_plat(gel_platform_t *p1, gel_platform_t *p2)
{
  if ((p1->machine == p2->machine) &&
      (p1->endianness == p2->endianness) &&
      (p1->float_type == p2->float_type))
      {
        if (p1->system != p2->system) {
#ifndef NDEBUG
          printf("Warning: ABI differs\n");
          gel_errno = GEL_WNONFATAL;
#endif
        }
        return 0;
      } else return 1;
}

/**
 * Trouve un symbole local d'apres son nom et le fichier qui le contient.
 * L'utilisateur ne doit pas desallouer le symbole avec free() apres usage.
 *
 * @param f Le descripteur du fichier qui doit contenir le symbole.
 * @param name Le nom du symbole.
 * @return Le descripteur du symbole.
 */
gel_sym_t *gel_find_file_symbol(gel_file_t *f, char *name)  {
	return(hash_get(((gel_file_s *)f)->syms, name));
}

/**
 * Recupere un symbole d'apres son index et sa section
 * Le symbole doit etre free() par l'utilisateur
 *
 * @param symtab La section
 * @param idx L'index du symbole
 * @return Le symbole gel_sym_t ou NULL
 */
gel_sym_t *gel_symbyidx(gel_sect_t *symtab, u32_t idx)
{
  gel_sym_t *sym;
  Elf32_Sym *esym;
  char *symname;
  gel_file_s *filep;

  if (gel_sect_load(symtab, NULL) == -1) {
    return NULL;
  }
  if (symtab->link >= symtab->container->sectnum)
    return NULL;
  esym = ((Elf32_Sym *)symtab->data) + idx;
  filep = (gel_file_s *)symtab->container;
  symname = gel_strtab(&filep->_, symtab->link, ENDIAN4(filep->plat->endianness, esym->st_name));
  if (symname == NULL)
    return NULL;
  assert (!(idx*sizeof(Elf32_Sym) > symtab->size));
  sym = new(gel_sym_t);
  if (sym == NULL) {
    gel_errno = GEL_ERESOURCE;
    return NULL;
  }
  memset(sym, 0, sizeof(gel_sym_t));
  sym->ct = GEL_SYMCONT_FILE;
  sym->cont.file = symtab->container;
  sym->vaddr = ENDIAN4(filep->plat->endianness, esym->st_value);
  sym->sect = ENDIAN2(filep->plat->endianness, esym->st_shndx);
  sym->info = esym->st_info;
  sym->size = ENDIAN4(filep->plat->endianness, esym->st_size);
  sym->name = symname;
  return sym;
}


/**
 * Initialise la position ("curseur") de l'enumeration
 *
 * @param en L'enumeration
 */
void gel_enum_initpos(gel_enum_t *en)
{
  en->initpos(en);
}

/**
 * Renvoie le prochain element et incremente la position
 *
 * @param en L'enumeration
 * @return L'element lu ou NULL si fin de liste
 */
void *gel_enum_next(gel_enum_t *en)
{
  return(en->next(en));
}

/**
 * Libere l'enumeration (uniquement l'enumeration, ne concerne pas l'objet
 * auquel elle est attachee
 *
 * @param en L'enumeration
 */
void gel_enum_free(gel_enum_t *en)
{
  en->free(en);
}

/**
 * Renvoie une enumeration des symboles locaux d'un fichier.
 * A liberer avec enum_free()
 *
 * @param f Le fichier.
 * @return L'enumeration, ou NULL si erreur.
 */
gel_enum_t *gel_enum_file_symbol(gel_file_t *f)
{
  return(hash_enum(((gel_file_s *)f)->syms));
}

/**
 * Remplace (recopie) un symbole par un autre dans la table
 * de hashage des symboles.
 * Les deux symboles doivent avoir le meme nom.
 *
 * @param im La table de hashage
 * @param symname Le nom du symbole a remplacer
 * @param newsym Le nouveau symbole a mettre a sa place.
 */
void gel_replacesym(gel_image_t *im, char *symname, gel_sym_t *newsym) {
  gel_sym_t *oldsym;

  assert(strcmp(newsym->name, symname) == 0);
  oldsym = gel_find_glob_symbol(im, symname);
  if (oldsym == NULL)
    return;

  memcpy(oldsym, newsym, sizeof(gel_sym_t));
}



