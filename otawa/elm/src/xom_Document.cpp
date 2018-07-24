/*
 *	$Id$
 *	xom::Document class interface
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

#include "xom_macros.h"
#include <elm/assert.h>
#include <elm/xom/Document.h>
#include <elm/xom/Element.h>
#include <elm/xom/NodeFactory.h>

#define NODE(p) ((xmlNodePtr)(p))
#define DOC(p) ((xmlDocPtr)(p))

namespace elm { namespace xom {

/**
 * @class Document Document.h "elm/xom.h"
 * The root object of an XML document.
 * @ingroup xom
 */


/**
 * Build a document from a reader node.
 * @param node	Parser node.
 */
Document::Document(void *node, NodeFactory *factory)
: ParentNode(node), fact(factory) {
	ASSERT(DOC(node)->type == XML_DOCUMENT_NODE);
	ASSERT(fact);
}


Document::Document(Document *document): ParentNode(0) {
	ASSERTP(0, "unsupported");
}


/**
 * Creates a new Document object with the specified root element.
 * @param root - the root element of this document
 * @warning		Fails if root already has a parent.
 */
Document::Document(Element *root_element):
	ParentNode(xmlNewDoc(BAD_CAST "1.0")),
	fact(&NodeFactory::default_factory)
{
	ASSERTP(root_element, "null root element");
	ASSERTP(!NODE(root_element->getNode())->parent, "root already has a parent");
	setRootElement(root_element);
}


/**
 */
Document::~Document(void) {
	xmlFreeDoc(DOC(node));
}

Node *Document::copy(void) {
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * Returns the base URI of this node as specified by XML Base, or the empty string if this is not known.
 * In most cases, this is the URL against which relative URLs in this node should be resolved.
 *
 * The base URI of a non-parent node is the base URI of the element containing the node. The base URI
 * of a document node is the URI from which the document was parsed, or which was set
 * by calling setBaseURI on on the document.
 *
 * The base URI of an element is determined as follows:
 * 	* If the element has an xml:base attribute, then the value of that attribute is converted
 * 	  from an IRI to a URI, absolutized if possible, and returned.
 *	* Otherwise, if any ancestor element of the element loaded from the same entity has an xml:base attribute,
 *	  then the value of that attribute from the nearest such ancestor is converted from an IRI to a URI,
 *	  absolutized if possible, and returned. xml:base attributes from other entities are not considered.
 *  * Otherwise, if setBaseURI() has been invoked on this element, then the URI most recently passed to that method is absolutized if possible and returned.
 *  * Otherwise, if the element comes from an externally parsed entity or the document entity, and the original base URI has not been changed by invoking setBaseURI(), then the URI of that entity is returned.
 *  * Otherwise, (the element was created by a constructor rather then being parsed from an existing document), the base URI of the nearest ancestor that does have a base URI is returned. If no ancestors have a base URI, then the empty string is returned.
 * Absolutization takes place as specified by the XML Base specification. However,
 * it is not always possible to absolutize a relative URI, in which case the empty string will be returned.
 * @return	the base URI of this node
 */
String Document::getBaseURI(void) {
	//return xmlNodeGetBase(DOC(node), NODE(node));
	if(!DOC(node)->URL)
		return "";
	else
		return DOC(node)->URL;
}


/**
 * Get the root element of the document.
 * @return	Root element or null.
 */
Element *Document::getRootElement(void) {
	xmlNodePtr root = xmlDocGetRootElement(DOC(node));
	if(!root)
		return 0;
	return (Element *)get(root);

}

String Document::getValue(void) {
	ASSERTP(0, "unsupported");
	return "";
}

void Document::replaceChild(Node *old_child, Node *new_child) {
	ASSERTP(0, "unsupported");
}

void Document::setBaseURI(String uri) {
	DOC(node)->URL = xmlCharStrdup(uri);
}

void Document::setRootElement(Element *root) {
	ASSERTP(root, "null root");
	xmlDocSetRootElement(DOC(node), NODE(root->getNode()));
}

String Document::toString(void) {
	ASSERTP(0, "unsupported");
	return "";
}

String Document::toXML(void) {
	ASSERTP(0, "unsupported");
	return "";
}

} } // elm::xom
