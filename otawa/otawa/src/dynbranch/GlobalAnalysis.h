/*
 *	GlobalAnalysis class interface
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
#ifndef OTAWA_DYNBRANCH_GLOBAL_ANALYSIS_FEATURE_H
#define OTAWA_DYNBRANCH_GLOBAL_ANALYSIS_FEATURE_H

#include <otawa/otawa.h>
#include <otawa/ipet.h>
#include <otawa/cfg.h>
#include <otawa/cfg/features.h>
#include <otawa/prog/sem.h>

#include "State.h"

#define TIME_NB_EXEC_GLOBAL 1000

namespace otawa { namespace dynbranch {

using namespace otawa ;
using namespace otawa::util ;

class GlobalAnalysis: public Processor {
public:
	static p::declare reg;
	GlobalAnalysis(p::declare& r = reg);
protected:
	void processWorkSpace(WorkSpace*) ;
	void configure(const PropList &props) ;
private:
	global::State entry ;
	bool time ;
};

extern p::feature GLOBAL_ANALYSIS_FEATURE;
extern Identifier<bool> TIME;

extern Identifier<global::State> GLOBAL_STATE_IN;
extern Identifier<global::State> GLOBAL_STATE_OUT;
extern Identifier<global::State> GLOBAL_STATE_ENTRY;

} } // otawa::dynbranch

#endif	// OTAWA_DYNBRANCH_GLOBAL_ANALYSIS_FEATURE_H

