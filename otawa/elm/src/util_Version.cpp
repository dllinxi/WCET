/*
 *	$Id$
 *	Version class implementation
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

#include <elm/util/Version.h>
#include <elm/io/BlockInStream.h>

namespace elm {


/**
 * @class Version
 * Useful for representing versions. The versions are represented as a triplet
 * composed of a major number, a minor number and a release number. Different
 * major numbers means that two versions are uncompatible. The minor version
 * number means ascending compatibility. The release number versions only
 * denotes different debugging versions (no change in the interface of a
 * compilation unit).
 * @ingroup utility
 */


/**
 * @fn Version::Version(int major, int minor = 0, int release = 0);
 * Build a new version.
 * @param major		Major number.
 * @param minor		Minor number.
 * @param release	Release number.
 */


/**
 * @fn Version::Version(const Version& version);
 * Build a version by cloning.
 * @param version	Cloned version.
 */


/**
 * @fn Version Version::nextRelease(void) const;
 * Build a version with the next release.
 */


/**
 * @fn Version Version::nextMinor(void) const;
 * Build a version with the next minor number.
 */


/**
 * @fn Version Version::nextMajor(void) const;
 * Build a version with the next major number.
 */


/**
 * @fn int Version::major(void) const;
 * Get the major number.
 */


/**
 * @fn int Version::minor(void) const;
 * Get the minor number.
 */


/**
 * @fn int Version::release(void) const;
 * Get the release number.
 */


/**
 * @fn bool Version::accepts(const Version& version) const;
 * Test if the current version accepts the given one, that is, the current one
 * is ascendent-compatible with the given one.
 * @param version	Version to compare with.
 */


/**
 * @fn int Version::compare(const Version& version) const;
 * Compare two versions. Notice that comparison is only performed on major and
 * minor numbers.
 * @param version	Version to compare with.
 * @return			0 for equality, <0 if current is less than passed one,
 * 					>0 else.
 */


/**
 * Set a version from a text string. The text string must have the following
 * form: [0-9]+(\.[0-9]+([0-9]+)?)?. If the string does not match, the
 * version 0.0.0 is set.
 * @param text	Text containing the version.
 * @return		Current version.
 */
Version& Version::operator=(const string& text) {
	string val, all = text;
	io::Input in;
	
	try {
		// major
		{
			int pos = all.indexOf('.');
			if(pos < 0)
				return *this;
			val = all.substring(0, pos);
			all = all.substring(pos + 1);
			io::BlockInStream stream(val);
			in.setStream(stream);
			in >> _major;
		}
		
		// minor
		{
			int pos = all.indexOf('.');
			if(pos < 0)
				return *this;
			val = all.substring(0, pos);
			all = all.substring(pos + 1);
			io::BlockInStream stream(val);
			in.setStream(stream);
			in >> _minor;
		}
		
		// release
		{
			io::BlockInStream stream(all);
			in.setStream(stream);
			in >> _release;
		}
	}
	catch(elm::Exception& e) {
		*this = ZERO;
	}
	return *this;
}


/**
 * @fn Version::Version(const string& text);
 * Build a version from a string.
 * @see operator=(const string& text)
 * @param text	String to build from.
 */


/**
 * Zero version, that is, 0.0.0.
 */
const Version Version::ZERO;


/**
 */
io::Output& operator<<(io::Output& out, const Version& version) {
	out << version.major()
		<< '.' << version.minor()
		<< '.' << version.release();
	return out;
}


}; // elm
