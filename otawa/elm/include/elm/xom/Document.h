/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/Document.h -- XOM Document class interface.
 */
#ifndef ELM_XOM_DOCUMENT_H
#define ELM_XOM_DOCUMENT_H

#include <elm/xom/ParentNode.h>

namespace elm { namespace xom {

// Predeclaration
class Builder;
class Element;
class NodeFactory;

// Document class
class Document: public ParentNode {
	friend class NodeFactory;
	friend class Node;
	NodeFactory *fact;
protected:
	Document(void *node, NodeFactory *fact);
public:
	Document(Document *document);
	Document(Element *root_element);
	virtual ~Document(void);

	virtual Node *copy(void);
	virtual String getBaseURI(void);
	// DocType	getDocType()
	virtual Element *getRootElement(void);
	virtual String getValue(void);
	virtual void replaceChild(Node *old_child, Node *new_child);
	virtual void setBaseURI(String uri);
	// void	setDocType(DocType doctype)
	virtual void setRootElement(Element *root);
	virtual String toString(void);
	virtual String toXML(void);
};

} } // elm::xom

#endif	// ELM_XOM_DOCUMENT_H
