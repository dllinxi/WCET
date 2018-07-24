/*
 *	$Id$
 *	VectorQueue class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-08, IRIT UPS.
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

#include <elm/genstruct/VectorQueue.h>

namespace elm { namespace genstruct {

/**
 * @class VectorQueue<T>
 * This class implements a queue as an extendable vector.
 * @param T	Type of queued items.
 * @param E Equivallence relation to use (default to @ref Equiv<T>).
 * @ingroup gen_datastruct
 */
 

/**
 * @fn VectorQueue::VectorQueue(int capacity);
 * Build a new vector queque. The passed capacity will be used for computing
 * the actual capacity as power of two of the capacity.
 * @param capacity	Capacity exponent.
 */


/**
 * @fn int VectorQueue::capacity(void) const;
 * Get the current capacity of the queue.
 * @return	Current capacity.
 */


/**
 * @fn int VectorQueue::size(void) const;
 * Get the current size of the queue.
 * @return	Queue size.
 */


/**
 * @fn bool VectorQueue::isEmpty(void) const;
 * Test if the queue is empty.
 * @return	True if the queue is empty, false else.
 */


/**
 * @fn void VectorQueue::put(const T& item);
 * Put an item at the head of the queue.
 * @param item	Item to put.
 */


/**
 * @fn const T& VectorQueue::get(void);
 * Get and remove the head item of the queue.
 * @warning It is an error to perform this call when the queue is empty.
 * @return	Head item.
 */


/**
 * @fn const T& VectorQueue::head(void);
 * Get the head item of the queue without removing it.
 * @warning It is an error to perform this call when the queue is empty.
 * @return	Head item.
 */


/**
 * @fn void VectorQueue::reset(void);
 * Reset the queue to its initialstate (except for the capacity) and remove
 * all enqueued items.
 */


/**
 * @fn bool VectorQueue::contains(const T& val) const;
 * Test if the queue contains the given value.
 * @param val	Value to look in the queue.
 * @return		True if the value is found, false else.
 */

} } // elm::genstruct
