/*
 *	$Id$
 *	Out class implementation
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

#include <unistd.h>
#if defined(__WIN32) || defined(__WIN64)
#include <windows.h>
#endif
#include <elm/io/OutStream.h>
#include <elm/io/io.h>

namespace elm { namespace io {

/**
 * @class OutStream
 * An output stream streams transfer a flow of bytes to another entity.
 * It is an abstract interface that must be implemented by standard IO, files,
 * pipes, compressors and so on.
 * @ingroup ios
 */

/**
 * @fn int OutStream::write(const char *buffer, int size);
 * This is the main method of an output stream: the given buffer is put on
 * the stream.
 * @param buffer	Byte buffer to write.
 * @param size		Size of the byte buffer.
 * @return				Number of transferred bytes or less than 0 for an error.
 */

/**
 * Write a byte to the stream.
 * @param byte	Byte to write.
 * @return				1 for success or less than 0 for an error.
 */
int OutStream::write(char byte) {
	char buffer = byte;
	return write(&buffer, 1);
}

/**
 * @fn int OutStream::flush()
 * Cause the current stream to dump its buffer to the medium.
 * @return 0 for success, less than 0 for error.
 */


/**
 * Return a message for the last error.
 * @return	Message of the last error.
 */
CString OutStream::lastErrorMessage(void) {
	return "";
}


// NullOutStream class
static class NullOutStream: public OutStream {
public:
	virtual int write(const char *buffer, int size) { return size; }
	virtual int flush(void) { return 0; }	
} _null;


/**
 * Provides an output stream that does nothing.
 */
OutStream& OutStream::null = _null;

} } // elm::io
