/*
 *	$Id: fast_mem.h,v 1.4 2009/01/21 07:30:54 casse Exp $
 *	fast_mem module interface
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
#ifndef PPC_VFAST_MEM_H
#define PPC_VFAST_MEM_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

#if defined(__cplusplus)
    extern  "C" {
#endif

#define PPC_MEM_STATE
#define PPC_MEM_INIT(s)
#define PPC_MEM_DESTROY(s)

#define PPC_VFAST_MEM
#ifdef PPC_NO_PAGE_INIT
#	define PPC_NOBITS_INIT
#endif

typedef uint32_t ppc_address_t;
typedef uint32_t ppc_size_t;
typedef struct ppc_memory_t ppc_memory_t;

/* creation function */
ppc_memory_t *ppc_mem_new(void);
void ppc_mem_delete(ppc_memory_t *memory);
ppc_memory_t *ppc_mem_copy(ppc_memory_t *memory);

/* read functions */
uint8_t ppc_mem_read8(ppc_memory_t *, ppc_address_t);
uint16_t ppc_mem_read16(ppc_memory_t *, ppc_address_t);
uint32_t ppc_mem_read32(ppc_memory_t *, ppc_address_t);
uint64_t ppc_mem_read64(ppc_memory_t *, ppc_address_t);
float ppc_mem_readf(ppc_memory_t *, ppc_address_t);
double ppc_mem_readd(ppc_memory_t *, ppc_address_t);
long double ppc_mem_readld(ppc_memory_t *, ppc_address_t);
void ppc_mem_read(ppc_memory_t *memory, ppc_address_t, void *buf, size_t size);


/* write functions */
void ppc_mem_write8(ppc_memory_t *, ppc_address_t, uint8_t);
void ppc_mem_write16(ppc_memory_t *, ppc_address_t, uint16_t);
void ppc_mem_write32(ppc_memory_t *, ppc_address_t, uint32_t);
void ppc_mem_write64(ppc_memory_t *, ppc_address_t, uint64_t);
void ppc_mem_writef(ppc_memory_t *, ppc_address_t, float);
void ppc_mem_writed(ppc_memory_t *, ppc_address_t, double);
void ppc_mem_writeld(ppc_memory_t *, ppc_address_t, long double);
void ppc_mem_write(ppc_memory_t *memory, ppc_address_t, void *buf, size_t size);

#ifdef PPC_MEM_SPY
typedef enum { ppc_access_read, ppc_access_write } ppc_access_t;
typedef void (*ppc_mem_spy_t)(ppc_memory_t *mem, ppc_address_t addr, ppc_size_t size, ppc_access_t access, void *data);
void ppc_mem_set_spy(ppc_memory_t *mem, ppc_mem_spy_t fun, void *data);
#endif

#if defined(__cplusplus)
}
#endif

#endif	/* PPC_VFAST_MEM_H */
