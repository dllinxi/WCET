/*
 *	$Id$
 *	AssocList class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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

#include <elm/genstruct/AssocList.h>

namespace elm { namespace genstruct {

/**
 * @class AssocList
 * An associative list is a sorted list providing assocition between a key
 * and to value. It may be used to store a collection of item indexed that
 * support fast retrieval or deletion.
 * 
 * Note that this collection is implemented as a list. If the size of the list
 * is n, each operation has a complexity of O(n/2) in average.
 * In practice, this class becomes wholly inefficient when the list contains
 * several tens of values.
 * 
 * @par Implemented concepts:
 * @li elm::concept::Collection
 * @li elm::concept::List
 * @li elm::concept::Map
 * @li elm::concept::MutableMap
 * 
 * @param K		Indexed key type.
 * @param T		Stored value type.
 * @param C		Comparator used to compare K keys (default to elm::Comparator<K>).
 * @param E		Equivalence relation for T (default to Equiv<T>).
 * @param N		Null value for T (default to type_infow<T>).
 * 
 * @ingroup gen_datastruct
 */


/**
 * @fn AssocList::AssocList(void);
 * Build an empty associative list.
 */


/**
 * @fn AssocList::AssocList(const AssocList& alist);
 * Build an associative list by cloning another one.
 * @param alist		Associative list to clone.
 */

} } // elm::genstruct

