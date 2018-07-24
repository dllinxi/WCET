/*
 *	$Id$
 *	BufferedOutStream class implementation
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
#include <elm/assert.h>
#include <elm/io/BufferedOutStream.h>

namespace elm { namespace io {

/**
 * @class BufferedOutStream
 * This stream provides buffered action for a given output stream.
 */

/**
 * Build a buffer out stream.
 * @param output	Output stream to buffer in.
 * @param size		Size of the buffeR.
 */
BufferedOutStream::BufferedOutStream(OutStream& output, size_t size)
: out(&output), top(0), buf_size(size) {
	ASSERTP(size != 0, "invalid null buffer size");
	buf = new char[buf_size];
}


/**
 */
BufferedOutStream::~BufferedOutStream(void) {
	flush();
	delete [] buf;
}


/**
 */
int BufferedOutStream::write(const char *buffer, int size) {
	int res = size;
	if(buf_size - top < (size_t)size) {
		if(top > 0)
			flush();
		while((size_t)size > buf_size) {
			if(out->write(buffer, buf_size) < 0)
				return -1;
			buffer += buf_size;
			size -= buf_size;
		}
	}
	memcpy(buf + top, buffer, (size_t)size);
	top += size;
	return res;
}


/**
 */
int BufferedOutStream::flush(void) {
	 if(top == 0)
		 return 0;
	 else {
		 int res = out->write(buf, top);
		 top = 0;
		 return res;
	 }
 }


/**
 */
CString BufferedOutStream::lastErrorMessage(void) {
	return out->lastErrorMessage();
}


} } // elm::io
