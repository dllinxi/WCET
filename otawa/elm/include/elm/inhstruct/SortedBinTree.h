/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * elm/inhstruct/SortedBinTree.h -- abstract binary tree.
 */
#ifndef ELM_INHSTRUCT_SORTEDBINTREE_H
#define ELM_INHSTRUCT_SORTEDBINTREE_H

#include <elm/inhstruct/BinTree.h>

namespace elm { namespace inhstruct {

// SortedBinTree class
class SortedBinTree: protected BinTree {
private:
	void insert(Node *node, Node *new_node);
	Node *relink(Node *left, Node *right);
	Node *remove(Node *node, Node *rem_node);
	
protected:
	virtual int compare(Node *node1, Node *node2) = 0;

public:
	virtual ~SortedBinTree(void) { }
	
	// Accessors
	inline bool isEmpty(void);
	inline int count(void);
	Node *get(Node *node);
	inline bool contains(Node *node);
	inline void visit(Visitor *visitor);
	inline void search(Visitor *visitor);
	
	// Modifiers
	void insert(Node *node);
	void remove(Node *node);
	inline void clear(void);
};


// SortedBinTree inlines
inline bool SortedBinTree::isEmpty(void) {
	return BinTree::isEmpty();
}

inline int SortedBinTree::count(void) {
	return BinTree::count();
}

inline bool SortedBinTree::contains(Node *node) {
	return get(node) != 0;
}

inline void SortedBinTree::visit(Visitor *visitor) {
	return BinTree::visit(visitor);
}
	
inline void SortedBinTree::search(Visitor *visitor) {
	BinTree::search(visitor);
}

inline void SortedBinTree::clear(void) {
	BinTree::clear();
}

} } // elm::inhstruct

#endif // ELM_INHSTRUCT_SORTEDBINTREE_H
