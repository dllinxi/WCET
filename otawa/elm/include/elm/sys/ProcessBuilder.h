/*
 *	ProcessBuilder class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-12, IRIT UPS.
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
#ifndef ELM_SYS_PROCESS_BUILDER_H
#define ELM_SYS_PROCESS_BUILDER_H

#include <elm/genstruct/Vector.h>
#include <elm/sys/Path.h>
#include <elm/sys/System.h>
#include <elm/sys/SystemIO.h>
#include <elm/sys/Process.h>

namespace elm { namespace sys {

// ProcessBuilder class
class ProcessBuilder {
public:
	ProcessBuilder(sys::Path command);
	void addArgument(string argument);
	inline void add(string argument) { addArgument(argument); }
	inline ProcessBuilder& operator+(string argument) { addArgument(argument); return *this; }
	inline ProcessBuilder& operator+=(string argument) { addArgument(argument); return *this; }
	void setInput(SystemInStream *_in);
	void setOutput(SystemOutStream *_out);
	void setError(SystemOutStream *_out);
	void setNewSession(bool enabled);
	Process *run(void) throw(SystemException);

private:
	genstruct::Vector<string> args;
	SystemInStream *in;
	SystemOutStream *out, *err;
	bool new_session;
};

} } // elm::sys

#endif // ELM_SYS_PROCESS_BUILDER_H
