/*
 *	$Id$
 *	MessageException class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <elm/io.h>
#include <elm/util/MessageException.h>

namespace elm {

/**
 * @class MessageException
 * This class is dedicated to the exception management with a human-readable
 * message.
 * @ingroup utility
 */


/**
 * Build an empty with a string message possibly containing printf-like
 * escape sequences for arguments.
 * @param message	Message formats.
 * @param args		Arguments.
 */
MessageException::MessageException(const String& message)
: msg(message) {
	//cerr << "elm::MessageException::MessageException(\"" << message << "\")\n";
}


/**
 */	
String MessageException::message(void) {
	//cerr << "elm::MessageException::message() = \"" << msg << "\"\n";
	return msg;
}

} // elm

