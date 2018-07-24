/*
 *	$Id$
 *	Manager class interface
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
#ifndef ELM_OPTION_MANAGER_H
#define ELM_OPTION_MANAGER_H

#include <elm/ptr.h>
#include <elm/genstruct/SortedBinMap.h>
#include <elm/genstruct/Vector.h>
#include <elm/option/Option.h>
#include <elm/option/SwitchOption.h>
#include <elm/option/ValueOption.h>
#include <elm/util/Version.h>
#include <elm/util/VarArg.h>
#include <elm/util/MessageException.h>

namespace elm { namespace option {

// Manager
class Manager {
	friend class Option;
public:

	class Make {
		friend class Manager;
	public:
		inline Make(void): _help(false), _version_opt(false) { }
		inline Make(cstring program, Version version = Version::ZERO): _program(program), _version(version), _help(false), _version_opt(false) { }
		inline Make& author(cstring s) { _author = s; return *this; }
		inline Make& copyright(cstring s) { _copyright = s; return *this; }
		inline Make& description(cstring s) { _description = s; return *this; }
		inline Make& free_argument(cstring s) { _free_argument_description = s; return *this; }
		inline Make& help(void) { _help = true; return *this; }
		inline Make& version(void) { _version_opt = true; return *this; }
	private:
		cstring _program;
		Version _version;
		cstring _author;
		cstring _copyright;
		cstring _description;
		cstring _free_argument_description;
		bool _help;
		bool _version_opt;
	};

	// shortcut to make options
	inline SwitchOption::Make make_switch(void) { return SwitchOption::Make(*this); }
	template <class T> typename ValueOption<T>::Make make_value(void) { return typename ValueOption<T>::Make(*this); }

	typedef const char * const *argv_t;
	Manager(void);
	Manager(int tag, ...);
	Manager(const Make& maker);
	virtual ~Manager(void);
	void addOption(Option *option) throw(OptionException);
	void removeOption(Option *option);
	void parse(int argc, argv_t argv) throw(OptionException);
	int manage(int artc, argv_t argv);

	virtual void displayHelp(void);
	virtual void displayVersion(void);

	// accessors
	inline cstring getProgram(void) const { return info._program; }
	inline const Version& getVersion(void) const { return info._version; }
	inline cstring getAuthor(void) const { return info._author; }
	inline cstring getCopyright(void) const { return info._copyright; }
	inline cstring getDescription(void) const { return info._description; }
	inline cstring getFreeArgumentDescription(void) const { return info._free_argument_description; }

protected:
	Make info;
	virtual void process(String arg);
	virtual void configure(int tag, VarArg& args);
	virtual void run(void) throw(elm::Exception);

private:
	genstruct::Vector<Option *> options;
	void processOption(Option *option, int& i, int argc, argv_t argv, const char *earg);
	void addShort(char cmd, Option *option) throw(OptionException);
	void addLong(cstring cmd, Option *option) throw(OptionException);
	void addCommand(string cmd, Option *option) throw(OptionException);
	genstruct::SortedBinMap<char, Option *> shorts;
	genstruct::SortedBinMap<string, Option *> cmds;
	UniquePtr<SwitchOption> _help_opt, _version_opt;
};

} } //elm::option

#endif // ELM_OPTION_MANAGER_H
