/*
 *	ASTLoader class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2003, IRIT UPS.
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
#ifndef OTAWA_AST_BLOCK_AST_H
#define OTAWA_AST_BLOCK_AST_H

#include <otawa/ast/AST.h>
#include <otawa/instruction.h>

namespace otawa { namespace ast {

// BlockAST class
class BlockAST: public AST {
	Inst *_block;
	t::uint32 _size;
protected:
	virtual ~BlockAST(void);
public:
	static Identifier<AST *> ID;
	BlockAST(Inst *block, t::uint32 size);
	inline Inst *block(void) const { return _block; };
	inline t::uint32 size(void) const { return _size; };

	// AST overload
	virtual Inst *first(void) { return _block; };
	virtual ast_kind_t kind(void) const { return AST_Block; };
	virtual BlockAST *toBlock(void) { return this; };
	virtual int countInstructions(void) const;
};
	
} } // otawa::ast

#endif // OTAWA_AST_BLOCK_AST_H
