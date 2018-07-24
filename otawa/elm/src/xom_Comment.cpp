/*
 *	$Id$
 *	xom::Comment class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
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
#include <elm/xom/Comment.h>
#include "xom_macros.h"

namespace elm { namespace xom {

/**
 * @class Comment
 * Node containing simple comment.
 * @ingroup xom
 */


/**
 * Make a comment from an existing XML node.
 * @param node	XML node.
 */
Comment::Comment(void *node): Node(node) {
}

/**
 * Build a new comment node.
 * @param data	Content of the text node.
 */
Comment::Comment(String data): Node(xmlNewComment(data)) {
	ASSERT(node);
}


/**
 * Build a new text node by cloning an existing one.
 * @param text	Text node to clone.
 */
Comment::Comment(const Comment *comment): Node(xmlCopyNode(NODE(comment->node), 1)) {
	ASSERT(node);
}


/**
 */
Node *Comment::copy(void) {
	return new Comment(this);
}


/**
 * Set the content of the text.
 * @param data	Text to put in.
 */
void Comment::setValue(String data) {
	xmlNodeSetContent(NODE(node), data);
}


/**
 * Get the text of this node.
 * @return	Node text.
 */
String Comment::getText(void) {
	return NODE(node)->content;
}


/**
 */
Node *Comment::getChild(int index) {
	return internGetChild(index);
}


/**
 */
int Comment::getChildCount(void) {
	return internGetChildCount();
}


/**
 */
String Comment::getValue(void) {
	return "";
}


/**
 */
String Comment::toXML(void) {
	return internToXML();
}

} } // elm::xom
