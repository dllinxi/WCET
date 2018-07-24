/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * src/SortedBinTree.cpp -- abstract binary tree implementation.
 */

#include <elm/assert.h>
#include <elm/inhstruct/SortedBinTree.h>

namespace elm { namespace inhstruct {


/*
 * @class SortedBinTree::Node
 * Node used in the inherited sorted binary tree.
 */

	
/*
 * @fn SortedBinTree::Node::Node(void);
 * Default constructor.
 */

	
/*
 * @fn SortedBinTree::Node::Node(Node *left, Node *right);
 * Constructor with left and right children.
 * @param left	Left child.
 * @param right	Right child.
 */


/*
 * @fn SortedBinTree::Node *SortedBinTree::Node::left(void) const;
 * Get the left child of this node.
 */


/*
 * @fn SortedBinTree::Node *SortedBinTree::Node::right(void) const;
 * Get the right child of this node.
 */


/**
 * @class SortedBinTree
 * This class provides a sorted binary tree structure absed on inheritance of its content nodes.
 */


/**
 * @fn int SortedBinTree::compare(Node *node1, Node *node2) ;
 * This method must be defined by inherited classes for providing a node comparing policy.
 * @param node1	First node is comparison.
 * @param node2	Second node in comparison.
 * @return		0 for equality, >0 if first node is greater than second one, <0 else.
 */


/**
 * @fn bool SortedBinTree::isEmpty(void);
 * Test if the tree is empty.
 * @return	True if tree is empty, false else.
 */
 
 
/**
 * @fn int SortedBinTree::count(void);
 * Count the number of nodes in the tree.
 * @return	Node count.
 */
 

/**
 * Get the node from the tree that is equal to the given one.
 * @param	node	Node to test for.
 * @return	Found node or null.
 */
SortedBinTree::Node *SortedBinTree::get(Node *node) {
	ASSERTP(node, "null node");
	Node *cur = root();
	while(cur) {
		int result = compare(node, cur);
		if(!result)
			break;
		else if(result < 0)
			cur = cur->left();
		else
			cur = cur->right();
	}
	return cur;
}


/**
 * @fn bool SortedBinTree::contains(Node *node);
 * Test if the tree contains a node equals to the given one.
 * @param node		Node to look for.
 * @return		True if there is a node equal to the given one.
 */


/**
 * @fn void SortedBinTree::visit(Visitor *visitor);
 * Visit in order the nodes of the tree.
 * @param visitor	Visitor object to call for each node.
 */


/**
 * @fn void SortedBinTree::search(Visitor *visitor);
 * Look for a special node in the given tree.For each node, the process() method of the visitor is called.
 * If it returns 0, search stops. If it returns, <0 traversal continue with left child else with the right child.
 * @param visitor	Visitor to use.
 */


/**
 * Add the given node to the tree.
 * @param node		Node to add.
 */
void SortedBinTree::insert(Node *node) {
	ASSERTP(node, "null node");
	if(isEmpty())
		setRoot(node);
	else
		insert(root(), node);
}


/**
 * Remove a node equals to the given one from the tree.
 * @param node		Node to remove.
 */
void SortedBinTree::remove(Node *node) {
	ASSERTP(node, "null node");
	setRoot(remove(root(), node));
}


/**
 * Perform the work of insertion of a node in the tree.
 * @param node		Currently processed node.
 * @param new_node	Node to insert.
 */
void SortedBinTree::insert(Node *node, Node *new_node) {
	int result = compare(new_node, node);
	if(result < 0) {
		if(!node->left())
			node->insertLeft(new_node);
		else
			insert(node->left(), new_node);
	}
	else {
		if(!node->right())
			node->insertRight(new_node);
		else
			insert(node->right(), new_node);
	}
}


/**
 * Relink the tree just after the removal of a node.
 * @param left		Sub-tree to put left.
 * @param right		Sub-tree to put right.
 * @return		Tree after removal.
 */
SortedBinTree::Node *SortedBinTree::relink(Node *left, Node *right) {
	if(!left)
		return right;
	else if(!right)
		return left;
	else if(!left->right()) {
		left->insertRight(right);
		return left;
	}
	else if(!right->left()) {
		right->insertLeft(left);
		return right;
	}
	else {
		left->insertLeft(relink(left->left(), left->right()));
		left->insertRight(right);
		return left;
	}
}


/**
 * Perform the work of removing a node equal to the given one.
 * @param node		Currently processed node.
 * @param rem_node	Removed node.
 * @return		Tree after removal.
 */
SortedBinTree::Node *SortedBinTree::remove(Node *node, Node *rem_node) {
	if(!node)
		return node;
	else {
		int result = compare(rem_node, node);
		if(!result) {
			Node *result = relink(node->left(), node->right());
			delete node;
			return result;
		}
		else if(result < 0)
			node->insertLeft(remove(node->left(), rem_node));
		else
			node->insertRight(remove(node->right(), rem_node));
		return node;
	}
}

} } // elm::inhstruct
