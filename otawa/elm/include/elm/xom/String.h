/*
 *	$Id$
 *	xom::String class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-09, IRIT UPS.
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
#ifndef ELM_XOM_STRING_H
#define ELM_XOM_STRING_H

#include <elm/string.h>
#include <elm/io.h>

namespace elm { namespace xom {

/**
 * XOM character type.
 */
typedef unsigned char char_t;

// String class
class String: public CString {
public:

	// Constructors
	inline String(void) { }
	inline String(const char *str): CString(str) { }
	inline String(const char_t *str): CString((const char *)str) { }
	inline String(const CString& str): CString(str) { }
	inline String(const String& str): CString(str) { }

	// Buffer management
	void copy(void);
	void free(void);

	// Other operations
	void escape(io::OutStream& out) const;
	inline void escape(io::Output& out) const { escape(out.stream()); }
	inline elm::string escape(void) const { StringBuffer buf; escape(buf.stream()); return buf.toString(); }

	// Operators
	inline String& operator=(const String& str) { buf = str.buf; return *this; }
	inline operator char_t *(void) const { return (char_t *)buf; }
};
inline io::Output& operator<<(io::Output& out, String str)
	{ out << cstring(str); return out; }

} } // elm::xom

#endif // ELM_XOM_STRING_H
