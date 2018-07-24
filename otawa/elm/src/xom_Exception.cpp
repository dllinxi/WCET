/*
 *	$Id$
 *	xom::Exception class interface
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

#include <elm/xom/Exception.h>

namespace elm { namespace xom {

/**
 * @class Exception
 * Exception thrown for errors during XOM work.
 * @ingroup xom
 */


/**
 * Build the exception from a formatted string.
 * @param node		Node causing the exception.
 * @param message	Message.
 */
Exception::Exception(Node *node, const String& message)
: MessageException(message) {
}


/**
 * @fn Node *Exception::node(void) const;
 * Get the node causing the exception.
 */

} } // elm::xom
