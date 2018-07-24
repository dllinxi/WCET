/*
 *	Vector class implementation
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

#include <elm/data/Vector.h>

namespace elm {

/**
 * @class Vector
 * This class provides a generic implementation of an extensible vector.
 * It provides fast access to its content using indexes but the addition of new elements
 * may be expensive if the number of elements exceeds the capacity. Therefore, it more
 * efficient with a small number of elements. For bigger arrays, see @ref FragTable.
 *
 * @par Performances
 * @li @ref indexed access -- O(1)
 * @li @ref addition at begin / end -- O(1) on average, O(n) if the vector need to be expanded.
 * @li @ref removal -- O(n)
 * @li @ref find -- O(n)
 * @li @ref memory -- 3 pointers
 *
 * @par Implemented concepts
 * @li @ref elm::concept::Array
 * @li @ref elm::concept::Collection
 * @li @ref elm::concept::List
 * @li @ref elm::concept::MutableArray
 * @li @ref elm::concept::MutableCollection
 * @li @ref elm::concept::MutableList
 * @li @ref elm::concept::Stack
 *
 * @param T	Type of data stored in the list.
 * @param M	Manager supporting equivalence and allocation.
 * @ingroup data
 */

/**
 * @fn int Vector::capacity(void) const;
 * Get the capacity of the vector.
 * @return	Capacity.
 */

/**
 * @fn void Vector::grow(int new_cap);
 * Make the capacity of the vector to grow, possibly causing
 * a buffer re-allocation. Notice that the length is unchanged.
 * @param new_cap	New capacity of the vector.
 */

/**
 * @fn void Vector::setLength(int new_length);
 * Change the length of the vector, possibly causing re-allocation
 * of the buffer.
 * @param new_length	New length of the vector.
 */

/**
 * @fn void Vector::copy(const Vector& vec);
 * Copy the given vector into the current one.
 * @param vec	Vector to copy.
 */

/**
 * @fn Table<T> Vector::detach(void);
 * Detach the buffer from the vector and return it as a table.
 * After this call, the vector stays unusable until a @ref setLength()
 * or a @ref grow() is performed.
 * @return	Vector buffer as a table. The caller is responsible for freeing
 * 			the obtained buffer.
 */

}	// elm
