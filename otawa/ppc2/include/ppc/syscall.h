/*
 *	syscall-embedded module interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011, IRIT UPS.
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
#ifndef PPC_SYSCALL_EMBEDDED_H
#define PPC_SYSCALL_EMBEDDED_H

#include "api.h"
#include "mem.h"

#if defined(__cplusplus)
    extern  "C" {
#endif

#define PPC_SYSCALL_STATE
#define PPC_SYSCALL_INIT(pf)
#define PPC_SYSCALL_DESTROY(pf)

void ppc_syscall(ppc_inst_t *inst, ppc_state_t *state);
void ppc_set_brk(ppc_platform_t *pf, ppc_address_t address);

#if defined(__cplusplus)
}
#endif

#endif /* PPC_SYSCALL_LINUX_H */
