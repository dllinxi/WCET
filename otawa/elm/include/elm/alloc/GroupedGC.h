/*
 *	GroupedGC class interface
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
#ifndef ELM_ALLOC_GROUPEDGC_H_
#define ELM_ALLOC_GROUPEDGC_H_

#include <elm/util/BitVector.h>
#include <elm/stree/Tree.h>
#include <elm/genstruct/SLList.h>
#include <elm/genstruct/DLList.h>
#include <elm/alloc/DefaultAllocator.h>

namespace elm {

class GroupedGC : public DefaultAllocator {
	friend class TempGroupedGC;
public:
	GroupedGC(t::size size = 4096);
	virtual ~GroupedGC(void);
	void clear(void);
	void doGC(void);
	virtual void *allocate(t::size size) throw(BadAlloc);
	virtual bool mark(void *data, t::size size);
	inline void setDisableGC(bool b) { disableGC = b; }
protected:
	virtual void beginGC(void);
	virtual void collect(void) { ASSERTP(false, "collect function must be implemented."); }
	virtual void endGC(void);

private:
	void newChunk(int index);
	void *allocFromFreeList(t::size size, unsigned int index);

	typedef struct block_t {
		block_t *next;
		t::intptr size;
	} block_t;

	// when init is true, the memory addresses in chunk will not show up in free_list
	typedef struct chunk_t {
		BitVector *bits;
		t::intptr size; // the remaining space of the chunk
		t::intptr index; // the index that the chunk is corresponding to
		t::intptr init; // whether a chunk is at its first use
		t::intptr blockCount;
		t::uint8 buffer[0];
	} chunk_t;

	genstruct::SLList<chunk_t *> chunks; // the list of the chunks
	t::size csize; // the chunk size
	block_t **free_list; // the free list of blocks
	inhstruct::DLList temps;

	typedef stree::Tree<void *, chunk_t *> tree_t;
	tree_t *st; // use to store the tree of the chunks vs the range of the memory addresses

	static inline t::size round(t::size size) { return (size + sizeof(block_t) - 1) & ~(sizeof(block_t) - 1); }


	// for future
	unsigned int shiftToIndex;   // the amount of the bit shift to obtain the exact bins' index


	bool disableGC;                  // if the GC is disabled

	// for testing
	unsigned long totalChunkSize;





	unsigned int currentMaxIndex;
	unsigned int maxSlotSize;


	// see the request distribution
	unsigned long* useDist;
	unsigned long* markDist;
	unsigned long* currMarkDist;
	unsigned long* freeDist;
	unsigned long* currFreeDist;
	unsigned long* chunkDist;
	unsigned long* gcDist;



	// ===========
	unsigned int maxAllocatableIndex;
	chunk_t **chunk_list;
	bool* chunk_init;
	unsigned long* blockCount;

	unsigned int requestCount;
	unsigned int requestThreshold;

	//unsigned int gcCount;
	unsigned int allocFuncCount;
	unsigned int allocCount;
	unsigned int markCount;
	unsigned int gcTimes;
	unsigned long totalAllocTime;
};

}	// elm

#endif /* ELM_ALLOC_GROUPEDGC_H_ */
