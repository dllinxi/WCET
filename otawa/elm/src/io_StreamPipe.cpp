/*
 *	io::StreamPipe class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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

#include <elm/io/StreamPipe.h>
#include <elm/io/InStream.h>
#include <elm/io/OutStream.h>
#include <elm/assert.h>

namespace elm { namespace io {

/**
 * @class StreamPipe
 * A stream pipe allows to pipe together a string of input stream and a string
 * of output stream. When the @ref proceed() method is called, the input stream
 * is read to the end and write back to the output stream.
 * @ingroup ios
 */


/**
 * build a stream pipe.
 * @param in			Input stream.
 * @param out			Output stream.
 * @param buffer_size	Size of the buffer.
 */
StreamPipe::StreamPipe(InStream& in, OutStream& out, int buffer_size)
: _in(in), _out(out), state(OK), bufs(buffer_size), buf(new char[buffer_size]) {
}


/**
 */
StreamPipe::~StreamPipe(void) {
	delete [] buf;
}


/**
 * Perform the copy from input stream to output stream.
 * @return	Number of copied bytes or <0 if there is an error.
 */
int StreamPipe::proceed(void) {
	int size = 0;
	while(true) {

		// read the block
		int inr = _in.read(buf, bufs);
		if(inr == InStream::FAILED) {
			state = IN_ERROR;
			return -1;
		}
		else if(inr == InStream::ENDED)
			break;

		// write the result
		int outr = _out.write(buf, inr);
		if(outr < 0) {
			state = OUT_ERROR;
			return -1;
		}
		size += outr;
	}
	return size;
}


/**
 * Get the message associated with the last error.
 * @return	Last error message.
 */
string StreamPipe::lastErrorMessage(void) const {
	switch(state) {
	case OK:		return "Success";
	case IN_ERROR:	return _in.lastErrorMessage();
	case OUT_ERROR:	return _out.lastErrorMessage();
	}
	return "invalid state";
}

} }	// elm::io



