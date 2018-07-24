/*
 *	$Id$
 *	WinInStream class implementation
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
#include <elm/io/WinInStream.h>
#include <windows.h>
#include <elm/system/SystemIO.h>
#include <elm/io.h>
#include <elm/string.h>

namespace elm {

namespace win {
	void setError(int);
	string getErrorMessage(void);
}	

namespace io {

/**
 * @class WinInStream
 * Input stream implementation using the Win low-level IO system.
 */


/**
 * Build a new Win input stream with the given file descriptor.
 * @param fd	File descriptor to use.
 */
WinInStream::WinInStream(void* fd): _fd(fd) {
}


/**
 * @fn int WinInStream::fd(void) const;
 * Get the file descriptor of this Win file stream.
 * @return	File descriptor.
 */


/**
 */
int WinInStream::read(void *buffer, int size) {
	DWORD r;
	if(ReadFile(_fd, buffer, size, &r, NULL))
		return r;
	else if(GetLastError() == ERROR_HANDLE_EOF)
		return 0;
	else {
		win::setError(GetLastError());
		return -1;
	}
}


/**
 */
CString WinInStream::lastErrorMessage(void) {
	static string buf;
	buf = win::getErrorMessage();
	return buf.toCString();
}

} } // elm::io
