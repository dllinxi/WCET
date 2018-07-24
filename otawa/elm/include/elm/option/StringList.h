/*
 *	$Id$
 *	option::StringList class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
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
#ifndef ELM_OPTION_STRING_LIST_H
#define ELM_OPTION_STRING_LIST_H

#include <elm/option/StandardOption.h>
#include <elm/genstruct/Vector.h>

namespace elm { namespace option {

// StringList class
class StringList: public StandardOption {
public:
	StringList(
		Manager& manager,
		char short_name,
		cstring description,
		cstring arg_description);
	StringList(
		Manager& manager,
		cstring long_name,
		cstring description,
		cstring arg_description);
	StringList(
		Manager& manager,
		char short_name,
		cstring long_name,
		cstring description,
		cstring arg_description);

	// Option overload
	virtual usage_t usage(void);
	virtual cstring argDescription(void);
	virtual void process(string arg);

	// Operators
	inline operator bool(void) const { return args; };
	inline const string& operator[](int index) const { return args[index]; }
	inline int count(void) const { return args.count(); }

private:
	cstring arg_desc;
	genstruct::Vector<string> args;
};

} } // elm::option

#endif	// ELM_OPTION_STRING_LIST_H
