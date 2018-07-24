/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/ParentNode.h -- XOM ParentNode class interface.
 */
#ifndef ELM_XOM_PARENT_NODE_H
#define ELM_XOM_PARENT_NODE_H

#include <elm/xom/Node.h>

namespace elm { namespace xom {

// ParentNode class
class ParentNode: public Node {
protected:
	inline ParentNode(void * node): Node(node) { };
	void internSetBaseURI(String URI);
public:
	virtual void appendChild(Node *child);
	virtual int	indexOf(Node *child);
	virtual void insertChild(Node *child, int position);
	virtual Node *removeChild(int position);
	virtual Node *removeChild(Node *child);
	virtual void replaceChild(Node *old_child, Node *new_child);
	virtual void setBaseURI(String URI) = 0;
	
	// Node overload
	virtual Node *getChild(int index);
	virtual int getChildCount(void);
};

} } // elm::xom

#endif	// ELM_XOM_PARENT_NODE_H
