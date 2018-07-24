/*
 *	$Id$
 *	Fletcher class interface
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
#ifndef ELM_CHECKSUM_FLETCHER_H
#define ELM_CHECKSUM_FLETCHER_H

#include <elm/types.h>
#include <elm/io/InStream.h>
#include <elm/io/OutStream.h>

namespace elm { namespace checksum {

// Fletcher class
class Fletcher {
public:
	Fletcher(void);
	virtual ~Fletcher(void);
	t::uint32 sum(void);
	void put(io::InStream& in);
	void put(const void *buffer, int length);
	void put(const CString& str);
	void put(const String& str);
	inline Fletcher& operator<<(const char *str)
		{ put(CString(str)); return *this; }
	inline Fletcher& operator<<(const CString& str)
		{ put(str); return *this; }
	inline Fletcher& operator<<(const String& str)
		{ put(str); return *this; }
	template <class T> inline Fletcher& operator<<(const T& value)
		{ put(&value, sizeof(T)); return *this; }

	// io::OutStream overload
	virtual int write(const char *buffer, int size);
	virtual int flush(void);
	virtual cstring lastErrorMessage(void);

private:
	t::uint32 sum1, sum2;
	t::uint32 len;
	char half[2];
	t::uint16 size;
	inline void shuffle(void);
	inline void add(void);
};

} } // elm::checksum

#endif // ELM_CHECKSUM_FLETCHER_H
 
