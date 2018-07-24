/*
 *	$Id$
 *	String class interface
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

#include <stdio.h>
#include <elm/string/StringBuffer.h>

namespace elm {

/**
 * @class StringBuffer
 * As the ELM @ref String class is immutable, the big building of strings
 * may have a prohibitive cost. Instead, this class provides an economic way
 * of concatenating strings.
 * @ingroup string
 */


/**
 * @fn StringBuffer::StringBuffer(int capacity, int increment);
 * Build a new string buffer.
 * @param capacity		Initial capacity of the buffer.
 * @param increment	Incrementation size when buffer is enlarged.
 */

	
/**
 * @fn String StringBuffer::toString(void);
 * Convert the buffer to a string. The string buffer must no more be used after this call.
 * @return String contained in the buffer.
 */
	
	
/**
 * @fn int StringBuffer::length(void) const;
 * Get the length of the string stored in the buffer.
 * @return	String length.
 */


/**
 * @fn void StringBuffer::reset(void);
 * Remove all characters from the string buffer.
 */


/**
 * @fn io::OutStream& StringBuffer::stream(void);
 * Get the an output stream to write to the string buffer.
 * @return	Stream to the string buffer.
 */

} // elm
