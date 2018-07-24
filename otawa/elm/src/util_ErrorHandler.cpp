/*
 *	$Id$
 *	ErrorHandler and ErrorBase class implementation
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

#include <elm/util/ErrorHandler.h>
#include <elm/io.h>

namespace elm {

/**
 * @class ErrorHandler
 * An error handler allows to get control on the production of error message.
 * The onError() methods must be overload by subclassing this class and the obtained
 * object must be passed to the @ref ErrorBase that generates the error messages.
 * @author	H. Cassé <casse@irit.fr>
 */


/**
 * Get a string representing the supplied error level.
 * @param level		Error level to get string of.
 * @return			Matching string.
 */
cstring ErrorHandler::getLevelString(error_level_t level) {
	static cstring names[] = {
		"",
		"INFO",
		"WARNING",
		"ERROR",
		"FATAL"
	};
	return names[level];
}


/**
 * This method is called each time an error message is generated.
 * As a default, it display the error on standard error output.
 * @param level		Level of the error.
 * @param message	Message to display.
 */
void ErrorHandler::onError(error_level_t level, const string& message) {
	cerr << getLevelString(level) << ": " << message << io::endl;
}


/**
 * Default error handler.
 */
ErrorHandler ErrorHandler::DEFAULT;


/**
 * @class ErrorBase
 * This is the base class of objects that deliver error events through @ref ErrorHandler.
 * The user class has hust to inherit from this ond to support error delivering.
 */


/**
 * @fn ErrorBase::ErrorBase(ErrorHandler *error_handler);
 * Build a new error base.
 * @param error_handler		Initial error handler (optional).
 */


/**
 * @fn void ErrorBase::setErrorHandler(ErrorHandler *error_handler);
 * Set the current error handler.
 * @param	Error handler to set.
 */


/**
 * @fn ErrorHandler *ErrorHandler::getErrorHandler(void) const;
 * Get the current error handler.
 * @return	Current error hanbler.
 */


/**
 * @fn void ErrorHandler::onError(error_level_t level, const string& message);
 * This method may be called by the class inheriting from @ref ErrorHandler .
 * This event will be transmitted to the error handler.
 * to generate an error event.
 * @param level		Level of the error event.
 * @param message	Message of the error event.
 */

}	// elm
