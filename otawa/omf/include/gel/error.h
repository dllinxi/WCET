/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-10, IRIT- UPS
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
#ifndef GEL_ERROR_H_
#define GEL_ERROR_H_

#include <gel/common.h>

__BEGIN_DECLS

/* Error codes */
#define GEL_ESUCCESS	0
#define GEL_EIO		2
#define GEL_EFORMAT	4
#define GEL_ENOTSUPP	6
#define GEL_ENOTFOUND	8
#define GEL_ENOTLOADED  10
#define GEL_EINVAL	12
#define GEL_ERESOURCE   14
#define GEL_EDEPEND	16
#define GEL_ESTACK	18
#define GEL_EMAX	18

/* Warning codes */
#define GEL_WDYNAMIC	1
#define GEL_WNONFATAL	3
#define GEL_WMAX	3

/* Macros */
#define GEL_IS_WARNING (gel_errno & 1)
#define GEL_IS_ERROR (!(gel_errno & 1))
#define GEL_ERRNO (gel_errno >> 1)

/* types */
typedef void (gel_error_fun_t)(int code, const char *msg);

/* data */
extern int gel_errno;

/* functions */
const char *gel_strerror(void);
gel_error_fun_t *gel_set_error_fun(gel_error_fun_t *fun);
void gel_set_error(int code, const char *fmt, ...);

__END_DECLS

#endif /* ERROR_H_ */
