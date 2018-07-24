#ifndef GEL_BASE_H_
#define GEL_BASE_H_

/*==============================================================================
 * gel_base_public.h
 *==============================================================================
 * \file ce fichier contient les definitions de fonctions accessibles au niveau
 * public pour le module gel_base.c
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Cr�e le : 02/08/2006
 *==============================================================================
 */

#include <gel/common.h>
#include <gel/file.h>

__BEGIN_DECLS

/**
 * fonction personnalisee de copie de chaines de carracteres
 */
char *mystrdup(char *s);


/**
 * Trouve un symbole local d'apres son nom et le fichier qui le contient.
 * L'utilisateur ne doit pas desallouer le symbole avec free() apres usage.
 *
 * @param f Le descripteur du fichier qui doit contenir le symbole.
 * @param name Le nom du symbole.
 * @return Le descripteur du symbole.
 */
gel_sym_t *gel_find_file_symbol(gel_file_t *f, char *name);

/**
 * Initialise la position ("curseur") de l'enumeration
 *
 * @param en L'enumeration
 */
void gel_enum_initpos(gel_enum_t *en);

/**
 * Renvoie le prochain element et incremente la position
 *
 * @param en L'enumeration
 * @return L'element lu ou NULL si fin de liste
 */
void *gel_enum_next(gel_enum_t *en);

/**
 * Libere l'enumeration (uniquement l'enumeration, ne concerne pas l'objet
 * auquel elle est attachee
 *
 * @param en L'enumeration
 */
void gel_enum_free(gel_enum_t *en);

/**
 * Renvoie une enumeration des symboles locaux d'un fichier.
 * A liberer avec enum_free()
 *
 * @param f Le fichier.
 * @return L'enumeration, ou NULL si erreur.
 */
gel_enum_t *gel_enum_file_symbol(gel_file_t *f);

/**
 * Remplace (recopie) un symbole par un autre dans la table
 * de hashage des symboles.
 * Les deux symboles doivent avoir le meme nom.
 *
 * @param im La table de hashage
 * @param symname Le nom du symbole a remplacer
 * @param newsym Le nouveau symbole a mettre a sa place.
 */

__END_DECLS

#endif /*GEL_BASE_H_*/
