/*
 *	$Id$
 *	ListOption class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2010, IRIT UPS.
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

#include <elm/option/ListOption.h>

namespace elm { namespace option {

// ListOption class
/**
 * @class ListOption
 * Option that accepts a list of value, that is, accepts several command
 * and their linked argument.
 * @param T	Type of arguments.
 * @ingroup options
 */


/**
 * @fn ListOption::ListOption(void);
 * Simple constructor.
 */


/**
 * @fn ListOption::ListOption(Manager& man, int tag, ...);
 * @param man	Owner manager.
 * @param tag	First tag.
 * @param ...	(tag, value) pair list (ended by @ref end).
 */


/**
 * @fn ListOption::ListOption(Manager& man, int tag, VarArg& args);
 * @param man	Owner manager.
 * @param tag	First tag.
 * @param args	(tag, value) pair list (ended by @ref end).
 */


/**
 * @fn const T& ListOption::get(int index) const;
 * Get the value at the given index.
 * @param index		Index of the requested value.
 * @return			Value at the given index.
 */


/**
 * @fn void ListOption::set(int index, const T& value);
 * Set a value in the list.
 * @param index	Index of the value to set.
 * @param value	Value to set.
 */


/**
 * @fn T& ListOption::ref(int index);
 * Get reference on a value.
 * @param index	Index of value to get reference for.
 * @return		Reference of value matching the index.
 */


/**
 * @fn void ListOption::add(const T& val);
 * Add a value to the list.
 * @param val	Value to add.
 */


/**
 * @fn void ListOption::remove(int index);
 * Remove value at the given index.
 * @param index	Index of the value to remove.
 */


/**
 * @fn int ListOption::count(void) const;
 * Get the count of values.
 * @return	Value count.
 */

} }	// elm::option
