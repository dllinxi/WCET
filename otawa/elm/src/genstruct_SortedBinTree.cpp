/*
 *	$Id$
 *	SortedBinTree class implementation
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

#include <elm/genstruct/SortedBinTree.h>

namespace elm { namespace genstruct {

/**
 * @class SortedBinTree
 * 
 * This class provides a collection implementation as a sorted binary tree.
 * Whatever the performed action (lookup, insertion or deletion), it has an
 * average complexity of O(log(n)) with a worst case of O(n). The worst case
 * is reached if the items are inserting in order. Notice that no method of
 * this class is implemented recursively.
 * 
 * This class implements the concept of @ref concept::MutableCollection. 
 * 
 * @param T	Type of items in the data structure.
 * @param C	Type describing the comparison operation (must implement @ref elm::concept::Comparator<T>, @ref elm::Comparator<T> as a default).
 */


/**
 * @class GenSortedBinTree
 * Generative class for sorted binary trees. Base class of SortedBinTree
 * or SortedBinMap.
 *
 * @param T	Type of items in the tree.
 * @param K	Key accessor (must implement @ref elm::concept::Key concept, Id<T> as a default).
 * @param C Comparator for keys (must implement @ref elm::concept::Comparator<T>,@ref  elm::Comparator<T> as a default).
 */

/**
 * @fn SortedBinTree<T, C>::SortedBinTree(void)
 * Build an empty sorted binary tree.
 */


/**
 * @fn SortedBinTree<T, C>::SortedBinTree(const SortedBinTree<T, C>& tree);
 * Build a sorted binary tree by copying an existing one.
 * @param tree	Tree to copy.
 */


/**
 * @fn bool SortedBinTree<T, C>::isEmpty(void);
 * Test if the tree is empty.
 * @return	True if the tree is empty, false else.
 */


/**
 * @fn int SortedBinTree<T, C>::count(void);
 * Count the items in the tree.
 * @return	Item count.
 */


/**
 * @fn Option<const T &> SortedBinTree<T, C>::get(const T& value) const;
 * Get the item matching the given value.
 * @param value	Value to look for.
 * @return		Matching value in the tree.
 */


/**
 * @fn const T& SortedBinTree<T, C>::get(const T& value, const T& def) const;
 * Look for the item matching the given value.
 * @param value	Value to look for.
 * @param def	Default value to return if the value is not found.
 * @return		Found value or the default value.
 */


/**
 * @fn bool SortedBinTree<T, C>::contains(const T& value) const;
 * Test if the tree contains the given value.
 * @param value	Value to look for.
 * @return		True if the value is found, false else.
 */


/**
 * @fn SortedBinTree<T, C>::operator bool(void) const;
 * Same as !isEmpty().
 */


/**
 * @fn void SortedBinTree<T, C>::clear(void);
 * Remove all items from the tree.
 */


/**
 * @fn void SortedBinTree<T, C>::add(const T &item);
 * Add an item in the tree. Notice that duplicated value are kept as is.
 * @param item	Item to store in the tree.
 */


/**
 * @fn void SortedBinTree<T, C>::addAll(const S<T> &items);
 * Add a collcection of items to the tree.
 * @param	Item collection to add.
 */


/**
 * @fn void SortedBinTree<T, C>::remove(const T &item);
 * Remove an item from the tree.
 * @warning	It is error to attempt to remove a value not contained in the tree.
 * @param item	Item to remove.
 */


/**
 * @fn void SortedBinTree<T, C>::removeAll(const S<T> &items);
 * Remove a collection of items from the tree.
 * @warning	It is error to attempt to remove a value not contained in the tree.
 * @param items	Collection of items to remove.
 */

} } // elm::genstruct
