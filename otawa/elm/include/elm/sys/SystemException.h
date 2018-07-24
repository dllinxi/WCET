/*
 *	SystemException class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-12, IRIT UPS.
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
#ifndef ELM_SYS_SYSTEM_EXCEPTION_H
#define ELM_SYS_SYSTEM_EXCEPTION_H

#include <elm/utility.h>

namespace elm { namespace sys {

// SystemException class
class SystemException: 	public Exception {
public:
	typedef enum error_t {
		OK = 0,
		NO_ACCESS,
		NO_MORE_RESOURCE,
		BAD_PATH,
		IO_ERROR,
		ALREADY_EXIST
	} error_t;

private:
	error_t err;
	String msg;

public:
	SystemException(error_t err, String msg);
	SystemException(int code, String header);
	inline error_t error(void) const;

	// Exception overload
	virtual String message(void);
};

// Inlines
inline SystemException::error_t SystemException::error(void) const {
	return err;
}

} } // elm::sys

#endif // ELM_SYS_SYSTEM_EXCEPTION_H

