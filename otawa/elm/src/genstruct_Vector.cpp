/*
 *	$Id$
 *	Vector class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-07, IRIT UPS.
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

#include <elm/genstruct/Vector.h>

namespace elm { namespace genstruct {

/**
 * @class Vector;
 * A vector is an array of items whose size grows dynamically.
 *
 * @par Implemented concepts
 * @li concept::Array
 * @li concept::Array
 * @li concept::List
 * @li concept::MutableArray
 * @li concept::MutableCollection
 * @li concept::MutableList
 * @li concept::Set
 * @li concept::Stack
 * 
 * @param T	Type of stored items.
 * @ingroup gen_datastruct
 */


/**
 * @fn Vector::Vector(int _cap);
 * Build a new vector.
 * @param _cap	Capacity represents the initial and growing size of the fector.
 */


/**
 * @fn Vector::Vector(const Vector<T>& vec);
 * Build a vector by copying an existing one.
 * @param vec	Vector to copy.
 */


/**
 * @fn Vector::~Vector(void);
 * Delete the vector and all its contained items.
 */


/**
 * @fn int Vector::capacity(void) const;
 * Get the capacity of the vector.
 * @return	Vector capacity.
 */


/**
 * @fn int Vector::length(void) const;
 * Get the number of items in the vector.
 * @return	Items number.
 */


/**
 * @fn bool Vector::isEmpty(void) const;
 * Test if vector is empty.
 * @return	True if vector is empty, false else.
 */


/**
 * @fn T Vector::get(int index) const;
 * Get an item from the vector.
 * @param index	Index of the item to get.
 * @return		Item at the given index.
 * @attention	It is an error to give an index higher or equal than the
 * vector length().
 */

/**
 * @fn T& Vector::item(int index);
 * Get a reference to an item in the vector, allowing to change the item.
 * @param index	Index of the item to get.
 * @return		Reference to the item at the given index.
 * @attention	It is an error to give an index higher or equal than the
 * vector length().
 */


/**
 * @fn void Vector::set(int index, const T value);
 * Change the value of the item at the given index.
 * @param index	Index of item to set.
 * @param value	Value to replace the item.
 * @attention	It is an error to give an index higher or equal than the
 * vector length().
 */


/**
 * @fn T& Vector::operator[](int index);
 * Same as item(int).
 */


/**
 * @fn T Vector::operator[](int index) const;
 * Same as item(int) const.
 */


/**
 * @fn bool Vector::contains(const T value) const;
 * Test if the given value is already in the vector.
 * @param value	Value to look for.
 * @return		True if the value is int he vector, false else.
 */


/**
 * @fn int Vector::indexOf(const T value, int start) const;
 * Compute the first index of the value in the vector.
 * @param value	Value to look for.
 * @param start	Start index for looking the value for.
 * @return		Index of the value in the vector or -1.
 * @attention	It is an error to give an index higher or equal than the
 * vector length().
 */


/**
 * @fn int Vector::lastIndexOf(const T value, int start) const;
 * Compute the last index of the value in the vector.
 * @param value	Value to look for.
 * @param start	Start index for looking the value for.
 * @return		Index of the value in the vector or -1.
 * @attention	It is an error to give an index higher or equal than the
 * vector length().
 */


/**
 * @fn Vector::operator bool(void) const;
 * Same as isEmpty().
 */


/**
 * @fn void Vector::add(const T value);
 * Add a value to the end of the vector, extending it if required.
 * @param value	Value to add.
 */


/**
 * @fn void Vector::removeAt(int index);
 * Remove the value at the given index. Values past the index are shifted to
 * the left.
 * @param index	Index of the value to remove.
 * @attention	It is an error to give an index higher or equal than the
 * vector length().
 */


/**
 * @fn void Vector::remove(const T value);
 * Remove the first item equal to value from the vector. Following items are
 * shifted to the left.
 * @param value	Value to remove.
 */


/**
 * @fn void Vector::insert(int index, const T value);
 * Insert a value at the given index position. Values at and after this position
 * are sgifted to the right. Vector is enlarged if required.
 * @param index	Index to insert the value to.
 * @param value	Value to insert.
 * @attention	It is an error to give an index higher or equal than the
 * vector length().
 */


/**
 * @fn void Vector::clear(void);
 * Remove all items from the vector.
 */


/**
 * @fn void Vector::grow(int new_cap);
 * Enlarge the vector with the given capacity.
 * @param new_cap	Size for enlarging the vector.
 */


/**
 * @fn void Vector::setLength(int new_length);
 * Set the length of the vector.
 * @param new_length	New length of the vector (must be less or equal to
 * the current length of the vector).
 */


/**
 * @fn void Vector::push(const T& value);
 * Push an item at the end of the vector..
 * @param value		Value to push in.
 * @seealso add()
 */


/**
 * @fn const T Vector::pop(void);
 * Pop an item from the end of the vector and shrink its length by 1.
 * @return	Popped item.
 */


/**
 * @fn const T Vector::top(void);
 * Get the last item of the vector, that is, the top of the stack represented
 * by the vector.
 * @return	Last vector item.
 */


/**
 * @fn void Vector::copy(const Vector& vec);
 * Copy the given vector in the current one.
 * @param vec	Vector to copy.
 */


/**
 * @fn Vector<T>& Vector::operator=(const Vector& vec);
 * Shortcut to @ref copy().
 */

} } // elm::genstruct
