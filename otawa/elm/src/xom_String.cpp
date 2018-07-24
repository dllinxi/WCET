/*
 *	xom::String class implementation
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

#include <elm/xom/String.h>
#include "xom_macros.h"
#include <string.h>
#include <stdlib.h>

namespace elm { namespace xom {

/**
 * @class String
 * String class used in XOM XML extension.
 */

/**
 * Copy the string (ensuring independence from the initialization string).
 */
void String::copy(void) {
	buf = strdup(chars());
}

/**
 * Free the string contained in this object.
 * Usually only used if a call to @ref copy() has been performed.
 */
void String::free(void) {
	::free((void *)buf);
	buf = "";
}

/**
 * The string is output and XML special characters are escaped
 * (mainly <, > and &).
 * @param out	Stream to output to.
 */
void String::escape(io::OutStream& out) const {
	for(const char *p = buf; *p; p++)
		switch(*p) {
		case '<':	out.write("&lt;", 4); break;
		case '>':	out.write("&gt;", 4); break;
		case '&':	out.write("&amp;", 5); break;
		default:	out.write(*p); break;
		}
}

} } // xelm::xom
