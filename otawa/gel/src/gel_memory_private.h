#ifndef GEL_MEMORY_PRIVATE_H_
#define GEL_MEMORY_PRIVATE_H_

/*==============================================================================
 * gel_memory_private.h
 *==============================================================================
 * \file ce fichier contient les definitions de fonctions privées pour le module
 * gel_memory.c
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Crée le : 02/08/2006
 *==============================================================================
 */

/**
 * Alloue un bloc memoire contenant un ou plusieur cluster
 * 
 * @param clustersize taille de cluster
 * @param blocks tableau de pointeurs vers les descripteur de block a charger
 *               en memoire. 
 *               /!\ ils doivent contenir les info sur les adresses virtuelles
 *                   les tailles et les debut de section executables
 * @param block_list liste chainee contenant les index des bloc qui seront
 *                  charges dans le cluster
 */
gel_memory_cluster_t* gel_create_memory_cluster 
    (int clustersize, gel_block_t** blocks, gel_list_t block_list);

#endif /*GEL_MEMORY_PRIVATE_H_*/
