/*
 *	avl::Tree class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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
#ifndef ELM_AVL_TREE_H
#define ELM_AVL_TREE_H

#include <elm/inhstruct/BinTree.h>

namespace elm { namespace avl {

// AVLTree class
class Tree: public inhstruct::BinTree {
public:
	virtual ~Tree(void);

	// AVLNode class
	class Node: public BinTree::Node {
		friend class Tree;
		int h;
	public:
		inline Node *_left(void) const { return (Node *)left(); };
		inline Node *_right(void) const { return (Node *)right(); };
		#ifdef ELM_DEBUG_AVLTREE
			virtual void dump(void) = 0;
		#endif
	};

private:
	Node *insert(Node *cur, Node *node);
	Node *remove(Node *cur, Node *node);
	Node *removeGreatest(Node *root, Node *&res);
	Node *removeLeast(Node *root, Node *&res);
	Node *rotateSingleLeft(Node *node);
	Node *rotateDoubleLeft(Node *node);
	Node *rotateSingleRight(Node *node);
	Node *rotateDoubleRight(Node *node);
	Node *remap(Node *left, Node *right);
	Node *balanceLeft(Node *cur, Node*& left);
	Node *balanceRight(Node *cur, Node*& left);
	void clean(Node *node);
	inline Node *_root(void) const { return (Node *)root(); };
	inline int height(Node *node) const { return node ? node->h : 0; };

	inline void computeHeight(Node *node) {
		int hleft = height(node->_left()), hright = height(node->_right());
		if(hleft > hright)
			node->h = hleft + 1;
		else
			node->h = hright + 1;
	}
	
protected:
	virtual int compare(Node *node1, Node *node2) = 0;
	virtual void free(Node *node);

public:

	// Accessors
	inline bool isEmpty(void) { return BinTree::isEmpty(); }
	inline int count(void) { return BinTree::count(); }
	Node *get(Node *node);
	inline bool contains(Node *node) { return get(node) != 0; }
	inline void visit(Visitor *visitor) { return BinTree::visit(visitor); }
	inline void search(Visitor *visitor) { BinTree::search(visitor); }
	
	// Modifiers
	void insert(Node *node);
	void remove(Node *node);
	inline void clean(void) { clean(_root()); }
	
	// Debug
	#ifdef ELM_DEBUG_AVLTREE
		void dump(Node *node = 0, int level = 0);
	#endif
};

} } // elm::avl

#endif // ELM_AVL_TREE_H
