/*==============================================================================
 * gel_memory.c
 *==============================================================================
 * \file Fichier contenant l'implantation de la gestion de la memoire de gel
 * pour la clusterisation de celle ci.
 *==============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <gel/error.h>
#include "gel_private.h"
#include "gel_mem.h"

/**
 * Fonction chargee de ajouter recusivement les block qui appartiennent au meme
 * cluster dans la liste chainee accu
 *
 * @param index index du bloc a partir duquel on doit calculer le cluster
 * @param used  tableau indiquand si un bloc est deja utilise ou non
 * @param files tableau de pointeurs vers les descripteur de fichiers a charger
 *              en memoire.
 *              /!\ ils doivent contenir les info sur les adresses virtuelles
 *                  les tailles et les debut de section executables
 * @param nb_blocks nombre de blocks dans les tableaux
 * @param clustersize taille des clusters minimale
 * @param accu  liste chainee contenant les blocks du cluster
 * @return -1 si une erreur apparait, 0 sinon
 */
static int recursive_cluster_calc(int index, short* used, gel_block_t** blocks,
                                  int nb_blocks, u32_t clustersize, gel_list_t accu)
{
	/* TODO : on pourrait ajouter une fonction qui verrifie si tout les blocks
	 *        ne se chevauchent pas */
	int i;
	int *val;
	/* ajoute l'index a l'accumulateur */
	val = new(int);
	if (val == NULL)
	{
		gel_errno = GEL_ERESOURCE;
		return -1;
	}
	*val = index;
	if (gel_list_add(accu,val) == -1)
	{
		gel_errno = GEL_ERESOURCE;
		delete(val);
		return -1;
	}

	/* marque le numero de block comme utilise*/
	used[index] = 1;

	/* recherche les blocks contigus a ce dernier */
	for (i = 0; i < nb_blocks; i++)
	{
		if (!used[i] && i != index)
		{
			if ( SAME_CLUSTER (VSTART_BLOCK(blocks[index]),VEND_BLOCK(blocks[i]),clustersize) ||
			     SAME_CLUSTER (VEND_BLOCK(blocks[index]),VSTART_BLOCK(blocks[i]),clustersize) )
			{
				if (recursive_cluster_calc(i, used, blocks, nb_blocks,
				                           clustersize, accu)
				    == -1)
				{
					return -1;
				}
			}
		}
	}

	return 0;
}


/**
 * Initialize the memory with blocks whose size multiple of cluster size
 * (as specified in the environment) according to the libraries to implement
 * in memory.
 * @param env		environment
 * @param blocks 	array to file to load descriptors
 * 					(their virtual address should have been computed)
 * @param nb_block	number of blocks to allocate in memory.
 */
gel_memory_image_t* gel_init_memory_image (
	gel_env_t *env,
	gel_block_t** blocks,
	int nb_block)
{
	int i ; /* compteurs de boucle */
	short *used;/* tableau qui indique si un block a deja ete incorpore ou pas*/
	gel_memory_image_t *memory; /* pointeur vers la representation memoire � retourner */
	u32_t clustersize; /* taille de cluster */
	gel_list_t accu; /* liste chaine utilisee pour le calcul des clusters */
	gel_list_t cluster_list ; /* liste des clusters */

	/* Display blocks */
	/*for(i = 0; i < nb_block; i++)
		fprintf(stderr, "BLOCK %d: %p (%08x)\n", i, blocks[i]->exec_begin,
			blocks[i]->exec_size);*/

	/* verifie si l'environnement est bien configure pour la clusterisation */
	if (! env->flags & GEL_ENV_CLUSTERISED_MEMORY || env->clustersize <= 0)
	{
		return (NULL);
	}

	/* recuperation de la taille de cluster  */
	clustersize = env->clustersize;

	/* allocation du tableau */
	used = newv(short, nb_block);
	if ( used == NULL)
	{
		gel_errno = GEL_ERESOURCE;
		return NULL;
	}

	/* allocation de la structure representant la memoire */
	memory = new(gel_memory_image_t);
	if ( memory == NULL )
	{
		gel_errno = GEL_ERESOURCE;
		delete(used);
		return NULL;
	}

	/* initialisation de la liste des clusters */
	cluster_list = gel_list_new();
	if (cluster_list == NULL)
	{
		gel_errno = GEL_ERESOURCE;
		delete(used);
		delete(memory);
		return NULL;
	}

	/* initialisation du tableau used */
	for ( i = 0; i< nb_block; i++)
	{
		used[i] = 0; /* met � false toute le entree du tableau*/
	}

	/* pour chaque block non deja mappe dans la memoire, on verifie si il n'y a
	 * pas de block dont l'adresse de debut ou de fin est distant de moins de
	 * un clustersize et on cree une liste de liste de numero de block*/
	for ( i = 0 ; i < nb_block; i++)
	{
		if (! used[i])
		{
			accu = gel_list_new();
			if (accu == NULL)
			{
				gel_errno = GEL_ERESOURCE;
				delete(used);
				delete(memory);
				gel_list_delete_items(cluster_list, (void (*)(void *))gel_list_delete);
				delete(cluster_list);
				return NULL;
			}

			if (recursive_cluster_calc(i,used,blocks,nb_block,clustersize,accu)
			    == -1)
			{
				gel_errno = GEL_ERESOURCE;
				delete(used);
				delete(memory);
				gel_list_delete_items(cluster_list,(void (*)(void *))gel_list_delete);
				delete(cluster_list);
				gel_list_delete(accu);
				return NULL;
			}

			if (gel_list_add(cluster_list,accu) == -1)
			{
				gel_errno = GEL_ERESOURCE;
				delete(used);
				delete(memory);
				gel_list_delete_items(cluster_list,(void (*)(void *))gel_list_delete);
				delete(cluster_list);
				gel_list_delete(accu);
				return NULL;
			}
		}
	}


	/* creation des blocs memoire */
	i = 0;
	memory->nb_clusters = gel_list_length(cluster_list);
	memory->clusters = newv(gel_memory_cluster_t *, memory->nb_clusters);
	if (memory->clusters == NULL)
	{
		gel_errno = GEL_ERESOURCE;
		delete(used);
		delete(memory);
		gel_list_delete_items(cluster_list,(void (*)(void *))gel_list_delete);
		delete(cluster_list);
		return NULL;
	}

	/* creation de tout les clusters */
	/*init_pos(cluster_list);
	for ( i = 0; ! fin_liste(cluster_list); i++)
	{
		accu=lire(cluster_list);
	}*/

	gel_list_first(cluster_list);
	for ( i = 0; !gel_list_end(cluster_list); i++)
	{
		accu = gel_list_get(cluster_list);
		memory->clusters[i] = gel_create_memory_cluster(clustersize,blocks,accu);
		if (memory->clusters[i] == NULL)
		{
			delete(used);
			delete(memory);
			gel_list_delete_items(cluster_list,(void (*)(void *))gel_list_delete);
			delete(cluster_list);
			gel_free_image_memory(memory);
			return NULL;
		}

	}




	/* enfin, retour de la structure cree */
	return memory;
}

/**
 * Lib�re proprement l'espace memoire occupee par une structure
 * gel_memory_image_t
 *
 * @param memory pointeur vers le gel_memory_image_t a liberer
 */
void gel_free_image_memory(gel_memory_image_t* memory)
{
	int i ;
	for (i=0; i < memory->nb_clusters; i++)
	{
		if (memory->clusters[i] != NULL)
		{
			delete (memory->clusters[i]->data );
			delete( memory->clusters[i]);
		}
	}

	free (memory);
}

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
    (int clustersize, gel_block_t** blocks, gel_list_t block_list)
{
	gel_memory_cluster_t* cluster; /* clusteur retourne par la fonction*/
	vaddr_t min_addr,max_addr; /* valeur min et max des addresses du bloc */
	int * block_index;


	/* alloue la place pour la structeur de description du cluster*/
	cluster = new(gel_memory_cluster_t);
	if (cluster == NULL)
	{
		gel_errno = GEL_ERESOURCE;
		return NULL;
	}

	/* calcul des adresses min/max du cluster a allouer */
	gel_list_first(block_list);
	block_index = (int *)gel_list_get(block_list);
	min_addr = VSTART_BLOCK(blocks[*block_index]);
	max_addr = VEND_BLOCK(blocks[*block_index]);
	/*fprintf(stderr, "range 1: %p - %p\n", min_addr, max_addr);*/

	while(!gel_list_end(block_list)) {
		block_index = (int *)gel_list_get(block_list);
		vaddr_t start = VSTART_BLOCK(blocks[*block_index]);
		vaddr_t end = VEND_BLOCK(blocks[*block_index]);
		/*fprintf(stderr, "start = %p end = %p\n", start, end);*/
		if(end - start > 0) {
			if(min_addr > start)
				min_addr = start;
			if(!end || max_addr < end)
				max_addr = end;
		}
	}
	//fprintf(stderr, "range 2: %p - %p\n", min_addr, max_addr);

	min_addr = CLUSTER_TRUNC(min_addr,clustersize);
	max_addr = CLUSTER_TRUNC(max_addr,clustersize) + clustersize-1;
	//fprintf(stderr, "range 3: %p - %p\n", min_addr, max_addr);

	/* creation du bloc memoire */
	cluster->data = newv(char, max_addr- min_addr + 1);
	if (cluster->data == NULL)
	{
		gel_errno = GEL_ERESOURCE;
		delete(cluster);
		return NULL;
	}

	/* remplissage de la structure*/
	cluster->vaddr = min_addr;
	cluster->size  = max_addr- min_addr+1;

	/* liaison blocks <-> cluster et remplissage des donnees sur les adresses*/
	gel_list_first(block_list);
	while (!gel_list_end(block_list) )
	{
		block_index = (int *)gel_list_get(block_list);

		blocks[*block_index]->cluster = cluster;
		/* adresse reelle ou sera implante le block */
		blocks[*block_index]->data = (u8_t*)((intptr_t)cluster->data +
		        blocks[*block_index]->exec_begin +
		        ( blocks[*block_index]->base_vaddr- (intptr_t)cluster->vaddr) );
		/* adresse reelle de base du block*/
		blocks[*block_index]->base_raddr =
		   (raddr_t)(blocks[*block_index]->data - blocks[*block_index]->exec_begin);
	}

	return cluster;
}

