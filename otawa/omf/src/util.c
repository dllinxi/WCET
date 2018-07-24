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

#include <stdlib.h>
#include <string.h>

#include <gel/util.h>
#include "gel_types_private.h"
#include "gel_mem.h"

/**
 * @mainpage Introduction
 * 
 * GEL is a library dedicated to the management of executable binary
 * files. For now, it supports ELF 32-bits (Exchange and Linked Format).lo
 * It allows either to load and examine a binary file structure
 * and also to build an image ready for execution for single files
 * or to support also librarues for PowerPC and x86.
 * 
 * It has been successfully on several architecture including (but not limited to):
 * @li PowerPC
 * @li ARM
 * @li Sparc
 * @li TriCore
 * @li M68HCS
 * @li x86
 * And possibly for all architecture based on ELF.
 * 
 * In addition, it supports debugging line information decoding for formats:
 * @li DWARF 3.0
 * @li GNU STAB
 */


/**
 * @defgroup util	Utilities Functions
 * 
 * This module provides several utilities function and data structure
 * to represent and handle efficiently data of the ELF files.
 */


/**
 * C'est une cellule de liste chainee
 */
struct ensemble {
  void *val;
  struct ensemble *next;
};


/**
 * Fonction de hachage standard
 *
 * @param id La chaine a hacher
 * @return La valeur de hachage
 */
static int hash_func(char *id) {
 uint32_t h = 0, g;
       while(*id) {
           h = (h << 4) + *id;
           if ((g = h) & 0xf0000000) {
               h = h ^ (g >> 24);
               h = h ^ g;
           }
           id++;
       }
       return h;
}

/**
 * Descripteur permettant d'acceder a une liste chainee;
 */
struct descripteur {
  struct ensemble *tete;
  struct ensemble *encours;
};


/**
 * Structure de position de hash_t
 */
struct hashpos_s {
  struct item *cur; 	/**< Position dans la liste chain�e */
  int cval; 			/**< Valeur de hachage */
};

/**
 * Table de hachage
 */
struct hashage {
  int size; /**< Taille de la table */
  struct item **h; /**< Liste chainee (pour les collisions) */
};

/* implementation de enum pour les hashtables */

/**
 * Implementation de enum_free pour les tables de hachage
 * Libere l'enumeration
 *
 * @param en Une enumeration
 */
void hash_enum_free(gel_enum_t *en) {
  delete(en->pos_info);
  delete(en);
}


/**
 * Implementation de enum_initpos pour les tables de hachage
 * Remet � zero la position
 *
 * @param en Une enumeration
 */
void hash_initpos(gel_enum_t *en) {
  struct hashpos_s *hp = en->pos_info;
  gel_hash_t obj = en->obj;
  hp->cur = obj->h[0];
  hp->cval = 0;
}


/**
 * Impl�mentation de enum_next pour les tables de hachage
 * Renvoie le prochain element, et avance
 *
 * @param en Une enumeration
 * @return Le prochain element
 */
void *hash_nextkey(gel_enum_t *en) {
  struct hashpos_s *hp = en->pos_info;
  gel_hash_t obj = en->obj;
  int i;
  item_t item;

  item = hp->cur;
  for (i = hp->cval ; i < obj->size; i++) {
    if (item != NULL) {
      hp->cur = item->next;
      return(item->key);
    }
    item = obj->h[i];
    hp->cval++;
  }
  return NULL;
}


/**
 * Renvoie une enumeration a partir d'une table de hachage
 *
 * @param h Une table de hachage
 * @return Une enumeration de la table de hachage
 */
gel_enum_t *hash_enum(gel_hash_t h) {
  gel_enum_t *tmp = new(struct gel_enum_s);
  if (tmp == NULL)
    return NULL;
  tmp->obj = h;
  tmp->pos_info = new(struct hashpos_s);
  if (tmp->pos_info == NULL) {
    delete(tmp);
    return(NULL);
  }
  tmp->free = hash_enum_free;
  tmp->initpos = hash_initpos;
  tmp->next = hash_nextkey;
  return(tmp);
}


/**
 * Rentre un couple (cle,valeur) dans la table de hachage
 *
 * @param hash La table de hachage
 * @param key La cle
 * @param value La valeur
 * @return Code d'erreur
 */
int hash_put(gel_hash_t hash, char *key, void *value) {
  item_t item;
  int hv = hash_func(key) % hash->size;

  item = new(struct item);
  if (item == NULL) {
    return -1;
  }

  item->key = key;
  item->value = value;

  item->next = hash->h[hv];
  hash->h[hv] = item;

  return 0;
}

/**
 * Recupere une valeur de la table de hachage
 *
 * @param key La cle
 * @param hash La table de hachage
 * @return La valeur, ou NULL
 */
void *hash_get(gel_hash_t hash, char *key) {
  item_t item;
  int hv = hash_func(key) % hash->size;

  for (item = hash->h[hv]; item != NULL; item = item->next) {
    if (!strcmp(key, item->key))
      return item->value;
  }

  return NULL;
}

/**
 * Supprime un element de la table de hachage
 *
 * @param hash La table de hachage
 * @param key La cle
 * return Code d'erreur
 */
int hash_del(gel_hash_t hash, char *key) {
  item_t item, *prev;
  int hv = hash_func(key) % hash->size;

  prev = &hash->h[hv];
  item = NULL;
  for (item = hash->h[hv]; item != NULL; item = item->next) {
    if (!strcmp(key, item->key))
    {
      *prev = item->next;
      delete(item->value);
      delete(item->key);
      delete(item);
      return 0;
    } else prev = &item->next;
  }

  return -1;
}

/**
 * Libere une table de hachage
 *
 * @param hash La table de hachage
 */
void hash_free(gel_hash_t hash) {
  int i;
  item_t item,old;

  for (i = 0 ; i < hash->size; i++) {
    item = hash->h[i];
    while(item != NULL) {
      old = item;
      item = item->next;
      delete(old->value);
      delete(old->key);
      delete(old);
    }
  }
  delete(hash->h);
  delete(hash);
}

/**
 * Create an hash table.
 *
 * @param size	Size of the hash table (prime numbers gives better results).
 * @return 		Create hash table or null.
 */
gel_hash_t gel_hash_new(int size) {
	gel_hash_t hash;

	hash = new(struct hashage);
	if(hash == NULL)
		return  NULL;
	hash->h = newv(struct item *, size);
	if (hash->h == NULL) {
		delete(hash);
		return (NULL);
	}
	hash->size = size;
	memset(hash->h, 0, size*sizeof(void*));
	return(hash);
}




/****** list management ******/

/**
 * Test if a list is empty.
 * @param a 	List to test.
 * @return 		1 if the list is empty, false.
 * @ingroup util
 */
int gel_list_empty(gel_list_t a) {
	return(a->tete == NULL);
}


/**
 * Build a new linked list.
 * @return	New list or null.
 * @ingroup util
 */
gel_list_t gel_list_new(void) {
	gel_list_t tmp;
	tmp = new(struct descripteur);
	if (tmp == NULL)
	return NULL;
	tmp->tete = NULL;
	tmp->encours = NULL;
	return(tmp);
}

/**
 * Move to the start of the list.
 * @param desc List to move on.
 * @ingroup util
 */
void gel_list_first(gel_list_t desc) {
	desc->encours = desc->tete;
}

/**
 * Test if the end of the list is reached.
 * @param desc 	List to test.
 * @return 		1 if the end is reached, 0 else.
 * @ingroup util
 */
int gel_list_end(gel_list_t desc) {
	return(desc->encours == NULL);
}

/**
 * Get the value of the current element of the list.
 * Then go to the next element.
 * @param desc 	List to get element of.
 * @return 		Got element or null (if at end).
 * @ingroup util
 */
void *gel_list_get(gel_list_t desc) {
	void *a;
	if (gel_list_end(desc))
		return NULL;
	a = desc->encours->val;
	desc->encours = desc->encours->next;
	return a;
}


/**
 * Test if an element is in the list.
 * @param desc 	List to look in.
 * @param val 	Looked item.
 * @return 		1 if the item is in, 0 else.
 * @ingroup util
 */
int gel_list_contains(gel_list_t desc, void *val) {
	struct ensemble *tmp = desc->tete;
	while (tmp != NULL) {
		if (tmp->val == val)
		return 1;
		tmp = tmp->next;
	}
	return 0;
}

/**
 * Add an element to the list (without adding two times the same element).
 * @param desc 	List to add to.
 * @param val 	Value to add.
 * @return		0 for success, -1 if there is no more memory.	
 * @ingroup util
 */
int gel_list_add(gel_list_t desc, void *val) {
	struct ensemble *tmp;

	if (gel_list_contains(desc, val))
		return 0;

	tmp = new(struct ensemble);
	if (tmp == NULL)
		return -1;

	tmp->next = (desc)->tete;
	tmp->val = val;
	(desc)->tete = tmp;
	return 0;
}


/**
 * Remove all elements from the list.
 * @param desc List to clear.
 * @ingroup util
 */
void gel_list_clear(gel_list_t desc) {
	struct ensemble *tmp = desc->tete;
	struct ensemble *old = NULL;

	while (tmp != NULL) {
		old = tmp;
		tmp = tmp->next;
		delete(old);
	}
}

/**
 * Delete the list.
 * @param desc List to delete.
 * @ingroup util
 */
void gel_list_delete(gel_list_t desc) {
	gel_list_clear(desc);
	delete(desc);
}

/**
 * Delete the items of the list with the given function.
 * @param desc 			List to clear.
 * @param free_func 	Function used to delete the items.
 * @ingroup util
 */
void gel_list_delete_items(gel_list_t desc, void (*free_func)(void *)) {
	struct ensemble *tmp = desc->tete;
	struct ensemble *old = NULL;

	while (tmp != NULL) {
		old = tmp;
		tmp = tmp->next;
		free_func(old);
	}
}


/**
 * Get the length (number of elements) in the list.
 * @param desc 	List to get length of.
 * @return 		List length.
 * @ingroup util
 */
int gel_list_length(gel_list_t desc) {
	struct ensemble *tmp = desc->tete;
	int res = 0;

	while(tmp != NULL) {
		tmp = tmp->next;
		res++;
	}
	return(res);
}


/**
 * Remove an element from the list.
 * @param desc 	List to remove from.
 * @param val 	Element to remove.
 * @return 		1 if the element is in the list, 0 else.
  * @ingroup util
*/
int gel_list_remove(gel_list_t desc, void *val) {
	struct ensemble *tmp = (desc)->tete;
	struct ensemble *old = NULL;

	while (tmp != NULL) {
		if (tmp->val == val) {
			if (old == NULL) {
				/* cas special: on enleve le premier element */
				(desc)->tete = ((desc)->tete)->next;
				delete(tmp);
			} else {
				/* on enleve un element quelconque */
				old->next = tmp->next;
				delete(tmp);
			}
			return 1;
		}
		old = tmp;
		tmp = tmp->next;
	}
  return 0;
}


/**
 * @typedef gel_hash_enum_t
 * Type of fast enumerator on an hash table.
 */


/**
 * Initialize an hash table enumerator.
 * @param en	Enumerator to initialize.
 * @param htab	Hash table to traverse.
 * @return		First item value or null if the has table is empty.
 */
void *gel_hash_enum_init(gel_hash_enum_t *en, gel_hash_t htab) {
	en->htab = htab;
	en->index = 0;
	while(!(en->item = htab->h[en->index])) {
		en->index++;
		if(en->index >= htab->size)
			return 0;
	}
	return en->item->value;
}


/**
 * Go to the next item in the hash table.
 * @param en	Enumerator.
 * @return		Value of the next item or null if there is no item.
 */
void *gel_hash_enum_next(gel_hash_enum_t *en) {
	en->item = en->item->next;
	if(!en->item) {
		en->index++;
		while(1) {
			if(en->index >= en->htab->size)
				return 0;
			en->item = en->htab->h[en->index];
			if(en->item)
				break;
			en->index++;
		}
	}
	return en->item->value;
}


/**
 * Initialize the hash enumerator to point past the end of the enumeration.
 * @param en	Enumerator to initialize.
 * @param hash	Hash table.
 */
void gel_hash_enum_end(gel_hash_enum_t *en, gel_hash_t htab) {
	en->htab = htab;
	en->index = en->htab->size - 1;
	en->item = 0;
}


/**
 * @define gel_hash_key(en)
 * Get the key of the current item.
 * @param en	Used enumerator.
 * @return		Key of the current item.
 */


/**
 * @define gel_hash_value(en)
 * Get the value of the current item.
 * @param en	Enumerator to use.
 * @return		Value of the current item.
 */


