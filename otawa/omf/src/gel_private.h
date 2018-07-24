/* $Id$ */

/*==============================================================================
 * gel_private.h
 *==============================================================================
 * \file ce fichier contient toutes les definitions qui sont utile au code prive
 * il contient donc les definitions publiques + pour plugin + privees.
 *==============================================================================
 * Auteur  : Sylvain Lemouzy (syl.lemouzy@free.fr)
 * Crée le : 02/08/2006
 *==============================================================================
 */

#ifndef GEL_PRIVATE_H
#define GEL_PRIVATE_H

#include <gel/gel_plugin.h>
#include "gel_types_private.h"
#include "gel_base_private.h"
#include "gel_load_private.h"
#include "gel_memory_private.h"

#define GEL_SPECIAL_STRTAB ".strtab"
#define GEL_SPECIAL_DYNSTR ".dynstr"
#define GEL_SPECIAL_SYMTAB ".symtab"
#define GEL_SPECIAL_DYNSYM ".dynsym"

    
/**
 * Cette fonction verifie que tout les symboles soit resolus dans l'image.
 * @param i L'image ELF a verifier
 * @return 0 si OK, -1 sinon.
 */
/*int gel_check_unresolved(gel_image_t *i); /!\ unused function */


/**
 * Cette fonction traite toute les entrees de relocation presentes dans l'image.
 * @param i L'image sur laquelle on doit faire la relocation
 * @return Code d'erreur (0 si succes, -1 sinon)
 */
/* int gel_do_reloc(gel_image_t *i); /!\  unused function */


#endif /* GEL_PRIVATE_H */
