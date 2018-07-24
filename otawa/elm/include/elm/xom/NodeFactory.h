/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/NodeFactory.h -- XOM NodeFactory class interface.
 */
#ifndef ELM_XOM_NODEFACTORY_H
#define ELM_XOM_NODEFACTORY_H

#include <elm/xom/String.h>

namespace elm { namespace xom {

// Predeclaration of classes
class Attribute;
class Builder;
class Comment;
class Document;
class Element;
class Node;
class Nodes;
class ProcessingInstruction;
class Text;


// NodeFactory class
class NodeFactory {
	friend class Builder;
	friend class Node;
public:
	static NodeFactory default_factory;

	virtual ~NodeFactory(void);
	virtual Document *startMakingDocument(void);
	virtual Element *startMakingElement(void);
	virtual Nodes makeAttribute(void);
	virtual Nodes makeComment(void);
	virtual Nodes makeDocType(String rootElementName, String publicID, String systemID);
	virtual Nodes makeProcessingInstruction(void);
	virtual Element	*makeRootElement(void);
	virtual Nodes makeText(void);
	virtual Nodes finishMakingElement(Element *element);
	virtual void finishMakingDocument(Document *document);

	virtual Attribute *makeAttribute(void *node);
	virtual Comment *makeComment(void *node);
	virtual Document *makeDocument(void *node);
	virtual ProcessingInstruction *makeProcessingInstruction(void *node);
	virtual Element	*makeElement(void *node);
	virtual Text *makeText(void *node);
};

} } // elm::xom

#endif // ELM_XOM_NODEFACTORY_H
