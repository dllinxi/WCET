/*
 *	$Id$
 *	xom::NodeFactory class interface
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
#include <libxml/tree.h>
#include <elm/xom/NodeFactory.h>
#include <elm/xom/Document.h>
#include <elm/xom/Element.h>
#include <elm/xom/Text.h>
#include <elm/xom/Nodes.h>
#include <elm/xom/Comment.h>

namespace elm { namespace xom {

/**
 * @class NodeFactory
 * Used for building nodes during a load. Builders use a NodeFactory object to construct
 * each Node object (Element, Text, Attribute, etc.) they add to the tree.
 * The default implementation simply calls the relevant constructor,
 * stuffs the resulting Node object in a length one Nodes object, and returns it.
 *
 * Subclassing this class allows builders to produce instance of subclasses
 * (for example, HTMLElement) instead of the base classes.
 *
 * Subclasses can also filter content while building. For example, namespaces could be added
 * to or changed on all elements. Comments could be deleted. Processing instructions can
 * be changed into elements. An xinclude:include element could be replaced with the content
 * it references. All such changes must be consistent with the usual rules of well-formedness.
 * For example, the makeDocType() method should not return a list containing two DocType
 * objects because an XML document can have at most one document type declaration.
 * Nor should it return a list containing an element, because an element cannot appear
 * in a document prolog. However, it could return a list containing any number of comments
 * and processing instructions, and not more than one DocType object.
 * @ingroup xom
 */

/**
 * Default node factory.
 */
NodeFactory NodeFactory::default_factory;


/**
 * Build an attribute from an XML node.
 * @param node	XML node.
 * @return		Built attribute.
 */
Attribute *NodeFactory::makeAttribute(void *node) {
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * Build a comment from an XML node.
 * @param node	XML node.
 * @return		Built comment.
 */
Comment *NodeFactory::makeComment(void *node) {
	return new Comment(node);
}


/**
 * Build a document from an input stream.
 * @param node	Node used by the parser.
 */
Document *NodeFactory::makeDocument(void *node) {
	return new Document(node, this);
}


/**
 * Build a comment from an XML node.
 * @param node	XML node.
 * @return		Built comment.
 */
ProcessingInstruction *NodeFactory::makeProcessingInstruction(void *node) {
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * Build an element from its low-level reference.
 * @param node	Low-level node reference.
 */
Element	*NodeFactory::makeElement(void *node) {
	return new Element(node);
}


/**
 * Build a text from its low-level node reference.
 * @param node	Low-level node reference.
 */
Text *NodeFactory::makeText(void *node) {
	return new Text(node);
}


/**
 */
NodeFactory::~NodeFactory(void) {
}


/**
 * Creates a new Document object. The root element of this document is initially set to
 * <root xmlns="http://www.xom.nu/fakeRoot"/>. This is only temporary. As soon as the real
 * root element's start-tag is read, this element is replaced by the real root. This fake root
 * should never be exposed.
 *
 * The builder calls this method at the beginning of each document, before it calls any other method
 * in this class. Thus this is a useful place to perform per-document initialization tasks.
 *
 * Subclasses may change the root element, content, or other characteristics of the document returned.
 * However, this method must not return null or the builder will throw a ParsingException.
 *
 * @return the newly created Document
 */
Document *NodeFactory::startMakingDocument(void) {
	xmlDoc *doc = xmlNewDoc((xmlChar *)"1.0");
	xmlNode *node = xmlNewNode(0, (xmlChar *)"root");
	xmlNs *ns = xmlNewNs(node, (xmlChar *)"http://www.xom.nu/fakeRoot", 0);
	node->ns = ns;
	xmlDocSetRootElement(doc, node);
	return new Document(doc, this);
}


Element *NodeFactory::startMakingElement(void) {
	ASSERTP(false, "unsupported");
	return 0;
}


Nodes NodeFactory::makeAttribute(void) {
	ASSERTP(false, "unsupported");
	return Nodes();
}


Nodes NodeFactory::makeComment(void) {
	ASSERTP(false, "unsupported");
	return Nodes();
}


Nodes NodeFactory::makeDocType(xom::String rootElementName, xom::String publicID, xom::String systemID) {
	ASSERTP(false, "unsupported");
	return Nodes();
}


Nodes NodeFactory::makeProcessingInstruction(void) {
	ASSERTP(false, "unsupported");
	return Nodes();
}


Element	*NodeFactory::makeRootElement(void) {
	ASSERTP(false, "unsupported");
	return 0;
}


Nodes NodeFactory::makeText(void) {
	ASSERTP(false, "unsupported");
	return Nodes();
}


Nodes NodeFactory::finishMakingElement(Element *element) {
	ASSERTP(false, "unsupported");
	return Nodes();
}


/**
 * Signals the end of a document. The default implementation of this method does nothing.
 * The builder does not call this method if an exception is thrown while building a document.
 *
 *@param document	the completed Document
 */
void NodeFactory::finishMakingDocument(Document *document) {
	// do nothing
}

} } // elm::xom
