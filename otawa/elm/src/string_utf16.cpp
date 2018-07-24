/*
 *	utf16 module implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2016, IRIT UPS.
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

#include <elm/string/utf16.h>

namespace elm { namespace utf16 {

/**
 * @class Char
 * Wrapper class around UTF-16 16 bits characters.
 * @ingroup string
 */


/**
 * Convert UTF-16 to UTF-8, to be embedded in a string.
 * @param buf	Buffer to convert to.
 */
void Char::toUTF8(StringBuffer& buf) {
	if(c <= 0x7F)
		buf << char(c);
	else if(c <= 0x7FF)
		buf << char(0b11000000 | (c >> 6))
			<< char(0b10000000 | (c & 0x3f));
	else
		buf << char(0b11100000 | (c >> 12))
			<< char(0b10000000 | ((c >> 6) & 0x3f))
			<< char(0b10000000 | (c & 0x3f));
}

} }		// elm::utf16
