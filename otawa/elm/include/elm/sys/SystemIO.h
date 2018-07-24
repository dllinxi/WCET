/*
 *	SystemInStream and SystemOutStream class interfaces
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-12, IRIT UPS.
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
#ifndef ELM_SYS_SYSTEM_IO_H
#define ELM_SYS_SYSTEM_IO_H

// Unix system streams
#if defined(__unix) || defined(__APPLE__)
#	include <elm/io/UnixInStream.h>
#	include <elm/io/UnixOutStream.h>

	namespace elm { namespace sys {

	class SystemInStream: public io::UnixInStream {
	public:
			SystemInStream(int fd);
	protected:
		friend class System;
	};

	class SystemOutStream: public io::UnixOutStream {
	public:
		SystemOutStream(int fd);
	protected:
		friend class System;
	};

	} }		// elm::sys

// Windows system streams
#elif defined(__WIN32) || defined(__WIN64)
#	include <elm/io/WinInStream.h>
#	include <elm/io/WinOutStream.h>

	namespace elm { namespace sys {

	class SystemInStream: public io::WinInStream {
	protected:
		friend class System;
	public:
		SystemInStream(void* fd);
	};

	class SystemOutStream: public io::WinOutStream {
	protected:
		friend class System;
	public:
		SystemOutStream(void* fd);
	};

	} }		// elm::sys

// unsupported OS
#else
#	error "Unsupported OS !"
#endif


// Standard Streams
namespace elm { namespace io {

extern sys::SystemInStream& in;
extern sys::SystemOutStream& out;
extern sys::SystemOutStream& err;

} } // elm::io

#endif // ELM_SYS_SYSTEM_IO_H
