/*
 *	$Id$
 *	RTTI module
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
#ifndef ELM_RTTI_H
#define ELM_RTTI_H

#include <elm/string.h>
#include <elm/types.h>

namespace elm {

// value_t structure
class value_t {
	CString _name;
	int _value;
public:
	static inline value_t end(void) { return value_t("", 0); }
	inline value_t(CString name, int value): _name(name), _value(value) { }
	inline CString name(void) const { return _name; }
	inline int value(void) const { return _value; }
};
inline value_t value(CString name, int value) {
	return value_t(name, value);
} 


// Field class
template <class V>
class Field {
	CString _name;
	V& _value;
public:
	inline Field(CString name, V& value): _name(name), _value(value) { }
	inline CString name(void) const { return _name; };
	inline V& value(void) const { return _value; };
};
template <class T>
inline Field<T> field(CString name, T& value) {
	return Field<T>(name, value);
}


// DefaultField class
template <class T>
class DefaultField: public Field<T> {
	const T& def;
public:
	inline DefaultField(CString name, T& value, const T& _default)
		: Field<T>(name, value), def(_default) { }
	inline const T& defaultValue(void) const { return def; }
};
template <class T>
inline DefaultField<T> field(CString name, T& value, const T& def) {
	return DefaultField<T>(name, value, def);
}


// Enumerations
#define ELM_ENUM(type) \
	namespace elm { \
		template <> struct type_info<type>: public enum_t { \
			static value_t __vals[]; \
			static inline CString name(void) { return "<enum " #type ">"; } \
			static inline value_t *values(void) { return __vals; } \
		}; \
	}
#define ELM_ENUM_BEGIN(type) \
	namespace elm { \
		value_t type_info<type>::__vals[] = {
#define ELM_ENUM_END \
			, value("", 0) \
		}; \
	}
#define ELM_VALUE(name) elm::value(elm::_unqualify(#name), name)

#ifndef ELM_NO_SHORTCUT
#	define ENUM(type) ELM_ENUM(type)
#	define VALUE(name) ELM_VALUE(name)
#	define ENUM_BEGIN(type) ELM_ENUM_BEGIN(type)
#	define ENUM_END ELM_ENUM_END
#endif


// AbstractClass class
class AbstractClass {
	CString _name;
	AbstractClass *_base;
public:
	inline AbstractClass(CString name, AbstractClass *base = 0)
		: _name(name), _base(base) { };
	virtual ~AbstractClass(void) { }
	inline CString name(void) const { return _name; };
	inline AbstractClass *base(void) const { return _base; };
	virtual void *instantiate(void) = 0;
	bool baseOf(AbstractClass *clazz);
};


// Class class
template <class T>
class Class: public AbstractClass {
public:
	inline Class(CString name, AbstractClass *base = 0)
		: AbstractClass(name, base) { };
	virtual void *instantiate(void) { return new T; }; 
};


// _unqualify function
inline CString _unqualify(CString name) {
	int pos = name.lastIndexOf(':');
	if(pos < 0)
		return name;
	else
		return name.substring(pos + 1);
}

} // elm

#endif // ELM_RTTI_H
