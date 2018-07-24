/*
 *	$Id$
 *	Option class implementation
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

#include <elm/util/VarArg.h>
#include <elm/option/Option.h>
#include <elm/option/Manager.h>

namespace elm { namespace option {

/**
 * @class Option
 * Base class of classes used for representing options with the OptionManager class.
 * @ingroup options
 */


/**
 * Common initialization.
 */
void Option::init(Manager& manager, int tag, VarArg& args) {
	manager.addOption(this);
	while(tag != end) {
		configure(manager, tag, args);
		tag = args.next<int>();
	}
}


/**
 * Common initialization.
 */
void Option::init(Manager& manager, int tag, ...) {
	VARARG_BEGIN(args, tag)
		init(manager, tag, args);
	VARARG_END
}


/**
 * Handle a configuration tag and its value.
 * May be override for extending the @ref Option class
 * but do no forget to call back the original configure() method.
 * @param manager	Owner manager.
 * @param tag		Current tag.
 * @param args		List of arguments.
 */
void Option::configure(Manager& manager, int tag, VarArg& args) {
	switch(tag) {
	case cmd:
		manager.addCommand(args.next<const char *>(), this);
		break;
	case short_cmd:
		manager.addShort(args.next<char>(), this);
		break;
	case long_cmd:
		manager.addLong(args.next<const char *>(), this);
		break;
	case option::description:
		desc = args.next<const char *>();
		break;
	default:
		ASSERTP(false, "unknown configuration tag: " << tag);
	}
}


/**
 * Build an option.
 * @param make	Option maker.
 */
Option::Option(const Make& make) {
	make.man.addOption(this);
	desc = make._desc;
	for(int i = 0; i < make.cmds.count(); i++) {
		string cmd = make.cmds[i];
		if(cmd.length() == 2 && cmd[0] == '-')
			make.man.addShort(cmd[1], this);
		else
			make.man.addCommand(cmd, this);
	}
}


/**
 * @typedef usage_t
 * Defines the kind of arguments used by an option.
 */


/**
 * @var usage_t arg_none
 * This option does not require an argument.
 */


/**
 * @var usage_t arg_optional
 * Accept but does not require an argument.
 */


/**
 * @var usage_t arg_required
 * Require an argument.
 */


/**
 * Display the name of the option to the given output stream.
 * @param out	Stream to output to.
 */
void Option::output(io::Output& out) {
	char sname = shortName();
	CString lname = longName();
	if(sname)
		out << '-' << sname;
	if(sname && lname)
		out << '|';
	if(lname)
		out << "--" << lname;
}

	
/**
 * Get the single-character short name of the option.
 * @return	Single character or '\\0' if there is no short name.
 * @deprecated
 */
char Option::shortName(void) {
	return 0;
}

	
/**
 * Get the multi-character long name of the option.
 * @return	Multi-character or an empty string if there is no long name.
 * @deprecated
 */
cstring Option::longName(void) {
	return "";
}

	
/**
 * Get the description of the option.
 * @return	Option description.
 */
cstring Option::description(void) {
	return desc;
}


/**
 * @fn usage_t Option::usage(void);
 * Get the argument usage type of this function.
 * @return	Argument usage type.
 */
 
 
/**
 * @fn CString Option::argDescription(void);
 * If the option requires an argument, get the description of the argument.
 * @return Argument description.
 */
 
 
/**
 * @fn void Option::process(String arg);
 * This method is called by the option manager when the option is encountered.
 * @param arg	If the option requires an argument, the value of the argument.
 */

} }	// elm::option
