/*
 * $Id$
 * Copyright (c) 2011, IRIT- UPS
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
#ifndef GEL_UTIL_H
#define GEL_UTIL_H

#include <gel/common.h>
#include <gel/types.h>

__BEGIN_DECLS

/* list management */
typedef struct descripteur *gel_list_t;
gel_list_t gel_list_new(void);
void gel_list_first(gel_list_t desc);
int gel_list_end(gel_list_t desc);
void *gel_list_get(gel_list_t desc);
int gel_list_contains(gel_list_t desc, void *val);
int gel_list_add(gel_list_t desc, void *val);
int gel_list_remove(gel_list_t desc, void *val);
void gel_list_clear(gel_list_t desc);
void gel_list_delete(gel_list_t desc);
void gel_list_delete_items(gel_list_t desc, void (*free)(void *));
int gel_list_length(gel_list_t desc);
int gel_list_empty(gel_list_t a);


/* hash table management */
typedef struct hashage *gel_hash_t;
gel_hash_t gel_hash_new(int size);

/**
 * Rentre un couple (cle,valeur) dans la table de hachage
 *
 * @param hash La table de hachage
 * @param key La cle
 * @param value La valeur
 * @return Code d'erreur
 */
int hash_put(gel_hash_t hash, char *key, void *value);

/**
 * Recupere une valeur de la table de hachage
 *
 * @param key La cle
 * @param hash La table de hachage
 * @return La valeur, ou NULL
 */
void *hash_get(gel_hash_t hash, char *key);

/**
 * Supprime un element de la table de hachage
 *
 * @param hash La table de hachage
 * @param key La cle
 * return Code d'erreur
 */
int hash_del(gel_hash_t hash, char *key);

/**
 * Libere une table de hachage
 *
 * @param hash La table de hachage
 */
void hash_free(gel_hash_t hash);

/**
 * Renvoie une enumeration a partir d'une table de hachage
 *
 * @param h Une table de hachage
 * @return Une enumeration de la table de hachage
 */
gel_enum_t *hash_enum(gel_hash_t h);


void hash_enum_free(gel_enum_t *en);



/**
 * Un element d'une table de hachage
 */
struct item {
  char *key; /**< Cle */
  void *value; /**< Valeur */
  struct item *next; /**< Prochain (liste chainee pour eviter les collisions) */
};


typedef struct item *item_t;

/* Straight enumeration of hash table */
typedef struct gel_hash_enum_t {
	gel_hash_t htab;
	int index;
	struct item *item;
} gel_hash_enum_t;
void *gel_hash_enum_init(gel_hash_enum_t *en, gel_hash_t htab);
void *gel_hash_enum_next(gel_hash_enum_t *en);
void gel_hash_enum_end(gel_hash_enum_t *en, gel_hash_t htab);
#define gel_hash_key(en)	((en)->item->key)
#define gel_hash_value(en)	((en)->item->value)

__END_DECLS

#endif	// GEL_UTIL_H

