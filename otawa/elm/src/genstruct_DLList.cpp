/*
 *	$Id$
 *	DLList class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-08, IRIT UPS.
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

#include <elm/genstruct/DLList.h>

namespace elm { namespace genstruct {

/**
 * @class DLList
 * A generic implementation of a double-link list. It may be traversed using
 * the @ref Iterator class or @ref BackIterator class.
 * 
 * @par Implemented Concepts
 * @ref elm::concept::BiDiList
 * @ref elm::concept::MutableList
 * @ref elm::concept::Queue
 * 
 * @param T	Type of items stored in the list.
 * @ingroup gen_datastruct
 */


/**
 * @fn T DLList::first(void) const;
 * Get the first item of the list.
 * @return	First item.
 * @warning It is an error to call this method on an empty list.
 */


/**
 * @fn T DLList::last(void) const;
 * Get the last item of the list.
 * @return	Last item.
 * @warning It is an error to call this method on an empty list.
 */


/**
 * @fn bool DLList::isEmpty(void) const;
 * Test if the list is empty.
 * @return	True if the list is empty, false else.
 */


/**
 * @fn int DLList::count(void) const;
 * Count the items in the list.
 * @return	Item count.
 */


/**
 * @fn bool DLList::contains(const T item) const;
 * Test if the given item is in the list. Notice that the T item type must
 * implement the equality / inequality operators.
 * @param	Item to look for.
 * @return	True if the item is found, false else.
 */


/**
 * @fn void DLList::remove(const T item);
 * Remove the given item from the list, or one the item if the list contains
 * this item many times. Notice that the T item type must
 * implement the equality / inequality operators.
 * @param	Item to remove.
 */


/**
 * @fn void DLList::addFirst(const T item);
 * Add an item at the first position.
 * @param item	Item to add.
 */


/**
 * @fn void DLList::addLast(const T item);
 * Add an item at the last position.
 * @param item	Item to add.
 */


/**
 * @fn void DLList::removeFirst(void);
 * Remove the first item from the list.
 * @warning It is an error to call this method on empty list.
 */


/**
 * @fn void DLList::removeLast(void);
 * Remove the last item from the list.
 * @warning It is an error to call this method on empty list.
 */


/**
 * @fn void DLList::clear(void);
 * Remove all items from the list.
 */


/**
 * @class DLList::Iterator
 * Forward iterator on a list of type @ref DLList.
 */


/**
 * @fn DLList::Iterator::Iterator(const DLList& list);
 * Build an iterator on the given list.
 * @param list	List to iterate on.
 */


/**
 * @fn DLList::Iterator::Iterator(const AbstractIterator& iter);
 * Build an iterator by cloning the given one.
 * @param iter	Iterator to clone.
 */


/**
 * @class DLList::BackIterator
 * Backward iterator on a list of type @ref DLList.
 */


/**
 * @fn DLList::BackIterator::BackIterator(const DLList& list);
 * Build a backward iterator on the given list.
 * @param list	List to iterate on.
 */


/**
 * @fn DLList::BackIterator::BackIterator(const AbstractIterator& iter);
 * Build a backward iterator by cloning the given one.
 * @param iter	Iterator to clone.
 */


} } // elm::genstruct
