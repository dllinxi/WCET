/*
 *	$Id$
 *	StackAllocator class interface
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

#ifndef ELM_ALLOC_STACKALLOCATOR_H_
#define ELM_ALLOC_STACKALLOCATOR_H_

#include <elm/types.h>
#include <elm/assert.h>
#include <elm/alloc/DefaultAllocator.h>
#include <elm/PreIterator.h>

namespace elm {

// StackAllocator class
class StackAllocator {
public:
	static StackAllocator DEFAULT;
	StackAllocator(t::size size = 4096);
	virtual ~StackAllocator(void);
	void *allocate(t::size size) throw(BadAlloc);
	inline void free(void *block) { }
	void clear(void);

	// mark management
	typedef char *mark_t;
	mark_t mark(void);
	void release(mark_t mark);

	// template access
	template <class T>
	inline T *allocate(int n = 1) throw(BadAlloc) { return static_cast<T *>(StackAllocator::allocate(n * sizeof(T))); }

protected:
	virtual void *chunkFilled(t::size size) throw(BadAlloc);

	typedef struct chunk_t {
		struct chunk_t *next;
		char buffer[0];
	} chunk_t;

	class ChunkIter: public PreIterator<ChunkIter, chunk_t *> {
	public:
		inline ChunkIter(const StackAllocator& a): cur(a.cur) { }
		inline ChunkIter(const ChunkIter& i): cur(i.cur) { }
		inline ChunkIter& operator=(const ChunkIter& i) { cur = i.cur; return *this; }
		inline bool ended(void) const { return !cur; }
		inline chunk_t *item(void) const { return cur; }
		inline void next(void) { cur = cur->next; }
	private:
		chunk_t *cur;
	};

	inline t::size chunkSize(void) const { return _size; }
	void newChunk(void) throw(BadAlloc);

private:
	chunk_t *cur;
	char *max, *top;
	t::size _size;
};

}		// elm

#endif /* ELM_ALLOC_STACKALLOCATOR_H_ */
