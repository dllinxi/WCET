/*
 *	$Id$
 *	RandomAccessStream class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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

#include <elm/deprecated.h>
#include <elm/assert.h>
#include <elm/io/RandomAccessStream.h>
#include <elm/sys/System.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

namespace elm { namespace io {

/**
 * @class RandomAccessStream
 * A stream allowing to move the read/write head along the file.
 */


/**
 * @fn pos_t RandomAccessStream::pos(void) const;
 * Get the current position in the file.
 * @return	Current position.
 */


/**
 * size_t RandomAccessStream::size(void) const; 
 * Get the size of the current stream.
 * @return	Stream size.
 */


/**
 * @fn bool RandomAccessStream::moveTo(pos_t pos) = 0;
 * Move the read/write pointer to the given position.
 * @param pos	Position to set the pointer to.
 * @return		True if the operation succeeded, false else.
 */


/**
 * @fn bool RandomAccessStream::moveForward(pos_t pos);
 * Move the read/write pointer to the given position, relatively to the current
 * position.
 * @param pos	Relative position to move to.
 * @return		True if the operation succeeded, false else.
 */


/**
 * @fn bool RandomAccessStream::moveBackward(pos_t pos);
 * Move the read/write pointer to the given position, relatively back from the
 * current position.
 * @param pos	Relative position to move backward from.
 * @return		True if the operation succeeded, false else.
 */


/**
 * @fn void RandomAccessStream::resetPos(void);
 * Reset the position of the read/write pointer to the start of the file.
 */


/**
 * Open a random access stream from a file.
 * @param path			Path of the file to open.
 * @param access		Type of access (one of READ, WRITE, READ_WRITE).
 * @return				Opened file.
 * @throws IOException	Thrown if there is an error.
 */
RandomAccessStream *RandomAccessStream::openFile(
	const sys::Path& path,
	access_t access )
	throw(sys::SystemException)
{
	return sys::System::openRandomFile(path, access);
};


/**
 * Create a random access stream from a file, removing it if it already exists.
 * @param path			Path of the file to open.
 * @param access		Type of access (one of READ, WRITE, READ_WRITE).
 * @return				Opened file.
 * @throws IOException	Thrown if there is an error.
 */
RandomAccessStream *RandomAccessStream::createFile(
	const sys::Path& path,
	access_t access)
	throw(sys::SystemException)
{
	return sys::System::createRandomFile(path, access);
}

} } // elm::io
