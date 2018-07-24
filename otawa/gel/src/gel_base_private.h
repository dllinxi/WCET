#ifndef GEL_BASE_PRIVATE_
#define GEL_BASE_PRIVATE_

/*==============================================================================
 * gel_base_private.h
 *==============================================================================
 * \file ce fichier contient les definitions de fonctions accessibles au niveau
 * private pour le module gel_base.c
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Cr�e le : 02/08/2006
 *==============================================================================
 */

/**
 * Compare deux plateformes
 *
 * @param p1 La plateforme 1
 * @param p2 La plateforme 2
 * @return 0 pour egal, un autre nombre pour different.
 */
int gel_compare_plat(gel_platform_t *p1, gel_platform_t *p2);

/**
 * Charge le contenu brut d'une section a l'adresse indiquee, ou bien a une
 * adresse determinee par le systeme.
 *
 * @param s Le descripteur de section a charger
 * @param addr L'adresse ou charger, ou bien NULL pour auto.
 *
 * @return Code d'erreur (0 si succes, -1 si erreur)
 */
int gel_sect_load(gel_sect_t *s, raddr_t addr);

/**
 * Pour desallouer une section.
 * L'appelant doit aussi enlever l'entree correspondante de gel_file_t->sects
 *
 * @param s La section a detruire
 * @return Void
 */
void gel_kill_sect(gel_sect_t *s);



#endif /*GEL_BASE_PRIVATE_*/
