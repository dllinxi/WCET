/*
 * $Id$
 * Copyright (c) 2004, IRIT-UPS.
 *
 * elm/inhstruct/SLList.h -- SLList class interface.
 */
#ifndef ELM_INHSTRUCT_SLLIST_H
#define ELM_INHSTRUCT_SLLIST_H

namespace elm { namespace inhstruct {

// Defined classes
class SLNode;
class SLList;
	
// SLNode class
class SLNode {
	SLNode *nxt;
	friend class SLList;
public:
	inline SLNode(void);
	inline SLNode *next(void) const;
	inline void insertAfter(SLNode *node);
	inline void removeNext(void);
};

// SLList class
class SLList {
	SLNode *fst;
public:
	inline SLList(void);
	inline SLNode *first(void) const;
	SLNode *last(void) const;
	int count(void) const;
	inline bool isEmpty(void) const;

	inline void addFirst(SLNode *node);
	void addLast(SLNode *node);
	inline void removeFirst(void);
	void removeLast(void);
};

// SLNode inlines
inline SLNode::SLNode(void): nxt(0) {
}
inline SLNode *SLNode::next(void) const {
	return nxt;
}
inline void SLNode::insertAfter(SLNode *node) {
	node->nxt = nxt;
	nxt = node;
}
inline void SLNode::removeNext(void) {
	nxt = nxt->nxt;
}

// SLList inlines
inline SLList::SLList(void): fst(0) {
}
inline SLNode *SLList::first(void) const {
	return fst;
}
inline void SLList::addFirst(SLNode *node) {
	node->nxt = fst;
	fst = node;
}
inline void SLList::removeFirst(void) {
	fst = fst->next();
}
inline bool SLList::isEmpty(void) const {
	return !fst;
}

} } // elm::inhstruct

#endif // ELM_INHSTRUCT_SLLIST_H
