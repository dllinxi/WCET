/*
 *	$Id$
 *	Pair class implementation
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

#include <elm/util/Pair.h>

namespace elm {
	
/**
 * @class Pair <elm/util/Pair.h>
 * This template class is mainly useful for returning or passing in parameter
 * a data item composed of two values.
 *
 * Pairs are useful to avoid developing specific structures or classes to group two data.
 * To help using pairs, some shortcut functions are provided:
 * @li pair(v1, v2) -- allows to build a pair of type Pair<T1, T2> with T1 type of v1, T2 type of v2.
 * @li let(r1, r2) = pair -- assign quickly a pair to a pair of variables r1 and r2.
 *
 * A common use of the last function is:
 * @code
 * T1 v1;
 * T2 v2;
 * let(v1, v2) = my_pair;
 * @endcode
 *
 *
 * @param T1	First value type.
 * @param T2	Second value type.
 * @ingroup utility
 */


/**
 * @var Pair::fst;
 * The first value of the pair : may be read or written.
 */


/**
 * @var Pair::snd;
 * The second value of the pair : may be read or written.
 */


/**
 * @fn Pair::Pair(void);
 * Build a pair with uninitialized content.
 */ 


/**
 * @fn Pair::Pair(const T1& _fst, const T2& _snd);
 * Build a pair from two values.
 * @param _fst	First value.
 * @param _snd	Second value.
 */


/**
 * @fn Pair::Pair(const Pair<T1, T2>& pair);
 * Build a pair by copying another pair.
 * @param pair	Pair to copy.
 */


/**
 * @fn Pair<T1, T2>& Pair::operator=(const Pair<T1, T2>& pair);
 * Assignment overload for pairs.
 * @param pair	Pair to assign.
 * @return		Current pair.
 */

} // elm
