/*
 *	$Id$
 *	UnixInStream class implementation
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

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <elm/io/UnixInStream.h>

namespace elm { namespace io {

/**
 * @class UnixInStream
 * Input stream implementation using the Unix low-level IO system.
 */

	
/**
 * @fn UnixInStream::UnixInStream(int fd);
 * Build a new Unix input stream with the given file descriptor.
 * @param fd	File descriptor to use.
 */

	
/**
 * @fn int UnixInStream::fd(void) const;
 * Get the file descriptor of this Unix file stream.
 * @return	File descriptor.
 */


/**
 */	
int UnixInStream::read(void *buffer, int size) {
	return ::read(_fd, buffer, size);
}


/**
 */
CString UnixInStream::lastErrorMessage(void) {
	return strerror(errno);
}


/**
 * Stream pointing to the standard error input.
 */
#ifdef __unix
	static UnixInStream unix_stdin(0);
	InStream& stdin = unix_stdin;
#endif

} } // elm::io
