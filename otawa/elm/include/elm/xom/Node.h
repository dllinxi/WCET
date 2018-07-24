/*
 *	$Id$
 *	xom::Comment class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-09, IRIT UPS.
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
#ifndef ELM_XOM_NODE_H
#define ELM_XOM_NODE_H

#include <elm/xom/String.h>

namespace elm { namespace xom {

// Predeclaration
class Comment;
class Node;
class Document;
class ParentNode;
class NodeFactory;
class Nodes;
class XPathContext;
class Builder;

// Node class
class Node {
	friend class Builder;
	friend class Elements;
	friend class XIncluder;

public:
	typedef enum kind_t {
		NONE = 0,
		ELEMENT,
		DOCUMENT,
		TEXT,
		COMMENT,
		ATTRIBUTE,
		PROCESSING_INSTRUCTION,
		DOCTYPE,
		NAMESPACE
	} kind_t;

protected:
	friend class ParentNode;
	void *node;
	Node(void *_node);
	void setNode(void *_node);
	Node *make(void *node);
	inline Node *get(void *node);
	Node *internCopy(void);
	Node *internGetChild(int index);
	int internGetChildCount(void);
	String internGetValue(void);
	String internToXML(void);
	static void freeNode(void *node);
public:
	virtual ~Node(void);
	inline void *getNode(void) const;

	kind_t kind(void) const;
	virtual Node *copy(void) = 0;
	void detach(void);
	bool equals(const Node * node) const;
	virtual String getBaseURI(void);
	virtual Node *getChild(int index) = 0;
	virtual int getChildCount(void) = 0;
	virtual Document *getDocument(void);
	virtual ParentNode *getParent(void);
	virtual String getValue(void) = 0;
	virtual Nodes *query(const String& xpath);
	virtual Nodes *query(const String& xpath, XPathContext *context);
	virtual String toXML(void) = 0;

	// Non-XOM methods
	int line(void) const;
};


// Node inlines
inline void *Node::getNode(void) const {
	return node;
}

} } // elm::xom

#endif // ELM_XOM_NODE_H
