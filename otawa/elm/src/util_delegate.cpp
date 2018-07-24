/*
 *	$Id$
 *	delegate classes implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007-08, IRIT UPS.
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

#include <elm/util/delegate.h>

namespace elm {

/**
 * @defgroup delegate	Reference delegates
 * 
 * Delegate class in ELM allows to get references on value managed by
 * specific class functions to ensure their access or their assignment.
 * 
 * They support the same operations as the C++ refercences:
 * @li	to access the value, they may used as is (automatic conversion) or
 * 		with the star '*' operator,
 * @li	to change the value, the assignment '=' operator may be used.
 * 
 * The following delegates exists:
 * 
 * @li @ref elm::ArrayDelegate allows to use values from a
 * 			@ref elm::concept::MutableArray whose access is based on get()/set()
 * 			methods and an index,
 * 
 * @li @ref elm::MapDelegate allows to use values from a
 * 			@ref elm::concept::MutableMap whose access is based on get()/put()
 * 			methods and an identifier.
 */


/**
 * @class ArrayDelegate
 * A delegate for accessing items of a @ref concept::MutableArray.
 * 
 * @param C		Type of the container.
 * @param I		Type of the index.
 * @param T		Type of the items.
 * 
 * @ingroup delegate
 */


/**
 * @fn ArrayDelegate::ArrayDelegate(C& container, const I& index);
 * Build a delegate for an item of the given container.
 * @param container		Item container.
 * @param index			Item index.
 */


/**
 * @fn ArrayDelegate::ArrayDelegate(const ArrayDelegate& delegate);
 * Build a delegate by cloning.
 * @param delegate	Delegate to clone.
 */


/**
 * @class MapDelegate
 * A delegate to reference item of @ref concept::MutableMap.
 * 
 * @param C		Type of the container.
 * @param I		Type of the identifier.
 * @param T		Type of the item.
 * @param D		Type to get the default value (as the get() operation requires
 * 				a default value, default to @ref Default class with a value
 * 				of 0).
 */


/**
 * @fn MapDelegate::MapDelegate(C& container, const I& identifier);
 * Build a delegate for an item of a map.
 * @param	container	Item container map.
 * @param	identifier	Item map identifier.
 */


/**
 * @fn MapDelegate::MapDelegate(const MapDelegate& delegate);
 * Build a delegate by cloning.
 * @param delegate	Delegate to clone.
 */

} // elm
