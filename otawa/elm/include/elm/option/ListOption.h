/*
 *	$Id$
 *	ListOption class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2010, IRIT UPS.
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
#ifndef ELM_OPTION_LISTOPTION_H_
#define ELM_OPTION_LISTOPTION_H_

#include <elm/option/ValueOption.h>
#include <elm/genstruct/Vector.h>

namespace elm { namespace option {

// ListOption class
template <class T>
class ListOption: public AbstractValueOption {
public:

	ListOption(Manager& man, int tag, ...): AbstractValueOption(man)
		{ VARARG_BEGIN(args, tag) init(man, tag, args); VARARG_END }
	ListOption(Manager& man, int tag, VarArg& args)
		: AbstractValueOption(man, tag, args) { }
	ListOption(const Make& make)
		: AbstractValueOption(make) { }

	inline const T& get(int index) const { return vals[index]; };
	inline void set(int index, const T& value) { vals[index] = value; };
	inline T& ref(int index) { return vals[index]; }
	inline void add(const T& val) { vals.add(val); }
	inline void remove(int index) { vals.removeAt(index); }
	inline int count(void) const { return vals.count(); }

	// Operators
	inline operator bool(void) const { return !vals.isEmpty(); }
	inline const T& operator[](int index) const { return vals[index]; }
	inline T& operator[](int index) { return vals[index]; }

	// Option overload
	virtual void process(String arg) { vals.add(read<T>(arg)); }
private:
	genstruct::Vector<T> vals;
};

} }	// elm::option

#endif /* ELM_OPTION_LISTOPTION_H_ */
