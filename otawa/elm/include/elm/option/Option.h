/*
 *	$Id$
 *	Option class interface
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
#ifndef ELM_OPTION_OPTION_H
#define ELM_OPTION_OPTION_H

#include <elm/util/VarArg.h>
#include <elm/string.h>
#include <elm/io.h>
#include <elm/genstruct/Vector.h>

namespace elm { namespace option {

class Manager;

// configuration tags
const int end			= 0,
		  program		= 1,	// const char *
		  version		= 2,	// Version *
		  author		= 3,	// const char *
		  copyright		= 4,	// const char *
		  description	= 5,	// const char *
		  help			= 5,	// alias for description
		  free_arg		= 6,	// const char *
		  cmd			= 7,	// const char *
		  short_cmd		= 8,	// char
		  long_cmd		= 9,	// const char *
		  def			= 10,	// option dependent
		  require		= 12,	// none
		  optional		= 13,	// none
		  arg_desc		= 14;	// const char *


// OptionException exception
class OptionException: public MessageException {
public:
	OptionException(const String& message);
};


// Argument usage
typedef enum usage_t {
	arg_none,
	arg_optional,
	arg_required
} usage_t;


// Option class
class Option {
public:

	class Make {
		friend class Option;
	public:
		inline Make(Manager *m): man(*m) { }
		inline Make(Manager& m): man(m) { }
		inline Make& cmd(string cmd) { cmds.add(cmd); return *this; }
		inline Make& description(cstring desc) { _desc = desc; return *this; }
	protected:
		Manager& man;
		genstruct::Vector<string> cmds;
		cstring _desc;
	};

	inline Option(void) { }
	Option(const Make& make);
	virtual ~Option(void) { }
	void output(io::Output& out);
	virtual cstring description(void);
	virtual usage_t usage(void) = 0;
	virtual cstring argDescription(void) = 0;
	virtual void process(String arg) = 0;

	// deprecated
	virtual char shortName(void);
	virtual CString longName(void);

protected:
	virtual void configure(Manager& manager, int tag, VarArg& args);
	void init(Manager& manager, int tag, ...);
	void init(Manager& manager, int tag, VarArg& args);
	cstring desc;
};

// Inlines
inline io::Output& operator<<(io::Output& out, Option& opt) { opt.output(out); return out; };

} } // elm::option

#endif // ELM_OPTION_OPTION_H
