/*
 *	$Id$
 *	xom::Element class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <elm/assert.h>
#include "xom_macros.h"
#include <elm/xom/Element.h>
#include <elm/xom/Text.h>
#include <elm/xom/Elements.h>
#include <elm/xom/Attribute.h>

namespace elm { namespace xom {

/**
 * @class Element
 * Represents an XML element.
 * @ingroup xom
 */


/**
 * Build an element with a namespace.
 * @param name	Qualified element name.
 * @param uri	URI of the namespace.
 */
void *Element::makeNS(String name, String uri) {
	int len;
	xmlNodePtr node = 0;
	const xmlChar *local_name = xmlSplitQName3(name, &len);
	if(!local_name) {
		node = xmlNewNode(NULL, name);
		NODE(node)->ns = xmlNewNs(node, uri, NULL);
	}
	else {
		node = xmlNewNode(NULL, local_name);
		char_t prefix[len + 1];
		memcpy(prefix, &name, len);
		prefix[len] = '\0';
		NODE(node)->ns = xmlNewNs(node, uri, prefix);
	}
	return node;
}


/**
 * Build an element from the parser representation.
 * @param node	Node in the parser representation.
 */
Element::Element(void *node): ParentNode(node) {
	ASSERT(NODE(node)->type == XML_ELEMENT_NODE);
}


/**
 * Creates a new element in no namespace.
 * @param name the name of the element
 * @throw IllegalNameException if name is not a legal XML 1.0 non-colonized name.
 */
Element::Element(String name): ParentNode(xmlNewNode(NULL, name)) {
	// !!TODO!! Check the validity of the name
}


/**
 * Creates a new element in a namespace.
 * @param name the qualified name of the element
 * @param uri the namespace URI of the element
 * @throw IllegalNameException if name is not a legal XML 1.0 name
 * @throw NamespaceConflictException if name's prefix cannot be used with uri
 * @throw MalformedURIException if uri is not an RFC 3986 absolute URI reference
 */
Element::Element(String name, String uri): ParentNode(makeNS(name, uri)) {
	// !!TODO!! Check the validity of the name
	// !!TODO!! Check the validity of the uri
}


/**
 * Creates a deep copy of an element. The copy is disconnected from the tree,
 * and does not have a parent.
 * @param element the element to copy
 */
Element::Element(const Element *element)
: ParentNode(xmlNewNode(NULL, NODE(element->getNode())->name)) {
	xmlNsPtr ns = NODE(element->getNode())->ns;
	NODE(node)->ns = xmlNewNs(NODE(node), ns->href, ns->prefix);
}


/**
 * Creates a very shallow copy of the element with the same name and namespace
 * URI, but no children, attributes, base URI, or namespace declaration. This
 * method is invoked as necessary by the copy method and the copy constructor.
 * @par
 *  Subclasses should override this method so that it returns an instance of the
 * subclass so that types are preserved when copying. This method should not add
 * any attributes, namespace declarations, or children to the shallow copy. Any
 * such items will be overwritten.
 * @return an empty element with the same name and namespace as this element
 */
Element	*Element::shallowCopy(void) {
	return new Element(this);
}


/**
 * Adds an attribute to this element, replacing any existing attribute with the
 * same local name and namespace URI.
 * @param attribute the attribute to add
 * @throw MultipleParentException if the attribute is already attached to an
 * element
 * @throw NamespaceConflictException - if the attribute's prefix is mapped to a
 * different namespace URI than the same prefix is mapped to by this element,
 * another attribute of this element, or an additional namespace declaration of
 * this element.
 */
void Element::addAttribute(Attribute *attribute) {
	ASSERTP(attribute, "null attribute");
	ASSERTP(!attribute->getNode(), "already added attribute");
	xmlAttrPtr attr;
	// !!TODO!! add support for namespace
	attr = xmlSetProp(NODE(node), attribute->getLocalName(), attribute->getValue());
	ASSERT(attr);
	attribute->setNode(attr);
}


/**
 * Declares a namespace prefix. This is only necessary when prefixes are used
 * in element content and attribute values, as in XSLT and the W3C XML Schema
 * Language. Do not use this method to declare prefixes for element and
 * attribute names.
 * @par
 * If you do redeclare a prefix that is already used by an element or attribute
 * name, the additional namespace is added if and only if the URI is the same.
 * Conflicting namespace declarations will throw an exception.
 *
 * @param prefix the prefix to declare.
 * @param uri the absolute URI reference to map the prefix to.
 * @param MalformedURIException if URI is not an RFC 3986 URI reference
 * @param IllegalNameException if prefix is not a legal XML non-colonized name
 * @param NamespaceConflictException if the mapping conflicts with an existing
 * element, attribute, or additional namespace declaration
 */
void Element::addNamespaceDeclaration(String prefix, String uri) {
	xmlNewNs(NODE(node), xom::String(uri), xom::String(prefix));
}


/**
 * Converts a string to a text node and appends that node to the children of
 * this node.
 * @param text String to add to this node.
 * @throw IllegalAddException if this node cannot have children of this type.
 */
void Element::appendChild(String text) {
	xmlNodeAddContent(NODE(node), text);
}


/**
 */
Node *Element::copy(void) {
	// !!TODO!!
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * Selects an attribute by index. The index is purely for convenience and has no
 * particular meaning. In particular, it is not necessarily the position of this
 * attribute in the original document from which this Element object was read.
 * As with most lists in Java, attributes are numbered from 0 to one less than
 * the length of the list.
 * @par
 * In general, you should not add attributes to or remove attributes from the
 * list while iterating across it. Doing so will change the indexes of the other
 * attributes in the list. it is, however, safe to remove an attribute from
 * either end of the list (0 or getAttributeCount()-1) until there are no
 * attributes left.
 * @param index the attribute to return.
 * @return the indexth attribute of this element
 */
Attribute *Element::getAttribute(int index) {
	int cnt = 0;
	for(struct _xmlAttr *attr = NODE(node)->properties; attr; attr = attr->next) {
		if(cnt == index)
			return static_cast<Attribute *>(get(attr));
		cnt++;
	}
	ASSERTP(false, "attribute index out of element bound");
	return 0;
}


/**
 * Returns the attribute with the specified name in no namespace, or null if
 * this element does not have an attribute with that name in no namespace.
 * @param name the name of the attribute .
 * @return the attribute of this element with the specified name.
 */
Attribute *Element::getAttribute(String name) {
	xmlAttr *attr = xmlHasProp(NODE(node), name);
	if(!attr)
		return 0;
	else
		return static_cast<Attribute *>(get(attr));
}


/**
 * Returns the attribute with the specified name and namespace URI, or null if
 * this element does not have an attribute with that name in that namespace.
 * @param localName the local name of the attribute.
 * @param namespaceURI the namespace of the attribute.
 * @return the attribute of this element with the specified name and namespace.
 */
Attribute *Element::getAttribute(String localName, String ns) {
	xmlAttr *attr = xmlHasNsProp(NODE(node), localName, ns);
	if(!attr)
		return 0;
	else
		return static_cast<Attribute *>(get(attr));
}


/**
 * Returns the number of attributes of this Element, not counting namespace
 * declarations. This is always a non-negative number.
 * @return the number of attributes in the container.
 */
int	Element::getAttributeCount(void) {
	int cnt = 0;
	for(struct _xmlAttr *attr = NODE(node)->properties; attr; attr = attr->next)
		cnt++;
	return cnt;
}


/**
 * Returns the value of the attribute with the specified name in no namespace,
 * or none if this element does not have an attribute with that name.
 * @param name the name of the attribute.
 * @return the value of the attribute of this element with the specified name.
 */
Option<String> Element::getAttributeValue(String name) {
	xmlChar *result = xmlGetProp(NODE(node), name);
	if(!result)
		return none;
	else
		return some(String(result));
}


/**
 * Returns the value of the attribute with the specified name and namespace URI,
 * or null if this element does not have such an attribute.
 * @param localName the name of the attribute.
 * @param namespaceURI the namespace of the attribute.
 * @return the value of the attribute of this element with the specified name
 * and namespace.
 */
Option<String> Element::getAttributeValue(String localName, String ns) {
	xmlChar *result = xmlGetNsProp(NODE(node), localName, ns);
	if(!result)
		return none;
	else
		return some(String(result));
}


/**
 * Returns a list of all the child elements of this element in document order.
 * @return a comatose list containing all child elements of this element.
 */
Elements *Element::getChildElements(void) {
	Elements *elems = new Elements();
	for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next)
		if(cur->type == XML_ELEMENT_NODE)
			elems->elems.add((Element *)make(cur));
	return elems;
}


/**
 * Returns a list of the child elements of this element with the specified name
 * in no namespace. The elements returned are in document order.
 * @param name The name of the elements included in the list .
 * @return A comatose list containing the child elements of this element with
 * the specified name.
 */
Elements *Element::getChildElements(String name) {
	Elements *elems = new Elements;
	for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next)
		if(cur->type == XML_ELEMENT_NODE
		&& name == String(cur->name))
			elems->elems.add((Element *)make(cur));
	return elems;
}


/**
 * Returns a list of the immediate child elements of this element with the
 * specified local name and namespace URI. Passing the empty string or null as
 * the local name returns all elements in the specified namespace. Passing null
 * or the empty string as the namespace URI returns elements with the specified
 * name in no namespace. The elements returned are in document order.
 * @param localName The name of the elements included in the list.
 * @param namespaceURI The namespace URI of the elements included in the list.
 * @return A comatose list containing the child elements of this element with
 * the specified name in the specified namespace.
 */
Elements *Element::getChildElements(String localName, String ns) {
	Elements *elems = new Elements;
	for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next)
		if(cur->type == XML_ELEMENT_NODE
		&& localName == String(cur->name)
		&& ns == String(cur->ns->href))
			elems->elems.add((Element *)make(cur));
	return elems;
}


/**
 * Returns the first child element with the specified name in no namespace. If
 * there is no such element, it returns null.
 * @param name The name of the element to return.
 * @return The first child element with the specified local name in no namespace
 * or null if there is no such element.
 */
Element	*Element::getFirstChildElement(String name) {
	for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next)
		if(cur->type == XML_ELEMENT_NODE
		&& name == String(cur->name))
			return (Element *)make(cur);
	return 0;
}


/**
 * Returns the first child element with the specified local name and namespace
 * URI. If there is no such element, it returns null.
 * @param localName The local name of the element to return.
 * @param namespaceURI The namespace URI of the element to return.
 * @return The first child with the specified local name in the specified
 * namespace, or null if there is no such element.
 */
Element	*Element::getFirstChildElement(String localName, String ns) {
	for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next)
		if(cur->type == XML_ELEMENT_NODE
		&& localName == String(cur->name)
		&& ns == String(cur->ns->href))
			return (Element *)make(cur);
	return 0;
}


/**
 * Returns the local name of this element, not including the namespace prefix
 * or colon.
 * @return The local name of this element.
 */
String Element::getLocalName(void) {
	return NODE(node)->name;
}


/**
 * Returns the number of namespace declarations on this element. This counts the
 * namespace of the element itself (which may be the empty string), the
 * namespace of each attribute, and each namespace added by
 * addNamespaceDeclaration. However, prefixes used multiple times are only
 * counted once; and the xml prefix used for xml:base, xml:lang, and xml:space
 * is not counted even if one of these attributes is present on the element.
 * @par
 * The return value is almost always positive. It can be zero if and only if the
 * element itself has the prefix xml; e.g. <xml:space />. This is not endorsed
 * by the XML specification. The prefix xml is reserved for use by the W3C,
 * which has only used it for attributes to date. You really shouldn't do this.
 * Nonetheless, this is not malformed so XOM allows it.
 * @return The number of namespaces declared by this element.
 */
int	Element::getNamespaceDeclarationCount(void) {
	int cnt = 0;
	for(xmlNs *ns = NODE(node)->nsDef; ns; ns = ns->next)
		cnt++;
	return cnt;
}


/**
 * Returns the prefix of this element, or the empty string if this element does
 * not have a prefix.
 * @return The prefix of this element.
 */
String Element::getNamespacePrefix(void) {
	if(NODE(node)->ns && NODE(node)->ns->prefix)
		return NODE(node)->ns->prefix;
	else
		return "";
}


/**
 * Returns the indexth namespace prefix declared on this element. Namespaces
 * inherited from ancestors are not included. The index is purely for
 * convenience, and has no meaning in itself. This includes the namespaces of
 * the element name and of all attributes' names (except for those with the
 * prefix xml such as xml:space) as well as additional declarations made for
 * attribute values and element content. However, prefixes used multiple times
 * (e.g. on several attribute values) are only reported once. The default
 * namespace is reported with an empty string prefix if present. Like most lists
 * in Java, the first prefix is at index 0.
 * @par
 * If the namespaces on the element change for any reason (adding or removing an
 * attribute in a namespace, adding or removing a namespace declaration,
 * changing the prefix of an element, etc.) then then this method may skip or
 * repeat prefixes. Don't change the prefixes of an element while iterating
 * across them.
 * @param index The prefix to return.
 * @return The prefix.
 **/
String Element::getNamespacePrefix(int index) {
	for(xmlNs *ns = NODE(node)->nsDef; ns; ns = ns->next) {
		if(index)
			index--;
		else if(ns->prefix)
			return ns->prefix;
		else
			return "";
	}
	ASSERTP(false, "namespace index out of bounds");
	return "";
}


/**
 * Returns the namespace URI of this element, or the empty string if this
 * element is not in a namespace.
 * @return The namespace URI of this element.
 */
String Element::getNamespaceURI(void) {
	if(NODE(node)->ns)
		return NODE(node)->ns->href;
	else
		return "";
}


/**
 * Returns the namespace URI mapped to the specified prefix within this element.
 * Returns null if this prefix is not associated with a URI.
 * @param prefix The namespace prefix whose URI is desired.
 * @return The namespace URI mapped to prefix.
 */
String Element::getNamespaceURI(String prefix) {
	for(xmlNs *ns = NODE(node)->nsDef; ns; ns = ns->next)
		if(prefix == ns->prefix)
			return ns->href;
	return "";
}


/**
 * Returns the complete name of this element, including the namespace prefix if
 * this element has one.
 * @return The qualified name of this element.
 * @warning	The string allocated by this call must be fried !
 */
String Element::getQualifiedName(void) {
	typedef const char *s;
	xmlNode *element = NODE(node);
	if(!element->ns || !element->ns->prefix)
		return strdup(s(element->name));
	else {
		char *buf = (char *)malloc(strlen(s(element->name)) + strlen(s(element->ns->prefix)) + 2);
		strcpy(buf, s(element->ns->prefix));
		strcat(buf, ":");
		strcat(buf, s(element->name));
		return buf;
	}
}


/**
 */
String Element::getValue(void) {
	return internGetValue();
}


/**
 * Converts a string to a text node and inserts that node at the specified
 * position.
 * @param position Where to insert the child.
 * @param text The string to convert to a text node and insert.
 */
void Element::insertChild(String text, int position) {
	ASSERTP(position >= 0, "position must be positive");
	ParentNode::insertChild(new Text(text), position);
}


/**
 * Insert a node at the given position.
 * @param position Where to insert the child.
 * @param node Node to insert.
 */
void Element::insertChild(Node *node, int position) {
	ASSERTP(position >= 0, "position must be positive");
	ParentNode::insertChild(node, position);
}


/**
 * Removes an attribute from this element.
 * @param attribute The attribute to remove.
 * @return The attribute that was removed.
 * @throw NoSuchAttributeException If this element is not the parent of
 * attribute.
 */
Attribute *Element::removeAttribute(Attribute *attribute) {
	// !!TODO!! Check if the attribute is owned by the current element.
	ASSERTP(0, "unsupported");
	return 0;
}

Nodes *Element::removeChildren(void) {
	ASSERTP(0, "unsupported");
	return 0;
}

void Element::removeNamespaceDeclaration(String prefix) {
	ASSERTP(0, "unsupported");
}

void Element::setBaseURI(String uri) {
	ASSERTP(0, "unsupported");
}

void Element::setLocalName(String local_name) {
	ASSERTP(0, "unsupported");
}

void Element::setNamespacePrefix(String prefix) {
	ASSERTP(0, "unsupported");
}

void Element::setNamespaceURI(String uri) {
	ASSERTP(0, "unsupported");
}

String Element::toString(void) {
	ASSERTP(0, "unsupported");
	return "";
}

String Element::toXML(void) {
	ASSERTP(0, "unsupported");
	return "";
}


void Element::appendChild(Node *node) {
	return ParentNode::appendChild(node);
}

} } // elm::xom
