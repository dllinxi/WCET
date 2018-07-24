/*
 *	$Id$
 *	StackAllocator class implementation
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
#include <elm/alloc/StackAllocator.h>

namespace elm {

/**
 * @class StackAllocator
 * A stack allocator allows to implement quick allocation scheme with even quicker de-allocation.
 * The allocation is performed as in stack, moving a simple pointer. Conversely, the de-allocation
 * is as quick as resetting this pointer to a previous position.
 *
 * Note that the allocation is only bound by the system memory: the stack is split in chunks. Each
 * time a chunk is full, a new one is allocated.
 * @ingroup alloc
 */


/**
 * Default stack allocator.
 */
StackAllocator StackAllocator::DEFAULT;

/**
 * @typedef StackAllocator::mark_t
 * This type represents the marks of the stack position.
 */


/**
 * Build a new stack allocator.
 * @param size		The size in bytes of the chunks (default to 4Kb).
 */
StackAllocator::StackAllocator(t::size size)
: cur(0), max(0),top(0),  _size(size) {
	ASSERTP(_size, "null size is not accepted for chunks of StackAllocator")
}


/**
 */
StackAllocator::~StackAllocator(void) {
	clear();
}


/**
 * Allocate a new block.
 * @param size	Size of the block.
 * @return		Allocated block.
 * @throws BadAlloc		If there is no more memory.
 * @note	The size must not be greater than the size of the chunks.
 */
void *StackAllocator::allocate(t::size size) throw(BadAlloc) {
	ASSERTP(size <= _size, "allocated block too big !")
	if(!cur || size_t(max - top) < size)
		return chunkFilled(size);
	char *res = top;
	top += size;
	return res;
}


/**
 * This method is called when there is no more place in the current chunk.
 * It may be overload to provide custom behavior of the allocator.
 * @param	size	Size of block to allocate.
 * @return			Allocated block.
 * @throiw BadAlloc	In case of fatal allocation error.
 */
void *StackAllocator::chunkFilled(t::size size) throw(BadAlloc) {
	newChunk();
	return allocate(size);
}


/**
 * @fn void StackAllocator::free(void *block);
 * Free the given block.
 * @param block		Block to free.
 * @note	In this allocator, do nothing.
 */


/**
 * Clear all allocated memory.
 */
void StackAllocator::clear(void) {
	while(cur) {
		chunk_t *next = cur->next;
		delete [] (char *)cur;
		cur = next;
	}
}


/**
 * Allocate a new chunk.
 */
void StackAllocator::newChunk(void) throw(BadAlloc) {
	try {
		chunk_t *chunk = (chunk_t *)new char[sizeof(chunk_t) + _size];
		chunk->next = cur;
		cur = chunk;
		top = chunk->buffer;
		max = chunk->buffer + _size;
		//cerr << "DEBUG: new chunk: " << (void *)cur->buffer << io::endl;
	}
	catch(std::bad_alloc&) {
		throw BadAlloc();
	}
}


/**
 * Return a mark on the current stack position.
 * The returned mark may be used to free the next allocated object after
 * the mark set.
 * @return	 Current stack position.
 */
StackAllocator::mark_t StackAllocator::mark(void) {
	return top;
}


/**
 * Release the stack onto the given mark.
 * @param mark		Stack position to free onto.
 */
void StackAllocator::release(mark_t mark) {
	while(cur) {
		if(mark >= cur->buffer && mark < cur->buffer + _size) {
			top = mark;
			break;
		}
		else {
			chunk_t *next = cur->next;
			delete [] (char *)cur;
			cur = next;
		}
	}
	ASSERTP(cur, "mark out of the current AllocatorStack");
}

} // elm
