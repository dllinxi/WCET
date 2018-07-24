/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * BinTree.cpp -- binary tree implementation.
 */

#include <elm/inhstruct/BinTree.h>

namespace elm { namespace inhstruct {


/**
 * @class BinTree::Node
 * Nodes of a binary tree. For being used, this class must be extended with custom tree nodes.
 */

	
/**
 * @class BinTree
 * Provide a binary tree implementation. For being used, the pure virtual method compare()
 * must be defined inan extended class.
 */

/**
 * @fn int BinTree::count(void) const;
 * Count the number of nodes in the tree.
 * @return Count of nodes in the tree.
 */
int BinTree::count(BinTree::Node *node) const {
	if(!node)
		return 0;
	else
		return 1 + count(node->left()) + count(node->right());
}

/**
 * @fn bool BinTree::contains(Node *node)
 * Check if node if the node is contained in the tree.
 * @param node Node to find.
 * @return True if the node is found, false else.
 */
bool BinTree::contains(BinTree::Node *current, Node *node) const {
	if(!current)
		return false;
	else if(current == node)
		return true;
	else if(contains(current->left(), node))
		return true;
	else
		return contains(current->right(), node);
}

/**
 * @fn void BinTree::visit(Visitor *visitor) const;
 * Visit the tree in-order, that is, the left sub-tree, the root node and the right sub-tree.
 * @param visitor	Object called for each node in the graph.
 */
bool BinTree::visit(BinTree::Visitor *visitor, BinTree::Node *node) const {
	if(!node)
		return true;
	else
		return
			visit(visitor, node->left())
			&& visitor->process(node)
			&& visit(visitor, node->right());
}

/**
 * @fn void BinTree::visitPreOrder(BinTree::Visitor *visitor) const;
 * Visit the tree in pre-order, that is, visit node, the left sub-tree and then the right sub-tree.
 * @param visitor	Object called for each node in the graph.
 */
bool BinTree::visitPreOrder(BinTree::Visitor *visitor, BinTree::Node *node) const {
	if(!node)
		return true;
	else
		return
			visitor->process(node)
			&& visit(visitor, node->left())
			&& visit(visitor, node->right());
}

/**
 * @fn void BinTree::visitPostOrder(BinTree::Visitor *visitor) const;
 * Visit the tree in post-order, that is, the left sub-tree and then, right sub-tree and the root node.
 * @param visitor	Object called for each node in the graph.
 */
bool BinTree::visitPostOrder(BinTree::Visitor *visitor, BinTree::Node *node) const {
	if(!node)
		return true;
	else
		return
			visit(visitor, node->left())
			&& visit(visitor, node->right())
			&& visitor->process(node);
}

/**
 * Search in the binary tree according the given visitor. If the vistor returns 0, search is stopped.
 * If it returns less than 0, search continue in left subtree else in right subtree.
 * @param visitor Visitor object.
 */
void BinTree::search(Visitor *visitor, Node *node) const {
	if(!node)
		return;
	else {
		int result = visitor->process(node);
		if(!result)
			return;
		else if(result < 0)
			search(visitor, node->left());
		else
			search(visitor, node->right());
	}
}


/**
 * @fn void BinTree::clear(void);
 * Remove all nodes from the tree.
 */

// Private 
void BinTree::clear(Node *node) {
	clear(node->left());
	clear(node->right());
	delete node;
}

} }	// elm::inhstruct
