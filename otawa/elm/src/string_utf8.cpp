/*
 *	utf8 module implementation
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

#include <elm/string/utf8.h>

namespace elm { namespace utf8 {

/**
 * @defgroup utf8 UTF-8 Support
 *
 * This module contains several classes and facilities to work with UTF-8 string.
 * Refer to https://tools.ietf.org/html/rfc3629.
 */

/**
 * @class Iter
 * Traverse an UTF-8 string Unicode character by unicode character.
 * @ingroup utf8
 */

/**
 */
void Iter::parse(void) throw(Exception) {

	// detect end
	if(p == q) {
		c = 0;
		return;
	}
	c = 0;

	// read first character
	char cc = *p++;
	if(!(cc & 0b10000000)) {
		c = cc;
		return;
	}

	// determine length
	if(!(cc & 0b01000000))
		throw Exception("utf8: bad encoding");
	int l = 1;
	while(l <= 3 && (cc & (1 << (6 - l))))
		l++;
	if(l == 4)
		throw Exception("utf8: bad encoding");
	c = cc & (((1 << (6 - l)) - 1));

	// read other characers
	for(int i = 0; i < l; i++) {
		if(p == q)
			throw Exception("utf8: character sequence too short");
		cc = *p++;
		if((cc & 0b11000000) != 0b10000000)
			throw Exception("utf8: bad encoding");
		c = (c << 6) | (cc & 0b00111111);
	}
}

} }		// elm::utf8
