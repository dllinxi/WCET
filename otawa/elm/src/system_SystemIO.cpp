/*
 *	System stream classes implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <stdio.h>
#include <elm/sys/SystemIO.h>
#if defined(__WIN32)
#	include <windows.h>
#endif

namespace elm { namespace sys {

/**
 * @class SystemInStream
 * This kind of stream is tied to a system input stream, that is, it may be
 * shared by processes or support system feature. @ref io::stdin is an example
 * of @ref SystemInStream.
 *  
 * @warning	Do not rely on the current inheritance of this class: it may change
 * according systems. The only right inheritance relation is with @ref io::InStream.
 */


/**
 * @class SystemOutStream
 * This kind of stream is tied to a system output stream, that is, it may be
 * shared by processes or support system feature. @ref io::stdout and
 * @ref io::stdout are examples of @ref SystemOutStream.
 *  
 * @warning	Do not rely on the current inheritance of this class: it may change
 * according systems. The only right inheritance relation is with @ref io::OutStream.
 */


// Unixes support
#if defined(__unix) || defined(__APPLE__)
	static sys::SystemInStream stdin_object(0);
	static sys::SystemOutStream stdout_object(1);
	static sys::SystemOutStream stderr_object(2);
		
	SystemInStream::SystemInStream(int fd): UnixInStream(fd) { }
	SystemOutStream::SystemOutStream(int fd): UnixOutStream(fd) { }

// Windows support
#elif defined(__WIN32) || defined(WIN64)

	class NoFaultOutStream: public SystemOutStream {
	public:
			NoFaultOutStream(HANDLE handle): SystemOutStream(handle) { }
			
			virtual int write(const char *buffer, int size) {
					// with Eclipse (normal mode), strange stdandard IO causing write errors: ignored.
					SystemOutStream::write(buffer, size);
					return size;
			}
	};
	
	static sys::SystemInStream stdin_object(GetStdHandle(STD_INPUT_HANDLE));
	static sys::NoFaultOutStream stdout_object(GetStdHandle(STD_OUTPUT_HANDLE));
	static sys::NoFaultOutStream stderr_object(GetStdHandle(STD_ERROR_HANDLE));

	SystemInStream::SystemInStream(void* fd): WinInStream(fd) { }
	SystemOutStream::SystemOutStream(void* fd): WinOutStream(fd) { }

// Unsupported OS
#else
#	error "Unsupported OS !"
#endif

} // sys

namespace io {

/**
 * Standard input stream.
 * @ingroup ios
 */
sys::SystemInStream& in = sys::stdin_object;

/**
 * Standard output stream.
 * @ingroup ios
 */
sys::SystemOutStream& out = sys::stdout_object;

/**
 * Standard error stream.
 * @ingroup ios
 */
sys::SystemOutStream& err = sys::stderr_object;

} } // elm::io
