/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * elm/block/DynBlock.h -- interface for DynBlock class.
 */
#ifndef ELM_BLOCK_DYNBLOCK_H
#define ELM_BLOCK_DYNBLOCK_H

namespace elm { namespace block {

// DynBlock class
class DynBlock {
public:
	inline DynBlock(int capacity = 256, int increment = 64)
		: _size(0), cap(capacity), inc(increment), buf(new char[capacity]) { }
	inline ~DynBlock(void) { if(buf) delete [] buf; }
	void put(const char *block, int size);
	void get(char *block, int size, int pos);
	char *alloc(int size);
	inline int size(void) const { return _size; }
	inline int capacity(void) const { return cap; }
	inline int increment(void) const { return inc; }
	inline void setSize(int new_size) { _size = new_size; }
	inline void reset(void) { _size = 0; if(!buf) buf = new char[cap]; }
	inline const char *base(void) const { return buf; }
	inline char *detach(void)
		{ char *result = buf; buf = 0; return result; }
private:
	int _size, cap, inc;
	char *buf;
	void grow(int min);
};

} } // elm::block

#endif // ELM_BLOCK_DYNBLOCK_H
