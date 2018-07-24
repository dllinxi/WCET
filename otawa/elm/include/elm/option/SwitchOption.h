/*
 *	$Id$
 *	SwitchOption class interface
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
#ifndef OTAWA_OPTION_SWITCHOPTION_H_
#define OTAWA_OPTION_SWITCHOPTION_H_

//#include <elm/option/Manager.h>
#include "Option.h"

namespace elm { namespace option {

// SwitchOption class
class SwitchOption: public Option {
public:
	SwitchOption(Manager& man, int tag ...);
	SwitchOption(const Make& make);

	inline void set(bool value) { val = value; }
	inline bool get(void) const { return val; }

	// shortcuts
	inline operator bool(void) const { return get(); }
	inline bool operator*(void) const { return get(); }
	inline SwitchOption& operator=(bool value) { set(value); return *this; }

	// overload
	virtual usage_t usage(void);
	virtual CString argDescription(void);
	virtual void process(string arg);

protected:
	virtual void configure(Manager& manager, int tag, VarArg& args);

private:
	bool val;
};

typedef SwitchOption Switch;

} } // otawa::option

#endif /* OTAWA_OPTION_SWITCHOPTION_H_ */
