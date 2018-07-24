#ifndef GEL_LOAD_PRIVATE_H
#define GEL_LOAD_PRIVATE_H

/*==============================================================================
 * gel_load_private.h
 *==============================================================================
 * \file ce fichier contient les definitions de fonctions accessibles au niveau
 * private pour le module gel_load.c
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Crée le : 02/08/2006
 *==============================================================================
 */


/**
 * Cette fonction, a partir d'un fichier ELF, ouvre tout les fichiers ELF
 * dont il depend (librairies, etc...), en evitant de charger le meme fichier deux fois.
 * @param f Le fichier ELF initial
 * @param num Pointeur pour stocker le nombre de fichier ELF supplementaires que la fonction a ouvert 
 * @param env Environnement associe a la future image
 * @return Tableau de gel_file_t, ou bien NULL en cas d'erreur (librairies manquante, ...)
 * Tableau a liberer par l'appelant.
 */
gel_file_t **gel_recursive_open(gel_file_t *f, int *num, gel_env_t *env);


/**
 * Fonction utilitaire permettant de liberer un tableau de gel_file_t
 *
 * @param tab Le tableau
 * @param num La longueur du tableau
 */
void gel_kill_filetab(gel_file_t **tab, int num);

/**
 * Fonction servant a entrer les symboles du fichier f dans la table de
 * hashage.
 *
 * @param f Le fichier
 * @param hash La table de hashage
 * @param no_local Si vrai, le container est un bloc d'image et on ne regarde pas les symboles locaux. Sinon le container est un fichier et on regarde les symboles locaux.
 * @return Code d'erreur (0 pour succes ou -1 pour erreur)
 */
int gel_hash_syms(gel_file_t *f, gel_hash_t hash, int no_local);




#endif /*GEL_LOAD_REIVATE_H*/
