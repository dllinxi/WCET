/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/Element.h -- XOM Element class interface.
 */
#ifndef ELM_XOM_ELEMENT_H
#define ELM_XOM_ELEMENT_H

#include <elm/util/Option.h>
#include <elm/xom/ParentNode.h>

namespace elm { namespace xom {

// Predeclaration
class Attribute;
class Elements;
class Nodes;

// Document class
class Element: public ParentNode {
protected:
	friend class NodeFactory;
	Element(void *node);
	virtual Element	*shallowCopy(void);
	static void *makeNS(String name, String uri);
public:
	Element(String name);
	Element(String name, String uri);
	Element(const Element *element);

	virtual void addAttribute(Attribute *attribute);
	virtual void addNamespaceDeclaration(String prefix, String uri);
	virtual void appendChild(String text);
	virtual void appendChild(Node *node);
	virtual Node *copy(void);
	virtual Attribute *getAttribute(int index);
	virtual Attribute *getAttribute(String name);
	virtual Attribute *getAttribute(String localName, String ns);
	virtual int	getAttributeCount(void);
	virtual Option<String> getAttributeValue(String name);
	virtual Option<String> getAttributeValue(String localName, String ns);
	virtual Elements *getChildElements(void);
	virtual Elements *getChildElements(String name);
	virtual Elements *getChildElements(String localName, String ns);
	virtual Element	*getFirstChildElement(String name);
	virtual Element	*getFirstChildElement(String localName, String ns);
	virtual String getLocalName(void);
	virtual int	getNamespaceDeclarationCount(void);
	virtual String getNamespacePrefix(void);
	virtual String getNamespacePrefix(int index);
	virtual String getNamespaceURI(void);
	virtual String getNamespaceURI(String prefix);
	virtual String getQualifiedName(void);
	virtual String getValue(void);
	virtual void insertChild(String text, int position);
	virtual void insertChild(Node *node, int position);
	virtual Attribute *removeAttribute(Attribute *attribute);
	virtual Nodes *removeChildren(void);
	virtual void removeNamespaceDeclaration(String prefix);
	virtual void setBaseURI(String uri);
	virtual void setLocalName(String local_name);
	virtual void setNamespacePrefix(String prefix);
	virtual void setNamespaceURI(String uri);
	virtual String toString(void);
	virtual String toXML(void);
};

} } // elm::xom

#endif // ELM_XOM_ELEMENT_H
