/*
 *	$Id$
 *	StringOption class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-09, IRIT UPS.
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
#ifndef ELM_OPTION_STRING_OPTION_H
#define ELM_OPTION_STRING_OPTION_H

#include <elm/option/ValueOption.h>

namespace elm { namespace option {

// StringOption class
class StringOption: public ValueOption<string> {
public:
	StringOption(Manager& manager, char short_name, cstring description,
		cstring arg_description, string value = "");
	StringOption(Manager& manager, cstring long_name, cstring description,
		CString arg_description, string value = "");
	StringOption(Manager& manager, char short_name, cstring long_name,
		cstring description, cstring arg_description, string value = "");

	// Option overload
	//virtual void process(String arg);
};

} } // elm::option

#endif	// ELM_OPTION_STRING_OPTION_H
