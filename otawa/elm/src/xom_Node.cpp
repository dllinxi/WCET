/*
 *	$Id$
 *	xom::Node class interface
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
#include <elm/xom/Node.h>
#include <elm/xom/Document.h>
#include <elm/xom/NodeFactory.h>
#include <elm/xom/Element.h>
#include <elm/xom/Text.h>
#include <elm/xom/Attribute.h>
#include <elm/xom/Nodes.h>
#include <elm/xom/Comment.h>

namespace elm { namespace xom {

/**
 * @defgroup xom XOM Module
 *
 * @code
 * #include <elm/xom.h>
 * @endcode
 *
 * This module support an implementation of the XOM API (http://www.xom.nu/)
 * for XML adapted to C++. All classes of this modules are contained in the
 * elm::xom namespace.
 * @par
 * C++ specificities includes :
 * @li the strings are encoded in UTF-8 and the @ref elm::xom::String class
 * provides facilities to handle them.
 * @li some methods builds new string and some only uses fixed string that does
 * not need to be fried. Read carefully the documentation to avoid memory leaks.
 * @li as the method getValue() may in some cases build a new string,
 * @ref elm::xom::Text provides a more efficient method, getText(), to get its
 * content without need of memory management.
 *
 * @internal XOM module is implemented on top of C library libxml2. As a
 * consequence, the used texts are encoded in UTF-8.
 */


// UnsupportedNode class
class UnsupportedNode: public Node {
public:
	inline UnsupportedNode(void *node): Node(node) {
	};
	virtual Node *copy(void) {
		return new UnsupportedNode(node);
	};
	Node *getChild(int index) {
		return internGetChild(index);
	}
	virtual int getChildCount(void) {
		return internGetChildCount();
	}
	virtual String getValue(void) {
		return internGetValue();
	}
	virtual String toXML(void) {
		return internToXML();
	};
};


/**
 * @class Node
 * The base class of nodes of a XOM XML tree.
 * Refer to http://www.xom.nu/ for more information.
 * @ingroup xom
 */


/**
 * Free a tree of nodes.
 */
void Node::freeNode(void *_node) {
	xmlNodePtr node = NODE(_node);
	if(node->_private)
		delete (Node *)node->_private;
	else {
		for(xmlNodePtr cur = node->children, next; cur; cur = next) {
			next = cur->next;
			freeNode(cur);
		}
		if(node->parent)
			xmlUnlinkNode(node);
		xmlFreeNode(node);
	}
}


/**
 */
Node::~Node(void) {
}


/**
 * Set the actual libxml node.
 * @param _node		Node to set.
 */
void Node::setNode(void *_node) {
	ASSERT(_node);
	if(node)
		NODE(node)->_private = 0;
	node = _node;
	NODE(node)->_private = this;
}


// Node inlines
Node::Node(void *_node): node(0) {
	if(_node)
		setNode(_node);
}



/**
 * Build the XOM tree from the parser nodes.
 * @param node	Parser node.
 * @return		Matching XOM node if any.
 */
Node *Node::make(void *node) {
	NodeFactory *fact = (((Document *)(NODE(node)->doc->_private)))->fact;
	Node *result;
	switch(NODE(node)->type) {
	case XML_ELEMENT_NODE:
		result = fact->makeElement(node);
		break;
	case XML_TEXT_NODE:
		result = fact->makeText(node);
		break;
	case XML_ATTRIBUTE_NODE:
		result = new Attribute(node);
		break;
	case XML_COMMENT_NODE:
		result = fact->makeComment(node);
		break;
	default:
		result = new UnsupportedNode(node);
		break;
	}
	NODE(node)->_private = result;
	return result;
}


/**
 * Get the kind of node.
 * @return	Kind of node.
 */
Node::kind_t Node::kind(void) const {
	static const kind_t kinds[] = {
		NONE,					// unused
		ELEMENT,				// XML_ELEMENT_NODE = 1
    	ATTRIBUTE,				// XML_ATTRIBUTE_NODE = 2
    	TEXT,					// XML_TEXT_NODE = 3
   		TEXT,					// XML_CDATA_SECTION_NODE = 4
    	NONE,					// XML_ENTITY_REF_NODE = 5
    	NONE,					// XML_ENTITY_NODE = 6
		PROCESSING_INSTRUCTION,	// XML_PI_NODE = 7
    	COMMENT,				// XML_COMMENT_NODE = 8
		DOCUMENT,				// XML_DOCUMENT_NODE = 9
    	NONE,					// XML_DOCUMENT_TYPE_NODE = 10
    	NONE,					// XML_DOCUMENT_FRAG_NODE = 11
    	NONE,					// XML_NOTATION_NODE = 12
    	NONE,					// XML_HTML_DOCUMENT_NODE = 13
    	NONE,					// XML_DTD_NODE = 14
    	NONE,					// XML_ELEMENT_DECL = 15
    	NONE,					// XML_ATTRIBUTE_DECL = 16
    	NONE,					// XML_ENTITY_DECL = 17
    	NONE,					// XML_NAMESPACE_DECL = 18
    	NONE,					// XML_XINCLUDE_START = 19
    	NONE,					// XML_XINCLUDE_END = 20
    	NONE					// XML_DOCB_DOCUMENT_NODE
	};

	ASSERT(NODE(node)->type < (int)(sizeof(kinds) / sizeof(kind_t)));
	return kinds[NODE(node)->type];
}


/**
 * Get the parent node.
 * @return	Parent node.
 */
ParentNode *Node::getParent(void) {
	return (ParentNode *)make(NODE(node)->parent);
}


/**
 * @fn String Node::getValue(void);
 * Get the node as a string with markup removed.
 * @return	Text value of the node. The returned string must be fried by
 * the caller.
 */


/**
 */
String Node::internGetValue(void) {
	return xmlNodeGetContent(NODE(node));
}


/**
 * Detach the current node from its parent.
 */
void Node::detach(void) {
	xmlUnlinkNode(NODE(node));
}


/**
 * Test if the current and the passed nodes are equals, that is, they are the
 * same object.
 */
bool Node::equals(const Node * node) const {
	return this == node;
}


/**
 * Returns the base URI of this node as specified by XML Base, or the empty
 * string if this is not known. In most cases, this is the URL against which
 * relative URLs in this node should be resolved.
 * @par
 * The base URI of a non-parent node is the base URI of the element containing
 * the node. The base URI of a document node is the URI from which the document
 * was parsed, or which was set by calling setBaseURI on on the document.
 * @par
 * The base URI of an element is determined as follows:
 * @par
 * @li If the element has an xml:base attribute, then the value of that
 * attribute is converted from an IRI to a URI, absolutized if possible, and
 * returned.
 * @li Otherwise, if any ancestor element of the element loaded from the same
 * entity has an xml:base attribute, then the value of that attribute from the
 * nearest such ancestor is converted from an IRI to a URI, absolutized if
 * possible, and returned. xml:base attributes from other entities are not
 * considered.
 * @li Otherwise, if setBaseURI() has been invoked on this element, then the URI
 * most recently passed to that method is absolutized if possible and returned.
 * @li Otherwise, if the element comes from an externally parsed entity or the
 * document entity, and the original base URI has not been changed by invoking
 * setBaseURI(), then the URI of that entity is returned.
 * @liOtherwise, (the element was created by a constructor rather then being
 * parsed from an existing document), the base URI of the nearest ancestor that
 * does have a base URI is returned. If no ancestors have a base URI, then the
 * empty string is returned.
 * @par
 * Absolutization takes place as specified by the XML Base specification.
 * However, it is not always possible to absolutize a relative URI, in which
 * case the empty string will be returned.
 *
 * @return the base URI of this node. This string must be fried by the caller.
 */
String Node::getBaseURI(void) {
	return xmlNodeGetBase(DOC(NODE(node)->doc), NODE(node));
}


/**
 * @fn Node *Node::getChild(int index);
 * Returns the child of this node at the specified position.
 * @param position the index of the child node to return
 * @return the positionth child node of this node
 */


/**
 */
Node *Node::internGetChild(int index) {
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * int Node::getChildCount(void);
 * Returns the number of children of this node. This is always non-negative
 * (greater than or equal to zero).
 * @return the number of children of this node
 */


/**
 */
int Node::internGetChildCount(void) {
	return 0;
}


/**
 * Returns the document that contains this node, or null if this node is not
 * currently part of a document. Each node belongs to no more than one document
 * at a time. If this node is a Document, then it returns this node.
 * @return the document this node is a part of
 */
Document *Node::getDocument(void) {
	return (Document *)get(NODE(node)->doc);
}


/**
 * Returns the nodes selected by the XPath expression in the context of this
 * node in document order as defined in XSLT. All namespace prefixes used in the
 * expression should be bound to namespace URIs by the second argument.
 * @par
 * Note that XPath expressions operate on the XPath data model, not the XOM data
 *  model. XPath counts all adjacent Text objects as a single text node, and
 * does not consider empty Text objects. For instance, an element that has
 * exactly three text children in XOM, will have exactly one text child in
 * XPath, whose value is the concatenation of all three XOM Text objects.
 * @par
 * You can use XPath expressions that use the namespace axis. However, namespace
 *  nodes are never returned. If an XPath expression only selects namespace
 * nodes, then this method will return an empty list.
 * @par
 * No variables are bound.
 * @par
 * The context position is the index of this node among its parents children,
 * counting adjacent text nodes as one. The context size is the number of
 * children this node's parent has, again counting adjacent text nodes as one
 * node. If the parent is a Document, then the DocType (if any) is not counted.
 * If the node has no parent, then the context position is 1, and the context
 * size is 1.
 * @par
 * Queries such as / *, //, and / * //p that refer to the root node do work when
 * operating with a context node that is not part of a document. However,
 * the query / (return the root node) throws an XPathException when applied to
 * a node that is not part of the document. Furthermore the top-level node in
 * the tree is treated as the first and only child of the root node, not as the
 * root node itself. For instance, this query stores parent in the result
 * variable, not child:
 * @code
 * Element parent = new Element("parent");
 * Element child = new Element("child");
 * parent.appendChild(child);
 * Nodes results = child.query("/ *");
 * Node result = result.get(0);
 * @endcode
 * @param xpath the XPath expression to evaluate
 * @param namespaces a collection of namespace prefix bindings used in the
 * XPath expression
 * @return a list of all matched nodes; possibly empty
 * @throw XPathException if there's a syntax error in the expression, the query
 * returns something other than a node-set
 */
Nodes *Node::query(const String& xpath, XPathContext *context) {
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * String Node::toXML(void);
 * Returns the actual XML form of this node, such as might be copied and pasted
 * from the original document. However, this does not preserve semantically
 * insignificant details such as white space inside tags or the use of
 * empty-element tags vs. start-tag end-tag pairs.
 * @return an XML representation of this node. The returned string must be fried
 * by the caller.
 */


/**
 */
String Node::internToXML(void) {
	String result;
	xmlBufferPtr buf = xmlBufferCreate();
	if(xmlNodeDump(buf, DOC(getDocument()->node), NODE(node), 0, 0) < 0)
		result = "";
	else
		result = buf->content;
	buf->content = 0;
	xmlBufferFree(buf);
	return result;
}


/**
 * Returns the nodes selected by the XPath expression in the context of this
 * node in document order as defined by XSLT. This XPath expression must not
 * contain any namespace prefixes.
 * @par
 * No variables are bound. No namespace prefixes are bound.
 * @param xpath the XPath expression to evaluate
 * @return a list of all matched nodes; possibly empty
 * @throw XPathException if there's a syntax error in the expression; or the
 * query returns something other than a node-set.
 */
Nodes *Node::query(const String& xpath) {
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * Get the line of the node in the source file (if any).
 * @return	File line of the node, -1 if no one is found.
 */
int Node::line(void) const {
	return xmlGetLineNo(NODE(node));
}


} } // elm::xom
