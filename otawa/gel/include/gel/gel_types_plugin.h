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
#ifndef GEL_TYPES_PLUGIN_H_
#define GEL_TYPES_PLUGIN_H_

#include <gel/common.h>
#include <gel/image.h>
#include <gel/types.h>

__BEGIN_DECLS

/* sys_plugin_t type */
typedef struct sys_plugin_s {
	u8_t system;
	u8_t version;
	void *dlh;
	gel_block_t **(*plug_image_map) (gel_env_t *env, gel_file_t *f,
			gel_file_t **tab, u32_t *num);
	int (*plug_image_load) (gel_env_t *env, gel_block_t **tab, u32_t num,
			vaddr_t* stack_pointer);
} sys_plugin_t;


/* arch_plugin_t type */
typedef struct arch_plugin_s {
	u16_t machine;
	u8_t version;
	u8_t stack_align;
	void *dlh;
	int (*plug_do_reloc)(gel_image_t *r, int flags);
	u32_t align;
	u32_t psize;
} arch_plugin_t;


/**
 * Structure de donnee decrivant une plateforme. (type de CPU et autre
 * parametres, endianness, .. )
 */
struct gel_platform_s {
  u16_t machine; /**< Machine tel que defini dans la specification ELF */
  u8_t system; /**< Systeme d'exploitation */
  int endianness; /**< Little endian ou big endian */
  int float_type; /**< Type de representation de flottant */
  int refcount; /**< Nombre de fois que cette plateforme est liee a un objet */
  sys_plugin_t *sys; /**< Structure d'information du plug-in systeme */
  arch_plugin_t *arch; /**< Structure d'information du plug-in d'archi */
};

/**
 * Structure decrivant l'image d'un fichier ELF en memoire (contrairement
 * a image_t il ne contient qu'un fichier ELF et non un processus entier.)
 * Il contient les donnees de tout les segments du fichier ainsi que les
 * informations de mapping adresse_reelle <==> adresse_virtuelle
 */
struct gel_block_s {
  gel_file_t *container; 			/**< Container file. */
  u8_t *data;						/**< Data of the block. */
  raddr_t base_raddr;				/**< Real base address (0 if not loaded). */
  vaddr_t base_vaddr;				/**< Virtual base address (0 not loaded). */
  vaddr_t exec_begin;				/**< Virtual address of the begin of the image area. */
  u32_t exec_size;					/**< Size of the image area. */
  gel_memory_cluster_t *cluster;	/**< Cluster for grouped allocation (deprecated). */
  gel_platform_t *plat;				/**< Current platform. */
};

__END_DECLS

#endif /*GEL_TYPES_PLUGIN_H_*/
