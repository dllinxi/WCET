/*
 *	$Id$
 *	Equiv class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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

#include <elm/util/Equiv.h>

namespace elm {

/**
 * @class Equiv
 * Default implementation of the concept @ref elm::concept::Equiv.
 * @param T	Type of compared values.
 */


/**
 * @fn bool Equiv::equals(const T& v1, const T& v2);
 * Compare two values.
 * @param v1	First value.
 * @param v2	Second value.
 * @return		True if both values are equal, false else.
 */


/**
 * @class EqualsEquiv
 * Implementation of the @ref elm::concept::Comparator for values providing
 * an equals() method.
 * @param T	Type of the values.
 */


/**
 * @class AssocEquiv
 * An associative equivallence may be used to transform data structure
 * into a map. If the items of data structure are represented as pair
 * of (key, values), this comparator provides comparison of items based on
 * the key member.
 * 
 * This class is also the default comparator of pairs.
 * @param K		Key type.
 * @param T		Value type.
 * @param E		Equivalence for the key.
 */

}
