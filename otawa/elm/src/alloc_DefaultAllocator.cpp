/*
 *	$Id$
 *	BadAlloc exception implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
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

#include <new>
#include <elm/alloc/DefaultAllocator.h>
#include <elm/assert.h>

namespace elm {

/**
 * @defgroup alloc	Allocation Module
 *
 * This module binds together classes providing different ways to perform
 * memory allocation. This kind of class is usually passed to any container
 * class performing automatic memory allocation.
 *
 * The chosen allocator
 * is usually passed as a template parameter to the collection type and
 * as a parameter to the collection constructor.
 *
 * An allocator class must implement the concept @ref elm::concept::Allocator.
 *
 * A collection supporting allocator matches the concept below:
 * @code
 * template <class A>
 * class CollectionWithAllocation {
 * public:
 *		CollectionWithAllocation(A& allocator = A::DEFAULT);
 *	};
 * @endcode
 *
 * An easy use of custom allocator is the overload of @c new and @c delete operators
 * of a class to let passing the allocator as a parameter. The code below gives an examples:
 * @code
 * class MyClass {
 * public:
 * 		// usual constructors, methods and attributes
 * 		inline void *operator new(std::size_t size, Allocator& alloc) { return alloc.allocate(size); }
 * 		inline void operator delete(void *p, std::size_t size)  { alloc.free(p); }
 * @endcode
 *
 * Then, you can create and delete the object with the following code:
 * @code
 * Allocator custom_alloc;
 * MyClass *object = new(custom_alloc) MyClass(arguments here);
 * // do the work with object
 * delete(custom_alloc) object;
 * @endcode
 */


namespace concept {

/**
 * Allocator concept
 * Memory allocation.
 */
class Allocator {

	/**
	 * Allocate a block of the given size.
	 * @param size		Size in bytes of the allocated block.
	 * @throw BadAlloc	If the allocation fails.
	 */
	void *allocate(t::size size) throw(BadAlloc);

	/**
	 * Free a block previously allocated.
	 * @param block	Block to free.
	 */
	void free(void *block);
};
}


/**
 * @class BadAlloc
 * This exception is thrown when a memory allocation fails.
 * @ingroup alloc
 */


/**
 */
BadAlloc::~BadAlloc(void) { }


/**
 */
String 	BadAlloc::message(void) { return "allocation failed"; }


/**
 * @class DefaultAllocator
 * Default implementation of a memory allocator just calling new and delete.
 * @ingroup alloc
 */


/**
 * Default allocator.
 */
DefaultAllocator DefaultAllocator::DEFAULT;


/**
 * Allocate a memory block of the given size.
 * @param size	Size of the block to allocate.
 * @return		Allocated block.
 * @throw BadAlloc	Thrown if there is no more system memory.
 */
void *DefaultAllocator::allocate(t::size size) throw(BadAlloc) {
	try {
		return new char[size];
	}
	catch(std::bad_alloc& e) {
		throw BadAlloc();
		return 0;
	}
}


/**
 * Mark a memory block of the given size so that it will not be released by the garbage collector.
 * @param data	The data to collect
 * @param size	Size of the block to allocate.
 * @return		If the data is already marked
 */
bool DefaultAllocator::mark(void *data, t::size size) {
	ASSERTP(false, "Please implement the mark() method in the specialized allocator");
	return false;
}

/**
 * @fn void DefaultAllocator::free(void *block);
 * Free the given block.
 * @param block	Block to free.
 */

} // elm
