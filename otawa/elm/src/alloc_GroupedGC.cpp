/*
 *	GroupedGC class implementation
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

#include <elm/assert.h>
#include <elm/alloc/GroupedGC.h>
#include <elm/stree/SegmentBuilder.h>

//#define AZX
//#define AZY
//#define AZZ

#define VERBOSE_GC
#define SIZE_OF_THE_EXACT_BIN 6 // in power of 2, 6 means 64 exact bins

bool willia = false;
unsigned long processedSemInstCount = 0;
unsigned int gcIndex = 0;

// global to maintain the current iteration of the GC
unsigned int gcCount = 0;

namespace elm {
//#define GCDBG
#define MEASURE_TIME
//#define MEASURE_TIME2

/**
 * @class GroupedGC
 * Basically, this allocator is a stack allocator
 * that support monitored reference collection for garbage collection.
 * This means that the user is responsible to provide the live references
 * at garbage collection time. This is done by overloading the @ref collect()
 * method and calling @ref mark() on each live block.
 *
 * @ingroup alloc
 */

/**
 * Initialize the allocator.
 * @param size	Size of chunks.
 */
GroupedGC::GroupedGC(t::size size)
: csize(round(size)), st(0) {

	//  S            exact-bins
	//  *  |------------------------------|
	//  0  |<-- 1 << exactBinSizeShift --->
	//  X  16 32 48 64 ....................

	// (size >> shiftToIndex) will provide the index of the exact-bin for the given size

	// assume we have a block size of 16 (the block size has to be the power of 2)
	// the shiftToIndex will be 4 which is (log2(16))
	// a request of size 16 will be 16 >> 4 => 1
	// a request of size 32 will be 32 >> 4 => 2 ...



	// calculate the number of the shifts to obtain the index for the exact bins
	shiftToIndex = 0;
	unsigned long temp = sizeof(block_t);
	while((temp & 1) == 0) {
		shiftToIndex++;
		temp = temp >> 1;
	}

	totalChunkSize = 0;
	disableGC = false;

	maxAllocatableIndex = 2048*4; // FIXME



	requestThreshold = 4;
	requestCount = 0;
	chunk_list = new chunk_t*[maxAllocatableIndex];
	free_list = new block_t* [maxAllocatableIndex];
	chunk_init = new bool[maxAllocatableIndex];
	blockCount = new unsigned long[maxAllocatableIndex];
	for(unsigned int i = 0; i < maxAllocatableIndex; i++) {
		free_list[i] = 0; // initialize the free lists
		chunk_list[i] = 0;
		chunk_init[i] = true;
		if(i > 0)
			blockCount[i] = csize / (sizeof(block_t) * i);
	}

	useDist = new unsigned long[maxAllocatableIndex];
	markDist = new unsigned long[maxAllocatableIndex];
	freeDist = new unsigned long[maxAllocatableIndex];
	chunkDist = new unsigned long[maxAllocatableIndex];
	gcDist = new unsigned long[maxAllocatableIndex];
	currMarkDist = new unsigned long[maxAllocatableIndex];
	currFreeDist = new unsigned long[maxAllocatableIndex];
	for(unsigned long i = 0; i < maxAllocatableIndex; i++) {
		useDist[i] = 0;
		markDist[i] = 0;
		freeDist[i] = 0;
		chunkDist[i] = 0;
		gcDist[i] = 0;
		currMarkDist[i] = 0;
		currFreeDist[i] = 0;
	}

	gcCount = 0;
	allocFuncCount = 0;
	markCount = 0;
	allocCount = 0;
	totalAllocTime = 0;
	gcIndex = 0;
	return;
}


/**
 * Add a new chunk to the corresponding index.
 */
void GroupedGC::newChunk(int index) {
	chunk_t *c = (chunk_t *)(new char[sizeof(chunk_t) + csize]);
	chunks.add(c);
	c->bits = 0;
	c->size = csize;
	c->index = index;
	c->init = 1;
	totalChunkSize = totalChunkSize + csize;
	ASSERT(chunk_list[index] == 0);
	chunk_list[index] = c;
	requestCount++;
	chunkDist[index]++;
	c->blockCount = csize / (sizeof(block_t) * (index));


	// shift the size so that the allocation (address descending) and gc (ascending order) can work on the same address
	int offset = csize % (sizeof(block_t)*index);
	if(offset) {
		c->size = c->size - offset;
	}

}

/**
 */
GroupedGC::~GroupedGC(void) {
}


/**
 * Reset the allocator.
 */
void GroupedGC::clear(void) {
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++)
		delete *c;
	free_list[0] = 0;
}


/**
 */
void GroupedGC::doGC(void) {



	gcCount++;

	for(unsigned long i = 0; i < maxAllocatableIndex; i++) {
		currMarkDist[i] = 0;
		currFreeDist[i] = 0;
	}
	beginGC();
	collect();
	endGC();

//	if(gcCount == 154) assert(0);
}


/**
 * Allocate memory from free block list.
 * @param size	Size of block to allocate.
 * @return		Allocated block or null if no block available.
 */
void *GroupedGC::allocFromFreeList(t::size size, unsigned int index) {
	//return new char[size];

	allocFuncCount++;

	// find the index
	ASSERT(index <= maxAllocatableIndex);

	if(free_list[index] != 0) {
		void *res = (t::uint8 *)free_list[index];
		free_list[index] = free_list[index]->next;
		useDist[index]++;
#ifdef AZX
		elm::cerr << __SOURCE_INFO__ << __TAB__ << "[" << allocFuncCount << "] for size = " << size  << " (index = " << index << "), use data from free_list" << " @ " << (void*)res << io::endl;
#endif
		return res;
	}

	// no data from free_list, look if chunk still exists
	if(chunk_list[index] != 0) {
		chunk_t *currChunk = chunk_list[index];
		ASSERT(currChunk->init != 0);
		t::intptr newSize = currChunk->size - size;
		void *res = (t::uint8 *)currChunk->buffer + newSize;
		currChunk->size = newSize;
		if(newSize < size) { // not enough for the next allocation
			chunk_list[index] = 0;
			currChunk->init = 0;
			ASSERT(newSize == 0);
		}
#ifdef AZX
		elm::cerr << __SOURCE_INFO__ << __TAB__ << "[" << allocFuncCount << "] for size = " << size  << " (index = " << index << "), use data from chunk" << " @ " << (void*)res << io::endl;
#endif
		useDist[index]++;
		return res;
	}

	/*
	elm::cout << __SOURCE_INFO__ << "requestCount = " << requestCount << ", can not find space for index " << index << ", making dist = " << dist[index] << io::endl;

	// no chunk available here, see if we can allocate a chunk
	if(requestCount < requestThreshold) {
		elm::cout << __SOURCE_INFO__ << "create new chunk for index " << index << io::endl;
		newChunk(index);
		chunk_t *currChunk = chunk_list[index];
		t::intptr newSize = currChunk->size - size;
		void *res = (t::uint8 *)currChunk->buffer + newSize;
		currChunk->size = newSize;
		elm::cout << __SOURCE_INFO__ << "res = " << (void*)res << io::endl;
#ifdef AZX
		elm::cerr << __SOURCE_INFO__ << __TAB__ << "[" << allocFuncCount << "] for size = " << size  << " (index = " << index << "), use data from new chunk" << " @ " << (void*)res << io::endl;
#endif
		dist[index] = dist[index] + 1;
		return res;
	}
	*/

	return 0;
}

/**
 * Perform an allocation of the given size.
 * @param size	Size of allocated memory.
 */
void *GroupedGC::allocate(t::size s) throw(BadAlloc) {
//void *aaa = new char[s];
//return aaa;


	ASSERTP(s < csize, "block too big for GroupedGC");



	// round the size
	t::size size = round(s);
	unsigned int index = size >> shiftToIndex;


	// look for a free block
	void *res = allocFromFreeList(size, index);
	if(res) {


		return res;
	}

	// in the disableGC mode, new chunk will be allocated directly
	if(chunk_init[index] == true) {
		chunk_init[index] = false;
		newChunk(index);
	}
	else if(!disableGC) {
		gcIndex = index;
		gcDist[index]++;

		// else perform a GC
		doGC();

		// check if necessary to create provisional chunk
		if(currFreeDist[index] < (blockCount[index] >> 1)) {
			newChunk(index);
		}
	}

	// now make some decision on totalChunkSize, totalFreed, and size
	// i.e. even though totalFreed maybe larger then size, but maybe its size is very small
	// so that GC will be activated again ..

	res = allocFromFreeList(size, index);
	if(res) {
		return res;
	}

	// if the storage is still not large enough after GC, allocate a new chunk
	//newChunk();

	newChunk(index);

	res = allocFromFreeList(size, index);

	ASSERT(res != 0);
	return res;
}


/**
 * Called to mark a block as alive.
 * @param data	Alive data block base.
 * @param size	Size of block.
 * @return		True if the block has already been marked, false else.
 * @warning		This function must only be called from the @ref collect() context!
 */
bool GroupedGC::mark(void *data, t::size size) {

	markCount++;

#ifdef AZZ
	elm::cout << __SOURCE_INFO__ << "mark" << markCount << " for " << (void*)data << io::endl;
#endif

	// find the chunk
	chunk_t *gcc = st->get(data);
	ASSERTP(gcc, _ << "during GC, block out of chunks: " << (void *)data << ":" << io::hex(size) << "!");

	markDist[gcc->index]++;

//	if(gcc->init > 0) { // if the chunk is at its first time use, then no need to GC it
//		return false;
//	}
	currMarkDist[gcc->index]++;

	bool res = true;
	//int p = (static_cast<t::uint8 *>(data) - gcc->buffer) / round(size);
	int p = (static_cast<t::uint8 *>(data) - gcc->buffer) / (sizeof(block_t) * gcc->index);
	if(!(*gcc->bits)[p]) {
		res = false;
		gcc->bits->set(p);
	}
	return res;
}


/**
 * Called before a GC starts. Overriding methods must call this one.
 */
void GroupedGC::beginGC(void) {
	// build the data structure
	stree::SegmentBuilder<void *, chunk_t *> builder(0);
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++) {
		builder.add(c->buffer, c->buffer + csize, c);
		c->bits = new BitVector(csize / (sizeof(block_t) * c->index));
	}

	// finalize the tree
	st = new tree_t();
	builder.make(*st);

}


/**
 * @fn void GroupedGC::collect(void);
 * This function is called when references for a garbage collection are needed.
 * It must call the @ref mark() with each alive reference.
 *
 * This function must be override in each use site in order to provide alive references.
 */


/**
 * 	1. Adding the non-marked blocks to the free_list
 * 	2. clear c->bits to prevent memory leakage
 */
void GroupedGC::endGC(void) {
	//return;
	requestCount = 0;
	unsigned int oooo = 0;


	// reset free list
	for(unsigned int i = 0; i < maxAllocatableIndex; i++) {
		free_list[i] = 0;
	}



	// build the list of free blocks
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++) {
		if(c->init > 0) {
			continue;
		}

		// traverse the bits
		int blockSize = (sizeof(block_t) * (c->index));
		unsigned long currentSize = 0;
		int index = c->index;
		unsigned int i = 0;
		unsigned int blockCount = c->blockCount; //csize / blockSize;
		//assert(c->blockCount == (csize/blockSize));
		while(i < blockCount) {
			if(!(*c->bits)[i]) {
				// free block here
				block_t *blk2 = static_cast<block_t *>(static_cast<void *>(c->buffer + i * blockSize));
				block_t *blk = static_cast<block_t *>(static_cast<void *>(c->buffer + currentSize));
				ASSERT(blk == blk2);
				blk->next = free_list[index];
#ifdef NON_OPTIMIZATION
				blk->size = blockSize; // actually not necessary
#endif

#ifdef VERBOSE_GC

				free_list[index] = blk;
				freeDist[index]++;
				currFreeDist[index]++;
				oooo = oooo + blockSize;
#endif


#ifdef AZY
				blk->size = blockSize;
				elm::cerr << __SOURCE_INFO__ << "Simple GC frees " << blk->size << " Bytes (" << (void*)blk <<  " to " << (void*)(((unsigned long)static_cast<void*>(blk))+blk->size) << ") to index = " << index << io::endl;
#endif
//WILLIE_BEGIN_FASTSTATE_GC(
#ifdef GCDBG
				if(debugGC > debugGCCond) elm::cerr << __SOURCE_INFO__ << "Simple GC frees " << blk->size << " Bytes (" << (void*)blk <<  " to " << (void*)(((unsigned long)static_cast<void*>(blk))+blk->size) << ") to index = " << index << io::endl;
#endif

//)WILLIE_END()

			}
			i++;
			currentSize += blockSize;
		}
	} // end of each chunk

	// free the GC resources
	for(genstruct::SLList<chunk_t *>::Iterator c(chunks); c; c++) {
		delete c->bits;
		c->bits = 0;
	}
	delete st;




}



}	// elm
