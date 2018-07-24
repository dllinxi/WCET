/*
 *	SimpleGC class interface
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
#ifndef ELM_ALLOC_SIMPLEGC_H_
#define ELM_ALLOC_SIMPLEGC_H_

#include <elm/util/BitVector.h>
#include <elm/stree/Tree.h>
#include <elm/genstruct/SLList.h>
#include <elm/genstruct/DLList.h>
#include <elm/alloc/DefaultAllocator.h>

namespace elm {

class SimpleGC;

class Temp: public inhstruct::DLNode {
public:
	inline Temp(SimpleGC& gc);
	virtual ~Temp(void);
	virtual void collect(SimpleGC& gc) = 0;
};

template <class T>
class TempPtr: public Temp {
public:
	inline TempPtr(SimpleGC& gc, T *ptr): Temp(gc), p(ptr) { }
	virtual void collect(SimpleGC& gc) { p->collect(gc); }

	inline operator T *(void) const { return p; }
	inline T *operator&(void) const { return *p; }
	inline T *operator->(void) const { return *p; }
	inline T& operator*(void) const { return *p; }
	inline operator bool(void) const { return p; }
	inline TempPtr<T>& operator=(T *ptr) { p = ptr; return *this; }

private:
	T *p;
};

class SimpleGC {
	friend class Temp;
public:
	SimpleGC(t::size size = 4096);
	virtual ~SimpleGC(void);
	void clear(void);
	void doGC(void);

	void *allocate(t::size size) throw(BadAlloc);
	inline void free(void *block) { }

protected:
	bool mark(void *data, t::size size);

	virtual void beginGC(void);
	virtual void collect(void) = 0;
	virtual void endGC(void);

private:
	void newChunk(void);
	void *allocFromFreeList(t::size size);

	typedef struct block_t {
		block_t *next;
		t::intptr size;
	} block_t;
	typedef struct chunk_t {
		BitVector *bits;
		t::uint8 buffer[0];
	} chunk_t;

	genstruct::SLList<chunk_t *> chunks;
	t::size csize;
	block_t *free_list;
	inhstruct::DLList temps;

	typedef stree::Tree<void *, chunk_t *> tree_t;
	tree_t *st;

	static inline t::size round(t::size size) { return (size + sizeof(block_t) - 1) & ~(sizeof(block_t) - 1); }
};

}	// elm

#endif /* ELM_ALLOC_SIMPLEGC_H_ */
