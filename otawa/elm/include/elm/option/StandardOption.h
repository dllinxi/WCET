/*
 *	$Id$
 *	StandardOption class interface
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
#ifndef ELM_OPTION_STANDARD_OPTION_H
#define ELM_OPTION_STANDARD_OPTION_H

//#include <elm/option/Manager.h>
#include <elm/option/Option.h>

namespace elm { namespace option {

// StandardOption class
class StandardOption: public Option {
public:
	
	// Constructors
	StandardOption(Manager& manager, int tag, ...);
	StandardOption(Manager& manager, int tag, VarArg& args);
	StandardOption(Manager& manager, char short_name, cstring description);
	StandardOption(Manager& manager, cstring long_name, cstring description);
	StandardOption(Manager& manager, char short_name, cstring long_name, cstring description);

	// CommandOption overload
	virtual char shortName(void);
	virtual cstring longName(void);
	virtual cstring description(void);

protected:
	virtual void configure(Manager& manager, int tag, VarArg& args);

private:
	cstring desc;
};

} } // elm::option

#endif // ELM_OPTION_STANDARD_OPTION_H
