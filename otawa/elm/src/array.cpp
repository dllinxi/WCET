/*
 *	Fast array utilities.
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

namespace elm {

/**
 * @namespace array
 * This namespace provides optimized to work with C native array (whatever the running OS).
 *
 * Most of these functions uses information of the type provided by the type_info<T> structure
 * to optimize or not the array operations. Particularly, they uses the is_deep definition
 * to know if a value may be copied byte-per-byte or need more complex processing because
 * of its data structure. Any non-scalar type is basically considered as non-deep but you may
 * change this behavior by specializing the type_info for your own type, MyType in the
 * example below:
 * @code
 * template <> struct type_info<MyType>: public class_t<MyType> {
 *	enum { is_deep = true };
 * }
 * @endcode
 *
 * @ingroup types
 */


namespace array {

/**
 * @fn void copy(T *target, const T *source, int size);
 * Copy source array of the given size to the target as fast as possible.
 * The arrays must not overlap.
 * @param target	Target array.
 * @param source	Source array.
 * @param size		Size of both arrays.
 * @ingroup array
 */

/**
 * @fn void move(T *target, const T *source, int size);
 * Copy source array of the given size to the target as fast as possible.
 * The arrays may overlap.
 * @param target	Target array.
 * @param source	Source array.
 * @param size		Size of both arrays.
 * @ingroup array
 */


/**
 * @fn void set(T *target, int size, const T& v);
 * Set the items of an array of the given size to the given value, as fast as possible.
 * @param target	Target array.
 * @param size		Size of both arrays.
 * @param v			Value to set.
 * @ingroup array
 */


/**
 * @fn void clear(T *target, int size);
 * Set the items of an array of the given size to 0.
 * @param target	Target array.
 * @param size		Size of both arrays.
 * @ingroup array
 */

}	// array

/**
 * @fn void quick_sort(A& array);
 * Quick sort a generic structure implementing the elm::concept::Array concept.
 * @param array		Array to sort.
 *
 * @param T			Type of items in array.
 * @param A			Type of the array.
 * @param C			Comparator to use (as a default, elm::Comparator<T>).
 * @ingroup array
 */

}	// elm
