/*
 * $Id$
 *
 * GEL is part of the GEL library.
 * Copyright (c) 2011, IRIT- UPS
 *
 * GEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GEL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GEL; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GEL_STABS_H_
#define GEL_STABS_H_

#include <stdint.h>
#include <gel/gel.h>
#include <gel/debug_line.h>

__BEGIN_DECLS

/* sections */
#define STABS_SECT_STAB		".stab"
#define STABS_SECT_STABSTR	".stabstr"

/* symbols */
#define N_UNDF		0x00	/** Start of object file */
#define N_GSYM		0x20	/** Global symbol */
#define N_FUN		0x24	/** Function or procedure */
#define N_OUTL		0x25	/** Outlined function */
#define N_STSYM		0x26	/** Initialized static symbol */
#define N_TSTSYM	0x27	/** Initialized TLS static variable */
#define N_LCSYM		0x28	/** Unitialized static symbol */
#define N_MAIN		0x2a	/** Name of main routine */
#define N_ROSYM		0x2c	/** Read-only static symbol */
#define N_FLSYM		0x2e	/** Global symbol */
#define N_TFLSYM	0x2f	/** Global symbol */
#define N_CMDLINE	0x34
#define N_OBJ		0x38	/** Object file or path name */
#define N_OPT		0x3c	/** Compiler options */
#define N_RSYM		0x40	/** Register symbol */
#define N_SLINE		0x44	/** Source line */
#define N_XLINE		0x45	/** Source line */
#define N_BROWS		0x48	/** Path to associated .cb file */
#define N_ILDPAD	0x4c	/** Pad string table */
#define N_ENDM		0x62	/** End module */
#define N_SO		0x64	/** Compilation source file or path name */
#define N_MOD		0x66	/** Fortran 95 module begin */
#define N_EMOD		0x68	/** Fortran 95 module end */
#define N_READ_MOD	0x6a	/** Fortran 95 use statement */
#define N_ALIAS		0x6c
#define N_LSYM		0x80	/** Local symbol */
#define N_BINCL		0x82	/** Begin include file */
#define N_SOL		0x84	/** Included or referenced source file */
#define N_PSYM		0xa0	/** Formal parameter */
#define N_EINCL		0xa2	/** End of include file */
#define N_ENTRY		0xa4	/** Fortram alternate entry */
#define N_LBRAC		0xc0	/** Start of scope (left bracket) */
#define N_USING		0xc4	/** C++ USING statement */
#define N_ISYM		0xc6	/** Position independent type, internal */
#define N_ESYM		0xc8	/** Position independent type, external */
#define N_PATCH		0xd0
#define N_CONSTRUCT	0xd2	/** Constructor description */
#define N_DESTRUCT	0xd4	/** Destructor description */
#define N_CODETAG	0xd8	/** Code generation detail */
#define N_FUN_CHILD	0xd9	/** Created when a nesting relationship */
#define N_RBRAC		0xe0	/** End of scope (right bracket) */
#define N_BCOMM		0xe2	/** Begin common block */
#define N_TCOMM		0xe3	/** Begin task common block */
#define N_ECOMM		0xe4	/** End common block */


/** STAB item */
typedef struct stab_t {
	const char *n_strx;		/** string information */
	uint8_t		n_type;		/**type */
	int8_t		n_other;	/** used by N_SLINE stab */
	int16_t		n_desc;		/** description value */
	uint32_t	n_value;	/** offset or value */
} stab_t;

/** STABS module datastructure */
typedef struct stabs_t stabs_t;

/* stabs errors */
#define STABS_ERR_BASE	(-256)
#define STABS_ERR_NONE	(-256)	/** No STABS section found */
#define STABS_ERR_NOSTR	(-258)	/** No STABS string section */
#define STABS_ERR_NORES	(-260)	/** No more ressource */
#define STABS_ERR_FMT	(-262)	/** Format error */

/* functions */
stabs_t *stabs_new(gel_file_t *file);
void stabs_delete(stabs_t *stabs);
const char *stabs_strerror(void);
int stabs_count(stabs_t *stabs);
stab_t *stabs_item(stabs_t *stabs, int i);
gel_line_map_t *stabs_new_line_map(gel_file_t *file);

__END_DECLS

#endif /* GEL_STABS_H_ */
