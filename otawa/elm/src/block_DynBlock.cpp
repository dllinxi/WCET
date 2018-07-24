/*
 * $Id$
 * Copyright (c) 2004-07, IRIT - UPS
 *
 * src/DynBlock.cpp -- implementation for DynBlock class.
 */

#include <elm/assert.h>
#include <cstring>
#include <elm/block/DynBlock.h>

namespace elm { namespace block {

/**
 * @class DynBlock
 * This class implements an exyensible block of memory.
 */

/*
 * Enlarge the current buffer with some given minimum.
 * @param min	Minimal enlargement size.
 */
void DynBlock::grow(int min) {

	// Compute new size
	int new_size = cap + ((min > inc) ? min : inc);
	
	// Allocate it
	char *new_buf = new char[new_size];
	
	// Initialize it
	memcpy(new_buf, buf, _size);
	delete [] buf;
	buf = new_buf;
}


/**
 * @fn DynBlock::DynBlock(int capacity, int increment);
 * Build a new dynamic block.
 * @param capacity		Initial capacity of allocated buffer.
 * @param increment	Increment for enlarging the buffer.
 */


/**
 * @fn DynBlock::~DynBlock(void);
 * Delete the buffer.
 */


/**
 * Put a new data piece in the current block. Enlarge the block if required.
 * @param block	Block to put in.
 * @param size	Size of the block.
 */ 
void DynBlock::put(const char *block, int size) {
	ASSERTP(block, "null block");
	ASSERTP(size >= 0, "size must be positive");
	
	// Need to enlarge?
	if(_size + size > cap)
		grow(_size + size - cap);
	
	// Put the block
	memcpy(buf + _size, block, size);
	_size += size;
}


/**
 * Get a part from the dynamic block. It is an error to pass a size too big, or a position out of range.
 * @param block	Block to put bytes in.
 * @param size	Size of the block.
 * @param pos		Position to get bytes from.
 */
void DynBlock::get(char *block, int size, int pos) {
	ASSERTP(block, "null block");
	ASSERTP(pos >= 0 && pos < _size, "out of bound position");
	ASSERTP(size >= 0 && pos + size <= _size, "size out of bounds");
	memcpy(block, buf + pos, size);
}


/**
 * @fn int DynBlock::size(void) const;
 * Get the ize of the block.
 * @return Block size.
 */


/**
 * @fn int DynBlock::capacity(void) const;
 * Get the capacity of the block.
 * @return Block capacity.
 */


/**
 * @fn int DynBlock::increment(void) const;
 * Get the incrementation value.
 * @return Incrementation value.
 */
 

/**
 * @fn void DynBlock::setSize(int new_size);
 * Set the size of the block. Must be less or equal to the current block size.
 * @param new_size	New size of the block.
 */


/**
 * @fn void DynBlock::reset(void);
 * Reset the block to a size of 0.
 */


/**
* @fn const char *DynBlock::base(void) const;
* Get the buffer base of the block.
 * @return	Block buffer base.
 */


/**
 * @fn char *DynBlock::detach(void);
 * Detach the block from this dynamic manager. The dynamic block must no more be
 * used after this call.
 * @return Block buffer that must be fried using "delete []".
 */


/**
 * Allocate a block of memory that may be initialized by caller application.
 * @param size	Size of block to allocate.
 * @return		Allocated block that may initialized by application.
 */
char *DynBlock::alloc(int size) {
	ASSERTP(size >= 0, "size must positive");
	
	// Need to enlarge?
	if(_size + size > cap)
		grow(_size + size - cap);
	
	// Allocate the block
	char *result = buf + _size;
	_size += size;
	return result;
}

} } // elm::block
