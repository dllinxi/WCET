/*
 *	BlockAllocatorWithGC class interface
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
#ifndef ELM_ALLOC_BLOCK_ALLOCATOR_WITH_GC
#define ELM_ALLOC_BLOCK_ALLOCATOR_WITH_GC

#include <elm/alloc/DefaultAllocator.h>
#include <elm/genstruct/Vector.h>
#include <elm/util/BitVector.h>

namespace elm {

// abstract version
class AbstractBlockAllocatorWithGC {
public:
	AbstractBlockAllocatorWithGC(t::size block_size, t::size chunk_size = 1 << 20);
	virtual ~AbstractBlockAllocatorWithGC(void);
	void *allocate(void) throw(BadAlloc);
	void collectGarbage(void);
	inline t::size blockSize(void) const { return bsize; }
	inline t::size chunkSize(void) const { return csize; }
	inline int freeCount(void) const { return free_cnt; }
	int totalCount(void) const;
	inline int usedCount(void) const { return totalCount() - freeCount(); }

	// Allocator concept compatibility
	void *allocate(t::size size) throw(BadAlloc);
	void free(void *block);

protected:
	bool mark(void *ptr);
	virtual void collect(void) = 0;
	virtual void beginGC(void);
	virtual void endGC(void);

	typedef struct free_t { free_t *next; } free_t;
	free_t *free_list;
	int free_cnt;

private:
	genstruct::Vector<t::uint8 *> chunks;
	t::uint8 *top;
	t::size bsize, csize;
	BitVector *coll;
};

// template version
template <class T>
class BlockAllocatorWithGC: public AbstractBlockAllocatorWithGC {
public:
	inline BlockAllocatorWithGC(t::size chunk_size = 1 << 20): AbstractBlockAllocatorWithGC(sizeof(T), chunk_size) { }
	inline T *allocate(void) throw(BadAlloc) { return static_cast<T *>(AbstractBlockAllocatorWithGC::allocate()); }

protected:
	inline bool mark(T *b) { return AbstractBlockAllocatorWithGC::mark(b); }
};

}	// elm

#endif	// ELM_ALLOC_BLOCK_ALLOCATOR_WITH_GC
