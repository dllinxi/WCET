/*
 *	$Id$
 *	StandardOption class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-10, IRIT UPS.
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

#include <elm/assert.h>
#include <elm/option/Manager.h>
#include <elm/option/StandardOption.h>

namespace elm { namespace option {

/**
 * @class StandardOption
 * This abstract class factorize the handling of options (short and long) and option description.
 * @ingroup options
 */


/**
 * Build an option with a short name.
 * @param manager		Parent option manager.
 * @param short_name	Single letter name.
 * @param description	Option description.
 * @deprecated
 */
StandardOption::StandardOption(Manager& manager, char short_name, CString description) {
	init(manager, short_cmd, short_name, option::description, &description, end);
}


/**
 * Build an option with a long name.
 * @param manager		Parent option manager.
 * @param long_name		Multiple letter name.
 * @param description	Option description.
 * @deprecated
 */
StandardOption::StandardOption(Manager& manager, CString long_name, CString description) {
	init(manager, long_cmd, &long_name, option::description, &description, end);
}


/**
 * Build an option with a short name and a long name.
 * @param manager		Parent option manager.
 * @param short_name	Single letter name.
 * @param long_name		Multiple letter name.
 * @param description	Option description.
 * @deprecated
 */
StandardOption::StandardOption(Manager& manager, char short_name, CString long_name, CString description) {
	init(manager, short_cmd, short_name, long_cmd, &long_name, option::description, &description, end);
}


// Option overload
char StandardOption::shortName(void) {
	return 0;
}


// Option overload
CString StandardOption::longName(void) {
	return "";
}


// Option overload
CString StandardOption::description(void) {
	return desc;
}

/**
 */
void StandardOption::configure(Manager& manager, int tag, VarArg& args) {
	switch(tag) {
	case option::description:
		desc = args.next<const char *>();
		break;
	default:
		Option::configure(manager, tag, args);
		break;
	}
}


} } // elm::option
