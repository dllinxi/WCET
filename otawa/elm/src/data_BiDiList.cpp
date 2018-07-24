/*
 *	BiDiList class implementation
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

#include <elm/data/BiDiList.h>


namespace elm {

/**
 * @class BiDiList
 * This class provides a generic implementation of double-link lists.
 *
 * @par Performances
 * @li @ref addition at begin / end -- O(1)
 * @li @ref removal -- O(1)
 * @li @ref find -- O(n)
 * @li @ref memory -- 4 pointers + 2 pointers / element
 *
 * @par Implemented concepts:
 * @li @ref elm::concept::Collection
 * @li @ref elm::concept::List
 * @li @ref elm::concept::MutableCollection
 * @li @ref elm::concept::MutableList
 * @li @ref elm::concept::Queue
 * @li @ref elm::concept::Stack
 *
 * @param T	Type of data stored in the list.
 * @param M	Manager supporting equivallence and allocation.
 * @ingroup data
 */

/**
 * @class BiDiList::Iter
 * Iterator on @ef BiDiList for forward traversal of elements.
 */

/**
 * @class BiDiList::BackIter
 * Iterator on @ef BiDiList for backward traversal of elements.
 */

} // elm
