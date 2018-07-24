/*
 *	FunProcessor class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005, IRIT UPS.
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
#ifndef OTAWA_PROC_FUNPROCESSOR_H
#define OTAWA_PROC_FUNPROCESSOR_H

#include <otawa/proc/Processor.h>
#include <otawa/ast/FunAST.h>

namespace otawa { namespace ast {
	
// Processor class
class FunProcessor: public Processor {
public:
	static p::declare reg;
	FunProcessor(p::declare& r = reg);
	~FunProcessor(void);

protected:
	virtual void processFun(WorkSpace *ws, FunAST *fa) = 0;
	virtual void processWorkSpace(WorkSpace *ws);
};

} }		// otawa::ast

#endif // OTAWA_PROC_FUNPROCESSOR_H
