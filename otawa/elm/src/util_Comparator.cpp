/*
 *	comparator module implementation
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

#include <elm/compare.h>

namespace elm {

/**
 * @class Comparator
 * Comparator concept (elm/compare.h) allows comparing two data. Comparator may be used
 * statically with the @ref compare() method but also as in instance with @ref doCompare() method.
 * As a default, @ref doCompare() performs a call to @ref compare().
 *
 * Several implementation of Comparator concept exists:
 * @li @ref Comparator -- default implementation based on == and < operators.
 * @li @ref CompareComparator -- assumes that data has a compare() method.
 * @li @ref AssocComparator -- works on pairs assuming order in based on the first component.
 * @li @ref ReverseComparator -- invert direction of another comparator.
 */

/**
 * @fn int Comparator::compare(const T& v1, const T& v2);
 * Compare v1 and v2 and returns:
 * @li > 0 if v1 > v2
 * @li = 0 if v1 = v2
 * @li < 0 if v1 < v2
 * @param v1	First value.
 * @param v2	Second value
 * @return	See above.
 */

/**
 * @fn int Comparator::doCompare(const T& v1, const T& v2);
 * Compare v1 and v2 and returns:
 * @li > 0 if v1 > v2
 * @li = 0 if v1 = v2
 * @li < 0 if v1 < v2
 * @param v1	First value.
 * @param v2	Second value
 * @return	See above.
 */


/**
 * @fn const T& Comparator::min(const T& v1, const T& v2);
 * Get the minimum of two values.
 * @param v1	First value.
 * @param v2	Second value.
 * @return		Minimum of v1 and v2.
 */


/**
 * @fn const T& Comparator::max(const T& v1, const T& v2);
 * Get the maximum of two values.
 * @param v1	First value.
 * @param v2	Second value.
 * @return		Maximum of v1 and v2.
 */


/**
 * @class CompareComparator
 * Implementation of the @ref elm::concept::Comparator for values providing
 * a compare() method.
 * @param T	Type of the values.
 */


/**
 * @class AssocComparator
 * An associative comparator may be used to transform an ordered data structure
 * into a map. If the items of ordered data structure are represented as pair
 * of (key, values), this comparator provides comparison of items based on
 * the key member.
 *
 * This class is also the default comparator of pairs.
 * @param K		Key type.
 * @param T		Value type.
 * @param C		Comparator for the key.
 */


/**
 * @class DefaultCompare
 * Default implementation of @ref Compare concept from a Comparator.
 *
 * @param T		Type of item to compare.
 * @param C		Comparator implementing the comparison.
 */

}
