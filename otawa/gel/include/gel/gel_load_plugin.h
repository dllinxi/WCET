#ifndef GEL_LOAD_PLUGIN_H_
#define GEL_LOAD_PLUGIN_H_

/*==============================================================================
 * gel_load_plugin.h
 *==============================================================================
 * \file ce fichier contient les definitions de fonctions accessibles au niveau
 * plugin pour le module gel_load.c
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Cr�e le : 02/08/2006
 *==============================================================================
 */

#include <gel/common.h>

__BEGIN_DECLS


/**
 * Sert a charger les informations de longueur de code et d'adresses 
 * virtuelles d'un fichier ELF
 *
 * @param e Le fichier ELF
 * @param base_vaddr L'adresse de base virtuelle
 * @return 0 si tout s'est bien passe, -1 sinon
 */
int gel_file_load_info (gel_file_t *e, vaddr_t base_vaddr);

int gel_file_load_data (gel_file_t *e, gel_block_t *block, raddr_t base_raddr);
                        
/**
 * Sert a charger un fichier ELF en memoire sans gerer les dependances
 * si base_raddr est egal � NULL, alors on implante le fichier a une adresse 
 * memoire choisie par le systeme
 *
 * @param e Le fichier ELF
 * @param base_vaddr L'adresse de base virtuelle
 * @param base_raddr adresse � partir de laquelle charger le fichier
 */
int gel_file_load (gel_file_t *e, vaddr_t base_vaddr, raddr_t base_raddr);

__END_DECLS

#endif /*GEL_LOAD_PLUGIN_H_*/
