/*
 *	$Id$
 *	Manager class implementation
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
#include <elm/io.h>
#include <elm/option/Manager.h>
#include <elm/string.h>
#include <elm/data/Vector.h>
#include <elm/sys/System.h>

namespace elm { namespace option {


/**
 * @defgroup options Command Line Options
 * 
 * @code
 * #include <elm/options.h>
 * @endcode
 * 
 * ELM provides several classes to manage efficiently and easily command line
 * options. There are currently two ways to use these classes, the old approach
 * that will become quickly deprecated (please no more use it) and the new one.
 *
 * @par The New Generation Approach (third generation)
 *
 * To perform the parsing of command line arguments, ELM provides two basic classes.
 * @ref elm::option::Manager represents the command as a base of documentation and as a list
 * of options items. The manager is initialized as in the following example:
 * @code
 * 	#include <elm/options.h>
 * 	...
 * 	option::Manager manager = Make("my command", Version(1, 0, 0))
 * 		.description("This is my command !")
 * 		.license("GPL V")
 * 		.author("me <me@here.there>");
 * @endcode
 *
 * According to your needs, as many configurations as needed can be passed
 * to the Make() object while maintaining type checking. Refer to @ref elm::option::Manager::Make
 * class documentation for the list of available configurations.
 *
 * The options works in the same way:
 * @code
 * option::SwitchOption opt1 = Make(manager).cmd("-o").cmd("--com").help("option 1");
 * @endcode
 *
 * As many ".cmd()" method calls can be added and other configurations can be passed
 * using the same syntax. Refer to the documentations of the different option classes
 * to get details about the configuration items (@ref elm::option::Option::Make,
 * @ref elm::option::SwitchOption::Make, ...).
 *
 * Using both configuration system for the manager and the option, a command is usually defined
 * as in the following example:
 * @code
 * class MyCommand: public option::Manager {
 * public:
 * 	MyCommand(void): option::Manager(Make("my-command", Version(1, 0, 0))
 * 		.description("This is my command !")
 * 		.license("GPL V")
 * 		.author("me <me@here.there>")),
 * 	opt(Make(*this).cmd("-o").cmd("--com").help("option 1")),
 * 	val(Make(*this).cmd("-v").cmd("--val").help("my value")) { }
 * 	...
 * private:
 * 	option::SwitchOption opt;
 * 	option::ValueOption val;
 * };
 * @endcode
 *
 * At this point, the options may be used as normal values in the program as below:
 * @code
 * 	if(opt)
 * 		doSomething(val);
 * @endcode
 *
 * Free arguments, not processed by an option item, are usually handled by overriding
 * the method
 * @code
 * class MyCommand: public option::Manager {
 * public:
 * 	...
 * protected:
 * 	virtual void process(String arg) {
 * 		if(!my_arg)
 * 			my_arg = arg;
 * 		else
 * 			throw OptionException("only one argument supported !");
 *	}
 *	...
 * private:
 * 	string my_arg;
 * };
 * @endcode
 *
 * The example above supports only one argument. If not defined, the argument @c arg
 * is copied to the field @c my_arg. If it is already defined, an @ref OptionException
 * is raised. In this case, the passed message will be displayed and the syntax
 * of the command will automatically be displayed to the user then.
 *
 * To use such a command, the @c main parameters, @c argc and @c argv, must be passed
 * to the @c parse() method:
 * @code
 * int main(int argc, char *argv[]) {
 * 	MyCommand cmd;
 * 	return cmd.parse(argc, argv);
 * }
 * @endcode
 *
 * This new approach allows to combine several benefits:
 * @li no more used of the risky processing of variable length arguments (second generation),
 * @li type checking ability,
 * @li readibility (see below),
 * @li extensibility (new configurations may be added without breaking the old configurations).
 *
 * @par The Old Generation Approach
 *
 * <b>Notice that the old generation approach for option definition is now considered
 * DEPRECATED. It will be removed in the next versions of ELM.</b>
 *
 * The goal of the new approach is to remove as much developer disturbance as possible.
 * The full option configuration is based on variable arguments formed as list of tags
 * possibly followed by an argument. This let configure the option and the option manager
 * in a more natural way.
 *
 * Let's go with a little example:
 * @code
 * #include <elm/options.h>
 * using namespace elm::option;
 *
 * class MyManager: public Manager {
 * public:
 *	MyManager(void): Manager(
 * 		option::program, "my_program",
 * 		option::version, new Version(1, 2),
 * 		option::copyright, "GPL (c) 2007 IRIT - UPS",
 * 		option::end)
 * 	{ };
 *
 *	virtual void process(string arg) {
 * 		cout << "add argument " << arg << io::endl;
 * 	}
 * } manager;
 *
 * BoolOption ok(manager,
 * 		option::cmd, "-c",
 * 		option::cmd, "--ok",
 * 		option::help, "is ok ?",
 * 		option::end);
 * StringOption arg(manager,
 * 		option::cmd, "-s",
 * 		option::cmd, "--string",
 * 		option::help, "set string",
 * 		option::arg_desc, "a simple string",
 * 		option::def, "",
 * 		option::end);
 * @endcode
 *
 *  This approach add a lot of flexibility because (1) all arguments do not need to be passed
 *  to the constructor (only the used ones) and (2) as many commands ('-' short name or '--' long name)
 *  as needed may be added to an option. As an example, look at the @ref BoolOption below:
 *  @code
 *  BoolOption verbose(manager,
 *  	option::short_cmd, 'c',
 *  	option::long_cmd, "verbose",
 *  	option::short_cmd, 'V',
 *  	option::end);
 *  @endcode
 *
 *  The only requirement is that the configuration list passed to the manager or to an option
 *  is terminated by and @ref option::end. This system of tags and arguments may also be used
 *  with the @ref Manager class that centralizes the list of options of an application.
 *
 *  The supported tags includes:
 *  @li @ref elm::option::end (all; no argument) -- mark end of tags
 *	@li @ref elm::option::program (manager; const char *) -- name of the program
 *	@li @ref elm::option::version (manager; @ref elm::Version *) -- version of the program
 *	@li @ref elm::option::author (manager; const char *) -- name of the authors
 *	@li @ref elm::option::copyright (manager, const char *) -- copyright of the program
 *	@li @ref elm::option::description (manager, option; const char *) -- description
 *	@li @ref elm::option::help -- same as @ref elm::option::description
 *	@li @ref elm::option::free_arg (manager; const char *) -- description of free arguments
 *	@li @ref elm::option::cmd (option; const char *) -- command (possibly including the minus prefix)
 *	@li @ref elm::option::short_cmd (option; char) -- one-letter short command (prefixed by "-")
 *	@li @ref elm::option::long_cmd (option; const char *) -- multiple lette command (prefixed by "--")
 *	@li @ref elm::option::def (option; option dependent) -- default value for valued options
 *	@li @ref elm::option::require (option; none) -- argument value is required
 *	@li @ref elm::option::optional (option; none) -- argument value is optional
 *	@li @ref elm::option::arg_desc (option; const char *) -- description of the argument value
 *
 *	In parenthesis, the first part design the application of the tag: to the manager or to the
 *	options. The second value give the type of the argument value of the option (if any).
 *
 *  In this new approach, you will need a small set of options classes:
 *  @li @ref elm::option::SwitchOption -- for boolean activable option
 *  @li @ref elm::option::ValueOption<T> -- for options with an argument value
 *  @li @ref elm::option::ListOption<T> -- for options with multiple arguments values
 *  @li @ref elm::option::ActionOption  -- for options causing an action
 *
 *  @ref elm::option::ValueOption<T> and @ref elm::option::ListOption<T> are provided for
 *  usual types (as support from the @ref elm::io::Input class) but may be customized for other
 *  types by overring some templates functions as below:
 *  @code
 *	template <> inline T ValueOption<T>::get(VarArg& args) { return args.next<T>(); }
 *	template <> inline T read<string>(string arg) { process arg to set the value }
 *	@endcode
 *
 *
 * @par The manager
 * 
 * The manager is the main entry to handle options. It provides the list of
 * options but also information about the program. To provides this kind of data,
 * one has to create a class inheriting fropm @ref Manager and to initialize
 * some fields in the constructor.
 * @li program -- name of the program,
 * @li version -- version of the program (as a string),
 * @li author -- author(s) of the program,
 * @li copyright -- licence of the program,
 * @li description -- a short description of the program,
 * @li free_argument_description -- description of non-option string in the
 * arguments.
 * 
 * Then, non-options string may be handled by overloading the Manager::process()
 * method. Each time one of this argument is found, this method will be called
 * with the string as parameter. For example, in the command below, the
 * Manager::process() method will be called twice: once with "file.txt" as
 * argument and once with "mydir/" as argument.
 * @code
 * $ cp file.txt mydir/
 * @endcode
 * 
 * @par Option Usage
 * 
 * All options inherits from the @ref Option class. Each one provides a
 * constructor taking the actual manager as parameter. The following parameters
 * may include (take a look at each class documentation for more details):
 * @li a one letter option identifier (just prefixed with '-'),
 * @li a full string option identifier (prefixed by a double '-'),
 * @li the description of the option,
 * @li the description of the option argument (if there is one),
 * @li the default value of the option argument (if there is one).
 * 
 * There are many class that allows parsing and retrieving options values as
 * @ref BoolOption, @ref IntOption, @ref StringOption or @ref EnumOption.
 * The option value may be accessed as a usual variable as in the example below:
 * @code
 * if(ok)
 * 	cout << "string argument is " << arg << io::endl;
 * @endcode
 * 
 * The @ref ActionOption class is a bit more complex because it allows
 * associating an action with an option. To achieve this, the user has to
 * derive a class from @ref ActionOption and to overload the
 * ActionOption::perform() method to implement the specific behaviour.
 */

// UnknownException
class UnknownException {
};


/**
 * @class OptionException
 * This class may be raised by options of by the option @ref Manager to
 * indicate that there is an error in command line arguments.
 * @ingroup options
 */


/**
 * Build an empty with a string message possibly containing printf-like
 * escape sequences for arguments.
 * @param message	Message formats.
 * @param ...		Arguments.
 */

OptionException::OptionException(const String& message)
: MessageException(message) {
}


/**
 * @class Manager
 * This class is used for managing the options of a command.
 * @ingroup options
 * @see @ref options
 */


/**
 */
Manager::Manager(void) {
}


/**
 * Build a manager with the new method.
 * @param config	First configuration item.
 * @param ...		end() ended configuration item list.
 * @deprecated		Since 22/03/13.
 */
Manager::Manager(int tag, ...) {
	VARARG_BEGIN(args, tag)
		while(tag != end) {
			configure(tag, args);
			tag = args.next<int>();
		}
	VARARG_END
}

/**
 * Build a new option manager.
 * @param maker		Information for initialization.
 */
Manager::Manager(const Make& maker): info(maker) {
	if(info._help)
		_help_opt = new SwitchOption(SwitchOption::Make(*this)
			.cmd("-h")
			.cmd("--help")
			.description("display this help message"));
	if(info._version_opt)
		_version_opt = new SwitchOption(SwitchOption::Make(*this)
			.cmd("-v")
			.cmd("--version")
			.description("display this application version"));
}


/**
 */
Manager::~Manager(void) {
}


/**
 * Called for each configuration tag.
 * May be overload to customize Manager configuration.
 * @param tag	Current tag.
 * @param args	Argument list.
 */
void Manager::configure(int tag, VarArg& args) {
	switch(tag) {
	case option::program:
		info._program = args.next<const char *>();
		break;
	case option::version: {
			Version *v = args.next<Version *>();
			ASSERT(v);
			info._version = *v;
			delete v;
		}
		break;
	case option::author:
		info._author = args.next<const char *>();
		break;
	case option::copyright:
		info._copyright = args.next<const char *>();
		break;
	case option::description:
		info._description = args.next<const char *>();
		break;
	case option::free_arg:
	case option::arg_desc:
		info._free_argument_description = args.next<const char *>();
		break;
	default:
		ASSERTP(false, "unknown configuration tag: " << tag);
	}
}


/**
 * Process an option with or without argument need.
 * @param option	Option to process.
 * @param i			Current index.
 * @param argc		Top index.
 * @pâram argv		Argument vector.
 * @param earg		Argument following an equal !
 */
void Manager::processOption(
	Option *option,
	int& i,
	int argc,
	Manager::argv_t argv,
	const char *earg
) {

	// No option
	if(!option)
		throw UnknownException();

	// Find the argument
	String arg;
	switch(option->usage()) {
	case arg_none:
		if(earg)
			throw OptionException(_ << "option \"" << argv[i]
				<< "\" requires no argument.");
		break;
	case arg_optional:
		if(earg)
			arg = earg;
		else if(i + 1 < argc && argv[i + 1][0] != '-')
			arg = argv[++i];
		break;
	case arg_required:
		if(earg)
			arg = earg;
		else if(i + 1 < argc && argv[i + 1][0] != '-')
			arg = argv[++i];
		else {
			throw OptionException(_ << "option \"" << argv[i]
				<< "\" requires an argument.");
		}
		break;	
	}
	
	// Process the option
	option->process(arg);
	if(_help_opt && _help_opt->get()) {
		displayHelp();
		sys::System::exit(1);
	}
	else if(_version_opt && _version_opt->get()) {
		displayVersion();
		sys::System::exit(1);
	}
}


/**
 * Called each time a free argumend (not tied to an option) is found.
 * @param arg	Free argument value.
 */
void Manager::process(String arg) {
}


/**
 * Add an option to the manager.
 * @param option				Option to add.
 * @throw OptionException		If the short name or the long name is already used.
 */
void Manager::addOption(Option *option) throw(OptionException) {
	ASSERTP(option, "null option");
	options.add(option);
}


/**
 * Remove the option from the manager.
 * @param option	Option to remove.
 */
void Manager::removeOption(Option *option) {
	options.remove(option);
}


/**
 * Add a short command.
 * @param cmd		Short command character.
 * @param option	Option to add.
 */
void Manager::addShort(char cmd, Option *option) throw(OptionException) {
	string str = _ << '-' << cmd;
	ASSERTP(!cmds.contains(str), "command '" << cmd << "' already used");
	cmds.put(str, option);
	shorts.put(cmd, option);
}


/**
 * Add a long command (prefixed with '--')
 * @param cmd		Long command string.
 * @param option	Option to add.
 */
void Manager::addLong(cstring cmd, Option *option) throw(OptionException) {
	string str = _ << "--" << cmd;
	ASSERTP(!cmds.contains(str), _ << "long command \"" << str << "\" already used");
	cmds.put(str, option);
}


/**
 * Add a generic command (no prefix required).
 * @param cmd		Command to add.
 * @param option	Option to add.
 */
void Manager::addCommand(string cmd, Option *option) throw(OptionException) {
	ASSERTP(!cmds.contains(cmd), "command \"" << cmd << "\" already used");
	cmds.put(cmd, option);
}


/**
 * Function called to run the application after
 * the command line parse. It is called by Manager::manage()
 * and can be overridden to provide its own behaviour to
 * the application.
 *
 * The default implementation do nothing.
 *
 * @throw elm::Exception	For any error during application run.
 */
void Manager::run(void) throw(elm::Exception) {
}


/**
 * Call to a complete management of the application by the option
 * manager. It will parse the given parameters and call the @ref run
 * method. In case of error, it will display it and provide an exit
 * code.
 *
 * It is able to make the main program code very simple:
 * @code
 * int main(int argc, char **argv) {
 * 		return MyApplication.manager(argc, argv);
 * }
 * @endcode
 *
 * @param argc	Argument count.
 * @param argv	Argument list.
 * @return		Exit code.
 */
int Manager::manage(int argc, argv_t argv) {
	try {
		parse(argc, argv);
		run();
		return 0;
	}
	catch(OptionException& e) {
		displayHelp();
		cerr << "ERROR: command line error: " << e.message() << io::endl;
		return 1;
	}
	catch(elm::Exception& e) {
		cerr << "ERROR: " << e.message() << io::endl;
		return 2;
	}
}


/**
 * Parse the given options.
 * @param argc				Argument count.
 * @param argv				Argument vector.
 * @throw OptionException	Thrown if the syntax contains an error.
 */
void Manager::parse(int argc, Manager::argv_t argv) throw(OptionException) {
	ASSERTP(argv, "null argv");
	ASSERTP(argc > 0, "negative argc");
	for(int i = 1; i < argc; i++) {
		string arg = argv[i];
		const char *earg = 0;

		// look for a command
		Option *option = cmds.get(arg, 0);
		if(!option) {
			int p = arg.indexOf('=');
			if(p >= 0) {
				option = cmds.get(arg.substring(0, p), 0);
				earg = argv[i] + p + 1;
			}
		}
		if(option)
			processOption(option, i, argc, argv, earg);

		// Free argument
		else if(arg.length() > 0 && arg[0] != '-')
			process(arg);
		
		// aggregated short name command
		else if(arg.length() > 1 && arg[1] != '-') {
			for(int j = 1; j < arg.length(); j++) {
				Option *option = shorts.get(arg[j], 0);
				if(!option)
					throw OptionException(_ << "option '" << arg[j] << "' in " << arg << " unknown !");
				processOption(option, i, argc, argv, 0);
			}
		}
		
		// none found: error
		else
			throw OptionException(elm::_ << "option \"" << arg << "\" is unknown.");
	}
}


/**
 * Display version information on the standard output.
 */
void Manager::displayVersion(void) {
	cout << info._program << ' ' << info._version << io::endl;
}


/**
 * Display the help text to standard error.
 */
void Manager::displayHelp(void) {

	// Display header
	cerr << info._program;
	if(version)
		 cerr << " V" << info._version;
	if(author)
		cerr << " by " << info._author;
	cerr << '\n';
	if(copyright)
		cerr << info._copyright << '\n';
	
	// Display syntax
	cerr << '\n'
		 << "USAGE: " << info._program;
	if(options)
		cerr<< " [OPTIONS]";
	if(info._free_argument_description)
		cerr << ' ' << info._free_argument_description;
	cerr << '\n';
	if(info._description)
		cerr << info._description << '\n';
	cerr << "\nOPTIONS may be:\n";

	// display the arguments
	Vector<Option *> done;
	typedef genstruct::SortedBinMap<string, Option *>::PairIterator iter;
	for(iter cmd(cmds); cmd; cmd++) {

		// already done?
		Option *option = (*cmd).snd;
		if(done.contains(option))
			continue;
		done.add(option);

		// display commands
		cerr << (*cmd).fst;
		iter ocmd = cmd;
		for(ocmd++; ocmd; ocmd++)
			if((*ocmd).snd == option)
				cerr << ", " << (*ocmd).fst;

		// display argument
		switch(option->usage()) {
		case arg_none:
			break;
		case arg_optional:
			cerr << " [" << option->argDescription() << ']';
			break;
		case arg_required:
			cerr << ' ' << option->argDescription();
			break;
		}

		// display description
		cerr << "\n\t" << option->description() << io::endl;
	}

}


/**
 * @fn SwitchOption::Make Manager::make_switch(void);
 * This function is a shortcut to @ref SwitchOption::Make() to reduce the definition
 * of options inside a @ef Manager class.
 */

/**
 * @fn ValueOption<T>::Make make_value(void);
 * This function is a shortcut to @ref ValueOption::Make() to reduce the definition
 * of options inside a @ef Manager class.
 */


/**
 * @class Manager::Make
 * This class is an initializer for class @ref Manager. It procides a safe and
 * readable way to initialize an option manager. It is usually passed directly
 * to the @ref Manager constructor.
 */

/**
 * @fn Manager::Make& Manager::Make::author(cstring s);
 * Specify the author of the application.
 */

/**
 * @fn Manager::Make& Manager::Make::copyright(cstring s);
 * Specify the copyright of the application.
 */

/**
 * @fn Manager::Make& Manager::Make::description(cstring s);
 * Specify the description of the application.
 */

/**
 * @fn Manager::Make& Manager::Make::free_argument(cstring s);
 * Specify description of the free arguments.
 */

/**
 * @fn Manager::Make& Manager::Make::help(void);
 * Ask the manager to create a switch option activated with "-h" or "--help"
 * that, when activated, display help message and the list of options.
 * Then it stops the application with an exit code of 1.
 */

} } // elm::option
