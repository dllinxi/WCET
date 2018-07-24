/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/Exception.h -- XOM Exception class interface.
 */
#ifndef ELM_XOM_EXCEPTION_H
#define ELM_XOM_EXCEPTION_H

#include <elm/xom/Node.h>
#include <elm/util/MessageException.h>

namespace elm { namespace xom {

// Exception class
class Exception: public MessageException {
	Node *_node;
public:
	Exception(Node *node, const String& message);
	inline Node *node(void) const { return _node; };
};

} } // elm::xom

#endif // ELM_XOM_XOM_EXCEPTION_H
