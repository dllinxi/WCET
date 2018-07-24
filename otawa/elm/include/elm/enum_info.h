/*
 *	enum_info class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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
#ifndef ELM_ENUM_INFO_H_
#define ELM_ENUM_INFO_H_

#include <elm/io/IOException.h>
#include <elm/type_info.h>

namespace elm {

template <class T>
struct enum_info: public enum_t {
	typedef struct value_t {
		inline value_t(cstring n, const T& v) { name = n; value = v; }
		cstring name;
		T value;
	} value_t;
	enum { is_defined_enum = 1 };
	static value_t values[];
	static cstring name(void);

	// value building
	static inline value_t value(const char *n, T v) { return value_t(n, v); }
	static inline value_t last(void) { return value_t("", T(0)); }

	// usage
	static cstring toString(T v)
		{ for(int i = 0; values[i].name; i++) if(v == values[i].value) return values[i].name; return "???"; }
	static T fromString(const string& name)
		{ for(int i = 0; values[i].name; i++) if(name == values[i].name) return values[i].value; throw io::IOException("bad enum value"); }

	// iterator on values
	class iterator {
		friend struct enum_info;
	public:
		inline iterator(const iterator& it): i(it.i), vs(it.vs) { }
		iterator& operator++(void) { i++; return *this; }
		iterator& operator++(int) { i++; return *this; }
		inline bool operator==(const iterator& it) const
			{ return vs == it.vs && (i == it.i || (it.i < 0 && !vs[i].name) || (i < 0 && !vs[it.i].name)); }
		inline bool operator!=(const iterator& it) const { return !operator==(it); }
		inline cstring name(void) const { return values[i].name; }
		inline T value(void) const { return values[i].value; }
	private:
		inline iterator(value_t *_vs, int _i): i(_i), vs(_vs) { }
		int i;
		value_t *vs;
	};
	inline static iterator begin(void) { return iterator(values, 0); }
	inline static iterator end(void) { return iterator(values, -1); }
};

} // elm

#endif /* ELM_ENUM_INFO_H_ */
