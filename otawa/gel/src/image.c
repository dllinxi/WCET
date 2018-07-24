/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2008-09, IRIT- UPS
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
#include "gel_private.h"
#include "gel_base_private.h"
#include "gel_mem.h"

/**
 * @defgroup image Image Loading
 *
 * This module handles the ability to load an execution image from
 * an ELF file.
 *
 */

#ifndef NDEBUG
#	define MARK		//fprintf(stderr, "MARK %s:%d\n", __FILE__, __LINE__)
#	define TRACE(x) /* x */
#else
#	define MARK
#	define TRACE(x)
#endif


void gel_unload_plugins(gel_platform_t *pf);

/* default environment */
static char *libpath[] = { "./", "/lib", "/usr/lib", "/usr/local/lib",
"/usr/X11R6/lib", NULL
};
static char *nulltab[] = {NULL};
static gel_env_t default_env = {
	libpath,
	nulltab,
	nulltab,
	NULL,
	0,
	STACKADDR_OSDEFAULT,
	STACKSIZE_OSDEFAULT,
	CLUSTERSIZE_OSDEFAULT,
	0,
	ENVFLAGS_OSDEFAULT,
	0,
	0,
	0,
	0,
	0
};


/**
 * Return the default environment that is initialized as below :
 * @li libpath = { "./", "/lib", "/usr/lib", "/usr/local/lib", "/usr/X11R6/lib"}
 * @li argv = {}
 * @li envp = {}
 * @li pltblock = NULL
 * @li pltblocksize = 0
 * @li stackaddr = STACKADDR_OSDEFAULT
 * @li stacksize = STACKSIZE_OSDEFAULT
 * @li clustersize = CLUSTERSIZE_OSDEFAULT
 * @li flags = ENVFLAGS_OSDEFAULT
 *
 * @return	Environment initialized with default values or NULL if there is no
 * 			more memory.
 * @ingroup image
 */
gel_env_t *gel_default_env(void) {
	gel_env_t *env;
	env = new(gel_env_t);
	if (env == NULL) {
		gel_errno = GEL_ERESOURCE;
		return NULL;
	}
	memcpy(env, &default_env, sizeof(gel_env_t));
	return env;
}


/**
 * Clean partially allocated list of blocks.
 * @param blocks	Blocks to clean.
 * @param cnt		Count of allocated blocks.
 * @return			Ever -1.
 */
static int clean_blocks(gel_block_t **blocks, int cnt) {
	int i;
	for(i = 0; i < cnt; i++)
		delete(blocks[i]);
	delete(blocks);
	gel_errno = GEL_ERESOURCE;
	return -1;
}


/**
 * Get information about loading segments (virtual and physical addresses).
 * @param file 			ELF file
 * @param base_vaddr	current base addresse
 * @return 				0 for success, -1 else
 * @ingroup image
 */
int gel_file_load_info (gel_file_t *file, vaddr_t base_vaddr) {
	int i, j;
	gel_prog_t *pr;
	gel_block_t **blocks;

	/* raw block array allocation */
	blocks = newv(gel_block_t *, file->prognum);
	if(blocks == NULL) {
		gel_errno = GEL_ERESOURCE;
		return -1;
	}

	/* load the block */
	j = 0;
	for (i = 0; i < file->prognum; i++)  {

		/* get program header information */
		pr = gel_getprogbyidx(file, i);
		if (pr == NULL)
			return clean_blocks(blocks, j);

		/* handles only loaded segments */
		if (pr->type != PT_LOAD)
			continue;

		/* Allocate the block */
		blocks[j] = new(gel_block_t);
		if(!blocks[j])
			return clean_blocks(blocks, j);

    	/* Initialize it */
		blocks[j]->plat = ((gel_file_s *)file)->plat;
		blocks[j]->base_vaddr = base_vaddr;
		blocks[j]->exec_size = pr->memsz;
		blocks[j]->container = file;
		blocks[j]->exec_begin = pr->vaddr;
		blocks[j]->base_raddr = 0;
		blocks[j]->data = NULL;
		j++;
	}

 	// Return result
 	file->blknum = j;
 	file->blocks = blocks;
	return 0;
}


/**
 * Load the data of a file from the ELF disk file in the actual memory.
 * If the base_raddr is null, the memory is allocated by the function.
 * @param e				ELF file.
 * @param block			Block of the file to load.
 * @param base_raddr	Base address of the block to load to.
 * @return				0 for success, -1 else.
 */
int gel_file_load_data (gel_file_t *e, gel_block_t *block, raddr_t base_raddr) {
	int i;
	gel_prog_t *pr;

	/* Need allocation ? */
	if (base_raddr == NULL) {
		block->data = newv(char, block->exec_size);
		if (block->data == NULL)  {
			gel_errno = GEL_ERESOURCE;
			return -1;
		}
	}
	else
		block->data = base_raddr;

	/* compute the real address */
	block->base_raddr = (raddr_t)(block->data - block->exec_begin);

	/* load all program header matching the block */
	int found = 0;
	for (i = 0; i < e->prognum; i++)  {

		/* get the program header */
		pr = gel_getprogbyidx(e, i);
		if(pr == NULL)  {
			if(!base_raddr)
				delete(block->data);
			return -1;
    	}

    	/* Load it */
		if(pr->type == PT_LOAD
		&& block->exec_begin <= pr->vaddr
		&& pr->vaddr + pr->memsz - 1 <= block->exec_begin + block->exec_size - 1) {
			gel_prog_load(pr, VADDR2RADDR(block, pr->vaddr));
			found = 1;
		}
	}
	
	/* check for real loading */
	if(!found) {
		gel_errno = GEL_EIO;
		return -1;
	}
	return 0;
}


/**
 * Load an ELFfile into memory without library management.
 * @param e				ELF file.
 * @param base_vaddr	Base virtual address.
 * @param base_raddr	Deprecated. Set it to null.
 * @return				0 for success, -1 else.
 */
int gel_file_load (gel_file_t *e, vaddr_t base_vaddr, raddr_t base_raddr) {
	int i;
	int retour;
	assert(base_raddr == 0);

	/* Get file information */
	retour = gel_file_load_info(e, base_vaddr);
	if(retour == -1)
    	return -1;

	/* Load the blocks */
	for(i = 0; i < e->blknum; i++) {
		/*fprintf(stderr, "LOADING %p\n", e->blocks[i]->exec_begin);*/
		if(gel_file_load_data(e, e->blocks[i], 0) == -1)
			return -1;
	}
	return 0;
}




/**
 * Sert a trouver une librairie en cherchant dans libpath[]
 *
 * @param name Le nom de la librairie
 * @param env L'environnement
 *
 * @return Le nom de la librairie precedee du path
 */
static char *gel_find_lib(char *name, gel_env_t *env) {
  int tmax;
  int i;
  char *buf;
  FILE *f;
  int found = 0;
  char **mylibpath;

  if (env->libpath == NULL) {
    mylibpath = libpath;
  } else mylibpath = env->libpath;
  i = 0;
  tmax = 0;
  while (mylibpath[i] != NULL) {
    if (strlen(mylibpath[i]) > tmax)
      tmax = strlen(mylibpath[i]);
    i++;
  }
  buf = newv(char, strlen(name) + tmax + 2);
  if (buf == NULL) {
    return NULL;
  }
  i = 0;
  while (mylibpath[i] != NULL) {
    strcpy(buf, mylibpath[i]);
    strcat(buf, "/");
    strcat(buf, name);
    f = fopen(buf, "r");
    if (f != NULL) {
      /* we found the library */
      fclose(f);
      found = 1;
      break;
    }
    i++;
  }
  if (found == 1) {
    return(buf);
  } else {
    delete(buf);
    return NULL;
  }
}

/**
 * Ouvre recursivement les dependances de f
 *
 * @param f Le fichier dont on ouvre les dependances
 * @param files La liste de fichiers des dependances ouvertes
 * @param env L'environnement
 * @return Code d'erreur
 */

static int gel_recursive_open_aux(gel_file_t *f, gel_list_t files, gel_env_t *env) {
  int i;
  char *buf;
  gel_file_t *e;
  int found,ret;
  gel_file_s *fp = (gel_file_s *)f, *ep;

  /* pour chacune des librairies a ouvrir */
  for (i = 0; i < fp->needednum; i++)
  {
  	/* recherche la librairie dans le systeme de fichier */
    buf = gel_find_lib(fp->needed[i], env);
    if (buf == NULL)
    {
      gel_errno = GEL_EDEPEND;
      return -1;
    }

	/* recherche si la librairie a charger l'a deja ete ou non */
    gel_list_first(files);
    found = 0;
    while(!gel_list_end(files))
    {
      e = gel_list_get(files);
      if (!strcmp(e->filename, buf))
	  {
        found = 1;
        break;
      }
    }
    if (!found)
    {
      /* library not previously loaded */
      e = gel_open(buf, NULL, GEL_OPEN_NOPLUGINS);
      ep = (gel_file_s *)e;
      if (e == NULL)
      {
        delete(buf);
        return -1;
      }
      /* verifie que la plateforme soit identique entre librairies */
      if (gel_compare_plat(ep->plat,fp->plat) != 0)
      {
#ifndef DEBUG
        printf("DEBUG: platform error between %s and %s\n", e->filename, f->filename);
#endif
        gel_errno = GEL_EFORMAT;
        gel_close(e);
        delete(buf);
        return -1;
      }
      else
      {
      	/* enregistre la plateforme */
        delete(ep->plat);
        ep->plat = fp->plat;
        ep->plat->refcount++;
      }
      /* ajoute la librairie dans la liste des librairies deja ouvertes */
      gel_list_add(files, e);
      /* ouvre les dependances du fichier qui vient d'�tre ouvert (recursion)*/
      ret = gel_recursive_open_aux(e, files, env);
      if (ret == -1)
      {
        delete(buf);
        return -1;
      }
    }
    delete(buf);
  }
  return 0;
}

gel_file_t **gel_recursive_open(gel_file_t *f, int *num, gel_env_t *env) {
  gel_list_t files;
  int ret, i;
  gel_file_t *e;
  gel_file_t **tab;

  files = gel_list_new();
  if (files == NULL) {
    gel_errno = GEL_ERESOURCE;
    return NULL;
  }
  ret = gel_recursive_open_aux(f, files, env);
  *num = gel_list_length(files);
  tab = newv(gel_file_t *, (*num ? *num : 1));
  if (tab == NULL)
    ret = -1;

  gel_list_first(files);
  i = 0;
  while (!gel_list_end(files)) {
    e = gel_list_get(files);
    gel_list_remove(files, e);
    if (ret == -1) {
      gel_close(e);
    } else {
      tab[i] = e;
      i++;
    }
  }
  gel_list_delete(files);
  if (ret == -1) {
    delete(tab);
    return NULL;
  }
  return tab;
}


/**
 * Close all opended libraries files in an image, keeping only the memory
 * blocks.
 * @param im	Image to work on.
 */
void gel_close_libs(gel_image_t *im) {
	int i;
	assert(im != NULL);
	for (i = 0; i < im->membersnum; i++)
  		if(im->members[i]->container
  		&& im->members[i]->container->type == ET_DYN) {
			gel_close(im->members[i]->container);
			im->members[i]->container = NULL;
		}
}


/**
 * Close the image and unallocated its ressources (including opened execitable
 * file and program headers).
 * @param im		Image to close.
 * @ingroup image
 */
void gel_image_close(gel_image_t *im) {
	assert(im != NULL);
	gel_close_libs(im);
	gel_image_close_only(im);
}


/**
 * Close only the image data structure, not the opened files.
 * The latter must be closed by hand by the user.
 * @param im	Image to close.
 * @ingroup image
 */
void gel_image_close_only(gel_image_t *im) {
	int i;
	assert(im != NULL);

	/* Free blocks */
	for (i = 0; i < im->membersnum; i++) {

		/* Unlink from file */
		if (im->members[i]->container != NULL) {
			delete(im->members[i]->container->blocks);
			im->members[i]->container->blocks = NULL;
			im->members[i]->container->blknum = 0;
		}

    	/* free memory if there is no cluster */
    	if (im->memory == NULL)
			delete(im->members[i]->data);
		delete(im->members[i]);
	}

	/* Free the member list */
	delete(im->members);
	hash_free(im->syms);

	/* free the clusters */
	if (im->memory != NULL) {
		for (i=0; i< im->memory->nb_clusters; i++) {
  			delete(im->memory->clusters[i]->data);
  			delete(im->memory->clusters[i]);
  		}
  		delete(im->memory);
	}

	/* Free platform reference */
	im->plat->refcount--;
	if (im->plat->refcount == 0)  {
		gel_unload_plugins(im->plat);
		delete(im->plat);
	}

	/* Free the image */
	delete(im);
}


/**
 * Fonction utilitaire permettant de liberer un tableau de gel_file_t
 *
 * @param tab Le tableau
 * @param num La longueur du tableau
 */
void gel_kill_filetab(gel_file_t **tab, int num)
{
  int i;
  for (i = 0; i < num; i++)
  {
    gel_close(tab[i]);
  }
  delete(tab);
}


/**
 * Load the given file used as the executable and build the matching execution
 * image. According the needs of the executable, load also the required
 * libraries. The built image must be released using gel_image_close().
 *
 * @param e 	Executable file to load the image of.
 * @param flags Miscalleneous flags (GEL_IMAGE_CLOSE_EXEC, GEL_IMAGE_CLOSE_LIBS,
 *				GEL_IMAGE_PLTBLOCK_LAZY, GEL_IMAGE_PLTBLOCK_NOW).
 * @param env	Used environment.
 * @return		Built image or NULL if there is an error (code in gel_errno).
 * @ingroup image
 */
gel_image_t *gel_image_load(gel_file_t *e, gel_env_t *env, int flags) {
	gel_file_t **tab;
	int num, i;
	gel_image_t *im;
	gel_file_s *ep = (gel_file_s *)e;
	MARK;

	/* some checks */
	assert(e->type == ET_EXEC);
	assert(ep->plat->sys != NULL);

	/* open the libraries */
	MARK;
	tab = gel_recursive_open(e, &num, env);
	if(tab == NULL)
		return NULL;

	/* image allocation */
	MARK;
	im = new(gel_image_t);
	if(im == NULL)  {
		gel_errno = GEL_ERESOURCE;
		gel_kill_filetab(tab, num);
		return NULL;
	}
	memset(im, 0, sizeof(gel_image_t));
	im->plat = ep->plat;
	im->plat->refcount++;
	im->ventry = e->entry;
	im->membersnum = num;
	im->env = env;

	/* virtual address mapping by the system plugin */
	MARK;
	im->members = ep->plat->sys->plug_image_map(env, e, tab, &im->membersnum);
	if (im->members == NULL) {
		gel_kill_filetab(tab, num);
		delete(im);
		return NULL;
	}

	/*fprintf(stderr, "BEFORE\n");
	for(i = 0; i < im->membersnum; i++)
		if(im->members[i]->exec_begin >= 0x3000
		&& im->members[i]->exec_begin < 0x02000000)
			fprintf(stderr, "BLOCK %p (%08x) at %p/r=%p/v=%p\n", im->members[i]->exec_begin,
				im->members[i]->exec_size, im->members[i]->data,
				im->members[i]->base_raddr, im->members[i]->base_vaddr); */

	/* clustzerized memory creation */
	TRACE(fprintf(stderr, "gel_init_memory_image() ... "));
	if ((env->flags & GEL_ENV_CLUSTERISED_MEMORY) && env->clustersize > 0) {
		im->memory = gel_init_memory_image(env, im->members,im->membersnum);
		if (im->memory == NULL) {
			gel_kill_filetab(tab, num);
			delete(im->members);
   			delete(im);
			TRACE(fprintf(stderr, "failed\n"));
  	   		return NULL;
		}
		TRACE(fprintf(stderr, "success\n"));
	}
	else
		TRACE(fprintf(stderr, "no cluster\n"));

	/*fprintf(stderr, "AFTER\n");
	for(i = 0; i < im->membersnum; i++)
		if(im->members[i]->exec_begin >= 0x3000
		&& im->members[i]->exec_begin < 0x02000000)
			fprintf(stderr, "BLOCK %p (%08x) at %p/r=%p/v=%p\n", im->members[i]->exec_begin,
				im->members[i]->exec_size, im->members[i]->data,
				im->members[i]->base_raddr, im->members[i]->base_vaddr);*/

	/* load of the image by the system plugin */
	MARK;
	i = ep->plat->sys->plug_image_load(env, im->members, im->membersnum, &im->stack_pointer);
	if (i != 0) {
		MARK;
		gel_kill_filetab(tab, num);
		delete(im->members);
		delete(im);
		return NULL;
	}

  /* hashage des symboles */
  MARK;
  im->syms = gel_hash_new(211);
  if (im->syms == NULL)
  {
  	MARK;
    gel_errno = GEL_ERESOURCE;
    gel_kill_filetab(tab, num);
    delete(im->members);
    delete(im);
    return NULL;
  }

	MARK;
  if (gel_hash_syms(e, im->syms, 1) == -1) {
  	MARK;
    gel_kill_filetab(tab, num);
    hash_free(im->syms);
    delete(im->members);
    delete(im);
    return NULL;

  }

	MARK;
  for (i = 0; i < num; i++) {
    if (gel_hash_syms(tab[i], im->syms, 1) == -1) {
		MARK;
      gel_kill_filetab(tab, num);
      hash_free(im->syms);
      delete(im->members);
      delete(im);
      return NULL;
    }
  }

	/* Compute the real entry point */
	MARK;
	for(i = 0; i < e->blknum; i++)
		if(e->entry >= e->blocks[i]->exec_begin
		&& e->entry < e->blocks[i]->exec_begin + e->blocks[i]->exec_size) {
			im->rentry = VADDR2RADDR(e->blocks[i], e->entry);
			break;
		}
  ep->image = im;

  MARK;
  for (i = 0; i < num; i++)
  {
    ((gel_file_s *)tab[i])->image = im;
  }

  /* relocation + edition de liens dynamique */
  MARK;
  if (ep->plat->arch == NULL)
  {
    gel_errno = GEL_WDYNAMIC;
  }
  else
  {

  	if (ep->plat->arch->plug_do_reloc(im, flags) == -1)
  	{
#		ifndef NDEBUG
       		fprintf(stderr, "Warning: relocation failed: %s \n", gel_strerror());
#		endif
       gel_errno = GEL_WDYNAMIC;
     }
  }

	MARK;
  if (flags & GEL_IMAGE_CLOSE_LIBS)
  {
    gel_close_libs(im);
  }

  if (flags & GEL_IMAGE_CLOSE_EXEC)
  {
  	  gel_close(e);
  }

  /*for(i = 0; i < im->membersnum; i++)
  	if(im->members[i]->exec_begin == 0x00620000)
  		fprintf(stderr, "===> %p/%p: %08x%08x%08x%08x\n",
  			0x00620000, VADDR2RADDR(im->members[i], 0x00620000),
  			*(int32_t *)VADDR2RADDR(im->members[i], 0x00620000),
  			*(int32_t *)VADDR2RADDR(im->members[i], 0x00620004),
  			*(int32_t *)VADDR2RADDR(im->members[i], 0x00620008),
  			*(int32_t *)VADDR2RADDR(im->members[i], 0x0062000c));*/

  MARK;
  delete(tab);
 
  return im;
}

/**
 * Convert virtual relocated address to a real address.
 * @param a		Virtual address to convert.
 * @param im	Image containing the virtual address.
 * @return		Converted real address.
 * @ingroup image
 */
raddr_t gel_vaddr_to_raddr(vaddr_t a, gel_image_t *im) {
	int i;
	assert(gel_test_vaddr(a, im));
	for (i = 0; i < im->membersnum; i++) {
		if((a >= RADDR2VRELOC(im->members[i], im->members[i]->data)) &&
		(a < RADDR2VRELOC(im->members[i], im->members[i]->data + im->members[i]->exec_size)))
			return VRELOC2RADDR(im->members[i], a);
	}
	return NULL;
}


/**
 * Convert a real address to the matching virtual address.
 * @param a		Real address to convert.
 * @param im	Image containing the real address.
 * @return		Converted virtual address.
 * @ingroup image
 */
vaddr_t gel_raddr_to_vaddr(raddr_t a, gel_image_t *im) {
	int i;
	assert(gel_test_raddr(a, im));
	for (i = 0; i < im->membersnum; i++) {
		if (((u8_t*)a >= im->members[i]->data) &&
		((u8_t*)a < im->members[i]->data + im->members[i]->exec_size))
			return RADDR2VRELOC(im->members[i], a);
	}
	return 0;
}


/**
 * Test if a virtual address really exists in an image.
 * @param a		Virtual address to test.
 * @param im	Current image.
 * @return		1 if it exists, 0 else.
 */
int gel_test_vaddr(vaddr_t a, gel_image_t *im) {
	int i;
	for (i = 0; i < im->membersnum; i++) {
		if ((a >= RADDR2VRELOC(im->members[i], im->members[i]->data)) &&
		(a < RADDR2VRELOC(im->members[i], im->members[i]->data + im->members[i]->exec_size)))
			return 1;
	}
	return 0;
}


/**
 * Test if a real address is in a memory of an image.
 *
 * @param a		Real address to test.
 * @param im	Image to test with.
 * @return		1 if it is contained, 0 else.
 */
int gel_test_raddr(raddr_t a, gel_image_t *im) {
	int i;
	for (i = 0; i < im->membersnum; i++) {
		if (((u8_t*)a >= im->members[i]->data) &&
		((u8_t*)a < im->members[i]->data + im->members[i]->exec_size))
			return 1;
	}
	return 0;
}


/**
 * Return the execution environment configuration of the current image.
 * The returned information becomes invalid as soon as the image is closed.
 * @param i		Image descriptor.
 * @return		Image environment.
 * @ingroup image
 */
gel_env_t *gel_image_env(gel_image_t *i) {
	return(i->env);
}


/**
 * @def GEL_ENV_CLUSTERISED_MEMORY
 * Passed in the gel_env_t flags, activate the use of a clusterized memory
 * scheme.
 * @ingroup image
 */

/**
 * @def GEL_ENV_NO_STACK
 * Passed in the gel_env_t flags, disable the allocation of a stack
 * (often used in embedded code).
 * @ingroup image
 */

/**
 * @def STACKADDR_OSDEFAULT
 * Passed in @ref gel_env_t::stackaddr, gives the default stack address for
 * the current OS.
 * @ingroup image
 */

/**
 * @def STACKSIZE_OSDEFAULT
 * Passed in @ref gel_env_t::stacksize, select the default stack size for the
 * current OS.
 * @ingroup image
 */

/**
 * @def ENVFLAGS_OSDEFAULT
 * Passed in @ref gel_env_t::flags, select the default value for the current OS.
 * @ingroup image
 */

/**
 * @def CLUSTERSIZE_OSDEFAULT
 * Default memory cluster size as found in the environment.
 * @ingroup image
 */


/**
 * @def GEL_IMAGE_CLOSE_EXEC
 * Flag of gel_image_load(). Close the executable file after image building.
 * @ingroup image
 */

/**
 * @def GEL_IMAGE_CLOSE_LIBS
 * gel_image_load() flag. Close the library files after image building.
 * @ingroup image
 */

/**
 * @def GEL_IMAGE_PLTBLOCK_LAZY
 * gel_image_load() flag. Do not relocate the PLT, use a predefined block
 * instead allowing lazy resolution.
 * @ingroup image
 */

/**
 * @def GEL_IMAGE_PLTBLOCK_NOW
 * gel_image_load() flag. Replaced the file PLT a new custom one.
 * @ingroup image
 */


/**
 * @struct gel_env_s
 * Structure describing the execution environment to build an image
 * (includes arguments, environment variable, stack and so on).
 * @ingroup image
 */

/**
 * @var gel_env_s::libpath
 * List of libraries path (equivalent of LD_LIBRARY_PATH).
 */

/**
 * @var gel_env_s::argv
 * Arguments passed to the program (MUST be null ended).
 */

/**
 * @var gel_env_s::envp
 * List of the environment variables (MUST be null ended).
 */

/**
 * @var gel_env_s::pltblock
 * Custom PLT block.
 */

/**
 * @var gel_env_s::pltblocksize
 * size of the custom PLT block.
 */

/**
 * @var gel_env_s::stackaddr
 * Initial address of the stack pointer.
 */

/**
 * @var gel_env_s::stacktop
 * Top address of the stack block (may be NULL).
 */

/**
 * @var gel_env_s::stacksize
 * Size of the stack block (default to @ref STACKSIZE_OSDEFAULT).
 */

/**
 * @var gel_env_s::clustersize
 * Size of cluster used to allocate the image.
 */

/**
 * @var gel_env_s::flags
 * Miscalleneous flags from
 * @li @ref GEL_ENV_CLUSTERISED_MEMORY
 * @li @ref GEL_ENV_NO_STACK
 */

/**
 * @var gel_env_s::argv_return
 * This field, if not null, contains a pointer to return the argv array address
 * in the image memory space.
 */

/**
 * @var gel_env_s::envp_return
 * This field, if not null, contains a pointer to return the envp array address
 * in the image memory space.
 */

/**
 * @var gel_env_s::sp_return
 * This field, if not null, contains a pointer to return the stack pointer
 * address in the image memory space.
 */

/**
 * @var gel_env_s::auxv_return
 * This field, if not null, contains a pointer to return the auxiliairy vector
 * address in the image memory space.
 */

/**
 * @var gel_env_s::auxv
 * USed auxiliary vector to use in the image.
 */


/**
 * @typedef gel_image_t
 * Image descriptor handler.
 * Build by gel_image_load()
 * @ingroup image
 */


/**
 * @typedef gel_memory_image_t
 * Image memory descriptor.
 * @ingroup image
 */


/**
 * @typedef gel_memory_cluster_t
 * Descriptor of a contiguous memory area.
 * @ingroup image
 */


/**
 * @typedef gel_memory_cluster_info_t
 * Public information of an image, got using gel_image_cluster_infos().
 * @ingroup image
 */

/**
 * @var gel_memory_cluster_info_t::raddr
 * Contains the real address of the cluster.
 */

/**
 * @var gel_memory_cluster_info_t::vaddr
 * Contains the virtual address of the cluster.
 */

/**
 * @var gel_memory_cluster_info_t::size
 * Size of the cluster.
 */


/**
 * @typedef gel_memory_image_info_t
 * Public information of an image memory. Provided by the function
 * gel_image_memory_infos().
 * @ingroup image
 */

/**
 * @var gel_memory_image_info_t::clusters
 * Array of clusters.
 */

/**
 * @var gel_memory_image_info_t::nb_clusters
 * Number of clusters in the array.
 */


/**
 * @typedef gel_image_info_t
 * Public information of an image, extracted from the image descriptor.
 * Provided by the function gel_image_infos().
 * @ingroup image
 */

/**
 * @var gel_image_info_t::members
 * Memory blocks array of the image.
 */

/**
 * @var gel_image_info_t::membersnum
 * Number of blocks in the image.
 */

/**
 * @var gel_image_info_t::ventry
 * Virtual address of the program entry.
 */

/**
 * @var gel_image_info_t::rentry
 * Real address of the program entry.
 */

/**
 * @var gel_image_info_t::stack_pointer
 * Virtual address of the stack top.
 */

/**
 * @var gel_image_info_t::memory
 * Memory descriptor if a clusterized memory is used, NULL else.
 */


/**
 * Extract public information from the image.
 * Got information becomes invalided as soon as the image is released.
 *
 * @param i		Image descriptor.
 * @param ii	Structure to get image information.
 * @return		0 for sucess, -f for error (code in @ref gel_errno).
 * @ingroup image
 */
int gel_image_infos(gel_image_t *i, gel_image_info_t *ii)
{
  ii->members = i->members;
  ii->membersnum = i->membersnum;
  ii->rentry = i->rentry;
  ii->ventry = i->ventry;
  ii->memory = i->memory;
  ii->stack_pointer = i->stack_pointer;
  return 0;
}


/**
 * Extract public information about the image memory.
 * Got information becomes invalid as soon as the image is released.
 *
 * @param mi	Image memory descriptor.
 * @param mii	Structure to store image memory information in.
 * @return		0 for sucess, -1 else (error code in @ref gel_errno).
 * @ingroup image
 */
int gel_memory_image_infos (gel_memory_image_t *mi,
                            gel_memory_image_info_t *mii)
{
	assert(mi);
	assert(mii);
	mii->clusters = mi->clusters;
	mii->nb_clusters = mi->nb_clusters;
	return 0;
}


/**
 * Extract public information of an image memory cluster.
 * Got information becomes invalid as soon as the image is closed.
 *
 * @param mc pointeur vers le descripteur du cluster memoire
 * @param mci pointeur vers la structure devant �tre remplie par les infos
 * @return Code d'erreur (0 pour succes, -1 pour erreur)
 * @ingroup image
 */
int gel_memory_cluster_infos(gel_memory_cluster_t *mc,
                             gel_memory_cluster_info_t *mci)
{
	/* recopie simple des valeurs */
	mci->raddr = mc->data;
	mci->size  = mc->size;
	mci->vaddr = mc->vaddr;
	return 0;
}


