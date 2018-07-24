/*
 *	$Id$
 *	int module implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011, IRIT UPS.
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

#include <elm/int.h>

namespace elm {

/**
 * @typedef t::int8
 * Signed 8-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::uint8
 * Unsigned 8-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::int16
 * Signed 16-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::uint16
 * Unsigned 16-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::int32
 * Signed 32-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::uint32
 * Unsigned 32-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::int64
 * Signed 64-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::uint64
 * Unsigned 64-bit integer type.
 * @ingroup types
 */

/**
 * @typedef t::size
 * Integer type to represent memory size (according to the system configuration).
 * @ingroup types
 */

/**
 * @typedef t::offset
 * Integer type to represent memory offset (according to the system configuration).
 * @ingroup types
 */

/**
 * @typedef t::uint
 * Default size unsigned integer type.
 * @ingroup types
 */

/**
 * @typedef t::intptr
 * Integer sufficiently big to store a pointer (according to the system configuration).
 * @ingroup types
 */


/**
 * @fn t::uint32 roundup(t::uint32 v, t::uint32 m);
 * Round upto upper multiple integer
 * @param v		Value to round.
 * @param m		Multiple to round with.
 * @return		Value round upto the upper multiple integer.
 * @ingroup types
 */


/**
 * @fn t::uint32 rounddown(t::uint32 v, t::uint32 m);
 * Round upto upper multiple integer
 * @param v		Value to round.
 * @param m		Multiple to round with.
 * @return		Value round upto the upper multiple integer.
 * @ingroup types
 */

/**
 * Compute the position of the left-most bit to one.
 * @param i		Integer to test.
 * @return		Position of left-most bit to one or -1 if the integer is 0.
 * @ingroup types
 */
int msb(t::uint32 i) {
	#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
	static const char tab[256] = {
		-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
		LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
		LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
	};

	t::uint32 m = i >> 16;
	if(m) {
		t::uint32 n = m >> 8;
		if(n)	return tab[n] + 24;
		else	return tab[m] + 16;
	}
	else {
		t::uint32 n = i >> 8;
		if(n)	return tab[n] + 8;
		else	return tab[i];
	}
}


/**
 * Compute the position of the left-most bit to one.
 * @param i		Integer to test.
 * @return		Position of left-most bit to one or -1 if the integer is 0.
 * @ingroup types
 */
int msb(t::uint64 i) {
	t::uint32 uw = (i >> 32);
	if(uw)
		return msb(uw) + 32;
	else
		return msb(t::uint32(i));
}


/**
 * Count the number of ones in the given byte.
 * @param i		Byte to count ones in.
 * @return		Number of ones in the byte.
 * @ingroup types
 */
int ones(t::uint8 i) {
	static int t[] = {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
	};
	return t[i];
}


/**
 * @fn int ones(t::uint16 i);
 * Count the number of ones in the given half-word.
 * @param i		Half-word to count ones in.
 * @return		Number of ones in the half-word.
 * @ingroup types
 */

/**
 * @fn int ones(t::uint32 i);
 * Count the number of ones in the given word.
 * @param i		Word to count ones in.
 * @return		Number of ones in the word.
 * @ingroup types
 */

/**
 * @fn int ones(t::uint64 i);
 * Count the number of ones in the given double-word.
 * @param i		Double-word to count ones in.
 * @return		Number of ones in the double-word.
 * @ingroup types
 */


/**
 * Get the least upper power of 2 for the given value.
 * If the value is a power of two, return it else compute
 * the least greater power.
 * @param v		Value to process.
 * @return		Least upper power of two.
 * @ingroup types
 */
t::uint32 leastUpperPowerOf2(t::uint32 v) {
	int m = msb(v);
	if(m < 0)
		return 0;
	else if(v == (t::uint32(1) << m))
		return v;
	else
		return 1 << (m + 1);
}

/**
 * Get the least upper power of 2 for the given value.
 * If the value is a power of two, return it else compute
 * the least greater power.
 * @param v		Value to process.
 * @return		Least upper power of two.
 * @ingroup types
 */
t::uint64 leastUpperPowerOf2(t::uint64 v) {
	int m = msb(v);
	if(m < 0)
		return 0;
	else if(v == (t::uint64(1) << m))
		return v;
	else
		return 1 << (m + 1);
}


/**
 * Perform multiplication on unsigned integer 32-bits.
 * @param	a		First number to multiply.
 * @param	b		Second number to multiply.
 * @param	over	Set to true if overflow occurs.
 * @return			Result of multiplication (only valide if over is false).
 * @ingroup types
 */
t::uint32 mult(t::uint32 a, t::uint32 b, bool& over) {
	t::uint32 r = a * b;
	over = r < a || r < b;
	return r;
}


/**
 * Perform multiplication on unsigned integer 64-bits.
 * @param	a		First number to multiply.
 * @param	b		Second number to multiply.
 * @param	over	Set to true if overflow occurs.
 * @return			Result of multiplication (only valide if over is false).
 * @ingroup types
 */
t::uint64 mult(t::uint64 a, t::uint64 b, bool& over) {
	t::uint64 r = a * b;
	over = r < a || r < b;
	return r;
}

}	// elm
