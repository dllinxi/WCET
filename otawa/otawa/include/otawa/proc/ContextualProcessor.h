/*
 *	$Id$
 *	ContextualProcessor class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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
#ifndef OTAWA_PROC_CONTEXTUALPROCESSOR_H_
#define OTAWA_PROC_CONTEXTUALPROCESSOR_H_

#include <otawa/proc/CFGProcessor.h>

namespace otawa {

class Edge;

// ContextualProcessor class
class ContextualProcessor: public CFGProcessor {
public:
	static MetaRegistration reg;
	ContextualProcessor(cstring name, const Version& version);
	ContextualProcessor(AbstractRegistration &_reg = reg);

protected:

	// v1 methods
	virtual void enteringCall(
		WorkSpace *ws,
		CFG *cfg,
		BasicBlock *caller,
		BasicBlock *callee);
	virtual void leavingCall(WorkSpace *ws, CFG *cfg, BasicBlock *to);
	virtual void avoidingRecursive(
		WorkSpace *ws,
		CFG *cfg,
		BasicBlock *caller,
		BasicBlock *callee);

	// v2 methods
	virtual void enteringCall(WorkSpace *ws, CFG *cfg, Edge *edge);
	virtual void leavingCall(WorkSpace *ws, CFG *cfg, Edge *edge);
	virtual void avoidingRecursive(WorkSpace *ws, CFG *cfg, Edge *edge);	

	virtual void processCFG (WorkSpace *fw, CFG *cfg);
	virtual void processBB(WorkSpace *ws, CFG *cfg, BasicBlock *bb) = 0;
};

} // otawa

#endif /* OTAWA_PROC_CONTEXTUALPROCESSOR_H_ */

