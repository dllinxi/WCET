/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-07, IRIT- UPS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GEL_MEM_H
#define GEL_MEM_H

#include <stdio.h>
#include <stdlib.h>

/* uncomment to activate */
/* #define GEL_DEBUG_MEM */
/* #define GEL_DEBUG_NOFREE */

/* functions */
void *gel_debug_new(const char *file, int line, const char *t, int size);
void *gel_debug_newc(const char *file, int line, const char *t, int size);
void *gel_debug_newv(const char *file, int line, const char *t, int size, int n);
void *gel_debug_newvc(const char *file, int line, const char *t, int size, int n);
void *gel_debug_delete(const char *file, int line, void *p);


/* macros */
#ifndef GEL_DEBUG_MEM
#	define new(t)			((t *)malloc(sizeof(t)))
#	define newc(t)			((t *)calloc(sizeof(t), 1))
#	define newv(t, n)		((t *)malloc(sizeof(t) * n))
#	define newvc(t, n)		((t *)calloc(sizeof(t), n))
#else
#	define new(t)			((t *)gel_debug_new(__FILE__, __LINE__, #t, sizeof(t)))
#	define newc(t)			((t *)gel_debug_newc(__FILE__, __LINE__, #t, sizeof(t)))
#	define newv(t, n)		((t *)gel_debug_newv(__FILE__, __LINE__, #t, sizeof(t), n))
#	define newvc(t, n)		((t *)gel_debug_newvc(__FILE__, __LINE__, #t, sizeof(t), n))
#endif

#ifdef GEL_DEBUG_NOFREE
#	define delete(p)
#elif defined(GEL_DEBUG_MEM)
#	define delete(p)		gel_debug_delete(__FILE__, __LINE__, p)
#else
#	define delete(p)		free(p)
#endif

#endif	// GEL_MEM_H
