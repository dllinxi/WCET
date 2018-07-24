/*
 *	$Id$
 *	Fletcher class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007, IRIT UPS.
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

#include <elm/checksum/Fletcher.h>
#include <elm/util/MessageException.h>
#include <elm/io/BlockInStream.h>

namespace elm { namespace checksum {

/**
 * @class Fletcher
 * Apply the Fletcher algorithm (http://en.wikipedia.org/wiki/Fletcher%27s_checksum)
 * to compute a checksum.
 * 
 * Data are checksummed calling any one of the @ref put() methods.
 * When all data has been checksummed, the checksum value is returned by
 * @ref sum. 
 */


/**
 * Build a new Fletcher checksum builder.
 */
Fletcher::Fletcher(void)
:	sum1(0xffff),
	sum2(0xffff),
	len(0),
	size(0)
{
}


/**
 */
void Fletcher::shuffle(void) {
	sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	len = 0;
}


/**
 */
void Fletcher::add(void) {
	sum1 += *(t::uint16 *)half;
	sum2 += sum1;
	len += 2;
	if(len == 360)
		shuffle();
}


/**
 * Checksum the given stream.
 * @param in		Stream to checksum.
 */
void Fletcher::put(io::InStream& in) {
	while(true) {
		while(size < 2) {
			int result = in.read(half + size, 2 - size);
			if(result < 0)
				throw MessageException("elm::checksum::Fletcher: error during stream read");
			if(!result)
				break;
			size += result;
		}
		if(!size)
			break;
		size = 0;
		add();
	}
}


/**
 * Return the checksum.
 * @param checksum	Current checksum.
 */
t::uint32 Fletcher::sum(void) {
	if(size == 1) {
		half[1] = 0;
		add();
	}
	shuffle();
	return (sum2 << 16) | sum1;
}


/**
 * Put a data block in the checksum.
 * @param block		Block address.
 * @param length	Block length.
 */
void Fletcher::put(const void *block, int length) {
	io::BlockInStream stream(block, length);
	put(stream);
}


/**
 * Put a C string in the checksum.
 * @param str	C string to put in.
 */
void Fletcher::put(const CString& str) {
	io::BlockInStream stream(str);
	put(stream);
}


/**
 * Put a string in the checksum.
 * @param str	String to put in.
 */
void Fletcher::put(const String& str) {
	io::BlockInStream stream(str);
	put(stream);	
}

/**
 */
int Fletcher::write(const char *buffer, int size) {
	put(buffer, size);
	return size;
}


/**
 */
int Fletcher::flush(void) {
	return 0;
}


/**
 */
cstring Fletcher::lastErrorMessage(void) {
	return "";
}


/**
 */
Fletcher::~Fletcher(void) {
}

} } // elm::checksum
