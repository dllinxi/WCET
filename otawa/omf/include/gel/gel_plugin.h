/* $Id$ */

#ifndef GEL_PLUGIN_H
#define GEL_PLUGIN_H

/*==============================================================================
 * gel_plugin.h
 *==============================================================================
 * \file ce fichier contient toutes les definitions accessibles � un plugin 
 * d'architecture ou systeme
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Cr�e le : 02/08/2006
 *==============================================================================
 */

/**
 * \file
 * Ce fichier decrit l'interface des plugins.
 * Il definit les types auxquels les plugins peuvent avoir acces.
 * Il declare aussi toute les fonctions que les plugins doivent implementer
 * pour pouvoir fonctionner.
 *
 * Tout les plugins doivent etre stocke dans $GEL_BASEDIR/plugins/
 * (GEL_BASEDIR est definie au moment de la compilation par l'utilisateur)
 * Les plugins ont le nom: plugin_NUM.so (ou NUM est le numero de la
 * machine tel que defini dans les specs ELF)
 * Les plugins pourront acceder aux informations sur la plateforme a l'aide
 * de la structure gel_platform_t.
 */

#include <gel/common.h>
#include "gel_public.h"
#include "gel_types_plugin.h"
#include "gel_base_plugin.h"
#include "gel_load_plugin.h"
#include "gel_memory_plugin.h"

__BEGIN_DECLS

/*==============================================================================
 * Definition des macros utilisees dans gel
 *==============================================================================
 */
 
#define PAGE_TRUNC(a,x) ((x) - ((x) & ((a)->arch->psize - 1)))

/* calcule l'adresse virtuelle de debut d'un bloc */
#define VSTART_BLOCK(block) (block->exec_begin + block->base_vaddr)

/* calcule l'adresse virtuelle de fin d'un bloc */
#define VEND_BLOCK(block) (block->exec_begin + block->base_vaddr + block->exec_size-1)

/* calcule le debut du cluster contenant une adresse */
#define CLUSTER_TRUNC(address,clustersize) ((address) - ((address) & (clustersize - 1)))

/** calcule si deux adresses sont situees dans le meme cluster memoire */
#define SAME_CLUSTER(address1, address2, clustersize) ((address1 & (~(clustersize -1))) == (address2 & (~(clustersize-1))))

/** Convertir d'adresse virtuelle en adresse virtuelle relogee */
#define VADDR2VRELOC(f,a) ((a) + (f)->base_vaddr)

/** Convertir d'adresse virtuelle relogee en adresse virtuelle "simple" */
#define VRELOC2VADDR(f,a) ((a) - (f)->base_vaddr)

/** Convertir d'adresse virtuelle en adresse reelle */
#define VADDR2RADDR(f,a) ((a) + (u8_t *)(f)->base_raddr)

/** Convertir d'adresse reelle en adresse virtuelle */
#define RADDR2VADDR(f,a) ((intptr_t)((raddr_t)(a) - (f)->base_raddr))

/** Convertir d'adresse reelle en virtuelle relogee */
#define VRELOC2RADDR(f,a) (VADDR2RADDR((f),VRELOC2VADDR((f),(a))))

/** Convertir d'adresse virtuelle relogee en adresse reelle */
#define RADDR2VRELOC(f,a) (VADDR2VRELOC((f), RADDR2VADDR((f),(a))))

/**
 * Recupere la plateforme associee a une section
 *
 * @param s La section dont on veut recuperer la plateforme
 * @return La structure plateforme voulue
 */
/*gel_platform_t *gel_sect_platform(gel_sect_t *s); unused function */

/**
 * Recupere la plateforme associee a une image
 *
 * @param i L'image dont on veut recuperer la plateforme
 * @return La structure plateforme voulue
 */
/*gel_platform_t *gel_image_platform(gel_image_t *i); unused functi�on*/

__END_DECLS

#endif /* GEL_PLUGIN_H */
