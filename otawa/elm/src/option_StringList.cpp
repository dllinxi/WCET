/*
 *	$Id$
 *	option::StringList class implementation
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

#include <elm/option/StringList.h>

namespace elm { namespace option {

/**
 * @class StringList
 * This class is used for getting a command line option composed of several strings
 * each one prefixed with the option short or long name.
 * @ingroup options
 */

/**
 * Build a string option with a short name.
 * @param manager			Parent option manager.
 * @param short_name		Single-letter name.
 * @param description		Option description.
 * @param arg_description	Argument description.
 */
StringList::StringList(
	Manager& manager,
	char short_name,
	cstring description,
	cstring arg_description)
:	StandardOption(manager, short_name, description),
	arg_desc(arg_description)
	{ }


/**
 * Build a string option with a long name.
 * @param manager			Parent option manager.
 * @param long_name			Multiple-letter name.
 * @param description		Option description.
 * @param arg_description	Argument description.
 */
StringList::StringList(
	Manager& manager,
	cstring long_name,
	cstring description,
	cstring arg_description)
:	StandardOption(manager, long_name, description),
	arg_desc(arg_description)
	{ }


/**
 * Build a string option with a short name and a long name.
 * @param manager			Parent option manager.
 * @param short_name		Single-letter name.
 * @param long_name			Multiple-letter name.
 * @param description		Option description.
 * @param arg_description	Argument description.
 */
StringList::StringList(
	Manager& manager,
	char short_name,
	cstring long_name,
	cstring description,
	cstring arg_description)
:	StandardOption(manager, short_name, long_name, description),
	arg_desc(arg_description)
	{ }


// Option overload
usage_t StringList::usage(void) {
	return arg_required;
}


// Option overload
cstring StringList::argDescription(void) {
	return arg_desc;
}


// Option overload
void StringList::process(String arg) {
	args.add(arg);
}


/**
 * @fn StringList::operator bool(void) const;
 * Test if some argument has been passed.
 * @return	True if there is some value, false else.
 */


/**
 * @fn const string& StringList::operator[](int index) const;
 * Get the indexed argument in the argument list.
 * @param index		Index of the argument to get.
 * @return			Argument matching the index.
 * @warning			It is an error if index >= count() .
 */


/**
 * @fn int StringList::count(void) const;
 * Get the count of arguments in this option.
 * @return	Argument count.
 */

} } // elm::option
