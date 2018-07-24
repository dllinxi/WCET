/*
 *	$Id$
 *	xom::Text class interface
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
#include <elm/xom/Text.h>
#include "xom_macros.h"

namespace elm { namespace xom {

/**
 * @class Text
 * Node containing simple text.
 * @ingroup xom
 */


/**
 * Build a new text node.
 * @param data	Content of the text node.
 */
Text::Text(String data): Node(xmlNewText(data)) {
	ASSERT(node);
}


/**
 * Build a new text node by cloning an existing one.
 * @param text	Text node to clone.
 */
Text::Text(const Text *text): Node(xmlCopyNode(NODE(text->node), 1)) {
	ASSERT(node);
}


/**
 */
Node *Text::copy(void) {
	return new Text(this);
}


/**
 * Set the content of the text.
 * @param data	Text to put in.
 */
void Text::setValue(String data) {
	xmlNodeSetContent(NODE(node), data);
}


/**
 * Get the text of this node.
 * @return	Node text.
 */
String Text::getText(void) {
	return NODE(node)->content;
}


/**
 */
Node *Text::getChild(int index) {
	return internGetChild(index);
}


/**
 */
int Text::getChildCount(void) {
	return internGetChildCount();
}


/**
 */
String Text::getValue(void) {
	return internGetValue();
}


/**
 */
String Text::toXML(void) {
	return internToXML();
}

} } // elm::xom
