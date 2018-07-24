/*
 *	$Id$
 *	StringOption class implementation
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

#include <elm/option/StringOption.h>

namespace elm { namespace option {

/**
 * @class StringOption
 * This class is used for getting a command line option with a string argument.
 * @ingroup options
 */

/**
 * Build a string option with a short name.
 * @param manager			Parent option manager.
 * @param short_name		Single-letter name.
 * @param description		Option description.
 * @param arg_description	Argument description.
 * @param value				Option initial value.
 */
StringOption::StringOption(
	Manager& manager,
	char short_name,
	CString description,
	CString arg_description,
	String value)
:	ValueOption<string>(manager, short_name, description, arg_description, value)
	{ }


/**
 * Build a string option with a long name.
 * @param manager			Parent option manager.
 * @param long_name			Multiple-letter name.
 * @param description		Option description.
 * @param arg_description	Argument description.
 * @param value				Option initial value.
 */
StringOption::StringOption(
	Manager& manager,
	CString long_name,
	CString description,
	CString arg_description,
	String value)
:	ValueOption<string>(manager, long_name, description, arg_description, value)
	{ }


/**
 * Build a string option with a short name and a long name.
 * @param manager			Parent option manager.
 * @param short_name		Single-letter name.
 * @param long_name			Multiple-letter name.
 * @param description		Option description.
 * @param arg_description	Argument description.
 * @param value				Option initial value.
 */
StringOption::StringOption(
	Manager& manager,
	char short_name,
	CString long_name,
	CString description,
	CString arg_description,
	String value)
:	ValueOption<string>(manager, short_name, long_name, description, arg_description, value)
	{ }


} } // elm::option
