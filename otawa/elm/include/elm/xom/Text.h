/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/Node.h -- XOM Text class interface.
 */
#ifndef ELM_XOM_TEXT_H
#define ELM_XOM_TEXT_H

#include <elm/xom/Node.h>

namespace elm { namespace xom {

// Text class
class Text: public Node {
	friend class NodeFactory;
protected:
	inline Text(void *node);
public:
	Text(String data);
	Text(const Text *text);
	void setValue(String data);
	String getText(void);

	// Node overload
	virtual Node *copy(void);
	virtual Node *getChild(int index);
	virtual int getChildCount(void);
	virtual String getValue(void);
	virtual String toXML(void);
};

// Text inlines
inline Text::Text(void *node): Node(node) {
}

} } // elm::xom

#endif /* ELM_XOM_TEXT_H */
