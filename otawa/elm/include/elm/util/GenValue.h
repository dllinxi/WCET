/*
 *	GenValue class interface
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

#ifndef ELM_UTIL_GENVALUE_H_
#define ELM_UTIL_GENVALUE_H_

namespace elm {

class AbstractValue {
public:
	virtual ~AbstractValue(void);
};

template <class T>
class GenValue: public AbstractValue {
public:
	inline GenValue(const T& value): _value(value) { }
	inline const T& value(void) const { return _value; }
private:
	T _value;
};

template <class T> GenValue<T> *gen_value(const T& v) { return new GenValue<T>(v); }

}	// elm

#endif /* ELM_UTIL_GENVALUE_H_ */
