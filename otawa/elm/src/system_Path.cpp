/*
 *	Path class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <elm/genstruct/Vector.h>
#include <elm/sys/Path.h>
#include <elm/sys/System.h>
#include <elm/sys/SystemException.h>

//#define DEBUG
#if defined(DEBUG) && !defined(NDEBUG)
#	define TRACE		cerr << __FILE__ << ":" << __LINE__ << io::endl;
#else
#	define TRACE
#endif

namespace elm { namespace sys {

/**
 * @class Path
 * This class represents a file path in the current file system.
 * @ingroup system_inter
 */


/**
 * Find the next separator in the path buffer.
 * @param start		Start position to look for.
 * @return			Position of next separator or -1.
 */
int Path::nextSeparator(int start) const {
	for(int i = start; i < buf.length(); i++)
		if(isSeparator(buf[i]))
			return i;
	return -1;
}


/**
 * Get the last separator.
 * @return		Position of last separator or null if there are none.
 */
int Path::lastSeparator(void) const {
	for(int i = buf.length() - 1; i >= 0; i--)
		if(isSeparator(buf[i]))
			return i;
	return -1;
}


/**
 * @fn Path::Path(String path);
 * Build a new path from the given string.
 * @param path	Path to store.
 */


/**
 * @fn Path::Path(Path& path);
 * Build a path from another path.
 */


/**
 * Symbol used to represent a backward move in the path.
 */
const string Path::BACK_PATH = "..";


/**
 * Build a path which is the current path relative to
 * the given base path.
 * @param base	Base path to be relative to.
 * @return		Same path but relative to the base.
 */
Path Path::relativeTo(Path base) const {
	Path r;
	while(base && !base.isPrefixOf(*this)) {
		r = r / BACK_PATH;
		base = base.parent();
	}
	return r / toString().substring(base.toString().length() + 1);
}


/**
 * Get the canonical form of the current path.
 * Usually, a relative path becomes an absolute one. Relative operators (.., .)
 * are removed if it is possible.
 */
Path Path::canonical(void) const {
	TRACE

	// Make it absolute
	Path path = *this;
	if(isRelative())
		path = absolute();
	TRACE

	// Select kept components
	genstruct::Vector<String> comps;
	int stop = path.nextSeparator(), start = 0;
	while(stop >= 0) {

		// Select the component
		String comp = path.buf.substring(start, stop - start);
		if(comp == "..") {
			if(comps.count() > 0)
				comps.setLength(comps.count() - 1);
		}
		else if(comp && comp != ".")
			comps.add(comp);

		// Go to next component
		start = stop + 1;
		stop = path.nextSeparator(start);
	}
	comps.add(path.buf.substring(start));
	TRACE


	// Rebuild path
	StringBuffer buffer;
	for(int i = 0; i < comps.length(); i++) {
#		if defined(__WIN32) || defined(__WIN64)
			if(i != 0 || !(comps[i].length() >= 1 && comps[i].endsWith(":")))
				buffer << SEPARATOR;
#		else
			buffer << SEPARATOR;
#		endif
		buffer << comps[i];
	}
	TRACE
	string r = buffer.toString();
	return r;
}


/**
 * Get the absolute path matching the current path.
 * @return	Matching absolute path.
 */
Path Path::absolute(void) const {
	if(isAbsolute())
		return *this;
	else if(isHomeRelative())
		return home() / Path(buf.substring(1));
	else
		return current() / *this;
}


/**
 * Change the current path.
 * @param path	New current path.
 */
void Path::setCurrent(Path& path) {
	if(chdir(&path.buf) < 0)
		throw SystemException(errno, "elm::system::Path");
}


/**
 * Build a new path by appending two paths.
 * @param path	Path to append.
 * @return		New path.
 */
Path Path::append(Path path) const {
	if(!buf)
		return path;
	else if(!path)
		return *this;
	else {
		StringBuffer buffer;
		buffer << buf << SEPARATOR << path.buf;
		return Path(buffer.toString());
	}
}


/**
 * Find the parent path of the current or an empty path if there is no parent.
 * @return	Parent path.
 */
Path Path::parent(void) const {
	int pos = lastSeparator();
	if(pos < 0)
		return Path("");
	else
		return Path(buf.substring(0, pos));
}


/**
 * @fn String& Path::toString(void);
 * Convert the path to string.
 * @return	Matching string.
 */


/**
 * Return the name part of the path.
 * @return	Name part.
 */
String Path::namePart(void) const {
	int pos = lastSeparator();
	if(pos < 0)
		return buf;
	else
		return buf.substring(pos + 1);
}


/**
 * Return the directory part of the path.
 * @return	Directory part.
 */
sys::Path Path::dirPart(void) const {
	int pos = lastSeparator();
	if(pos < 0)
		return "";
	else
		return Path(buf.substring(0, pos));
}


/**
 * Test if the path is absolute.
 * @return	True if it is absolute, false else.
 */
bool Path::isAbsolute(void) const {
#	if defined(__WIN32) || defined(__WIN64)
		return (buf.length() >= 2 && buf[1] == ':') || (buf.length() >= 1 && (buf[0] == SEPARATOR || buf[0] == '/'));
#	else
		return buf.length() > 0 && buf[0] == SEPARATOR;
#	endif
}


/**
 * Test if the path is relative.
 * @return	True if it is relative, false else.
 */
bool Path::isRelative(void) const {
	return !isAbsolute();
}


/**
 * Test if the path is home-relative. On Unix, it is usually starting with "~".
 * @return	True if it is home-relative, false else.
 */
bool Path::isHomeRelative(void) const {
#	if defined(__WIN32) || defined(__WIN64)
		return false;
#	else
		return buf.length() > 0 && buf[0] == '~';
#	endif
}


/**
 * @fn bool Path::equals(Path& path);
 * Test if two paths are lexically equals.
 * @return	True if they equals, false else.
 */


/**
 * @fn bool Path::subPathOf(Path& path);
 * Test if the current path is a sub-path of given path.
 * This means that the path is a prefix of the current
 * path.
 * @return	True if it contains, false else.
 * @deprecated	Use isPrefixedBy() instead.
 */


/**
 * @fn bool Path::isPrefixOf(const Path& path) const;
 * Test if the current path is prefix of the given one.
 * @param path	Path to test.
 * @return		True if the current path is prefix of the given one.
 */


/**
 * @fn bool Path::prefixedBy(const Path& path);
 * Test if the current path if prefixed by the given one.
 * @param path	Prefix path.
 * @return		True if path is a prefix of the current path.
 */


/**
 * Get the current path.
 * @return	Current path.
 */
Path Path::current(void) {
	int size = 256;
	while(true) {
		char buf[size];
		if(getcwd(buf, size))
			return Path(buf);
		else
			size *= 2;
	}
	return Path("");
}


/**
 * Get the path of the home directory.
 * @return	Home directory.
 */
Path Path::home(void) {
	return Path(getenv("HOME"));
}


/**
 * Get a temporary directory. If the OS does not provide any temporary
 * directory, the current directory is selected. As a fallback, a "tmp"
 * directory is created in the user home. If it can't be, the user home
 * is returned.
 * @return Temporary directory path.
 */
Path Path::temp(void) {
	static bool init = false;
	static Path path;
	if(!init) {
#		if defined(__WIN32) || defined(__WIN64)
			char buf[MAX_PATH + 1];
			DWORD r = GetTempPath(MAXPATH + 1, buf);
			if(!r)
				throw SystemException("cannot get temporary file");
			else {
				init = true;
				path = Path(buf);
			}
#		else

			// look in possible paths
			Path paths[] = {
				home() / "tmp",
				"/tmp",
				current(),
				home(),
				Path()
			};
			for(int i = 0; paths[i]; i++)
				if(paths[i].isDir() && paths[i].isWritable()) {
					path = paths[i];
					init = true;
					break;
				}

			// else try to make HOME/tmp
			if(!init) {
				path = home() / "tmp";
				try {
					System::makeDir(path);
					init = true;
				}
				catch(SystemException& e) {
				}
			}

			// fallback to home
			if(!init) {
				init = true;
				path = home();
			}

#		endif
	}
	return path;
}


/**
 * @fn Path& Path::operator=(Path& path);
 * Allows assignment in the path.
 * @param path	Assigned value.
 */


/**
 * @fn bool Path::operator==(Path& path);
 * Same as @ref equals().
 */


/**
 * @fn bool Path::operator!=(Path& path);
 * Same as !@ref equals().
 */


/**
 * @fn Path Path::operator/(Path& path);
 * Same as @ref append.
 */


/**
 * @fn Path::operator String& (void);
 * Convert to string.
 */


/**
 * @fn Path::operator bool (void);
 * Test if the path is null.
 * @return	True if it is not null, false else.
 */


/**
 * Get the base part of the path, that is, the path without the extension
 * of the file part.
 * @return	Base part of the path.
 */
Path Path::basePart(void) const {
	int lsep = lastSeparator();
	int pos = buf.lastIndexOf('.');
	if(pos <= lsep)
		return *this;
	else
		return buf.substring(0, pos);
}


/**
 * Get the extension of the referenced file name.
 * @return	Extension  (without ".") or an empty string if there is no extension.
 */
String Path::extension(void) const {
	int lsep = lastSeparator();
	int pos = buf.lastIndexOf('.');
	if(pos <= lsep)
		return "";
	else
		return buf.substring(pos + 1);
}


/**
 * Change the extension if there is some one or add the given extension.
 * @param new_extension	New extension to put in.
 * @return				New path with extension set.
 */
Path Path::setExtension(CString new_extension) const {
	StringBuffer nbuf;
	int lsep = lastSeparator();
	int pos = buf.lastIndexOf('.');
	if(pos <= lsep)
		nbuf << buf;
	else
		nbuf << buf.substring(0, pos);
	nbuf << '.' << new_extension;
	return nbuf.toString();
}


/**
 * Test if the path matches a file, a directory or any file system object.
 * @return	True if it exists (or is not accessible), false else.
 */
bool Path::exists(void) const {
	int res = ::access(&toString(), F_OK);
	return res == 0 || errno != ENOENT;
}


/**
 * Test if the path matches a file.
 * @return	True if it is a file (and is accessible), false else.
 */
bool Path::isFile(void) const {
	struct stat buf;
	int res = stat(&toString(), &buf);
	if(res != 0)
		return false;
	return S_ISREG(buf.st_mode);
}


/**
 * Test if the path matches a directory.
 * @return	True if it is a directory (and is accessible), false else.
 */
bool Path::isDir(void) const {
	struct stat buf;
	int res = stat(&toString(), &buf);
	if(res != 0)
		return false;
	return S_ISDIR(buf.st_mode);
}


/**
 * Test if the path matches a readable file system object.
 * @return	True if it readable (and accessible), false else.
 */
bool Path::isReadable(void) const {
	int res = ::access(&toString(), R_OK);
	return res == 0;
}


/**
 * Test if the path matches a writable file system object.
 * @return	True if it writable (and accessible), false else.
 */
bool Path::isWritable(void) const {
	int res = ::access(&toString(), W_OK);
	return res == 0;
}

/**
 * Test if the path matches an executable file system object.
 * @return	True if it executable (and accessible), false else.
 */
bool Path::isExecutable(void) const {
	int res = ::access(&toString(), X_OK);
	return res == 0;
}


/**
 * @class Path::PathIter
 * This iterator allows to iterate over a list of paths, contained in a string,
 * separated by the OS path separator.
 */


/**
 * @fn Path::PathIter::PathIter(const string& str);
 */


/**
 * @fn Path::PathIter::PathIter(const PathIter& iter);
 */


/**
 * @fn bool Path::PathIter::ended(void) const;
 */


/**
 * @fn Path Path::PathIter::item(void) const;
 */


/**
 * @fn void Path::PathIter::next(void);
 */


/**
 * @fn void Path::PathIter::look(void);
 */

} } // elm::sys
