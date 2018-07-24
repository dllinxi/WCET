/*
 *	$Id$
 *	BoolOption class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-09, IRIT UPS.
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
#ifndef ELM_OPTION_BOOL_OPTION_H
#define ELM_OPTION_BOOL_OPTION_H

#include <elm/option/StandardOption.h>

namespace elm { namespace option {

// BoolOption class
class BoolOption: public StandardOption {
	bool val;
public:
	BoolOption(Manager& manager, char short_name, CString description,
		bool value = false);
	BoolOption(Manager& manager, CString long_name, CString description,
		bool value = false);
	BoolOption(Manager& manager, char short_name, CString long_name,
		CString description, bool value = false);
	inline bool get(void) const { return val; }
	inline void set(bool value) { val = value; };

	// Option overload
	virtual usage_t usage(void);
	virtual CString argDescription(void);
	virtual void process(String arg);
	
	// Operators
	inline operator bool(void) const { return get(); };
	inline bool operator*(void) const { return get(); }
	inline BoolOption& operator=(bool value) { val = value; return *this; }

	// deprecated
	inline bool value(void) const { return val; };
};

} } // elm::option

#endif // ELM_OPTION_BOOL_OPTION_H
