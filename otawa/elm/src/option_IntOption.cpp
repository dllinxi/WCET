/*
 *	$Id$
 *	IntOption class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-09, IRIT UPS.
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
#include <stdlib.h>
#include <elm/option/IntOption.h>
#include <elm/io/BlockInStream.h>
#include <elm/io/Input.h>

namespace elm { namespace option {


/**
 * @class IntOption
 * This class is used for getting integer option usually marked by simple flag.
 * @ingroup options
 */

/**
 * Build an integer option with a short name.
 * @param manager			Parent option manager.
 * @param short_name		Single-letter name.
 * @param description		Option description.
 * @param arg_description	Help text describing the nature of the argument.
 * @param value				Option initial value.
 */
IntOption::IntOption(
	Manager& manager,
	char short_name,
	CString description,
	CString arg_description,
	int value
):	ValueOption<int>(manager, short_name, description, arg_description, value)
	{ }


/**
 * Build an integer option with a long name.
 * @param manager			Parent option manager.
 * @param long_name			Multiple-letter name.
 * @param description		Option description.
 * @param arg_description	Help text describing the nature of the argument.
 * @param value				Option initial value.
 */
IntOption::IntOption(
	Manager& manager,
	CString long_name,
	CString description,
	CString arg_description,
	int value
):	ValueOption<int>(manager, long_name, description, arg_description, value)
	{ }


/**
 * Build an integer option with a short name and a long name.
 * @param manager			Parent option manager.
 * @param short_name		Single-letter name.
 * @param long_name			Multiple-letter name.
 * @param description		Option description.
 * @param arg_description	Help text describing the nature of the argument.
 * @param value				Option initial value.
 */
IntOption::IntOption(
	Manager& manager,
	char short_name,
	CString long_name,
	CString description,
	CString arg_description,
	int value
):	ValueOption<int>(manager, short_name, long_name, description, value)
	{ }



// Option overload
void IntOption::process(String arg) {
	io::BlockInStream ins(arg);
	io::Input in(ins);
	set(in.scanLong());
}

} } // elm::option
