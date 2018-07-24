#ifndef GEL_BASE_PLUGIN_H_
#define GEL_BASE_PLUGIN_H_

/*==============================================================================
 * gel_base_plugin.h
 *==============================================================================
 * \file ce fichier contient les definitions de fonctions accessibles au niveau
 * plugin pour le module gel_base.c
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Crée le : 02/08/2006
 *==============================================================================
 */


/**
 * Recupere un symbole d'apres son index et sa section
 * Le symbole doit etre free() par l'utilisateur
 *
 * @param symtab La section 
 * @param idx L'index du symbole
 * @return Le symbole gel_sym_t ou NULL
 */
 
gel_sym_t *gel_symbyidx(gel_sect_t *symtab, u32_t idx);

#endif /*GEL_BASE_PLUGIN_H_*/
