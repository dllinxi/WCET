/*
 *	$Id$
 *	SystemException class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
 * 
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software 
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <elm/assert.h>
#include <errno.h>
#include <elm/sys/SystemException.h>



namespace elm { namespace sys {

/**
 * @class SystemException
 * This exception is usually thrown due to system related errors.
 * @ingroup system_inter
 */


/**
 * Build a simple system exception.
 * @param error		Error kind.
 * @param message	Message.
 */
SystemException::SystemException(error_t error, String message)
: err(error), msg(message) {
}


/**
 * Build a system exception from real error code getting message from the system
 * and appending the header with it.
 * @param code		Real code.
 * @param header	Header used for building the message.
 */
SystemException::SystemException(int code, String header) {
	
	// Compute error
	switch(code) {
	case EACCES:
	case EPERM:
	case EROFS:
		err = NO_ACCESS;
		break;
	case EIO:
		err = IO_ERROR;
		break;
#ifndef __WIN32
	case ELOOP:
#endif
	case ENAMETOOLONG:
	case ENOMEM:
	case EMFILE:
	case ENFILE:
	case ENOSPC:
		err = NO_MORE_RESOURCE;
		break;
	case EFAULT:
	case ENOENT:
	case ENOTDIR:
	case ECHILD:
	case ESRCH:
		err = BAD_PATH;
		break;
	case EEXIST:
		err = ALREADY_EXIST;
		break;
	case EINVAL:
	case EINTR:
		ASSERT(0);
	}
	
	// Compute message
	StringBuffer buf;
	buf << header << ": " << strerror(code);
	msg = buf.toString();
}


/**
 * @fn error_t SystemException::error(void) const;
 * Get the error kind.
 * @return	Error kind.
 */


/**
 */
String SystemException::message(void) {
	return msg;
}
	
} } // elm::sys

