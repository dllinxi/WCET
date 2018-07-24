/*
 *	$Id$
 *	InFileStream class implementation
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

#if defined(__unix) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/stat.h>
#elif defined(__WIN32) || defined(__WIN64)
#include <windows.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <elm/io/InFileStream.h>
#include <elm/io/io.h>

namespace elm { namespace io {

/**
 * @class InFileStream
 * Stream for outputting to a file.
 * @ingroup ios
 */

#if defined(__unix) || defined(__APPLE__)
	bool InFileStream::isReady(void) {
		return fd() >= 0;
	}
#elif defined(__WIN32) || defined(__WIN64)
	bool InFileStream::isReady(void) {
		return _fd != INVALID_HANDLE_VALUE;
	}
#endif

/**
 * Build an output file stream by creating a new file or deleting an old one.
 * @param path Path of the file to write to.
 */
#if defined(__unix) || defined(__APPLE__)
InFileStream::InFileStream(CString path)
: SystemInStream(open(path.chars(), O_RDONLY, 0777)) {
	if(fd() < 0)
		throw IOException(_ << "cannot open file \"" << path
			<< "\" : " << lastErrorMessage());
}
#elif defined(__WIN32) || defined(__WIN64) 
InFileStream::InFileStream(CString path)
: SystemInStream(CreateFile(&path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) {
}
#endif

/**
 * Destructor (close the file if it is opened).
 */
InFileStream::~InFileStream(void) {
	close();
}

/**
 * Close the file. Subsequent writes will fail.
 */
void InFileStream::close() {
#if defined(__unix)|| defined(__APPLE__)
	if(_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
#elif defined(__WIN32) || defined(__WIN64)
	CloseHandle(_fd);
#endif
}

} } // elm::io
