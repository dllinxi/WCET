/*
 *	$Id$
 *	WinOutStream class implementation
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
#include <elm/io/WinOutStream.h>
#include <elm/io.h>
#include <windows.h>
#include <elm/system/SystemIO.h>
#include <stdio.h>


namespace elm { namespace io {

/**
 * @class WinOutStream <elm/io.h>
 * Output stream implementation using the Win low-level IO system
 * (only on OSes supporting POSIX).
 */


/**
 * Build an Win output stream.
 * @param fd	File descriptor.
 */
WinOutStream::WinOutStream(void* fd): _fd(fd) {
}

/**
 * @fn WinOutStream::WinOutStream(int _fd);
 * Build a Win output stream using a file descriptor identifier.
 * @param _fd	File descriptor identifier.
 */


/**
 * Return the last occurred error message.
 * @return	Last error message.
 */
CString WinOutStream::lastErrorMessage(void) {
	return strerror(errno);
}


// Overloaded
int WinOutStream::write(const char *buffer, int size) {
	DWORD r;
	// late binding of stdout, stderr: very ugly but don't know any better solution
	if(_fd == NULL) {
		if(this == &err)
			_fd = GetStdHandle(STD_ERROR_HANDLE);
		else if(this == &out)
			_fd = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	if(::WriteFile(_fd, buffer, size, &r, NULL))
		return r;
	else
		return -1;
}

// Overloaded
int WinOutStream::flush(void) {
	return 0;
}


/**
 * Stream pointing to the standard output.
 */
static system::SystemOutStream Win_stdout(GetStdHandle(STD_OUTPUT_HANDLE)/*(HANDLE)_get_osfhandle(0)*/);
//system::SystemOutStream& out = Win_stdout;

/**
 * Stream pointing to the standard error output.
 */
static system::SystemOutStream Win_stderr(GetStdHandle(STD_ERROR_HANDLE));
//system::SystemOutStream& err = Win_stderr;

} } // elm::io
