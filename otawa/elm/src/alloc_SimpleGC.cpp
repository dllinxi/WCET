/*
 *	SimpleGC class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2014, IRIT UPS.
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

#include <elm/alloc/SimpleGC.h>
#include <elm/stree/SegmentBuilder.h>

namespace elm {

/**
 * @class Temp
 * This class records itself to a garbage collector as representing
 * a set of alive values. When a garbage collection is started,
 * the collect() method is called.
 *
 * @ingroup alloc
 */

/**
 * Build a temporary.
 * @param gc	Garbage collector.
 */
Temp::Temp(SimpleGC& gc) {
	gc.temps.addFirst(this);
}

/**
 */
Temp::~Temp(void) {
	remove();
}


/**
 * @class class TempPtr
 * This class is pointer wrapper class to use with @ref SimpleGC.
 * It records a pointer allocated from SimpleGC as alive until
 * it is deleted (at end of its declaring block for example).
 *
 * It is useful to record pointer allocated from the garbage collector
 * but not already stored in point reachable from the application
 * data structures. It is particularly useful as a function local
 * variable when several objects are allocated to form a complex
 * structure before being stored in the main data structures.
 *
 * @pram T	Type of pointer data.
 * @ingroup alloc
 */


/**
 * @class SimpleGC
 * Basically, this allocator is a stack allocator
 * that support monitored reference collection for garbage collection.
 * This means that the user is responsible to provide the live references
 * at garbage collection time. This is done by overloading the @ref collect()
 * method and calling @ref mark() on each live block.
 *
 * @sa Temp, TempPtr.
 * @ingroup alloc
 */

/**
 * Initialize the allocator.
 * @param size	Size of chunks.
 */
SimpleGC::SimpleGC(t::size size)
: csize(round(size)), free_list(0), st(0) {
}


/**
 * Add a new chunk.
 */
void SimpleGC::newChunk(void) {
	chunk_t *c = (chunk_t *)(new char[sizeof(chunk_t) + csize]);
	chunks.add(c);
	c->bits = 0;
	block_t *b = (block_t *)c->buffer;
	b->next = free_list;
	b->size = csize;
	free_list = b;
}


/**
 */
SimpleGC::~SimpleGC(void) {
}


/**
 * Reset the allocator.
 */
void SimpleGC::clear(void) {
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++)
		delete *c;
	free_list = 0;
}


/**
 */
void SimpleGC::doGC(void) {
	beginGC();
	for(inhstruct::DLNode *node = temps.first(); !node->atEnd(); node = node->next())
		static_cast<Temp *>(node)->collect(*this);
	collect();
	endGC();
}


/**
 * Allocate memory from free block list.
 * @param size	Size of block to allocate.
 * @return		Allocated block or null if no block available.
 */
void *SimpleGC::allocFromFreeList(t::size size) {
	for(block_t *block = free_list, **prev = &free_list; block; prev = &block->next, block = block->next)
		if(block->size >= size) {
			void *res = (t::uint8 *)block + block->size - size;
			block->size -= size;
			if(!block->size)
				*prev = block->next;
			return res;
		}
	return 0;
}


/**
 * Perform an allocation of the given size.
 * @param size	Size of allocated memory.
 */
void *SimpleGC::allocate(t::size size) throw(BadAlloc) {
	ASSERTP(size < csize, "block too big for SimpleGC");

	// round the size
	size = round(size);

	// look for a free block
	void *res = allocFromFreeList(size);
	if(res)
		return res;

	// else perform a GC
	doGC();
	res = allocFromFreeList(size);
	if(res)
		return res;

	// finally, allocate a new chunk
	newChunk();
	return allocFromFreeList(size);
}


/**
 * Called to mark a block as alive.
 * @param data	Alive data block base.
 * @param size	Size of block.
 * @return		True if the block has already been marked, false else.
 * @warning		This function must only be called from the @ref collect() context!
 */
bool SimpleGC::mark(void *data, t::size size) {

	// find the chunk
	chunk_t *gcc = st->get(data);
	ASSERTP(gcc, _ << "during GC, block out of chunks: " << (void *)data << ":" << io::hex(size) << "!");
	int p = (static_cast<t::uint8 *>(data) - gcc->buffer) / sizeof(block_t);
	int s = (size + sizeof(block_t) - 1) / sizeof(block_t);

	// mark it and make result
	bool res = true;
	int i;
	for(i = p; i < p + s; i++)
		if(!(*gcc->bits)[i]) {
			res = false;
			gcc->bits->set(i);
		}
	return res;
}


/**
 * Called before a GC starts. Overriding methods must call this one.
 */
void SimpleGC::beginGC(void) {

	// build the data structure
	stree::SegmentBuilder<void *, chunk_t *> builder(0);
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++) {
		builder.add(c->buffer, c->buffer + csize, c);
		c->bits = new BitVector(csize / sizeof(block_t));
	}

	// finalize the tree
	st = new tree_t();
	builder.make(*st);
}


/**
 * @fn void SimpleGC::collect(void);
 * This function is called when references for a garbage collection are needed.
 * It must call the @ref mark() with each alive reference.
 *
 * This function must be override in each use site in order to provide alive references.
 */


/**
 */
void SimpleGC::endGC(void) {

	// reset free list
	free_list = 0;

	// build the list of free blocks
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++) {

		// traverse the bits
		int i = 0, b = -1, cs = csize / sizeof(block_t);
		while(i < cs) {

			// skip ones
			while(i < cs && (*c->bits)[i])
				i++;

			// if zero found
			if(i < cs) {
				b = i;
				i++;

				// skip zeroes
				while(i < cs && !(*c->bits)[i])
					i++;

				// create the free block
				// if b == 0, we could remove the block (if facility was available)
				block_t *blk = static_cast<block_t *>(static_cast<void *>(c->buffer + b * sizeof(block_t)));
				blk->size = (i - b) * sizeof(block_t);
				blk->next = free_list;
				free_list = blk;
			}
		}
	}

	// free the GC resources
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++) {
		delete c->bits;
		c->bits = 0;
	}
	delete st;
}

}	// elm
