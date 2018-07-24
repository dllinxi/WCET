/*
 *	imm::List class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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

#include <elm/imm/list.h>
#include <elm/imm/sorted_list.h>

namespace elm { namespace imm {

/**
 * @defgroup imm	Immutable Data Structure
 *
 * ELM provides a set of immutable data structure like lists and tree.
 * The goal of this module is to allow functional programming style
 * while supporting limited but effective garbage collection.
 * Basically, the immutable nature of the data structure allows (a) to handle more easily the memory management
 * and (b) providing an efficient and compact way of memory use while avoiding a lot of time-costly memory
 * operations.
 *
 * Coming with this immutable nature, an efficient garbage collector is provided.
 * It is quite limited because it has no way to collect the living instances of the data structures items.
 * Therefore, it requires the user to specialize it and to provided a collect() function responsible
 * to supply the list of living data structure items.
 */


/**
 * @class list
 * Implementation of immutable and garbage collected list.
 *
 * As C++ does not allow
 * to implement easily and efficiently a garbage collector, the user of this class
 * is responsible to provided its own collector using the function list<T>::add(object)
 * with object inheriting from list<T>::Collector and implementing the member function
 * list<T>::Collector::collect(). This function must call mark() for each list in use
 * as in the example below:
 *
 * @code
 * list<int> first_list;
 * genstruct::Vector<list<int> > lists;
 *
 * class MyCollector: public list<int>::Collector {
 * protected:
 *	virtual void collect(void) {
 *		mark(first_list);
 *		for(int i = 0; i < lists.count(); i++)
 *			mark(lists[i]);
 *	}
 * };
 *
 * int main(void) {
 * 	MyCollector coll;
 * 	list<int>::add(coll);
 *
 * 	// work with lists now
 *
 * }
 * @endcode
 *
 * @param T		Type of items in the list.
 * @ingroup imm
 */


/**
 * @fn list<T> list::null;
 */


/**
 * @fn list::list(void);
 * Build an empty list.
 */


/**
 * @fn list::list(const list& l);
 * Clone a list.
 */


/**
 * @fn const T& list::hd(void) const;
 * Get the first item of the list.
 * @return		Head item.
 */


/**
 * @fn list<T> list::tl(void) const;
 * Get the sub-list after the first item in the list.
 * @return		Tail of the list.
 */


/**
 * @fn int list::length(void) const;
 * Compute the length of the list.
 * @return	List length.
 */


/**
 * @fn list<T> list::cons(const T& h, list<T> t);
 * Build a new list by prepending h to t.
 * @param h		Item to prepend.
 * @param t		List to use as tail.
 * @return		Built list.
 */


/**
 * @fn bool list::isEmpty(void) const;
 * Test if the list is empty.
 * @return	True if the list is empty, false else.
 */


/**
 * @fn bool list::contains(const T& v);
 * Test if v is in the list. The @ref Equiv<T> is used to perform the test
 * of equality. Therefore, a user can provide its own implementation of @ref Equiv<T>
 * to change the usual behavior of the equality tests.
 * @param v		Item to test.
 * @return		True if v is in the list, false else.
 */


/**
 * @fn bool list::equals(list<T> l) const;
 * Test if the current list and l are equals.
 * @param l		List to compare.
 * @return		True if both lists are equal, false else.
 */


/**
 * @fn list<T> list::remove(const T& h);
 * Build a new list without the first instance of h.
 * @param h		Item to remove.
 * @return		List without first instance of h.
 */


/**
 * @fn list<T> cons(const T& h, list<T> tl);
 * Build a new list node with h value and prepend it to tl.
 * @param h		Item to store in the new node.
 * @param t		List to preprend to.
 * @return		Created list.
 */

} }	// elm::imm
