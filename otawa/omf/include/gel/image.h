/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2008, IRIT- UPS
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
#ifndef GEL_IMAGE_H_
#define GEL_IMAGE_H_

#include <gel/common.h>
#include <gel/types.h>
#include <gel/file.h>

__BEGIN_DECLS

/* auxiliairy vector */
typedef struct auxv_t {
	int	a_type;
	union {
		int32_t a_val;
		void *a_ptr;
		void (*a_fcn)();
	} a_un;
} auxv_t;


/* environment flags */
#define GEL_ENV_CLUSTERISED_MEMORY	0x00000001
#define GEL_ENV_NO_STACK			0x00000002


/* environment default values */
#define STACKADDR_OSDEFAULT 0
#define STACKSIZE_OSDEFAULT 0
#define ENVFLAGS_OSDEFAULT 0
#define CLUSTERSIZE_OSDEFAULT 4096


/* environment */
typedef struct gel_env_s {
	char **libpath;
	char **argv;
	char **envp;
	char *pltblock;
	u32_t pltblocksize;
	vaddr_t stackaddr;
	vaddr_t stacktop;
	u32_t stacksize;
	u32_t clustersize;
	u32_t flags;
	int argc_return;
	vaddr_t argv_return;
	vaddr_t envp_return;
	vaddr_t sp_return;
	vaddr_t auxv_return;
	struct auxv_t *auxv;
} gel_env_t;


/* other types */
typedef struct gel_image_s gel_image_t;
typedef struct gel_memory_image_s gel_memory_image_t;
typedef struct gel_memory_cluster_s gel_memory_cluster_t;


/* cluster information */
typedef struct gel_memory_cluster_info_t {
	u8_t* raddr;
	vaddr_t vaddr;
	u32_t size;
} gel_memory_cluster_info_t;


/* image memory information */
typedef struct gel_memory_image_info_t {
	struct gel_memory_cluster_s** clusters;
	u32_t nb_clusters;
} gel_memory_image_info_t;


/* image information */
typedef struct gel_image_info_t {
	gel_block_t **members;
	u32_t membersnum;
	vaddr_t ventry;
	raddr_t rentry;
	vaddr_t stack_pointer;
	gel_memory_image_t* memory;
} gel_image_info_t;


/* image block information */
typedef struct gel_block_info_t {
  gel_file_t *container;
  vaddr_t base_vaddr;
  raddr_t base_raddr;
  u32_t vreloc;
} gel_block_info_t;


/* gel_image_load flags */
#define GEL_IMAGE_CLOSE_EXEC	0x00000001
#define GEL_IMAGE_CLOSE_LIBS	0x00000002
#define GEL_IMAGE_PLTBLOCK_LAZY 0x00000004
#define GEL_IMAGE_PLTBLOCK_NOW  0x00000008


/* primitives */
int gel_block_infos(gel_block_t *b, gel_block_info_t *bi);
void gel_close_libs(gel_image_t *im);
gel_env_t *gel_default_env();
void gel_image_close(gel_image_t *i);
void gel_image_close_only(gel_image_t *i);
gel_enum_t *gel_enum_glob_symbol(gel_image_t *im);
gel_sym_t *gel_find_glob_symbol(gel_image_t *i, char *name);
gel_env_t *gel_image_env(gel_image_t *i);
int gel_image_infos(gel_image_t *i, gel_image_info_t *ii);
gel_image_t *gel_image_load(gel_file_t *e, gel_env_t *env, int flags);
int gel_memory_image_infos(gel_memory_image_t *mi, gel_memory_image_info_t *mii);
vaddr_t gel_raddr_to_vaddr(raddr_t a, gel_image_t *i);
int gel_test_raddr(raddr_t a, gel_image_t *i);
int gel_test_vaddr(vaddr_t a, gel_image_t *i);
raddr_t gel_vaddr_to_raddr(vaddr_t a, gel_image_t *i);
int gel_memory_cluster_infos(gel_memory_cluster_t *mc,  gel_memory_cluster_info_t *mci);
void gel_replacesym(gel_image_t *im, char *symname, gel_sym_t *newsym);

__END_DECLS

#endif /* GEL_IMAGE_H_ */
