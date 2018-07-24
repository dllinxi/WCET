/*
 *	$Id$
 *	AutoString and InputString class interfaces
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007-09, IRIT UPS.
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
#ifndef ELM_STRING_AUTOSTRING_H
#define ELM_STRING_AUTOSTRING_H

#include <elm/string/StringBuffer.h>
#include <elm/io/StringInput.h>
#ifdef ELM_TEST_AUTOSTR
#	include <elm/io.h>
#endif

namespace elm {

// AutoString class
class AutoString {
public:
	template <class T>
	inline AutoString& operator<<(const T& value)
		{ buf << value; return *this; }

	inline operator const string(void) {
		String result = buf.toString();
		delete this;
		return result;
	}

	// Debugging
	#ifdef ELM_TEST_AUTOSTR
		inline AutoString(void) { cout << "allocated " << this << io::endl; }
		inline ~AutoString(void) { cout << "fried " << this << io::endl; }
	#endif

private:
	StringBuffer buf;
};

// AutoStringStartup class
class AutoStringStartup {
public:
	template <class T>
	inline AutoString& operator<<(const T& value) {
		AutoString *str = new AutoString();
		return (*str) << value;
	}
};

// autostr object
extern AutoStringStartup autostr;
extern AutoStringStartup &_;


// StringInput class
/*class StringInput {
public:
	StringInput(const char *str);
	StringInput(const string& str);
	StringInput(cstring str);
	~StringInput(void);
	inline StringInput(StringInput& _in) { in.setStream(_in.in.stream()); _in.in.setStream(io::in); }
	inline StringInput& operator=(StringInput& _in) { in.setStream(_in.in.stream()); _in.in.setStream(io::in); return *this; }
	template <class T> StringInput& operator>>(T& val) { in.operator>>(val); return *this; }
private:
	io::Input in;
};*/


// Even faster autostring
#ifndef ELM_AUTOSTR_FAST_NO

template <class T>
inline AutoString& operator<<(CString str, const T& value)
	{ return autostr << str << value; }
template <class T>
inline AutoString& operator<<(const String& str, const T& value)
	{ return autostr << str << value; }

template <class T>
inline io::StringInput operator>>(const string& str, T& val)
	{ io::StringInput in(str); in >> val; return in; }

template <class T>
inline io::StringInput operator>>(cstring str, T& val)
	{ io::StringInput in(str); in >> val; return in; }

#endif // ELM_AUTOSTR_FAST_NO

} // elm

#endif // ELM_STRING_AUTOSTRING_H
