/*
 *	$Id$
 *	BufferedInStream class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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
#include <elm/io/BufferedInStream.h>

namespace elm { namespace io {

/**
 * @class BufferedInStream
 * Input stream providing bufferization for reading another stream.
 */


/**
 * @var int BufferedInstream::default_size;
 * Default size of buffer.
 */


/**
 * Build a buffered input stream.
 * @param input	Input stream to get data from.
 * @param size	Default size of the used buffer.
 */
BufferedInStream::BufferedInStream(InStream& input, int size)
:	in(input), buf(new char[size]), pos(0), top(0), buf_size(size) {
	ASSERTP(size > 0, "strictly positive buffer size required");
}


/**
 */
BufferedInStream::~BufferedInStream(void) {
	delete [] buf;
}


/**
 */
int BufferedInStream::read(void *buffer, int size) {
	
	// refill buffer
	if(pos >= top) {
		int size = in.read(buf, buf_size);
		if(size <= 0)
			return size;
		pos = 0;
		top = size;
	}
	
	// transfer data
	if(pos + size > top)
		size = top - pos;
	memcpy(buffer, buf + pos, size);
	pos += size;
	return size;
}

} } // elm::io
