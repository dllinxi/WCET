/*
 *	System class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-12, IRIT UPS.
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
#ifndef ELM_SYS_SYSTEM_H
#define ELM_SYS_SYSTEM_H

#include <elm/util/Pair.h>
#include <elm/sys/Path.h>
#include <elm/sys/SystemIO.h>
#include <elm/sys/SystemException.h>

namespace elm { namespace io {
	class InStream;
	class OutStream;
	class RandomAccessStream;
} } // elm::io

namespace elm { namespace sys {

// System class
class System {
public:
	typedef int access_t;
	static const int READ = 1;
	static const int WRITE = 2;
	static const int READ_WRITE = READ | WRITE;

	static Pair<SystemInStream *, SystemOutStream *> pipe(void) throw(SystemException);
	static unsigned int random(unsigned int top);
	static io::OutStream *createFile(const Path& path) throw(SystemException);
	static io::OutStream *appendFile(const Path& path) throw(SystemException);
	static io::InStream *readFile(const Path& path) throw(SystemException);
	static io::RandomAccessStream *openRandomFile(const Path& path, access_t access = READ) throw(SystemException);
	static io::RandomAccessStream *createRandomFile(const Path& path, access_t access = READ) throw(SystemException);
	static Path getUnitPath(void *address);
	static cstring getEnv(cstring key);
	static bool hasEnv(cstring key);
	static cstring library_prefix, library_suffix, exec_suffix;
	static string getLibraryFileName(const string& name);
	static string getPluginFileName(const string& name);
	static void makeDir(const sys::Path& path) throw(SystemException);
	static void removeDir(const sys::Path& path) throw(SystemException);
	static sys::Path getTempFile(void) throw(SystemException);
	static sys::Path getTempDir(void) throw(SystemException);
	static int coreCount(void);
	static void exit(int code = 0);
};

} } // elm::system

#endif // ELM_SYS_SYSTEM_H

