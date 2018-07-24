/*
 *	$Id$
 *	ValueOption class implementation
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

#include <elm/option/ValueOption.h>

namespace elm { namespace option {

/**
 * @class ValueOption
 * A generic option template for option receiving an argument.
 * It only requires to implement the process() virtual function.
 * @param T	Type of value of the argument.
 * @ingroup options
 */


/**
 * @fn  ValueOption::ValueOption(Make& make);
 * Build a value option from an initializer. It is usually used as below (using a value of type int):
 * @code
 * #include <elm/options.h>
 * ...
 * option::Manager man;
 * ValueOption<int> opt(Make()
 * 		.cmd("-i")
 * 		.cmd(".int")
 * 		.description("use an integer")
 * 		.argDescription("INT")
 * 		.usage(arg_optional)
 * 		.def(0));
 * @endcode
 *
 * The initializer supports several commands starting or by "-", a default value @c def,
 * a usage definition (see @ref usage_t) and an argument description that will follow the command
 * summary when the command line help message is displayed.
 */



/**
 * @fn ValueOption::ValueOption(Manager& manager, char short_name, cstring description, cstring arg_description, const T& value);
 * Build a function with only a short name.
 * @param manager			Owner option manager
 * @param short_name		Single letter name accessible with simple '-'.
 * @param description		Description of the option.
 * @param arg_description	Argument description in option display.
 * @param value				Default value of the option.
 */


/**
 * @fn ValueOption::ValueOption(Manager& manager, CString long_name, CString description, CString arg_description, const T& value);
 * Build a function with only a long name.
 * @param manager			Owner option manager
 * @param long_name			Multiple letter name accessible with '--'.
 * @param description		Description of the option.
 * @param arg_description	Argument description in option display.
 * @param value				Default value of the option.
 */


/**
 * @fn ValueOption::ValueOption(Manager& manager, char short_name, CString long_name, CString description, CString arg_description, const T& value);
 * Build a function with a short name and a long name.
 * @param manager			Owner option manager
 * @param short_name		Single letter name accessible with simple '-'.
 * @param long_name			Multiple letter name accessible with '--'.
 * @param description		Description of the option.
 * @param arg_description	Argument description in option display.
 * @param value				Default value of the option.
 */


/**
 * @fn const T& ValueOption::get(void) const;
 * Get the argument value of the option.
 * @return	Argument value.
 */


/**
 * @fn void ValueOption::set(const T& value);
 * Set the value of the option argument.
 * @param value	Value to set.
 */


/**
 * @fn operator ValueOption::const T&(void) const;
 * Same as get().
 */


/**
 * @fn ValueOption<T>& ValueOption::operator=(const T& value);
 * Same as set().
 */


/**
 * @fn const T& ValueOption::operator*(void) const;
 * Same as get().
 */


/**
 * @fn const T& Valueoption::value(void) const;
 * Get the argument value of the option.
 * @return	Argument value.
 * @deprecated	Only kept for compatibility issues.
 */


/**
 * @class AbstractValueOption
 * Value type independent parts of a value option.
 */


/**
 * Build an abstract value option.
 * @param man	Owner option manager.
 */
AbstractValueOption::AbstractValueOption(Manager& man): use(arg_required) {
	//man.addOption(this);
}


/**
 * Constructor.
 * @param man	Owner manager.
 * @param tag	First tag.
 * @param ...	Tag and arguments.
 */
AbstractValueOption::AbstractValueOption(Manager& man, int tag, ...): use(arg_required) {
	VARARG_BEGIN(args, tag)
		init(man, tag, args);
	VARARG_END
}

/**
 * Constructor.
 * @param man	Owner manager.
 * @param tag	First tag.
 * @param args	Tag and arguments.
 */
AbstractValueOption::AbstractValueOption(Manager& man, int tag, VarArg& args): use(arg_required) {
	init(man, tag, args);
}


/**
 * Constructor with an initializer as in the example velow:
 * @param make	Current initializer.
 */
AbstractValueOption::AbstractValueOption(const Make& make)
: Option(make), arg_desc(make.arg_desc), use(make._usage) {
}


/**
 */
cstring AbstractValueOption::description(void) {
	return desc;
}


/**
 */
usage_t AbstractValueOption::usage(void) {
	return use;
}


/**
 */
cstring AbstractValueOption::argDescription(void) {
	return arg_desc;
}


/**
 */
void AbstractValueOption::configure(Manager& manager, int tag, VarArg& args) {
	switch(tag) {
	case option::description:
		desc = args.next<const char *>();
		break;
	case option::arg_desc:
		arg_desc = args.next<const char *>();
		break;
	case require:
		use = arg_required;
		break;
	case optional:
		use = arg_optional;
		break;
	default:
		Option::configure(manager, tag, args);
	}
}

/**
 */
template <> string read<string>(string arg) {
		return arg;
}

/**
 */
template <> cstring read<cstring>(string arg) {
	ASSERTP(false, "CString cannot be used as abstract argument of options because they are not able to manage memory allocation.");
	return "";
}

} } // elm::option
