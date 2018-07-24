/*
 *	$Id$
 *	Generic data structures documentation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
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

namespace elm { namespace genstruct {

/**
 * @defgroup gen_datastruct Old Data Structures
 *
 * ELM provides a collection of generic/template based data structures.
 * All data structure implements the @ref elm::concept::Collection concept.
 * The sections below list the different data structures according to the
 * way they are used. A specific data structure may be listed in different sections.
 *
 * @section gen_utility Utility Classes
 *
 * To make ELM container more flexibility, utility classes allow tuning finely
 * the behavior of the containers. Utility classes (and mainly concepts)includes:
 * @li elm::concept::Adapter -- separation between key and value concerns.
 * @li elm::Comparator -- comparison of keys for sorted containers.
 *
 * @section array Array
 *
 * An array (concept @elm::concept::Array and/or elm::concept::MutableArray)
 * whose items are accessible by their index.
 *
 * @li @ref AllocatedTable -- ELM wrapper around C++ table managing its memory
 * @li @ref FixArray -- Embeds a fixed size C++ table
 * @li @ref FragTable -- extensible array accepting big number of items
 * @li @ref Table -- ELM wrapper around C++ table
 * @li @ref Vector -- extensible array
 * @li @ref VectorQueue -- extensible array manageable as a queue
 *
 * @section map Map
 *
 * Map provides associative array whose items are retrievable thanks to a specific key
 * (concept @ref Map and/or @ref MutableMap).
 *
 * @li @ref AssocList -- associative single link list
 * @li @ref HashTable -- map implemented as a hashing table
 * @li @ref SortedBinMap -- map implemented by a binary tree
 *
 * @section stack Stack
 *
 * A stack implements the usual Last-In-First-Out usage (@ref Stack).
 *
 * @li @ref DLList -- double link list (bidirectionnal traversal)
 * @li @ref SLList -- single link list (unidirectionnal traversal)
 * @li @ref Vector -- extensible table
 *
 * @section queue Queue
 *
 * A queue implements the usual First-In-First-Out usage (@ref Queue).
 *
 * @li @ref DLList -- double link list (bidirectionnal traversal)
 * @li @ref ListQueue -- single-link list implementation of queue.
 * @li @ref VectorQueue -- extensible array manageable as a queue
 *
 * @section list List
 *
 * A list is a specific collection that allows to list the contained arguments
 * (@ref List) using an iterator.
 *
 * @li @ref AllocatedTable -- ELM wrapper around C++ table managing its memory
 * @li @ref AVLTree -- AVL balanced binary tree
 * @li @ref DLList -- double link list (bidirectionnal traversal)
 * @li @ref FragTable -- extensible array accepting big number of items
 * @li @ref HashTable -- map implemented as a hashing table
 * @li @ref SortedBinMap -- map implemented by a binary tree
 * @li @ref SLList -- single link list (unidirectionnal traversal)
 * @li @ref SortedList -- sorted single-link list
 * @li @ref Table -- ELM wrapper around C++ table
 * @li @ref Vector -- extensible array
 * @li @ref VectorQueue -- extensible array manageable as a queue
 *
 * @section tree Tree
 *
 * Tree are may be implemented according different data structures.
 *
 * @li @ref SortedBinTree -- sorted binary tree
 * @li @ref Tree -- children-sibling list tree implementation
 *
 * @section set Set
 *
 * Set stores items without any order and provides fast access to contained
 * items (@ref Set).
 *
 * @li @ref AVLTree -- AVL balanced tree set
 * @li @ref Vector -- extensible table
 *
 * @section graph Graph Structure
 *
 * @li @ref DAGNode -- Directed Acyclic Graph implementation
 *
 * @section deprec_data Deprecated Data Structure
 * ELM is currently moving to a more powerful and easier to use data structure model.
 * Whatever, old data structure are maintained for ascending compatibility but
 * are classified as deprecated:
 * @li @ref elm::genstruct::SortedSLList
 */


/**
 * @class FixArray
 *
 * This class provides a safe wrapper around fixed-size array of C embedding it
 * inside the class (no allocation for the array).
 * It provides the following concepts:
 * @li @ref elm::concept::Collection
 * @li @ref elm::concept::Array
 * @li @ref elm::concept::MutableArray
 *
 * @param T		Type of array items.
 * @param S		Size of the array.
 * @param E		(optional) Equivalence implementation.
 */

} // genstruct

namespace concept {

/**
 * For sorted data, adapter classes (elm/adapter.h) allows to separate
 * in the stored object the key part from the value part. This allows sorted container
 * to adapt themselves easily to the concept of sets and maps.
 *
 * Existing
 * @li @ref elm::IdAdapter -- key and value are the same object,
 * @li @ref elm::PairAdapter -- stored data is made of pair associating a key and a value.
 * @ingroup gen_datastruct
 */
class Adapter {
public:

	/**
	 * Type of the part of data used as a key.
	 */
	typedef void key_t;

	/**
	 * Type of the part of data used as a value.
	 */
	typedef void val_t;

	/**
	 * Data type itself.
	 */
	typedef void data_t;

	/**
	 * Get the key of corresponding data.
	 * @param data	Concerned data.
	 * @return		Matching key.
	 */
	const key_t& key(const data_t& data);

	/**
	 * Get the value of the corresponding data.
	 * @param data	Concerned data.
	 * @return		Matching value.
	 */
	const val_t& value(const data_t& data);

	/**
	 * Get a reference on the value part of data.
	 * @param data	Concerned data.
	 * @return		Matching value reference.
	 */
	val_t& ref(data_t& data);
};

} // concept

} // elm
