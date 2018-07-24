/*
 *	Char class implementation
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

#include <elm/assert.h>
#include <elm/string/Char.h>

namespace elm {

static t::uint64 bin_set[] = { 0x3000000000000, 0x00000000, 0x00000000, 0x00000000 };
static t::uint64 dec_set[] = { 0x3ff000000000000, 0x00000000, 0x00000000, 0x00000000 };
static t::uint64 hex_set[] = { 0x3ff000000000000, 0x7e0000007e, 0x00000000, 0x00000000 };
static t::uint64 space_set[] = { 0x100002e00, 0x00000000, 0x00000000, 0x00000000 };
static t::uint64 letter_set[] = { 0x00000000, 0x7fffffe07fffffe, 0x00000000, 0x00000000 };
static t::uint64 lower_set[] = { 0x00000000, 0x7fffffe00000000, 0x00000000, 0x00000000 };
static t::uint64 upper_set[] = { 0x00000000, 0x07fffffe, 0x00000000, 0x00000000 };

static inline bool is(unsigned char c, t::uint64 set[]) {
	return set[c >> 6] & (t::uint64(1) << (c & 0x3f));
}

/**
 * @class Char
 * Wrapper class around the C++ char type to provide
 * methods for character testing and conversion.
 * @ingroup string
 */


/**
 * Convert binary to character.
 * @param v	Binary digit.
 * @return	Character.
 */
Char Char::bin(int v) {
	ASSERT(v == 0 || v == 1);
	return '0' + v;
}


/**
 * Convert to decimal value.
 * @param	Hexadecimal digit.
 * @return	Character.
 */
Char Char::dec(int v) {
	ASSERT(0 <= v && v <= 9);
	return '0' + v;
}


/**
 * @fn Char Char::hex(int v);
 * Convert to hexadecimal character.
 * @param v	Digit to convert.
 * @return	Character.
 */


/**
 * Convert to hexadecimal character with lower-case letters.
 * @param v	Digit to convert.
 * @return	Character.
 */
Char Char::lowerHex(int v) {
	ASSERT(0 <= v && v <= 15);
	if(v < 10)
		return v + '0';
	else
		return v + 'a';
}


/**
 * Convert to hexadecimal character with upper-case letters.
 * @param v	Digit to convert.
 * @return	Character.
 */
Char Char::upperHex(int v) {
	ASSERT(0 <= v && v <= 15);
	if(v < 10)
		return v + '0';
	else
		return v + 'A';
}


/**
 * Test if the character is a binary digit.
 * @return	True if binary digit, false else.
 */
bool Char::isBin(void) const {
	return is(c, bin_set);
}


/**
 * Test if the character is a decimal digit.
 * @return	True if decimal digit, false else.
 */
bool Char::isDec(void) const {
	return is(c, dec_set);
}


/**
 * Test if the character is a decimal digit.
 * @return	True if decimal digit, false else.
 */
bool Char::isHex(void) const {
	return is(c, hex_set);
}


/**
 * Test if the character is a decimal digit.
 * @return	True if decimal digit, false else.
 */
bool Char::isSpace(void) const {
	return is(c, space_set);
}


/**
 * Test if the character is a decimal digit.
 * @return	True if decimal digit, false else.
 */
bool Char::isPrintable(void) const {
	return 32 <= c && c < 127;
}


/**
 * Test if the character is a decimal digit.
 * @return	True if decimal digit, false else.
 */
bool Char::isLetter(void) const {
	return is(c, letter_set);
}


/**
 * Test if the character is a decimal digit.
 * @return	True if decimal digit, false else.
 */
bool Char::isLowerCase(void) const {
	return is(c, lower_set);
}


/**
 * Test if the character is a decimal digit.
 * @return	True if decimal digit, false else.
 */
bool Char::isUpperCase(void) const {
	return is(c, upper_set);
}


/**
 * Convert binary character to digit.
 * @return	Digit value or -1 (if not binary).
 */
int Char::asBin(void) const {
	if(c == '0')
		return 0;
	else if(c == '1')
		return 1;
	else
		return -1;
}


/**
 * Convert decimal character to digit.
 * @return	Digit value or -1 (if not decimal).
 */
int Char::asDec(void) const {
	if('0' <= c && c <= '9')
		return c - '0';
	else
		return -1;
}


/**
 * Convert hexadecimal character to digit.
 * @return	Digit value or -1 (if not hexadecimal).
 */
int Char::asHex(void) const {
	if('0' <= c && c <= '9')
		return c - '0';
	else if('a' <= c && c <= 'f')
		return c - 'a' + 10;
	else if('A' <= c && c <= 'F')
		return c - 'A' + 10;
	else
		return -1;
}

}	// elm
