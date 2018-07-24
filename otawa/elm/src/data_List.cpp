/*
 *	elm::SLList class implementation
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

#include <elm/data/List.h>

namespace elm {

namespace genstruct {

/**
 * @class SLList
 * @deprecated Please use otawa::SLList <otawa/data/SLList.h> instead.
 * @ingroup gen_datastruct
 */

} // genstruct

/**
 * @class List
 * This class provides a generic implementation of single-link lists.
 *
 * @par Performances
 * @li @ref addition -- O(1)
 * @li @ref removal -- O(n)
 * @li @ref find -- O(n)
 * @li @ref memory -- 2 pointers + 1 pointer / element
 *
 * @par Implemented concepts:
 * @li @ref elm::concept::Collection
 * @li @ref elm::concept::List
 * @li @ref elm::concept::MutableCollection
 * @li @ref elm::concept::MutableList
 * @li @ref elm::concept::Stack
 *
 * @param T	Type of data stored in the list.
 * @param M	Manager supporting equivallence and allocation.
 * @ingroup data
 */


/**
 * @fn void List::copy(const List<T>& list);
 * Copy the given list in the current list (removing the previous items of the current list).
 * @param list	List to copy in.
 */


/**
 * @fn void List::add(const T& value);
 * Add an item to the list. In fact, alias for addFirst().
 * @param value	Value to add.
 */


/**
 * @fn const T& List::first(void) const;
 * Get the first item of the list.
 * @return	First item.
 * @warning	It is an error to call this method if the list is empty.
 */


/**
 * @fn const T& List::last(void) const;
 * Get the last item of the list. Remark that this method is really inefficient.
 * Its working time is in O(n), n number of nodes in the list. Use it only with
 * small list or revert to more powerful data structures.
 * @return	Last item.
 * @warning	It is an error to call this method if the list is empty.
 */


/**
 * @fn int List::count(void) const;
 * Count the items in the list.
 * @return	Item count.
 */


/**
 * @fn bool List::isEmpty(void) const;
 * Test if the list is empty.
 * @return	True if the list is empty, false else.
 */


/**
 * @fn void List::addFirst(const T& item);
 * Add the given item at the first position in the list.
 * @param item	Item to add.
 */


/**
 * @fn void List::addLast(const T& item);
 * Add the given item at the last position in the list. Remark that this method is really inefficient.
 * Its working time is in O(n), n number of nodes in the list. Use it only with
 * small list or revert to more powerful data structures.
 * @param item	Item to add.
 */


/**
 * @fn void List::removeFirst(void);
 * Remove the first item from the list.
 * @warning	It is an error to call this method if the list is empty.
 * @warning	This method may break iterator work!
 */


/**
 * @fn void List::removeLast(void);
 * Remove the last item from the list. Remark that this method is really inefficient.
 * Its working time is in O(n), n number of nodes in the list. Use it only with
 * small list or revert to more powerful data structures.
 * @warning		It is an error to call this method if the list is empty.
 * @warning		This method may break iterator work!
 */


/**
 * @fn prec_iter List::find(const T& item) const;
 * Find an element in the list.
 * @param item	Element to look for.
 * @return		Iterator on the found element or iterator on the list end.
 */


/**
 * @fn prec_iter List::find(const T& item, const prec_iter& pos) const;
 * Find an element in the list from the given position.
 * @param item	Element to look for.
 * @param pos	Position to look from.
 * @return		Iterator on the found element or iterator on the list end.
 */


/**
 * @fn iter List::find(const T& item, const iter& pos) const;
 * Find an element in the list from the given position.
 * @param item	Element to look for.
 * @param pos	Position to look from.
 * @return		Iterator on the found element or iterator on the list end.
 */


/**
 * @fn bool List::contains (const T &item) const;
 * Test if an item is in the list.
 * @param item	Item to look for.
 * @return		True if the item is the list, false else.
 */


/**
 * @fn void List::remove(const T& item);
 * Remove the given item from the list or just one if the list contains many
 * items equals to the given one. The item type T must support the equality /
 * inequality operators.
 * @warning		This method may break iterator work!
 * @param item	Item to remove.
 */


/**
 * @fn void List::remove(prec_iter &iter);
 * Remove the item pointed by the iterator.
 * After this call, the iterator points to the next item (if any).
 * @param iter	Iterator pointing the item to remove.
 * @warning		The iterator must point to an actual item, not at the end of the list.
 */


/**
 * @fn void List::set(const iter &pos, const T &item);
 * change the value of an element of the list.
 * @param pos	Iterator on the element to set.
 * @param item	Value to set.
 * @warning		The iterator must point to an actual element, not the end of the list.
 */


/**
 * @fn void List::addAfter(const iter& pos, const T& value);
 * Add an item after the one pointed by pos.
 * @param pos	Iterator pointing the element to add after.
 * @param value	Value to add.
 * @warning	pos must not point at the end of the list!
 */


/**
 * @fn void List::addBefore(const prec_iter& pos, const T& value);
 * Add item before the element pointer by pos.
 * @param pos	Iterator pointing the element to add before.
 * @param value	Value to add.
 * @warning	pos must not point at the end of the list!
 */


/**
 * @fn List::iter SLList::begin(void) const;
 * Get an iterator on the beginning of the list.
 * @return Beginning iterator.
 */


/**
 * @fn List::iter List::end(void) const;
 * Get an iterator on the end of the list.
 * @return Ending iterator.
 */


/**
 * @fn bool List::equals(const List<T>& l) const;
 * Test if both lists are equal.
 * @param l		List to compare with.
 * @return		True if they are equal, false else.
 */


/**
 * @fn bool List::includes(const List<T>& l) const;
 * Test if the current list includes the given list.
 * @param l		List to test for inclusion.
 * @return		True if current list includes l, false, else.
 */

/**
 * @class List::iter
 * Simple iterator on the list.
 */


/**
 * @class List::prec_iter
 * Iterator on a list storing also a pointer to the preceding
 * element. This allows some special operation like SLList::removeBefore().
 */


/**
 * @class List::mutable_iter
 * Iterator allowing the modification of the current element.
 */


/**
 * @class List::sub_iter
 * Iterator on a sub-list defined by a beginning iterator
 * and an ending (exclusive) iterator.
 */

} // elm
