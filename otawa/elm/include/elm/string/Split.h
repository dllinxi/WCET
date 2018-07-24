/*
 *	Split class
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-08, IRIT UPS.
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
#ifndef ELM_STRING_SPLIT_H_
#define ELM_STRING_SPLIT_H_

#include <elm/string/String.h>

namespace elm { namespace str {

// Split class
class Split: public PreIterator<Split, String> {
public:
	inline Split(void): l(-1), p(-1) { }
	inline Split(const String& str, char chr): s(str), ss(String::make(chr)), l(-1), p(-1) { find(); }
	inline Split(const String& str, String sub): s(str), ss(sub), l(-1), p(-1) { find(); }
	inline bool ended(void) const { return l >= s.length(); }
	inline String item(void) { return s.substring(l + 1, p - l - 1); }
	inline void next(void) { if(p >= s.length()) l = s.length(); else find(); }

private:
	inline void find(void)
		{ l = p; p = s.indexOf(ss, l + 1); if(p < 0) p = s.length(); }
	String s;
	String ss;
	int l, p;
};

} }	// elm::str

#endif /* ELM_STRING_SPLIT_H_ */
