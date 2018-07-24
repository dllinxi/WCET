/*
 *	$Id$
 *	OutFileStream class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-08, IRIT UPS.
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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elm/io/OutFileStream.h>
#if defined(__WIN32) || defined(__WIN64)
#include <windows.h>
#endif

namespace elm { namespace io {

/**
 * @class OutFileStreal
 * Stream for outputting to a file.
 * @ingroup ios
 */

#if defined(__WIN32) || defined(__WIN64)
bool OutFileStream::isReady(void) { return GetHandleInformation(fd(),NULL) != 0;
}
#elif defined(__unix) || defined(__APPLE__)
bool OutFileStream::isReady(void) { return fd() >= 0; };
#endif


/**
 * Build an output file stream by creating a new file or deleting an old one.
 * @param path Path of the file to write to.
 */
#if defined(__unix) || defined(__APPLE__)
OutFileStream::OutFileStream(const char *path)
: SystemOutStream(open(path, O_CREAT | O_TRUNC | O_WRONLY, 0777)) {
}
#elif defined(__WIN32) || defined(__WIN64)
OutFileStream::OutFileStream(const char *path)
: SystemOutStream(CreateFile(path,
		GENERIC_WRITE,
		FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) { }
#endif


/**
 * Build an output file stream by creating a new file or deleting an old one.
 * @param path Path of the file to write to.
 */
#if defined(__unix) || defined(__APPLE__)
OutFileStream::OutFileStream(const Path& path)
: SystemOutStream(open(&path.toString(), O_CREAT | O_TRUNC | O_WRONLY, 0777)) {
}
#elif defined(__WIN32) || defined(__WIN64)
OutFileStream::OutFileStream(const Path & path)
: SystemOutStream(CreateFile(path.toString().chars(),
		GENERIC_WRITE,
		FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) { }

#endif


/**
 * Destructor (close the file if it is opened).
 */
OutFileStream::~OutFileStream(void) {
	close();
}

/**
 * Close the file. Subsequent writes will fail.
 */
void OutFileStream::close() {
#if defined(__unix) || defined(__APPLE__)
	if(_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
#elif defined(__WIN32) || defined(__WIN64)
	CloseHandle(_fd);
#endif
}

} } // elm::io
