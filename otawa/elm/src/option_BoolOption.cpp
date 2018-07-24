/*
 *	$Id$
 *	BoolOption class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-07, IRIT UPS.
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
#include <elm/option/BoolOption.h>

namespace elm { namespace option {


/**
 * @class BoolOption
 * This class is used for getting boolean option usually marked by simple flag.
 * @ingroup options
 */

/**
 * !!TODO!!
 * Very sad side effect not detected by compiler BoolOption(_, _, str, str) is viewed as first cast,
 * last string being converted to boolean.
 * Too bad for user. Perhaps change the order of constructors. Else either the default initialization
 * must be mandatory or only one complete form of option building muste exists.
 * Same problem arises with strings and there is no solution.
 */

/**
 * Build a boolean option with a short name.
 * @param manager		Parent option manager.
 * @param short_name	Single-letter name.
 * @param description	Option description.
 * @param value			Option initial value.
 */
BoolOption::BoolOption(Manager& manager, char short_name, CString description, bool value)
: StandardOption(manager, short_name, description), val(value) {
}


/**
 * Build a boolean option with a long name.
 * @param manager		Parent option manager.
 * @param long_name		Multiple-letter name.
 * @param description	Option description.
 * @param value			Option initial value.
 */
BoolOption::BoolOption(Manager& manager, CString long_name, CString description, bool value)
: StandardOption(manager, long_name, description), val(value) {
}


/**
 * Build a boolean option with a short name and a long name.
 * @param manager		Parent option manager.
 * @param short_name	Single-letter name.
 * @param long_name		Multiple-letter name.
 * @param description	Option description.
 * @param value			Option initial value.
 */
BoolOption::BoolOption(Manager& manager, char short_name, CString long_name,
CString description, bool value)
: StandardOption(manager, short_name, long_name, description), val(value) {
}


/**
 * @fn bool BoolOption::value(void) const;
 * Get the current value of the option.
 * @return	Option value.
 * @deprecated
 */


/**
 * @fn bool BoolOption::get(void) const;
 * Get the current value of the option.
 * @return	Option value.
 */


/**
 * @fn void BoolOption::set(bool value);
 * Set the value of the option.
 * @param value	New option value.
 */


// Option overload
usage_t BoolOption::usage(void) {
	return arg_none;
}


// Option overload
CString BoolOption::argDescription(void) {
	return "";
}


// Option overload
void BoolOption::process(String arg) {
	val = true;
}


/**
 * @fn BoolOption::operator bool(void) const;
 * Boolean conversion for getting option value.
 */


/**
 * @fn bool BoolOption::operator*(void) const;
 * Get the current value of the option.
 * @return	Option value.
 */

} } // elm::option
