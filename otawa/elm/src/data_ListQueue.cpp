/*
 *	ListQueue class interface
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

#include <elm/data/ListQueue.h>

namespace elm {

/**
 * @class ListQueue
 * Simple implementation of a queue as a single link list.
 * Notice that this class provides operator << and >> for easy use.
 *
 * @param T	Type of items stored in queue.
 *
 * Implemented concepts:
 * @li @ref elm::concept::Queue.
 *
 * Access complexity:
 * @li put(n) -- O(n)
 * @li get(n) -- O(n)
 *
 * Memory usage:
 * @li two pointers for object itself,
 * @li one pointer + data for stored items.
 *
 * @ingroup data
 */


/**
 * @fn bool ListQueue::isEmpty(void) const;
 * Test if the queue is empty.
 * @return	True if empty, false else.
 */


/**
 * @fn const T &ListQueue::head(void) const;
 * Get the head of the queue.
 * @return	Queue head.
 */


/**
 * @fn T ListQueue::get(void);
 * Get and remove the head of the queue.
 * @return	Queue head.
 */


/**
 * @fn void ListQueue::put(const T &item);
 * Add a new item at the end of the queue.
 * @param item	Item to add.
 */


/**
 * @fn void ListQueue::reset(void);
 * Reset the queue.
 */

}
