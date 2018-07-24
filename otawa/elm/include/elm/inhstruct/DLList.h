/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * dllist.h -- double link list classes interface.
 */
#ifndef ELM_INHSTRUCT_DLLIST_H
#define ELM_INHSTRUCT_DLLIST_H

#include <elm/assert.h>

namespace elm { namespace inhstruct {

// DLList class
class DLList;
class DLNode {
	friend class DLList;
	DLNode *nxt, *prv;
public:
	inline DLNode *next(void) const;
	inline DLNode *previous(void) const;
	inline bool atBegin(void) const;
	inline bool atEnd(void) const;
	
	inline void replace(DLNode *node);
	inline void insertAfter(DLNode *node);
	inline void insertBefore(DLNode *node);
	inline void remove(void);
	inline void removeNext(void);
	inline void removePrevious(void);
};


// DLList class
class DLList {
	DLNode hd, tl;
public:
	inline DLList(void);
	inline DLList(DLList& list);

	inline DLNode *first(void) const;
	inline DLNode *last(void) const;
	inline bool isEmpty(void) const;
	inline int count(void) const;

	inline void addFirst(DLNode *node);
	inline void addLast(DLNode *node);
	inline void removeFirst(void);
	inline void removeLast(void);
};


// DLNode methods
DLNode *DLNode::next(void) const {
	return nxt;
}
DLNode *DLNode::previous(void) const {
	return prv;
}
void DLNode::replace(DLNode *node) {
	ASSERTP(node, "null node for replacement");
	nxt->prv = node;
	node->nxt = nxt;
	prv->nxt = node;
	node->prv = prv;
}
void DLNode::insertAfter(DLNode *node) {
	ASSERTP(node, "null node to insert");
	nxt->prv = node;
	node->nxt = nxt;
	nxt = node;
	node->prv = this;
}
void DLNode::insertBefore(DLNode *node) {
	ASSERTP(node, "null node to insert");
	prv->nxt = node;
	node->prv = prv;
	prv = node;
	node->nxt = this;
}
void DLNode::remove(void) {
	prv->nxt = nxt;
	nxt->prv = prv;
}
void DLNode::removeNext(void) {
	ASSERTP(!nxt->atEnd(), "no next node");
	nxt->remove();
}
void DLNode::removePrevious(void) {
	ASSERTP(!prv->atBegin(), "no previous node");
	prv->remove();
}
bool DLNode::atBegin(void) const {
	return prv == 0;
}
bool DLNode::atEnd(void) const {
	return nxt == 0;
}


// DLList methods
DLList::DLList(void) {
	hd.nxt = &tl;
	hd.prv = 0;
	tl.prv = &hd;
	tl.nxt = 0;
}

inline DLList::DLList(DLList& list) {
	hd.prv = 0;
	tl.nxt = 0;
	if(list.isEmpty()) {
		hd.nxt = &tl;
		tl.prv = &hd;
	}
	else {	
		hd.nxt = list.hd.nxt;
		list.hd.nxt = 0;
		hd.nxt->prv = &hd;
		tl.prv = list.tl.prv;
		list.tl.prv = 0;
		tl.prv->nxt = &tl;

	}
}

DLNode *DLList::first(void) const {
	return hd.nxt;
}
DLNode *DLList::last(void) const {
	return tl.prv;
}
bool DLList::isEmpty(void) const {
	return hd.nxt == &tl;
}
int DLList::count(void) const {
	int cnt = 0;
	for(DLNode *cur = hd.nxt; cur != &tl; cur =cur->nxt)
		cnt++;
	return cnt;
}
void DLList::addFirst(DLNode *node) {
	ASSERTP(node, "null node added");
	hd.insertAfter(node);
}
void DLList::addLast(DLNode *node) {
	ASSERTP(node, "null node added");
	tl.insertBefore(node);
}
void DLList::removeFirst(void) {
	ASSERTP(!isEmpty(), "list empty");
	hd.removeNext();
}
void DLList::removeLast(void) {
	ASSERTP(!isEmpty(), "list empty");
	tl.removePrevious();
}


} } // elm::inhstruct

#endif	// ELM_INHSTRUCT_DLLIST_H
