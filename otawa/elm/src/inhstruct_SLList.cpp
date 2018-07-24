/*
 * $Id$
 * Copyright (c) 2004, IRIT-UPS.
 *
 * src/inhstruct_SLList.cpp -- SLList class implementation.
 */

#include <elm/inhstruct/SLList.h>

namespace elm { namespace inhstruct {


/**
 * @class SLNode <elm/inhstruct/SLList.h>
 * Single link node for SLList. It represents the nodes of the SLList
 * implementation. It is usually extended for getting real user nodes.
 */

/**
 * @fn SLNode::SLNode(void);
 * Build a simple not-linked node.
 */


/**
 * @fn SLNode *SLNode::next(void) const;
 * Get the following next node.
 * @return	Next node.
 */


/**
 * @fn void SLNode::insertAfter(SLNode *node);
 * Insert a node after the current one.
 * @param node	Node to insert after.
 */


/**
 * @fn void SLNode::removeNext(void);
 * Remove the next node if any.
 * @warning It is an error to call this method if there is no next node.
 */


/**
 * @class SLList <elm/inhstruct/SLList.h>
 * This class implements a single-link list data structure with nodes
 * (@ref SLNode) that must be extended to contain useful information.
 */

/**
 * @fn SLNode *SLList::last(void) const;
 * Get te first node of the list. Remark that this method is really inefficient.
 * Its working time is in O(n), n number of nodes in the list. Use it only with
 * small list or revert to more powerful data structures.
 * @return Last list node.
 */
SLNode *SLList::last(void) const {
	if(!fst)
		return 0;
	else {
		SLNode *node = fst;
		while(node->next())
			node = node->next();
		return node;
	}
}

/**
 * Count the nodes in the list.
 * @return Count of nodes in the list.
 */
int SLList::count(void) const {
	int cnt = 0;
	for(SLNode *node = fst; node; node = node->next())
		cnt++;
	return cnt;
}

/**
 * Add a node to the end of the list.
 * @param node Node to add at the last position of the list. Remark that this
 * method is really inefficient.bIts working time is in O(n), n number of nodes
 * in the list. Use it only with small list or revert to more powerful data
 * structures.
 * @warning It is an error to add the same nodes many times to one or to many
 * lists.
 */
void SLList::addLast(SLNode *node) {
	if(!fst)
		addFirst(node);
	else
		last()->insertAfter(node);
}

/**
 * Remove the last node of the list.
 * @warning Remark that this method is really inefficient. If you have to perform this task too often,
 * please revert to other data structure.
 */
void SLList::removeLast(void) {
	
	// Find the last and its previous
	SLNode *node = fst, *prv = 0;
	while(node->next()) {
		prv = node;
		node = node->next();
	}
	
	// Remove the node
	if(prv)
		prv->removeNext();
	else
		fst = fst->next();
}

/**
 * @fn SLNode *SLList::first(void) const;
 * Get te first node of the list.
 * @return First list node.
 * @warning It is an error to call this method on an empty list.
 */


/**
 * @fn bool SLList::isEmpty(void) const;
 * Test if the list is empty.
 * @return	True if it is empty, false else.
 */


/**
 * @fn void SLList::addFirst(SLNode *node);
 * Add a node at the first position in the list.
 * @param node	Node to add.
 * @warning It is an error to add the same nodes many times to one or to many
 * lists.
 * @warning It is an error to call this method on an empty list.
 */


/**
 * @fn void SLList::removeFirst(void);
 * Remove the first node of the list.
 * @warning It is an error to call this method on an empty list.
 */ 

} }	// elm::namespace
