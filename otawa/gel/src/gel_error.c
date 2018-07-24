/*
 * $Id$
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

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <gel/error.h>
#include <gel/stabs.h>

/**
 * @defgroup error	Error Management
 *
 * GEL provides a standard way of error management:
 * @li gel_errno -- contain the last error.
 * @li gel_strerror -- get the message matching the gel_errno.
 */

/**
 * Usually, the GEL functions shows an error by returning a -1 value or
 * a NULL pointer. More details about the error may be found in this
 * variable.
 *
 * This variable contains one of definitions GEL_Exxx or GEL_Wxxx.
 * @ingroup error
 */
int gel_errno = GEL_ESUCCESS;


/* default error handler */
static void default_error(int code, const char *msg) {
	const char *pref;
	if(GEL_IS_WARNING)
		pref = "WARNING";
	else
		pref = "ERROR";
	fprintf(stderr, "%s: %d: %s\n", pref, code, msg);
}

/* error function */
static gel_error_fun_t *error_fun = default_error;

/* Message for the warnings. */
static char *gel_warntab[] =  {
	"Failed to load .dynamic section",
	"Unknown non-fatal error",
	NULL
};


/* Messages for the errors. */
static char *gel_errtab[] =  {
	"Success",
	"I/O Error",
	"Malformed file",
	"Not supported",
	"Not found",
	"Not loaded",
	"Invalid argument",
	"Insufficient resources",
	"Missing dependency",
	"Stack / code collision",
	NULL
};


/* STABS errors */
static const char *stabs_msgs[] = {
	"STABS: no section found",
	"STABS: no string section",
	"STABS: no more resource"
};


/**
 * Get the message associated with the current @ref gel_errno.
 * @return Error message.
 * @ingroup error
 */
const char *gel_strerror(void) {
	
	/* STABS errors */
	if(gel_errno <= STABS_ERR_BASE)
		return stabs_msgs[-(gel_errno - STABS_ERR_BASE) >> 1];
	
	/* GEL errors */
	if(GEL_IS_WARNING) {
		if (gel_errno > GEL_WMAX)
			return("unknown warning");
		else
			return gel_warntab[GEL_ERRNO];
  	}
  	else {
		if (gel_errno > GEL_EMAX)
			return("unknown error");
		else
			return gel_errtab[GEL_ERRNO];
	}
}


/**
 * Set the function which will be called when an error or a warning arise.
 * @param fun	New error function.
 * @return		Old error function.
 */
gel_error_fun_t * gel_set_error_fun(gel_error_fun_t *fun) {
	gel_error_fun_t *old = fun;
	error_fun = fun;
	return old;
}


/**
 * Generate an error using new error function system.
 * @param code	Error code.
 * @param fmt	Message format.
 * @param ...	Format arguments.
 */
void gel_set_error(int code, const char *fmt, ...) {
	char msg[256];
	va_list(args);

	/* prepare message */
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	/* launch the error */
	gel_errno = code;
	error_fun(code, msg);
}


/**
 * @def GEL_ESUCCESS
 * Last operation has been successful.
 * @ingroup error
 */


/**
 * @def GEL_EIO
 * Error inf input / ouput.
 * @ingroup error
 */


/**
 * @def GEL_EFORMAT
 * Error in ELF file format.
 * @ingroup error
 */


/**
 * @def GEL_ENOTSUPP
 * Unsupported ELF feature.
 * @ingroup error
 */


/**
 * @def GEL_ENOTFOUND
 * File, symbol, section not found.
 * @ingroup error
 */


/**
 * @def GEL_ENOTLOADED
 * Data not already loaded.
 * @ingroup error
 */


/**
 * @def GEL_ESUCCESS
 * Index out of bounds.
 * @ingroup error
 */


/**
 * @def GEL_ESUCCESS
 * Not enough ressources (usually memory).
 * @ingroup error
 */


/**
 * @def GEL_EDEPEND
 * Dependency problem.
 * @ingroup error
 */


/**
 * @def GEL_WDYNAMIC
 * Dynamic problem.
 * @ingroup error
 */


/**
 * @def GEL_WNONFATAL
 * Non-fatal problem.
 * @ingroup error
 */


/**
 * @def GEL_IS_WARNING
 * Test if @ref gel_errno contains a warning.
 * @return	True if there is a warning.
 * @ingroup error
 */


/**
 * @def GEL_IS_ERROR
 * Test if @ref gel_errno contains an error.
 * @return	True if there is an error.
 * @ingroup error
 */


/**
 * @def GEL_ERRNO
 * Get the @ref gel_errno with warning bit removed.
 * @ingroup error
 */
