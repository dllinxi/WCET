/*
 *	$Id$
 *	SortedBinMap class implementation
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

#include <elm/genstruct/SortedBinMap.h>

namespace elm { namespace genstruct {

/**
 * @class SortedBinMap
 * A map implemented using the @ref SortedBinTree class.
 * 
 * Implemented concepts: @ref concept::Collection, @ref concept::Map.
 * @param K		Type of keys,
 * @param T		Type of values,
 * @param C		Used comparator (must implement @ref concept::Comparator).
 */


/**
 * @fn SortedBinMap::SortedBinMap(void);
 * Build an empty map.
 */


/**
 * @fn SortedBinMap::SortedBinMap(const SortedBinMap& map);
 * Build a copy of an existing @ref SortedBinMap.
 */	


/**
 * @fn int SortedBinMap::count(void) const;
 * Count the item of entries in the map.
 * @return	Number of entries in the map.
 */


/**
 * @fn bool SortedBinMap::contains(const K &key) const;
 * Test if an entry with the given key is in the map.
 * @param key	Key to look for.
 * @return		True if the key is in the map, false else.
 */


/**
 * @fn bool SortedBinMap::isEmpty(void) const;
 * Test if the map is empty.
 * @return	True if the map is empty, false else.
 */


/**
 * @fn SortedBinMap::operator bool(void) const;
 * Same !isEmpty().
 */


/**
 * @fn Option<const T> SortedBinMap::get(const K &key) const;
 * Look for a value matching the given key.
 * @param key	Key to look for.
 * @return		Optional matching value.
 */


/**
 * @fn T SortedBinMap::get(const K &key, const T &def) const:
 * Look for a value matching the given key.
 * @param key	Key to look for.
 * @param def	Default value returned if the key is not found.
 * @return		Found value or the default value.
 */


/**
 * @fn void SortedBinMap::put(const K& key, const T& value);
 * Put a new value in the map.
 * @param key	Key of the stored value.
 * @param value	Stored value.
 */


/**
 * @fn void SortedBinMap::remove(const K& key);
 * Remove a value from the map.
 * @param key	Key of the value to remove.
 * @warning	It is an error to pass a key not stored in the map.		
 */


/**
 * @fn void SortedBinMap::remove(const ValueIterator& iter);
 * Remove the value pointed by the given iterator.
 * @param iter	Iterator pointing to the value to remove.
 * @warning		It is an error to use an ended iterator.
 */


/**
 * @class SortedBinTree::Iterator
 * Iterator on the entries of the binary tree, that is, of type Pair<K, T>.
 */


/**
 * @class SortedBintree::KeyIterator
 * Iterator on the keys of the entries stored in the tree (traversed in order
 * of the comparator).
 */


/**
 * @class SortedBintree::ValueIterator
 * Iterator on the values stored in the tree (traversed in order
 * of the comparator on the keys).
 */

} } // elm::genstruct
