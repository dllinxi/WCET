/*
 *	utf8 module interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2016, IRIT UPS.
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
#ifndef ELM_STRING_UTF8
#define ELM_STRING_UTF8

#include <elm/PreIterator.h>
#include <elm/string.h>
#include <elm/util/MessageException.h>

namespace elm { namespace utf8 {

typedef t::uint32 char_t;

class Exception: public MessageException {
public:
	inline Exception(string m): MessageException(m) { }
};


class Iter: public PreIterator<Iter, char_t> {
public:
	inline Iter(const char *str, t::size size) throw(Exception)
		: p(str), q(p + size), c(0) { parse(); }
	inline Iter(cstring str) throw(Exception)
		: p(&str), q(p + str.length()), c(0) { parse(); }
	inline Iter(string str) throw(Exception)
		: p(&str), q(p + str.length()), c(0) { parse(); }

	inline bool ended(void) const { return !c; }
	inline char_t item(void) const { return c; }
	inline void next(void) throw(Exception) { return parse(); }

private:
	void parse(void) throw(Exception);
	const char *p, *q;
	char_t c;
};

} }	// elm::utf8

#endif	// ELM_STRING_UTF8
