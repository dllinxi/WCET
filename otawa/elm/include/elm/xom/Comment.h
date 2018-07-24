/*
 *	$Id$
 *	xom::Comment class interface
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
#ifndef ELM_XOM_COMMMENT_H_
#define ELM_XOM_COMMMENT_H_

#include <elm/xom/Node.h>

namespace elm { namespace xom {

// Comment class
class Comment: public Node {
	friend class NodeFactory;
protected:
	Comment(void *node);
public:
	Comment(const Comment *comment);
	Comment(String data);
	void setValue(String data);
	String getText(void);

	// Node overload
	virtual Node *copy(void);
	virtual Node *getChild(int index);
	virtual int getChildCount(void);
	virtual String getValue(void);
	virtual String toXML(void);
};

} } // elm::xom

#endif /* ELM_XOM_H_ */
