/*
 *	$Id$
 *	BlockOutStream class implementation
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
#include <elm/io/BlockOutStream.h>

namespace elm { namespace io {

/**
 * @class BlockOutStream
 * This class provides an out stream stocking output bytes in memory
 * in a continuous blocks.
 * @ingroup ios
 */


/**
 * @fn BlockOutStream::BlockOutStream(int size, int inc);
 * Build a new block output stream.
 * @param size	Initial size of the block.
 * @param inc	Value of the increment for enlarging the block.
 */


/**
 * @fn char *BlockOutStream::block(void) const;
 * Get a pointer on the start of the storage block. This pointer remains
 * only valid until an output is performed on the stream.
 * @return	Current storage memory block.
 */


/**
 * @fn int BlockOutStream::size(void) const;
 * Get the current size of stored bytes in this stream.
 */


/**
 * @fn char *BlockOutStream::detach(void);
 * Detach the block from the stream. After this call, the stream will
 * perform no management on the memory block and the caller is responsible
 * for deleting it.
 * It is an error to perform more output after this call unless
 * @ref restart() is called.
 * @return	Base of the block.
 */


/**
 * @fn void BlockOutStream::clear(void);
 * Clear the block receiving bytes from output.
 */


/**
 * @fn void BlockOutStream::setSize(int size);
 * Change the size of the current block.
 * @param size	New size occupied by the block.
 */


/**
 */	
int BlockOutStream::write(const char *buffer, int size) {
	_block.put(buffer, size);
	return 0;
}


/**
 */
int BlockOutStream::write(char byte) {
	_block.put(&byte, 1);
	return 0;
}


/**
 */
int BlockOutStream::flush(void) {
	return 0;
}


/**
 * Convert the buffer to a CString object.
 * (this object must not be used after this call and the caller is responsible
 * for freeing the C string buffer).
 * @return	Buffer converted to C string.
 */
CString BlockOutStream::toCString(void) {
	write('\0');
	return CString(detach());
}


/**
 * Convert the buffer to a String object.
 * @return	String object.
 */
String BlockOutStream::toString(void) {
	return String(_block.base(), _block.size());
}

} }	// elm::io
