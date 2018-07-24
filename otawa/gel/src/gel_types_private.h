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
#ifndef GEL_TYPES_PRIVATE_H_
#define GEL_TYPES_PRIVATE_H_

#include <gel/prog.h>
#include <gel/gel_types_plugin.h>

/**
 * structure d'Enumeration (comme la classe Enumeration en Java)
 * Ceci fournit une liste pouvant etre parcourue a l'aide des fonctions
 * "initpos" et "next"
 * C'est une "interface", les pointeurs de fonctions dans la structure doivent
 * pointer vers une implementation reelle des fonctions.
 */
struct gel_enum_s {
  void *pos_info; /**< Information de position actuelle (dependant de l'implementation) */
  void *obj; /**< Objet associe a l'enumeration (liste, table, ..) */
  void (*initpos)(struct gel_enum_s *); /**< Pointeur de fonction sur l'implementation d'initpos */
  void *(*next)(struct gel_enum_s *); /**< Pointeur de fonction sur l'implementation d'initpos */
  void (*free)(struct gel_enum_s *);  /**< Pointeur de fonction sur l'implementation d'initpos */
};




/**
 * Description of an ELF file.
 */
typedef struct gel_file_s {
	gel_file_info_t _;		/**< Public fields. */
	int fd; 				/**< File descriptor to the ELF file. */
	gel_image_t *image;		/**< Built image. */
	u32_t needednum;		/**< Library number. */
	u32_t sectoff;			/**< Offset of the section header table. */
	u32_t progoff;			/**< Offset of the program header table. */
	gel_sect_t **sects;		/**< Table of section headers. */
	gel_prog_t **progs;		/**< Table of program headers. */
	gel_hash_t syms;		/**< Table of symbols. */
	char **needed;			/**< Table of libraries names. */
	u32_t str;				/**< String table section index. */
	gel_platform_t *plat;	/**< Associated platform. */
	gel_block_t **blocks;	/**< Table of allocated blocks. */
} gel_file_s;



/**
 * Structure contenant les donnees privees specifique a une section
 */
struct gel_sect_s {
  gel_file_t *container; /**< Fichier ELF dans lequel reside la section */
  u16_t sect_index; /**< Index de la section */
  raddr_t data; /**< Donnees de la section (lorsqu'elle est chargee, sinon NULL) */
  u32_t link; /**< Section associee (strtab, ou autre) */
  u32_t info; /**< Section associee (BIS) */
  char *name; /**< Nom de la section */
  u32_t type; /**< Type de la section tel que defini par la spec ELF */
  vaddr_t vaddr; /**< Adresse virtuelle du debut de la section */
  u32_t offset; /**< Offset dans le fichier du debut de la section */
  u32_t size; /**< Taille (octets) */
  u32_t flags; /**< Flags tel que definis par la spec ELF */

};

/**
 * Description of a program header.
 */
typedef struct gel_prog_s {
	gel_prog_info_t _;	/**< Public information. */
	u16_t index;		/**< Program header index. */
	raddr_t data;		/**< Segment address in host memory. */
} gel_prog_s;

/**
 * cluster memoire
 */
struct gel_memory_cluster_s
{
	u8_t* data; /**< pointeur vers la zone memoire reelle du cluster */
	vaddr_t vaddr; /**< adresse virtuelle de debut de ce cluster */
	u32_t size;   /**< taille du cluster en octets */
};

/**
 * representation memoire d'une image
 * permet de clusteriser l'image memoire
 */
struct gel_memory_image_s {
	gel_memory_cluster_t** clusters;/**< tableau de pointeurs vers les fragments de memoire */
	u32_t nb_clusters; /**< nombre de fragements de memoire */
};

/**
 * Structure contenant les donnees privees specifiques a une image
 */
struct gel_image_s {
  gel_platform_t *plat; /**< Plateforme associee a l'Image */
  gel_block_t **members; /**< Tableau de pointeurs sur les blocks de l'image */
  gel_memory_image_t *memory; /**< pointeur vers la representation de la memoire
                                   de l'image */
  gel_hash_t syms; /**< Table de hashage des symboles globaux de l'image */
  u32_t membersnum; /**< Nombre de fichiers composant l'image */
  vaddr_t ventry; /**< Adresse virtuelle du point d'entree */
  raddr_t rentry; /**< Adresse reelle du point d'entree */
  vaddr_t stack_pointer; /** adresse virtuelle de sommet de pile */
  gel_env_t *env; /**< Environnement de l'image */
};

/**
 * Structure contenant les donnees privees specifiques a un symbole.
 */
struct gel_sym_s {
#define GEL_SYMCONT_FILE	1
#define GEL_SYMCONT_BLOCK	2
  u8_t ct; /**< Type de container: FILE ou BLOCK */
  /**
   * Union representant le container (soit file, soit block)
   */
  union {
    gel_file_t *file; /**< Contenant FILE */
    gel_block_t *block; /**< Contenant BLOCK */
  } cont;
  vaddr_t vaddr; /**< Adresse virtuelle du symbole */
  u16_t sect; /**< Section a laquelle le symbole appartient */
  u8_t info; /**< Infos symbole tel que defini dans la spec ELF */
  u32_t size; /**< Taille du symbole */
  char *name; /**< Nom du symbole */
};

/* private primitives */
int gel_sect_load(gel_sect_t *s, raddr_t addr);
void gel_load_plugins(gel_platform_t *f, const char *plugpath, int quiet);
void gel_unload_plugins(gel_platform_t *pf);
int gel_hash_syms(gel_file_t *f, gel_hash_t hash, int no_local);

#endif /*GEL_TYPES_PRIVATE_H_*/
