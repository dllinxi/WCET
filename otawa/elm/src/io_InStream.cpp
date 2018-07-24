/*
 *	$Id$
 *	InStream class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-07, IRIT UPS.
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

#include <elm/io/InStream.h>
#if defined(__LINUX)
#include <elm/io/UnixInStream.h>
#elif defined(__WIN32) || defined(__WIN64)
#include <elm/io/WinInStream.h>
#endif

namespace elm { namespace io {

/**
 * @class InStream
 * This class must be shared by all classes implementing input streams. It provides common
 * facilities provided by input streams.
 * @ingroup ios
 */


/**
 * @fn InStream::~InStream(void);
 * Common virtual destructor.
 */


/**
 * @fn int InStream::read(void *buffer, int size);
 * Read some bytes from the stream and store them in the given buffer.
 * This method does not ever read as many bytes as available in the buffer even if the stream
 * is not ended. Insteadn a read returning 0 bytes means the end of stream is reached.
 * @param buffer	Buffer to write bytes in.
 * @param size	Size of the buffer.
 * @return		Number of read bytes, 0 for end of stream, -1 for an error.
 */


/**
 * Read one byte from the stream.
 * @return	Read byte, FAILED for an error, ENDED for end of stream.
  */
int InStream::read(void) {
	char buf;
	int size = read(&buf, 1);
	if(size == 1)
		return (unsigned char)buf;
	else if(size == 0)
		return ENDED;
	else
		return FAILED;
}


/**
 * Return a message for the last error.
 * @return	Message of the last error.
 */
CString InStream::lastErrorMessage(void) {
	return "";
}


// NullInStream class
static class NullInStream: public InStream {
public:
	virtual int read(void *buffer, int size) { return 0; }
} _null;


/**
 * An input stream that is always at end.
 */
InStream& InStream::null = _null;

} } // elm::io
