/*
 *	$Id$
 *	xom::ParentNode class interface
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
#include <elm/xom/ParentNode.h>

namespace elm { namespace xom {

/**
 * @class ParentNode
 * This class provides to the node facilities for containing children.
 * @ingroup xom
 */


/**
 * @fn ParentNode::ParentNode(void * node);
 * Build a node from a reader.
 * @param node	Node built by the reader.
 */


/**
 * Appends a node to the children of this node.
 * @param child		node to append to this node
 * @note Fails if (1) if this node cannot have children of this type,
 * 		(2) if child already has a parent.
 */
void ParentNode::appendChild(Node *child) {
	ASSERTP(!NODE(child->getNode())->parent, "node with multiple parent is forbidden");
	xmlNode *cnode = NODE(child->getNode()), *pnode = NODE(node);

	if(cnode->doc && cnode->doc != pnode->doc) {
		xmlNodePtr old = cnode;
		cnode = xmlDocCopyNode(cnode, pnode->doc, 1);
		child->setNode(cnode);
		freeNode(old);
	}
	xmlAddChild(pnode, cnode);
}


/**
 * Get the child node at the given position.
 * @param position	Position of the looked child.
 * @return			Child at the given position.
 */
Node *ParentNode::getChild(int position) {
	xmlNodePtr cur = NODE(node)->children;
	for(int i = 0; i < position; i++, cur = cur->next)
		ASSERTP(cur, "position out of bounds");
	ASSERTP(cur, "position out of bounds");
	return Node::make(cur);
}


/**
 * Get the count of children of the current node.
 * @return	Children count.
 */
int	ParentNode::getChildCount(void) {
	int cnt = 0;
	for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next)
		cnt++;
	return cnt;
}


/**
 * Returns the position of a node within the children of this node. This is a
 * number between 0 and one less than the number of children of this node. It
 * returns -1 if child  does not have this node as a parent.
 * @par
 * This method does a linear search through the node's children. On average, it
 * executes in O(N) where N is the number of children of the node.
 * @param child the node whose position is desired
 * @return the position of the argument node among the children of this node
 */
int	ParentNode::indexOf(Node *child) {
	int cnt = 0;
	for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next, cnt++)
		if(cur != NODE(child->getNode()))
			return cnt;
	return -1;
}


/**
 * Inserts a child node at the specified position. The child node previously at
 * that position (if any) and all subsequent child nodes are moved up by one.
 * That is, when inserting a node at 2, the old node at 2 is moved to 3, the old
 * child at 3 is moved to 4, and so forth. Inserting at position 0 makes the
 * child the first child of this node. Inserting at the position getChildCount()
 * makes the child the last child of the node.
 * @par
 * All the other methods that add a node to the tree ultimately invoke this
 * method.
 * @param position where to insert the child
 * @param child the node to insert
 * @throw IllegalAddException if this node cannot have a child of the argument's
 * type.
 * @throw MultipleParentException if child already has a parent
 */
void ParentNode::insertChild(Node *child, int position) {
	ASSERTP(child, "null node");
	ASSERTP(position >= 0, "position must be positive");
	if(position == 0) {
		if(!NODE(node)->children)
			xmlAddChild(NODE(node), NODE(child->getNode()));
		else
			xmlAddPrevSibling(NODE(node)->children, NODE(child->getNode()));
	}
	else {
		int cnt = 1;
		for(xmlNodePtr cur = NODE(node)->children; cur; cur = cur->next, cnt++)
			if(cnt == position) {
				xmlAddNextSibling(cur, NODE(child->getNode()));
				return;
			}
		ASSERTP(0, "position out of bounds");
	}
}


/**
 * Removes the child of this node at the specified position. Indexes begin at 0
 * and count up to one less than the number of children in this node.
 * @param position index of the node to remove
 * @param the node which was removed
 */
Node *ParentNode::removeChild(int position) {
	return removeChild(getChild(position));
}


/**
 * Removes the specified child of this node.
 * @param child child node to remove.
 * @return the node which was removed.
 */
Node *ParentNode::removeChild(Node *child) {
	xmlUnlinkNode(NODE(child->getNode()));
	return child;
}


/**
 * Replaces an existing child with a new child node. If oldChild is not a child
 * of this node, then a NoSuchChildException is thrown.
 * @param oldChild the node removed from the tree
 * @param newChild the node inserted into the tree
 * @throw MultipleParentException if newChild already has a parent.
 * @throw IllegalAddException if this node cannot have children of the type of
 * newChild.
 */
void ParentNode::replaceChild(Node *old_child, Node *new_child) {
	xmlReplaceNode(NODE(old_child->getNode()), NODE(new_child->getNode()));
}

} } // elm::xom

