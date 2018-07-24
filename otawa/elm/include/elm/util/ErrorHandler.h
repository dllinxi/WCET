/*
 *	$Id$
 *	ErrorHandler and ErrorBase class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
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
#ifndef ELM_UTIL_ERRORHANDLER_H_
#define ELM_UTIL_ERRORHANDLER_H_

#include <elm/assert.h>
#include <elm/string.h>

namespace elm {

// error level type
typedef enum error_level_t {
	level_none = 0,
	level_info,
	level_warning,
	level_error,
	level_fatal
} error_level_t;

// ErrorHandler class
class ErrorHandler {
public:
	virtual void onError(error_level_t level, const string& message);
	static cstring getLevelString(error_level_t level);
	static ErrorHandler DEFAULT, STD;
	virtual ~ErrorHandler() {};
};

// ErrorBase class
class ErrorBase {
public:
	inline ErrorBase(ErrorHandler *error_handler = &ErrorHandler::DEFAULT)
		{ setErrorHandler(error_handler); }
	inline void setErrorHandler(ErrorHandler *error_handler)
		{ ASSERTP(error_handler, "null error handler"); handler = error_handler; }
	inline ErrorHandler *getErrorHandler(void) const { return handler; }

protected:
	inline void onError(error_level_t level, const string& message) { handler->onError(level, message); }

private:
	ErrorHandler *handler;
};

}	// elm

#endif /* ELM_UTIL_ERRORHANDLER_H_ */
