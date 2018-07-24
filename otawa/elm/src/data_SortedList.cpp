/*
 *	$Id$
 *	SortedSLList class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2015, IRIT UPS.
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

#include <elm/genstruct/SortedSLList.h>

namespace elm { namespace genstruct {


/**
 * @class SortedSLList
 * @author C. Ballabriga <ballabri@irit.fr>
 * @deprecated 	Use @ref SortedList in place.
 */

}	// genstruct

/**
 * @class SortedList
 *
 * This class provides a sorted list implementation using single-link lists
 * (@ref List). Elements are sorted in increasing order of the used comparator.
 *
 * Its performances are not very good but the memory footprint
 * is very low. Use it only for small collection of elements.
 *
 * @par Performances
 * @li adding an item: O(n) worst, O(n/2) average.
 * @li looking/removing an item: O(n) worst, O(n/2) average.
 * @li memory footprint: same as @ref List.
 *
 * @par Implemented concepts
 * @ref elm::concept::Collection
 * @ref elm::concept::MutableCollection
 * @ref elm::concept::List
 *
 * @param T	Type of data stored in the list.
 * @param A	Adapter to access key part (must implement concept @ref elm::concept::Adapter).
 * @param C	Comparator of the stored items (must implement the concept @ref elm::concept::Comparator).
 *
 * @author C. Ballabriga
 * @author H. Cassé <casse@irit.fr>
 * @ingroup data
 */


/**
 * @fn const T& SortedList::first(void) const;
 * Get the first item of the list.
 * @return	First item.
 * @warning	It is an error to call this method if the list is empty.
 */


/**
 * @fn const T& SortedList::last(void) const;
 * Get the last item of the list. Remark that this method is really inefficient.
 * Its working time is in O(n), n number of nodes in the list. Use it only with
 * small list or revert to more powerful data structures.
 * @return	Last item.
 * @warning	It is an error to call this method if the list is empty.
 */


/**
 * @fn int SortedList::count(void) const;
 * Count the items in the list.
 * @return	Item count.
 */


/**
 * @fn bool SortedList::isEmpty(void) const;
 * Test if the list is empty.
 * @return	True if the list is empty, false else.
 */


/**
 * @fn void SortedList::addFirst(const T& item);
 * Add the given item at the first position in the list.
 * @param item	Item to add.
 */


/**
 * @fn void SortedList::addLast(const T& item);
 * Add the given item at the last position in the list. Remark that this method is really inefficient.
 * Its working time is in O(n), n number of nodes in the list. Use it only with
 * small list or revert to more powerful data structures.
 * @param item	Item to add.
 */


/**
 * @fn void SortedList::removeFirst(void);
 * Remove the first item from the list.
 * @warning	It is an error to call this method if the list is empty.
 */


/**
 * @fn void SortedList::removeLast(void);
 * Remove the last item from the list. Remark that this method is really inefficient.
 * Its working time is in O(n), n number of nodes in the list. Use it only with
 * small list or revert to more powerful data structures.
 * @warning	It is an error to call this method if the list is empty.
 */


/**
 * @fn void SortedList::remove(const T& item);
 * Remove the given item from the list or just one if the list contains many
 * items equals to the given one. The item type T must support the equality /
 * inequality operators.
 * @param item	Item to remove.
 */


/**
 * @class SortedList::Iterator
 * Iterator for generic single link-list.
 */


/**
 * @fn bool SortedList::Iterator::ended(void) const;
 * Test if the end of the list is reached.
 * @return	True if the end is reached, false.
 */


/**
 * @fn T SortedList::Iterator::item(void) const;
 * Get the current item.
 * @return	Current item.
 * @warning It is an error to call this method if the end of the list is
 * reached.
 */


/**
 * @fn void SortedList::Iterator::next(void);
 * Go to the next item.
 */

} // elm
