/*
 *	AutoComparator class interface
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
#ifndef ELM_UTIL_AUTO_COMPARATOR_H
#define ELM_UTIL_AUTO_COMPARATOR_H

#include <elm/assert.h>

namespace elm {

// AutoComparator template
template <class T>
class AutoComparator {
public:
	inline int _compare(const T& v) const { return ((T*)this)->compare(v); }
	inline bool operator==(const T& v) const { return _compare(v) == 0; }
	inline bool operator!=(const T& v) const { return _compare(v) != 0; }
	inline bool operator<(const T& v) const { return _compare(v) < 0; }
	inline bool operator<=(const T& v) const { return _compare(v) <= 0; }
	inline bool operator>(const T& v) const { return _compare(v) > 0; }
	inline bool operator>=(const T& v) const { return _compare(v) >= 0; }
};

// AutoComparator template
template <class T>
class AutoPartialComparator {
public:
	inline int _compare(const T& v) const { return ((T*)this)->compare(v); }
	inline int _equals(const T& v) const { return ((T*)this)->equals(v); }
	inline bool operator==(const T& v) const { return _equals(v); }
	inline bool operator!=(const T& v) const { return !_equals(v); }
	inline bool operator<(const T& v) const { return _compare(v) < 0; }
	inline bool operator<=(const T& v) const { return _compare(v) <= 0; }
	inline bool operator>(const T& v) const { return _compare(v) > 0; }
	inline bool operator>=(const T& v) const { return _compare(v) >= 0; }
};
} // elm

#endif // ELM_UTIL_AUTO_COMPARATOR_H
