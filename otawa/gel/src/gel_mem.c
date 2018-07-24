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

#include "gel_mem.h"

#ifdef GEL_DEBUG_MEM

void *gel_debug_new(const char *file, int line, const char *t, int size) {
	void *r = (void *)malloc(size);
	fprintf(stderr, "DEBUG:%s:%d: new<%s> = %p\n", file, line, t, r);
	return r;
}

void *gel_debug_newc(const char *file, int line, const char *t, int size) {
	void *r = (void *)calloc(size, 1);
	fprintf(stderr, "DEBUG:%s:%d: new<%s> = %p\n", file, line, t, r);
	return r;
}

void *gel_debug_newv(const char *file, int line, const char *t, int size, int n) {
	void *r = (void *)malloc(size * n);
	fprintf(stderr, "DEBUG:%s:%d: new<%s[%d]> = %p\n", file, line, t, n, r);
	return r;
}

void *gel_debug_newvc(const char *file, int line, const char *t, int size, int n) {
	void *r = (void *)calloc(size, n);
	fprintf(stderr, "DEBUG:%s:%d: new<%s[%d]> = %p\n", file, line, t, n, r);
	return r;
}

void *gel_debug_delete(const char *file, int line, void *p) {
	free(p);
	fprintf(stderr, "DEBUG:%s:%d: delete(%p)\n", file, line, p);
}

#endif	/* GEL_MEM_DEBUG */
