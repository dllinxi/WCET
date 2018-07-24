/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * src/inhstruct_DLList.cpp -- inhstruct::DLList implementation.
 */

#include <elm/inhstruct/DLList.h>

namespace elm { namespace inhstruct {

/**
 * @class DLNode <elm/inhstruct/DLList.h>
 * This class represents node in the DL list. In order to use the list,
 * a user class must inherit from this one for attaching data.
 */


/**
 * @fn DLNode *DLNode::next(void) const;
 * Get the next node in the list.
 * @return	Next node.
 */


/**
 * @fn DLNode *DLNode::previous(void) const;
 * Get the previous node in the list.
 * @return	Previous node.
 */


/**
 * @fn bool DLNode::atBegin(void) const;
 * Test if the current node is at the list begin.  This method is used for
 * back-traversing the list. When the current node is at begin, the traversal
 * must be stopped.
 * @return	True if it is the list begin.
 */


/**
 * @fn bool DLNode::atEnd(void) const;
 * Test if the current node is at the list end. This method is used for
 * traversing the list. When the current node is at end, the traversal must
 * be stopped.
 * @return 	True if it is the list end.
 */


/**
 * @fn void DLNode::replace(DLNode *node);
 * Replace the current node by the given one.
 * @param node	Remplacement node.
 */


/**
 * @fn void DLNode::insertAfter(DLNode *node);
 * Insert a node after the current one.
 * @param node	Node to insert.
 */


/**
 * @fn void DLNode::insertBefore(DLNode *node);
 * Insert a node before the current one.
 * @param node	Node to insert.
 */


/**
 * @fn void DLNode::remove(void);
 * Remove the current node from the list. Does not perform the deletion of the
 * node.
 */


/**
 * @fn void DLNode::removeNext(void);
 * Remove the next node from the list. It is an error to call this method on
 * the last node of the list. Does not perform the deletion of the node.
 */


/**
 * @fn void DLNode::removePrevious(void);
 * Remove the previous node from the list. It is an error to call this method
 * on the first node of the list. Does not perform the deletion of the node.
 */


/**
 * @class DLList <elm/inhstruct/DLList.h>
 * This class implements a double-linked list using node inherited from the
 * user.
 */


/**
 * @fn DLList::DLList(void);
 * Build an empty list.
 */


/**
 * @fn DLNode *DLList::first(void) const;
 * Get the first node in the list.
 * @return	First node. If the list is empty, return the end marker.
 */


/**
 * @fn DLNode *DLList::last(void) const;
 * Get the last node in the list.
 * @return Last node. If the list is empty, return the begin marker.
 */


/**
 * @fn bool DLList::isEmpty(void) const;
 * Test if the list is empty.
 * @return	True if the list is empty, false else.
 */


/**
 * @fn int DLList::count(void) const;
 * Count the number of nodes in the list.
 * @return	Number of nodes.
 */


/**
 * @fn void DLList::addFirst(DLNode *node);
 * Add a node as the first node in the list.
 * @param node	Node to add.
 */


/**
 * @fn void DLList::addLast(DLNode *node);
 * Add a node as the last node in the list.
 * @param node	Node to add.
 */


/**
 * @fn void DLList::removeFirst(void);
 * Remove the first node of the list. Does not perform the deletion of the node.
 * It is an error to call this method when the list is empty.
 */


/**
 * @fn void DLList::removeLast(void);
 * Remove the last node of the list. Does not perform the deletion of the node.
 * It is an error to call this method when the list is empty.
 */

} } // elm::inhstruct

