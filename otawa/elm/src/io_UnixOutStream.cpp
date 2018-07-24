/*
 *	$Id$
 *	UnixOutStream class implementation
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
#include <errno.h>
#include <unistd.h>
#include <elm/io/UnixOutStream.h>
#include <elm/io.h>

namespace elm { namespace io {

/**
 * @class UnixOutStream <elm/io.h>
 * Output stream implementation using the Unix low-level IO system
 * (only on OSes supporting POSIX).
 */


/**
 * Build an Unix output stream.
 * @param fd	File descriptor.
 */
UnixOutStream::UnixOutStream(int fd): _fd(fd) {
	if(fd < 0)
		throw io::IOException(strerror(errno));
}

/**
 * @fn UnixOutStream::UnixOutStream(int _fd);
 * Build a Unix output stream using a file descriptor identifier.
 * @param _fd	File descriptor identifier.
 */


/**
 * Close an Unix output stream.
 */
UnixOutStream::~UnixOutStream() {
#if defined(__unix) || defined(__APPLE__)
	if(_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
#elif defined(__WIN32) || defined(__WIN64)
	CloseHandle(_fd);
#endif
}


/**
 * Return the last occurred error message.
 * @return	Last error message.
 */
CString UnixOutStream::lastErrorMessage(void) {
	return strerror(errno);
}


// Overloaded
int UnixOutStream::write(const char *buffer, int size) {
	return ::write(_fd, buffer, size);
}

// Overloaded
int UnixOutStream::flush(void) {
	return 0;
}

} } // elm::io
