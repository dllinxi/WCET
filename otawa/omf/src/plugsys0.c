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
 * \file Ce fichier est le plug-in systeme par defaut.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gel/error.h>
#include <gel/gel_plugin.h>
#include <gel/gel_elf.h>
#include "gel_mem.h"

#define ALIGN(v, p2)	((v + (1 << p2) - 1) & ~((1 << p2) - 1))
#ifdef NDEBUG
#	define TRACE
#else
#	define TRACE	/*fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);*/
#endif

#define CONTAINS(b, a)	((b)->exec_begin <= (a) && (a) < (b)->exec_begin + (b)->exec_size)

/**
 * @struct sys_plugin_s
 * Plugin for handling system specific things.
 */

/**
 * @var sys_plugin_s sys_plugin_s::system
 * System identifier.
 */

/**
 * @var sys_plugin_s sys_plugin_s::version;
 * Version of the plug-in.
 */

/**
 * @var sys_plugin_s sys_plugin_s::dlh;
 * DL-handle of the plug-in.
 */

/**
 * @var sys_plugin_s sys_plugin_s::plug_image_map
 * Function that customize the loading from the file, possibly fixing virtual
 * addresses or adding new memory blocks.
 */

/**
 * @var sys_plugin_s sys_plugin_s::plug_image_load
 * Function that builds the image from the file.
 */

extern int gel_errno;

#define MARK	// fprintf(stderr, "MARK %s:%d\n", __FILE__, __LINE__)

/*
 * Valeurs par defaut pour la pile
 */
#define STACKSIZE_THISDEFAULT 65536
#define STACKADDR_THISDEFAULT 0xc0000000

/**
 * Calcule l'espace a reserver pour la pile
 *
 * @param env L'environnement.
 * @param plat La plateforme
 * @param limit La limite inferieure de la pile.
 * @return Un nouveau bloc contenant la pile.
 */
static gel_block_t * map_stack (gel_env_t *env, gel_platform_t *plat, vaddr_t limit) {
	int i;
	u32_t stacksize;
    vaddr_t stackaddr;
	gel_block_t* stack_block ;

	/* allocation memoire du descripteur du block de la pile */
	stack_block = new(gel_block_t);
	if (stack_block == NULL)
	{
    	gel_errno = GEL_ERESOURCE;
    	return NULL;
  	}

  	/* recuperation de la taille et adresse de pile
  	 * /!\ on prends pour hypothese que l'adresse de debut de pile est
  	 *    correctement allignee*/
  	stacksize = (env->stacksize == STACKSIZE_OSDEFAULT) ?
  					 STACKSIZE_THISDEFAULT : env->stacksize;
    stackaddr = (env->stackaddr == STACKADDR_OSDEFAULT) ?
    				 STACKADDR_THISDEFAULT : env->stackaddr;



	/* calcul de l'adresse du debut de block de pile */
	stack_block->base_vaddr = 0;
	/* aligmement du debut de pile */
	stack_block->exec_begin  =  stackaddr - stacksize ;
	stack_block->exec_begin -= (stack_block->exec_begin % plat->arch->align);

	/* fin de remplissage de la structure*/
	stack_block->exec_size  = stackaddr - stack_block->exec_begin+1;
	stack_block->plat = plat;
	stack_block->base_raddr = 0;
	stack_block->data = NULL;
	stack_block->container = NULL;

	/* check to avoid stack conflict */
	if(stack_block->exec_begin < limit) {
			gel_errno = GEL_ESTACK;
			delete(stack_block);
			return NULL;
		}

  	return stack_block;
}


/**
 * Build the stack in the image.
 *
 * @param env 			Current environment.
 * @param plat 			Current platform.
 * @param stack_block	Pointer to the stack block descriptor.
 * @param stack_pointer Pointer to the stack top.
 * @return				New block containing the stack.
 */
static int build_stack(
	gel_env_t* env,
	gel_block_t* stack_block,
	gel_platform_t *plat,
	vaddr_t* stack_pointer)
{
	gel_cursor_t bc;
	vaddr_t ptr;
	int num_arg, num_env, i, num_aux;
	u32_t init_size;

	/* allocate the map if it is not already mapped */
	if(stack_block->data == NULL) {
		stack_block->data = newv(char, stack_block->exec_size);
		if(stack_block->data == NULL)  {
			gel_errno = GEL_ERESOURCE;
			delete(stack_block);
			return -1;
		}
		stack_block->base_raddr = (raddr_t)(stack_block->data - stack_block->exec_begin);
	}

	/* compute initial stack size (arg, env., ..) */
	init_size = 0;

	/* count arguments */
	num_arg = 0;
	if(env->argv)
		for(num_arg = 0; env->argv[num_arg] != NULL; num_arg++);

	/* count environment variables */
	num_env = 0;
	if(env->envp)
		for(; env->envp[num_env] != NULL; num_env++);

	/* count auxiliairy vectors */
	num_aux = 0;
	if(env->auxv)
		for(num_aux = 1; env->auxv[num_aux].a_type != AT_NULL; num_aux++);

	/* compute memory required by arguments */
	ptr = 0;
	for (i = 0; i < num_arg; i++)
		ptr += strlen(env->argv[i]) + 1;

	/* compute memory required by environnement */
	for (i = 0; i < num_env; i++)
		ptr += strlen(env->envp[i]) + 1;

	/* compute used stack size */
	init_size = (num_arg + num_env + 2 ) * sizeof(vaddr_t) + ptr + sizeof(u32_t);
	init_size += num_aux * sizeof(auxv_t);
	/* Fixed by casse@irit.fr */
	init_size = ALIGN(init_size, plat->arch->stack_align);

	/* if required, move the stack top previously mapped so that top points
	 * to the right position. */
	if(env->stacktop) {
		fprintf(stderr, "WARNING: moving stack so that stacktop = 0x%08x\n", env->stacktop);
		u32_t stacksize = (env->stacksize == STACKSIZE_OSDEFAULT) ?
				STACKSIZE_THISDEFAULT : env->stacksize;
		vaddr_t stackaddr;
		stackaddr = env->stacktop + init_size;
		stack_block->exec_begin  =  stackaddr - stacksize ;
		stack_block->exec_begin -= (stack_block->exec_begin % plat->arch->align);
		stack_block->exec_size  = stackaddr - stack_block->exec_begin+1;
		stack_block->base_raddr = (raddr_t) (stack_block->data - stack_block->exec_begin);
	}

	/* update the current stack pointer */
	/* !!TODO!! very ugly patch for bad exec_size (???) */
	*stack_pointer = stack_block->exec_begin + stack_block->exec_size
		-1 - init_size;
	env->sp_return = *stack_pointer;

	/* Creation d'un curseur pour l'ecriture dans la pile */
	if(gel_block2cursor(stack_block, &bc) == -1) {
		delete(stack_block->data);
		delete(stack_block);
		return -1;
	}

	/* se deplace au sommet de la pile */
	gel_move_abs(&bc, VADDR2RADDR(stack_block,*stack_pointer));
	/* TODO : A quoi sert l'aligmement du curseur ??? */
	/* gel_align_lo(&bc); */
	if (gel_cursor_bounds(&bc) == -1)  {
		delete(stack_block->data);
		delete(stack_block);
		gel_errno = GEL_ERESOURCE;
		return -1;
	}

	/* ecrit les donnees necessaires dans la pile en descendant */
	env->argc_return = num_arg;
	gel_write_u32(bc, num_arg); /* write argc on stack */

	/* write argv[] pointers */
	env->argv_return = gel_cursor_vaddr(bc);
	ptr = *stack_pointer + (num_arg + num_env + 2) * sizeof(vaddr_t)
		+ num_aux * sizeof(auxv_t) + sizeof(u32_t);
	for (i = 0; i < num_arg; i++) {
		//gel_write_u32(bc, stack_block->exec_begin + (num_arg+num_env+2)*sizeof(vaddr_t) + ptr);
		gel_write_u32(bc, ptr);
		ptr += strlen(env->argv[i]) + 1;
	}
	gel_write_u32(bc, 0); /* write null u32 */

	/* write envp[] pointers */
	env->envp_return = gel_cursor_vaddr(bc);
	for (i = 0; i < num_env; i++) {
		gel_write_u32(bc, ptr);
		ptr += strlen(env->envp[i]) + 1;
	}
	gel_write_u32(bc, 0); /* write null u32 */

	/* write the auxiliairy vector */
	if(env->auxv) {
		env->auxv_return = gel_cursor_vaddr(bc);
		gel_write_data(bc, env->auxv, num_aux * sizeof(auxv_t));
	}

	/* write argv strings */
	ptr = *stack_pointer + (num_arg + num_env + 2)*sizeof(vaddr_t)
		+ num_aux * sizeof(auxv_t) + sizeof(u32_t);
	for (i = 0; i < num_arg; i++) {
		gel_write_block(&bc, env->argv[i], strlen(env->argv[i]) + 1,
				VADDR2RADDR(stack_block, ptr));
		ptr += strlen(env->argv[i]) + 1;
	}

	/* write envp strings */
	for (i = 0; i < num_env; i++) {
		gel_write_block(&bc, env->envp[i], strlen(env->envp[i]) + 1,
				VADDR2RADDR(stack_block, ptr));
		ptr += strlen(env->envp[i]) + 1;
	}

	/**stack_pointer -= sizeof(vaddr_t);*/
	return 0;
}


/**
 * Cette fonction charge les informations (adresses virtuelles, tailles...)
 * des fichiers gel_file_t (f et elements de tab[]) qu'elle recoit en parametre
 * elle decide des adresse virtuelles compatible avec le systeme d'exploitation
 * La fonction peut aussi creer des blocs supplementaires, comme par exemple
 * pour la pile.
 *
 * @param env L'environnement
 * @param f L'executable
 * @param tab Tableau des librairies dont l'executable depend
 * @param num Pointeur sur le nombre de blocs/futur blocs (sans compter le bloc de l'executable f). Peut etre modifie par la fonction.
 * @return Le tableau des blocs nouvellement crees.
 */
gel_block_t ** plugin_image_map (
	gel_env_t *env,
	gel_file_t *f,
	gel_file_t **tab,
	u32_t *num
) {
	vaddr_t vaddr, top_vaddr;
	gel_block_t **members;
	int i, j, cnt = 0, k;
	gel_file_info_t fi,fp;
	assert(env);
	assert(f);
	assert(tab);
	assert(num);

	/* Get the file information */
	TRACE;
	if (gel_file_load_info(f, 0) == -1)
		return NULL;

	/* Build executable information */
	TRACE;
	gel_file_infos(f, &fp);
	vaddr = 0;

	/* Process all libraries */
	TRACE;
	for (i = -1; i < (int)*num; i++)  {
		gel_file_t *file;

		/* Select the file */
		TRACE;
		if(i == -1)
			file = f;
		else
			file = tab[i];

		/* Get library information */
		TRACE;
    	gel_file_infos(file, &fi);
		if (i >= 0 && fi.type != ET_DYN)  {
			gel_errno = GEL_EFORMAT;
			return NULL;
		}

		/* Download the information */
		TRACE;
		if (gel_file_load_info(file, vaddr) == -1)
			return NULL;
		gel_file_infos(file, &fi);
		cnt += fi.blknum;

		/* - Calcul de l'adresse virtuelle de base de la prochaine librairie */
		/* 1 -> recuperation de l'adresse virtuelle relogee de la fin de la
		 * librairie courrante */
		TRACE;
		top_vaddr = vaddr;
		for(j = 0; j < fi.blknum; j++) {
			TRACE;
			/* vaddr_t nvaddr = vaddr + fi.blocks[j]->exec_begin;						!! Definitively a mistake !! */
			vaddr_t nvaddr = vaddr + fi.blocks[j]->exec_begin + fi.blocks[j]->exec_size;	/* !!TODO!! test it! */
			TRACE;
			if(nvaddr > top_vaddr)
				top_vaddr = nvaddr;
			TRACE;
			// RADDR2VRELOC(fi.block, fi.block->data + fi.block->exec_size);
		}
		TRACE;
		/* 2 -> calcul de l'adresse virtuelle de la prochaine page memoire */
		vaddr = (top_vaddr + fi.blocks[0]->plat->arch->psize - 1) & ~(fi.blocks[0]->plat->arch->psize - 1);
		TRACE;
	}

	/* remplissages des infos sur les differents blocks charges en memoire */
	/* allocation */
	TRACE;
	if(!(env->flags & GEL_ENV_NO_STACK))
		cnt++;
	members = newv(gel_block_t*, cnt); /* 2 pour fichier principal et pile*/
	if (members == NULL) {
		gel_errno = GEL_ERESOURCE;
		return NULL;
	}

	/* Initialize the blocks */
	TRACE;
	j = 0;
	for (i = -1 ; i < (int)*num; i++)  {
		gel_file_t *file;

		/* Select the file */
		if(i == -1)
			file = f;
		else
			file = tab[i];

		/* Process it */
		gel_file_infos(file, &fi);
		for(k = 0; k < fi.blknum; k++) {
			/*fprintf(stderr, "members[%d] = fi.blocks[%d] = %p\n",
				j, k, fi.blocks[k]);*/
			members[j++] = fi.blocks[k];
		}
	}

	/* mappage de la pile */
	TRACE;
	if(!(env->flags & GEL_ENV_NO_STACK)) {
		members[cnt - 1] = map_stack(env, fp.blocks[0]->plat, vaddr);
		if (members[cnt - 1] == NULL) {
			MARK;
			return NULL;
		}
	}
	*num = cnt;
	return members;
}


 /**
  * Cette fonction charge les donnees des gel_file_t (elements de tab[])
  * qu'elle recoit en parametre, aux adresse virtuelles decid�es anterieurement
  * par plugin_image_map
  *
  * Pour l'allocation memoire des blocks : 2 cas se presentent
  * - 1 : bloc->data est defini, on copie directement le bloc a cette adresse
  * - 2 : bloc->data == NULL, on implante le bloc dans un emplacement memoire
  *       alloue par malloc
  *
  * /!\ cette fonction doit �tre appelee apres l'appel de "plugin_image_map"
  *
  * @param env L'environnement
  * @param tab Tableau des blocs a charger (est egal au tableau retourn� par
  *            plugin_image_map)
  * @param num nom�bre de blocs
  * @param stack_pointer pointeur vers l'addresse virtuelle du sommet de pile
  *                      (si pas de pile la valeur pointee doit etre negative
  * @return -1 si une erreur est rencontree, 0 sinon
  */
int plugin_image_load (
	gel_env_t *env,
	gel_block_t **tab,
	u32_t num,
	vaddr_t* stack_pointer
) {
	int i;

	/* Load data for all blocks */
	for (i = 0; i < num; i++) {
		if(tab[i]->container
		&& gel_file_load_data(tab[i]->container, tab[i], tab[i]->data) == -1)
			return -1;
	}
			
	/* Fill the stack */
	if(env->flags & GEL_ENV_NO_STACK)
		return 0;
	else
		return  (build_stack(env, tab[num-1],tab[num-1]->plat,stack_pointer));
}

/**
 * Null plugin for architecture.
 */
sys_plugin_t null_plugin_sys = {
  0,
  1,
  NULL,
  plugin_image_map,
  plugin_image_load
};
