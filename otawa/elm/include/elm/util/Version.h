/*
 *	$Id$
 *	Version class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-08, IRIT UPS.
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
#ifndef ELM_UTIL_VERSION_H
#define ELM_UTIL_VERSION_H

#include <elm/assert.h>
#include <elm/io.h>

namespace elm {

class Version {
public:
	static const Version ZERO;

	// Constructors
	inline Version(int major = 0, int minor = 0, int release = 0);
	inline Version(const Version& version);
	inline Version(const char *text) { *this = string(text); }
	inline Version(const cstring text) { *this = string(text); }
	inline Version(const string& text) { *this = text; }
	inline Version nextRelease(void) const;
	inline Version nextMinor(void) const;
	inline Version nextMajor(void) const;
	
	// Accessors
	inline int major(void) const;
	inline int minor(void) const;
	inline int release(void) const;
	inline bool accepts(const Version& version) const;
	inline int compare(const Version& version) const;
	
	// Operators
	inline operator bool(void) const;
	inline Version& operator=(const Version& version);
	inline Version& operator=(const char *text) { return *this = string(text); }
	inline Version& operator=(const cstring text) { return *this = string(text); }
	Version& operator=(const string& text);
	inline bool operator==(const Version& version) const;
	inline bool operator!=(const Version& version) const;
	inline bool operator>(const Version& version) const;
	inline bool operator>=(const Version& version) const;
	inline bool operator<(const Version& version) const;
	inline bool operator<=(const Version& version) const;
private:
	unsigned char _major;
	unsigned char _minor;
	unsigned short _release;
};

io::Output& operator<<(io::Output& out, const Version& version);

// Inlines
inline Version::Version(int major, int minor, int release)
: _major(major), _minor(minor), _release(release) {
	ASSERTP(major >= 0, "major number must be positive");
	ASSERTP(minor >= 0, "minor number must be positive");
	ASSERTP(release >= 0, "release number be positive");
}

inline Version::Version(const Version& version)
: _major(version._major), _minor(version._minor), _release(version._release) {
}

inline Version Version::nextRelease(void) const {
	return Version(_major, _minor, _release + 1);
}

inline Version Version::nextMinor(void) const {
	return Version(_major, _minor + 1, 0);
}

inline Version Version::nextMajor(void) const {
	return Version(_major + 1, 0, 0);
}

inline int Version::major(void) const {
	return _major;
}

inline int Version::minor(void) const {
	return _minor;
}

inline int Version::release(void) const {
	return _release;
}

inline bool Version::accepts(const Version& version) const {
	return _major != version._major || _minor <= version._minor;
}

inline int Version::compare(const Version& version) const {
	int res = _major - version._major;
	if(res)
		res = _minor - version._minor;
	return res;
}
	
inline Version::operator bool(void) const {
	return _major || _minor || _release;
}

inline Version& Version::operator=(const Version& version) {
	_major = version._major;
	_minor = version._minor;
	_release = version._release;
	return *this;
}

inline bool Version::operator==(const Version& version) const {
	return compare(version) == 0;
}

inline bool Version::operator!=(const Version& version) const {
	return compare(version) != 0;
}

inline bool Version::operator>(const Version& version) const {
	return compare(version) > 0;
}

inline bool Version::operator>=(const Version& version) const {
	return compare(version) >= 0;
}

inline bool Version::operator<(const Version& version) const {
	return compare(version) < 0;
}

inline bool Version::operator<=(const Version& version) const {
	return compare(version) <= 0;
}

}; // elm

#endif // ELM_UTIL_VERSION_H
