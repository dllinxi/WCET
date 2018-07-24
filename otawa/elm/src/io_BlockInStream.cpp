/*
 *	$Id$
 *	BlockInStream class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
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
#include <string.h>
#include <elm/io/BlockInStream.h>

namespace elm { namespace io {

/**
 * @class BlockInStream
 * Input stream from a memory block.
 * @ingroup ios
 */


/**
 * Build a new memory block input stream.
 * @param block	Block to read from.
 * @param size	Block size.
 */ 
BlockInStream::BlockInStream(const void *block, int size)
: _block((const char *)block), _size(size), off(0) {
}


/**
 * Build a block input stream from a C string.
 * @param str	C string to read.
 */	
BlockInStream::BlockInStream(const char *str)
: _block(str), _size(strlen(str)), off(0) {
	ASSERTP(str, "null string pointer");
}


/**
 * Build a block input stream from a CString object.
 * @param str	String to read to.
 */
BlockInStream::BlockInStream(const CString& str)
: _block(str.chars()), _size(str.length()), off(0) {
}


/**
 * Build a block input stream from a String object.
 * @param str	String to read from.
 */
BlockInStream::BlockInStream(const String& str)
: _block(str.chars()), _size(str.length()), off(0) {
}


/**
 * @fn char *BlockInStream::block(void) const;
 * Get the current memory block.
 * @return	Memory block.
 */


/**
 * @fn int BlockInStream::size(void) const;
 * Get the size of the current memory block.
 * @return	Memory block size.
 */


/**
 * @fn int BlockInStream::mark(void) const;
 * Get the current offset in the memory block.
 * @return	Current offset.
 */


/**
 * @fn void BlockInStream::move(int mark);
 * Move the read position to the given mark.
 * @param mark	Position mark to move to.
 */
 
/**
 * @fn void BlockInStream::moveForward(int size);
 * Move forward in the stream.
 * @param size	Count of bytes to move forward to.
 */


/**
 * @fn void BlockInStream::moveBackward(int size);
 * Move backward in the stream.
 * @param size Count of bytes to move backward to.
 */


/**
 * @fn void BlockInStream::reset(void);
 * Reset the stream to the start.
 */


/**
 */	
int BlockInStream::read(void *buffer, int size) {
	
	// Check size
	if(off + size > _size)
		size = _size - off;
	
	// Perform the copy
	memcpy(buffer, _block + off, size);
	off += size;
	return size;
}


/**
 */
int BlockInStream::read(void) {
	if(off >= _size)
		return ENDED;
	else
		return (unsigned char)_block[off++];
}

} } // elm::io

