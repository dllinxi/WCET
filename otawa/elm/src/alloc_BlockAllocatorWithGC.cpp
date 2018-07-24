/*
 *	BlockAllocatorWithGC class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2012, IRIT UPS.
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

#include <elm/alloc/BlockAllocatorWithGC.h>

namespace elm {

/**
 * @class AbstractBlockAllocatorWithGC
 * Implements the block allocator whose the template form is @ref BlockAllocatorWithGC.
 */


/**
 * Build the allocator.
 * @param block_size	Size of the blocks.
 * @param chunk_size	Size of the memory chunks (1Mb as a default).
 */
AbstractBlockAllocatorWithGC::AbstractBlockAllocatorWithGC(t::size block_size, t::size chunk_size)
: free_list(0), free_cnt(0), bsize(block_size), csize(chunk_size) {
	ASSERTP(block_size < chunk_size, "block size must be lower than chunk size");
	chunks.add(new t::uint8[csize]);
	top = chunks.top();
}


/**
 */
AbstractBlockAllocatorWithGC::~AbstractBlockAllocatorWithGC(void) {
	for(int i = 0; i < chunks.count(); i++)
		delete [] chunks[i];
}


/**
 * Allocate a block of the given size.
 * This function accepts as size only the size of elements.
 * @param size		Size of the element.
 * @return			Allocated block.
 * @throw BadAlloc	If the allocation fails.
 */
void *AbstractBlockAllocatorWithGC::allocate(t::size size) throw(BadAlloc) {
	ASSERT(size == bsize);
	return allocate();
}


/**
 */
void *AbstractBlockAllocatorWithGC::allocate(void) throw(BadAlloc) {

	// take it from free blocks
	if(free_list) {
		void *r = free_list;
		free_list = free_list->next;
		free_cnt--;
		return r;
	}

	// any place in the current block
	if(top + bsize <= chunks.top() + csize) {
		void *r = top;
		top += bsize;
		return r;
	}

	// else collect garbage
	else {
		collectGarbage();
		if(free_list) {
			void *r = free_list;
			free_list = free_list->next;
			free_cnt--;
			return r;
		}
		else {
			chunks.add(new t::uint8[csize]);
			top = chunks.top() + bsize;
			return chunks.top();
		}
	}
}


/**
 * Function for compatibility with Allocator concept.
 * Do nothing yet.
 * @param block		Block to free.
 */
void AbstractBlockAllocatorWithGC::free(void *block) {
}


/**
 * Perform a garbage collection.
 */
void AbstractBlockAllocatorWithGC::collectGarbage(void) {

	// allocate bit vector
	coll = new BitVector(chunks.count() * csize / bsize, false);
	beginGC();

	// collect the blocks
	collect();

	// collect the died blocks
	free_cnt = 0;
	for(BitVector::ZeroIterator n(*coll); n; n++) {
		int chunk = *n / (csize / bsize);
		free_t *nfree = (free_t *)(chunks[chunk] + (*n - chunk * (csize / bsize)) * bsize);
		nfree->next = free_list;
		free_list = nfree;
		free_cnt++;
	}

	// clean up bit vector
	endGC();
	delete coll;
}


/**
 * @fn t::size AbstractBlockAllocatorWithGC::blockSize(void) const;
 * Get the block size.
 * @return	Block size (in bytes).
 */


/**
 * @fn t::size AbstractBlockAllocatorWithGC::chunkSize(void) const;
 * Get the memory chunk size.
 * @return	Memory chunk size (in bytes).
 */


/**
 * Mark a block is living.
 * @param ptr	Pointer on the block to mark.
 * @return		True if the block was not already marked (continue shallow traversal), false else.
 */
bool AbstractBlockAllocatorWithGC::mark(void *ptr) {

	// find the chunk
	int chunk = -1;
	for(int i = 0; i < chunks.count(); i++)
		if(chunks[i] <= ptr && ptr < chunks[i] + csize) {
			chunk = i;
			break;
		}
	ASSERTP(chunk >= 0, "collected block is out of the allocator");

	// check the bit
	int n = ((t::uint8 *)ptr - chunks[chunk]) / bsize + chunk * (csize / bsize);
	if(coll->bit(n))
		return false;
	else {
		coll->set(n);
		return true;
	}
}


/**
 * Possibly overridden by the actual implementation,
 * called before starting the garbage collection.
 * As a default, do nothing.
 */
void AbstractBlockAllocatorWithGC::beginGC(void) {
}


/**
 * Possibly overridden by the actual implementation,
 * called after ending the garbage collection.
 * As a default, do nothing.
 */
void AbstractBlockAllocatorWithGC::endGC(void) {
}


/**
 * Compute the total count of allocated blocks (including used and fried ones).
 * @return	Total count of allocated blocks.
 */
int AbstractBlockAllocatorWithGC::totalCount(void) const {
	return (csize / bsize) * (chunks.length() - 1) + (top - chunks.top()) / bsize;
}


/**
 * @fn AbstractBlockAllocatorWithGC::int freeCount(void) const;
 * Get the count of blocks in the free list.
 * @return	Free list blocks.
 */


/**
 * @fn int AbstractBlockAllocatorWithGC::usedCount(void) const;
 * Get the count of apparently used. This count is only precise just after a garbage collection.
 * Else it may includes non-living blocks that have not been already collected.
 * @return	In-use blocks.
 */


/**
 * @fn void AbstractBlockAllocatorWithGC::collect(void);
 * This method must be overridden by the actual implementation
 * of the allocation to perform living block collection, that is,
 * call the mark() method for each living block.
 */


/**
 * @class BlockAllocatorWithGC
 * This class provides an allocator of block of same size supporting garbage collection.
 * The blocks are allocated with a call to BlockAllocatorWithGC::allocate(). When no more
 * memory is available, a garbage collection is launched: it is performed with the help
 * of the user class that must provides all living blocks by overriding the method BlockAllocatorWithGC::collect().
 * For each living block, this method must perform a call to BlockAllocatorWithGC::mark() to record it. If the block
 * has already been collected, this method return false else, true returned and the block may be deeply collected
 * (sub-blocks linked to this one must be collected in turn).
 *
 * Below, a simple example implementing a-la Lisp / Scheme lists.
 * @code
 * class Cons {
 * public:
 * 	...
 *  int hd;
 * 	Cons *tl;
 * };
 *
 * class ConsAllocator: public BlockAllocatorWithGC<Cons> {
 * public:
 * 		...
 * protected:
 * 	virtual void collect(void) {
 * 		// for each list header hd
 * 			collect(hd);
 * 	}
 *
 * private:
 * 	void collect(Cons *cons) {
 * 		while(mark(cons))
 * 			cons = cons->tl;
 * 	}
 * };
 * @endcode
 *
 * @param T		Type of the allocated blocks.
 */


/**
 * @fn BlockAllocatorWithGC::BlockAllocatorWithGC(t::size chunk_size);
 * Build a block allocator.
 * @param chunk_size	Size of the memory chunks (1Mb as a default).
 */


/**
 * T *allocate BlockAllocatorWithGC::(void) throw(BadAlloc);
 * Allocate a block. This method is very fast unless a garbage collection needs to be invoked.
 * @return	Allocated block.
 * @throw BadAlloc	If no more memory is available.
 */


/**
 * @fn bool BlockAllocatorWithGC::mark(T *b);
 * Mark the given block as living.
 * @param b	Block to mark as living.
 * @return	True if the block is already collected, false else.
};*/

}	// elm
