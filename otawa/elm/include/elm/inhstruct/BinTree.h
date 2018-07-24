/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * BinTree.h -- binary interface.
 */
#ifndef ELM_INHSTRUCT_BINTREE_H
#define ELM_INHSTRUCT_BINTREE_H

namespace elm { namespace inhstruct {

// BinTree class
class BinTree {
public:
	// Node class
	class Node {
		Node *l, *r;
	public:
		inline Node(void);
		inline Node(Node *left, Node *right);
		inline Node *left(void) const;
		inline Node *right(void) const;
		inline void insertLeft(Node *node);
		inline void insertRight(Node *node);
	};

	// Visitor class
	class Visitor {
	public:
		virtual ~Visitor(void) { }
		virtual int process(Node *node) = 0;
	};
	
private:
	Node *_root;
	int count(Node *node) const;
	bool contains(Node *current, Node *node) const;
	bool visit(Visitor *visitor, Node *node) const;
	bool visitPreOrder(Visitor *visitor, Node *node) const;
	bool visitPostOrder(Visitor *visitor, Node *node) const;
	void search(Visitor *visitor, Node *node) const;
	void clear(Node *node);

public:
	inline BinTree(void);
	inline bool isEmpty(void) const;
	inline bool contains(Node *node);
	inline int count(void) const;
	inline Node *root(void) const;
	inline void setRoot(Node *node);
	inline void visit(Visitor *visitor) const;
	inline void visitPreOrder(Visitor *visitor) const;
	inline void visitPostOrder(Visitor *visitor) const;
	void search(Visitor *visitor) const;
	inline void clear(void);
};


// BinTree::Node inlines
inline void BinTree::Node::insertLeft(BinTree::Node *node) {
	l = node;
}
inline void BinTree::Node::insertRight(BinTree::Node *node) {
	r = node;
}
inline BinTree::Node::Node(void): l(0), r(0) {
}
inline BinTree::Node::Node(BinTree::Node *left, BinTree::Node *right)
: l(left), r(right) {
}
inline BinTree::Node *BinTree::Node::left(void) const {
	return l;
}
inline BinTree::Node *BinTree::Node::right(void) const {
	return r;
}

// BinTree inlines
inline BinTree::BinTree(void): _root(0) {
}
inline bool BinTree::isEmpty(void) const {
	return _root == 0;
}
inline bool BinTree::contains(Node *node) {
	return contains(_root, node);
}
inline int BinTree::count(void) const {
	return count(_root);
}
inline BinTree::Node *BinTree::root(void) const {
	return _root;
}
inline void BinTree::setRoot(BinTree::Node *node) {
	_root = node;
}
inline void BinTree::visit(BinTree::Visitor *visitor) const {
	visit(visitor, _root);
}
inline void BinTree::visitPreOrder(BinTree::Visitor *visitor) const {
	visitPreOrder(visitor, _root);
}
inline void BinTree::visitPostOrder(BinTree::Visitor *visitor) const {
	visitPostOrder(visitor, _root);
}

inline void BinTree::clear(void) {
	clear(_root);
}

} }	// elm::inhstruct

#endif	// ELM_INHSTRUCT_BINTREE_H
