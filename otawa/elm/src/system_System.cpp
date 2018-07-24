/*
 *	System class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-13, IRIT UPS.
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
#	include <dlfcn.h>
#elif defined(__WIN32)
#	include <windows.h>
#	undef min
#	undef max
#endif
#include <elm/io/io.h>
#include <elm/io/WinInStream.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include <elm/assert.h>
#include <elm/deprecated.h>
#include <elm/io/RandomAccessStream.h>
#include <elm/sys/System.h>
#include <elm/sys/SystemException.h>

namespace elm {

#if defined(__WIN32) || defined(__WIN64)
	namespace win {
		static int last_error;
		
		int getError(void) {
			return last_error;
		}
		
		void setError(void) {
			last_error = GetLastError();
		}

		void setError(int code) {
			last_error = code;
		}
		string getLastErrorMessage(void) {
			setError();
			return getErrorMessage();
		}
		
		string getErrorMessage(void) {
			char buf[256];
			FormatMessage( 
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				last_error,
				0,
				buf,
				sizeof(buf),
				NULL);
			return _ << buf << " (" << last_error << ")";
		}
	}
#endif	
	
namespace sys {

/**
 * @defgroup system_inter System Interface
 *
 * These classes provides a unified interface to the OS services.
 *
 * @section file_man File Management
 *
 * The first file management class is @ref Path that is used to represent file
 * paths and provides many operations:
 * @li appending path,
 * @li extracting / setting file and directory part,
 * @li getting absolute form,
 * @li getting and setting extension.
 *
 * It provides also system information about paths:
 * @li getting / setting current path,
 * @li getting home directory,
 * @li getting path item separator,
 * @li getting path separator in path lists.
 *
 * Other classes represent files themselves. They are derivated from the
 * @ref FileItem class and may be either @ref Directory or @ref File. A file
 * item is obtained by the @ref FileItem::get() method taking the path of
 * looked file. The file item gives the information as name, path, type and
 * access rights. The files provides also information about their size while
 * directories content may be retrieved using the @ref Directory::Iterator or
 * made using the @ref Directory::make method. Once their use is completed,
 * this objects must be released with the method @ref FileItem::release().
 *
 * @section proc_man Process Management
 *
 * The @ref Process objects provides control on the process of the OS:
 * @li getting liveness,
 * @li getting return code after death,
 * @li killing,
 * @li waiting until end of the process.
 *
 * A process may be created using the @ref ProcessBuilder class. It allows
 * to set process configuration like arguments and standard IO. Once the
 * method ProcessBuilder::run() is called, the process is created, either
 * a duplication of the current one, or a new command,
 * with the set configuration and a process object is returned.
 *
 * @section misc_ser Miscalleneous Classes
 *
 * First, ELM provides a class to measure times as precisely as possible on
 * the current OS: the class @ref StopWatch.
 *
 * Then, the static class @ref System provides some services like:
 * @li @ref System::pipe() -- to create piped streams.
 */

// PipeInStream class
#if defined(__unix) || defined(__APPLE__)
	class PipeInStream: public SystemInStream {
		friend class System;
		PipeInStream(int fd): SystemInStream(fd) { }
	public:
		virtual ~PipeInStream(void) { close(fd()); }
	};

#elif defined(__WIN32) || defined(__WIN64)
	class PipeInStream: public SystemInStream {
		friend class System;
		PipeInStream(HANDLE fd): SystemInStream(fd) { }
	public:
		virtual ~PipeInStream(void) { CloseHandle(fd()); }

		virtual int read(void *buffer, int size) {
			int r = SystemInStream::read(buffer, size);
			if(r < 0 && win::getError() == ERROR_BROKEN_PIPE)
				return 0;
			else
				return r;
		}
	};

#else
#	error "pipe unsupported"
#endif


// PipeOutStream class
#if defined(__unix) || defined(__APPLE__)
	class PipeOutStream: public SystemOutStream {
		friend class System;
		PipeOutStream(int fd): SystemOutStream(fd) { }
	public:
		~PipeOutStream(void) { close(fd()); }
	};

#elif defined(__WIN32) || defined(__WIN64)
	class PipeOutStream: public SystemOutStream {
		friend class System;
		PipeOutStream(HANDLE fd): SystemOutStream(fd) { }
	public:
		~PipeOutStream(void) { CloseHandle(fd()); }
	};

#else
#	error "pipe unsupported"
#endif


/**
 * @class System
 * Non-instatiable object giving access to system facilities.
 * @ingroup system_inter
 */


/**
 * Create a  pipe with input / output end streams.
 * @return	Linked streams.
 * @throws	System exception.
 */
Pair<SystemInStream *, SystemOutStream *> System::pipe(void)
throw (SystemException) {
#if defined(__unix) || defined(__APPLE__)
	int fds[2];
	// Create the pair
	if(::pipe(fds) < 0) {
		ASSERT(errno != EFAULT);
		throw SystemException(errno, "pipe creation");
	}

	/* Configure the close-on-exec flag
	 * !!WARNING!! added to implement a working ProcessBuilder: I'm not sure
	 * if it is the best way to do this (yet it is POSIX compliant). */
	fcntl(fds[0], F_SETFD, FD_CLOEXEC);
	fcntl(fds[1], F_SETFD, FD_CLOEXEC);

	// Return result
	return pair(
			static_cast<SystemInStream *>(new PipeInStream(fds[0])),
			static_cast<SystemOutStream *>(new PipeOutStream(fds[1])));

#elif defined(__WIN32) || defined(__WIN64)
	HANDLE fdsread = NULL;
	HANDLE fdswrite = NULL;
	
	// prepare security
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
   
	// build the pipe
	if(CreatePipe(&fdsread, &fdswrite, &saAttr, 0) == 0)
		throw SystemException(errno, win::getErrorMessage());

	// redundant settings?
	if(!SetHandleInformation(fdsread, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
		throw SystemException(errno, win::getErrorMessage());
	if(!SetHandleInformation(fdswrite, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
		throw SystemException(errno, win::getErrorMessage());
	
	return pair(
			static_cast<SystemInStream *>(new PipeInStream(fdsread)),
			static_cast<SystemOutStream *>(new PipeOutStream(fdswrite)));
#else
#error "Unsupported System"
#endif
}

/**
 * Generate an integer random number in interval [0, top[.
 * @param top	Maximum exclusive value.
 * @return		Random number.
 */
unsigned int System::random(unsigned int top) {
	static bool init = false;
	if (!init) {
		srand( time(NULL));
		init = true;
	}
	int op = rand();
	return int(double(op) * top / RAND_MAX);
}

/**
 * Create a new file and open it to write.
 * The created file must be fried by the caller (causing the file closure).
 * @param path	Path of the file to open.
 * @return		Opened file.
 * @throws		SystemException	Thrown if there is an error.
 */
io::OutStream *System::createFile(const Path& path) throw (SystemException) {

#if defined(__unix) || defined(__APPLE__)
	int fd = ::open(&path.toString(), O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd == -1)
		throw SystemException(errno, "file creation");
	return new SystemOutStream(fd);

#elif defined(__WIN32) || defined(__WIN64)
	HANDLE fd;
	fd=CreateFile(&path.toString(),
			GENERIC_READ,
			FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if(fd == INVALID_HANDLE_VALUE)
		throw SystemException(errno, "file creation");
	return new SystemOutStream(fd);

#else
#error "System Unsupported"
#endif
}

/**
 * Open a file for reading.
 * The opened file must be fried by the caller (causing the closure).
 * @param path	Path of the file to open.
 * @return		Opened file.
 * @throws		SystemException	Thrown if there is an error.
 */
io::InStream *System::readFile(const Path& path) throw(SystemException) {

#	if defined(__unix) || defined(__APPLE__)
		int fd = ::open(&path.toString(), O_RDONLY);
		if(fd == -1)
			throw SystemException(errno, "file reading");
		return new SystemInStream(fd);

#	elif defined(__WIN32) || defined(__WIN64)
		HANDLE fd;
		fd = CreateFile(
			&path.toString(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if(fd == INVALID_HANDLE_VALUE)
			throw SystemException(errno, "file creation");
		return new io::WinInStream(fd);

#	else
#		error "System Unsupported"
#	endif
}


/**
 * Open a file for appending write.
 * The opened file must be fried by the caller (causing the closure).
 * @param path	Path of the file to open.
 * @return		Opened file.
 * @throws		SystemException	Thrown if there is an error.
 */
io::OutStream *System::appendFile(const Path& path) throw(SystemException) {
#if defined(__unix) || defined(__APPLE__)
	int fd = ::open(&path.toString(), O_APPEND | O_CREAT | O_WRONLY, 0777);
	if(fd == -1)
		throw SystemException(errno, "file appending");
	return new SystemOutStream(fd);
#elif defined(__WIN32) || defined(__WIN64)
	HANDLE fd;
	fd=CreateFile(&path.toString(),
			GENERIC_READ,
			FILE_APPEND_DATA,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if(fd == INVALID_HANDLE_VALUE)
		throw SystemException(errno, "file creation");
	return new SystemOutStream(fd);
#else
#error "System Unsupported"
#endif
}


#if defined(__unix) || defined(__APPLE__)
// UnixRandomAccessStream class
class UnixRandomAccessStream: public io::RandomAccessStream {
public:
	inline UnixRandomAccessStream(int _fd): fd(_fd) {}
	virtual ~UnixRandomAccessStream(void)
	{	close(fd);}

	virtual int read(void *buffer, int size)
	{	return ::read(fd, buffer, size);}
	virtual int write(const char *buffer, int size)
	{	return ::write(fd, buffer, size);}
	cstring lastErrorMessage(void)
	{	DEPRECATED; return strerror(errno);}
	int flush(void)
	{	return 0;}

	virtual pos_t pos(void) const
	{	return lseek(fd, 0, SEEK_CUR);}
	virtual size_t size(void) const
	{	struct stat s; fstat(fd, &s); return s.st_size;}
	virtual bool moveTo(pos_t pos)
	{	return (pos_t)lseek(fd, pos, SEEK_SET) == pos;}
	virtual bool moveForward(pos_t pos)
	{	return (pos_t)lseek(fd, pos, SEEK_CUR) == pos;}
	virtual bool moveBackward(pos_t _pos)
	{	return (pos_t)lseek(fd, pos() - _pos, SEEK_SET) == _pos;}

private:
	int fd;
};

// Build the flags
static inline int makeFlags(System::access_t access) {
	switch(access) {
	case System::READ: return O_RDONLY;
	case System::WRITE: return O_WRONLY;
	case System::READ_WRITE: return O_RDWR;
	default: ASSERT(false); return 0;
	}
}
#elif defined(__WIN32)
class WinRandomAccessStream: public io::RandomAccessStream {
public:
	inline WinRandomAccessStream(int _fd) : fd(_fd){
		//this->fd=_open_osfhandle((long)_fd,O_RDWR);
	}
	virtual ~WinRandomAccessStream(void)
	{	close(fd);}

	virtual int read(void *buffer, int size)
	{	return ::read(fd, buffer, size);}
	virtual int write(const char *buffer, int size)
	{	return ::write(fd, buffer, size);}
	cstring lastErrorMessage(void)
	{	DEPRECATED; return strerror(errno);}
	int flush(void)
	{	return 0;}

	virtual pos_t pos(void) const
	{	return lseek(fd, 0, SEEK_CUR);}
	virtual size_t size(void) const
	{	struct stat s;
	fstat(fd, &s); return s.st_size;}
	virtual bool moveTo(pos_t pos)
	{	return (pos_t)lseek(fd, pos, SEEK_SET) == pos;}
	virtual bool moveForward(pos_t pos)
	{	return (pos_t)lseek(fd, pos, SEEK_CUR) == pos;}
	virtual bool moveBackward(pos_t _pos)
	{	return (pos_t)lseek(fd, pos() - _pos, SEEK_SET) == _pos;}

private:
	int fd;
};

// Build the flags
static inline int makeFlags(System::access_t access) {
	switch(access) {
	case System::READ: return O_RDONLY;
	case System::WRITE: return O_WRONLY;
	case System::READ_WRITE: return O_RDWR;
	default: ASSERT(false); return 0;
	}
}
#else
#error "System Unsupported"
#endif

/**
 * Open a random access stream from a file.
 * @param path			Path of the file to open.
 * @param access		Type of access (one of READ, WRITE, READ_WRITE).
 * @return				Opened file.
 * @throws IOException	Thrown if there is an error.
 */
io::RandomAccessStream *System::openRandomFile(
		const sys::Path& path,
		access_t access )
throw(SystemException)
{
	int fd = ::open(&path.toString(), makeFlags(access));
	if(fd < 0)
		throw SystemException(errno, _ << "cannot open \"" << path << "\"");
	else
#		if defined(__unix)  || defined(__APPLE__)
			return new UnixRandomAccessStream(fd);
#		elif defined(__WIN32) || defined(__WIN64)
			return new WinRandomAccessStream(fd);
#		else
#			error "Unsupported on this OS !"
#		endif
};

/**
 * Create a random access stream from a file, removing it if it already exists.
 * @param path			Path of the file to open.
 * @param access		Type of access (one of READ, WRITE, READ_WRITE).
 * @return				Opened file.
 * @throws IOException	Thrown if there is an error.
 */
io::RandomAccessStream *System::createRandomFile(
		const sys::Path& path,
		access_t access)
throw(SystemException)
{
	ASSERTP(access != READ, "file creation requires at least a write mode");
	int fd = ::open(&path.toString(), makeFlags(access) | O_CREAT | O_TRUNC, 0666);
	if(fd < 0)
		throw SystemException(errno, _ << "cannot create \"" << path << "\"");
	else
#		if defined(__unix)  || defined(__APPLE__)
			return new UnixRandomAccessStream(fd);
#		elif defined(__WIN32) || defined(__WIN64)
			return new WinRandomAccessStream(fd);
#		else
#			error "Unsupported on this OS !"
#		endif
}


/**
 * Get the path of the object item (library, program) containing the symbol
 * whose address is given.
 * @param address	Address of the looked symbol.
 * @return			Path to the object containing the symbol or an empty path
 * 					if it can not be found.
 * @notice			This method gives a usable result only when the GLIBC is used.
 * 					Be careful: it may be hard to get object path of an external symbol
 * 					due to relocation function stub and due to duplication of some small data.
 */
Path System::getUnitPath(void *address) {
#	if defined(__USE_GNU) || defined(__APPLE__)
	Dl_info info;
	if(!dladdr(address, &info))
		return "";
	else
		return Path(info.dli_fname).canonical();
#elif defined(__WIN32) || defined(__WIN64)
	MEMORY_BASIC_INFORMATION mbi;
	if(VirtualQuery(address,&mbi,sizeof(mbi)) == 0)
		return "";
	HMODULE mod = reinterpret_cast<HMODULE>(mbi.AllocationBase);

	LPTSTR buf = new TCHAR[255];
	GetModuleFileName(mod,buf,255);

	CString arr(buf);

	String fullpath = arr;

	return fullpath;
#	else
	return "";
#	endif
}


/** Prefix of the dynamic libraries of the current OS. */
cstring System::library_prefix = "lib";


/** Suffix of the dynamic libraries of the current OS. */
cstring System::library_suffix =
#	if defined(__WIN32) || defined(__WIN64)
		".dll";
#	elif defined(__CYGWIN__)
		".dll";
#	elif defined(__APPLE__)
		".dylib";
#	elif defined(__unix)
		".so";
#	else
#		error "Unsupported OS."
#	endif


/** Suffix of the executables of the current OS. */
cstring System::exec_suffix =
#	if defined(__WIN32) || defined(__WIN64)
		".exe";
#	elif defined(__APPLE__) || defined(__unix)
		"";
#	else
#		error "Unsupported OS."
#	endif


/**
 * Get the name of a file representing a dynamic library according
 * to the host OS.
 * @param name	Name of the library.
 * @return		Matching library file name.
 */
string System::getLibraryFileName(const string& name) {
	return _ << library_prefix << name << library_suffix;
}


/**
 * Get the name of a file representing a plugin according
 * to the host OS.
 * @param name	Name of the plugin.
 * @return		Matching plugin file name.
 */
string System::getPluginFileName(const string& name) {
	return _ << name << library_suffix;
}


/**
 * Get an environment variable value.
 * @param key	Key name of the environment variable.
 * @return		Value of the environment variable or an empty string.
 */
cstring System::getEnv(cstring key) {
	char *res = getenv(&key);
	if(!res)
		return "";
	else
		return res;
}

/**
 * Test if an environment variable is defined.
 * @param key	Key name of the environment variable.
 * @return		True if the environment variable is defined, false else.
 */
bool System::hasEnv(cstring key) {
	return getenv(&key);
}

/**
 * Build the directory matching the given path.
 * @param path	Path of the directory to build.
 * @throw SystemException	If the directory cannot be built.
 */
void System::makeDir(const sys::Path& path) throw(SystemException) {
#	if defined(__WIN32) || defined(__WIN64)
		if(!CreateDirectory(&path.toString().toCString(), NULL))
			throw SystemException(0, _ << "cannot create " << path << ": " << win::getLastErrorMessage()));
#	else
		int r = mkdir(&path.toString().toCString(), 0777);
		if(r)
			throw SystemException(r, _ << "cannot create " << path << ": " << strerror(errno));
#	endif
}

/**
 * Remove a directory.
 * @param path	Path of the directory to remove.
 */
void System::removeDir(const sys::Path& path) throw(SystemException) {
#	if defined(__WIN32) || defined(__WIN64)
		if(!RemoveDirectory(&path.toString().toCString(), NULL))
			throw SystemException(0, _ << "cannot remove " << path << ": " << win::getLastErrorMessage()));
#	else
		int r = rmdir(&path.toString().toCString());
		if(r)
			throw SystemException(r, _ << "cannot remove " << path << ": " << strerror(errno));
#	endif
}


/**
 * Get the path of a non-existing temporary file: the file is
 * created that should prevent another application to use the
 * same temporary file name.
 *
 * The application is responsible for cleaning up the file.
 * @return	Temporary file path.
 */
sys::Path System::getTempFile(void) throw(SystemException) {
#	if defined(__WIN32) || defined(__WIN64)
		// From https://msdn.microsoft.com/en-us/library/windows/desktop/aa363875%28v=vs.85%29.aspx
		char buf[MAX_PATH + 1];
		UINT r = GetTempFileName(&temp().toString(), "elf,", 0, buf);
		if(r == 0)
			throw SystemException(_ << "cannot create a temporary file: " << win::getLastErrorMessage());
		else
			return Path(buf);
#	else
		string tmp = (Path::temp() / "elf,XXXXXX").toString();
		char t[tmp.length() + 1];
		strncpy(t, &tmp, tmp.length());
		t[tmp.length()] = '\0';
		int fd = mkstemp(t);
		close(fd);
		return Path(t);
	#	endif
}

/**
 * Get the path of a non-existing temporary directory that is
 * created and whose path is returned.
 *
 * The application is responsible for cleaning up the directory.
 * @return	Temporary directory path.
 */
sys::Path System::getTempDir(void) throw(SystemException) {
#	if defined(__WIN32) || defined(__WIN64)
	// Windows: https://msdn.microsoft.com/en-us/library/windows/desktop/aa363875%28v=vs.85%29.aspx
	int cnt = 0;
	while(cnt < 1000) {
		Path path = temp() / (_ << "elf," << cnt);
		try {
			makeDir(path);
			return path;
		}
		catch(System) {
			cnt++;
		}
	}
	throw SystemException(_ << "cannot create a temporary directory: " << win::getLastErrorMessage());

#	else
		string tmp = (Path::temp() / "elf,XXXXXX").toString();
		char t[tmp.length() + 1];
		strncpy(t, &tmp, tmp.length());
		t[tmp.length()] = '\0';
		mkdtemp(t);
		return Path(t);
	#	endif
}


/**
 * Get the number of cores of the current host.
 * @notice	ELM is not ever able to determine the number of cores according
 * 			to the underlying OS: when it cant, it only assume 1 core!
 * @return	Number of core.
 */
int System::coreCount(void) {
	static int nb = -1;
	if(nb < 0) {
#		ifdef __linux
			try {
				io::InStream *in_stream = readFile("/proc/cpuinfo");
				io::Input in(*in_stream);
				nb = 0;
				string line = in.scanLine();
				while(line) {
					if(line.startsWith("processor\t:"))
						nb++;
					line = in.scanLine();
				}
				if(nb < 0)
					nb = 1;
				delete in_stream;
			}
			catch(SystemException& e) {
				nb = 1;
			}
#		elif defined(__WIN32) || defined(__WIN64)
			LPSYSTEM_INFO info;
			GetSystemInfo(&info);
			nb = info.dwNumberOfProcessors;
#		else
			nb = 1;
#		endif
	}
	return nb;
}


/**
 * Stop the application.
 * @param code	Code to return (usually 0 for success).
 */
void System::exit(int code) {
	::exit(code);
}

} } // elm::sys
