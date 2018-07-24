/*
 *	$Id: old_elf.h,v 1.10 2009/07/21 13:17:58 barre Exp $
 *	old_elf module interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
 *
 *	GLISS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	GLISS is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PPC_OLD_ELF_H
#define PPC_OLD_ELF_H

#include "grt.h"
#include "mem.h"
#include "api.h"

#if defined(__cplusplus)
    extern  "C" {
#endif

#define PPC_LOADER_STATE
#define PPC_LOADER_INIT(s)
#define PPC_LOADER_DESTROY(s)

/* ppc_loader_t type */
typedef struct ppc_loader_t ppc_loader_t;


/* loader management */
ppc_loader_t *ppc_loader_open(const char *path);
void ppc_loader_close(ppc_loader_t *loader);
void ppc_loader_load(ppc_loader_t *loader, ppc_platform_t *pf);
ppc_address_t ppc_loader_start(ppc_loader_t *loader);


/* system initialization (used internally during platform and state initialization) */
ppc_address_t ppc_brk_init(ppc_loader_t *loader);

/* section access */
typedef struct ppc_loader_sect_t {
	const char *name;
	ppc_address_t addr;
	int size;
	enum {
		PPC_LOADER_SECT_UNKNOWN = 0,
		PPC_LOADER_SECT_TEXT,
		PPC_LOADER_SECT_DATA,
		PPC_LOADER_SECT_BSS
	} type;
} ppc_loader_sect_t;
int ppc_loader_count_sects(ppc_loader_t *loader);
void ppc_loader_sect(ppc_loader_t *loader, int sect, ppc_loader_sect_t *data);

/* symbol access */
typedef struct {
	const char *name;
	ppc_address_t value;
	int size;
	int sect;
	enum {
		PPC_LOADER_SYM_NO_TYPE,
		PPC_LOADER_SYM_DATA,
		PPC_LOADER_SYM_CODE
	} type;
	enum {
		PPC_LOADER_NO_BINDING,
		PPC_LOADER_LOCAL,
		PPC_LOADER_GLOBAL,
		PPC_LOADER_WEAK
	} bind;
} ppc_loader_sym_t;
int ppc_loader_count_syms(ppc_loader_t *loader);
void ppc_loader_sym(ppc_loader_t *loader, int sym, ppc_loader_sym_t *data);



#if defined(__cplusplus)
}
#endif

#endif	/* PPC_OLD_ELF_H */
