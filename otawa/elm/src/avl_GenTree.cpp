/*
 *	avl::GenTree class implementation
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

#include <elm/avl/GenTree.h>
#include <elm/avl/Set.h>
#include <elm/avl/Map.h>
#include <elm/genstruct/Vector.h>

namespace elm { namespace avl {

/**
 */
void AbstractTree::insert(unsigned char da[], int dir, Node *node, Node *q, Node *y, Node *z) {
	int k;
	Node *p;
	
	// Link it
	cnt++;
	q->links[dir] = node;
	if(!y)
		return;
	
	// update balance factors
	for(p = y, k = 0; p != node; p = p->links[da[k]], k++)
		if (da[k] == 0)
      		p->balance--;
		else
			p->balance++;
	
	// rebalancing
	Node *w;
	if(y->balance == -2) {
		Node *x = y->links[0];
		if(x->balance == -1) {
			w = x;
			y->links[0] = x->links[1];
			x->links[1] = y;
			x->balance = y->balance = 0;
		}
		else {
			w = x->links[1];
			x->links[1] = w->links[0];
			w->links[0] = x ;
			y->links[0] = w->links[1];
			w->links[1] = y;
			if(w->balance == -1) x->balance = 0, y->balance = +1;
			else if(w->balance == 0) x->balance = y->balance = 0;
			else x->balance = -1, y->balance = 0;
			w->balance = 0;
		}
	}
	else if(y->balance == + 2) {
		Node *x = y->links[1];
		if(x->balance == +1) {
			w = x;
			y->links[1] = x->links[0];
			x->links[0] = y;
			x->balance = y->balance = 0;				
		}
		else {
			w = x->links[0];
			x->links[0] = w->links[1];
			w->links[1] = x ;
			y->links[1] = w->links[0];
			w->links[0] = y;
			if(w->balance == +1) x->balance = 0, y->balance = -1;
			else if(w->balance == 0) x->balance = y->balance = 0;
			else x->balance = +1, y->balance = 0;
			w->balance = 0;
		}
	}
	else
		return;
	z->links[y != z->links[0]] = w;
}


/**
 */
void AbstractTree::remove(Node *pa[], unsigned char da[], int k, Node *p) {

	// remove the item
	if(p->links[1] == 0) {
		if(!k)
			this->root = p->links[0];
		else
			pa[k - 1]->links[da[k - 1]] = p->links[0];
	}
	else {
		Node *r = p->links[1];
		if(r->links[0] == 0) {
			r->links[0] = p->links[0];
			r->balance = p->balance;
			pa[k - 1]->links[da[k - 1]] = r ;
			da[k] = 1;
			pa[k++] = r ;
		}
 		else {
			Node *s;
			int j = k++;
			for (;;) {
				da[k] = 0;
				pa[k ++] = r;
				s = r->links[0];
				if(s->links[0] == 0)
					break;
				r = s;
			}
			s->links[0] = p->links[0];
			r->links[0] = s->links[1];
			s->links[1] = p->links[1];
			s->balance = p->balance;
			pa[j - 1]->links[da[j - 1]] = s;
			da[j] = 1;
			pa[j] = s;
 		}
	}
	
	// update the balance factors and rebalance
	while(--k > 0) {
		Node *y = pa[k];
		if(da[k] == 0) {
			// Update y’s balance factor after left-side AVL deletion 172 
			y->balance++;
			if(y->balance == +1)
				break;
			else if(y->balance == +2) {
				Node *x = y->links[1];
				if(x->balance == -1) {
					// Left-side rebalancing case 1 in AVL deletion 174
					Node *w ;

					// Rotate right at x then left at y in AVL tree 159
					w = x->links[0];
					x->links[0] = w->links[1];
					w->links[1] = x ;
					y->links[1] = w->links[0];
					w->links[0] = y;
					if(w->balance == +1)
						x->balance = 0, y->balance = -1;
					else if(w->balance == 0)
						x->balance = y->balance = 0;
					else
						x->balance = +1, y->balance = 0;
					w->balance = 0;
											
					pa[k - 1]->links[da[k - 1]] = w;
				}
				else {
					// Left-side rebalancing case 2 in AVL deletion 175
					y->links[1] = x->links[0];
					x->links[0] = y;
					pa[k - 1]->links[da[k - 1]] = x ;
					if(x->balance == 0) {
						x->balance = -1;
						y->balance = +1;
  						break;
					}
					else
						x->balance = y->balance = 0;
				}
			}
		}
		else {
			//  Update y’s balance factor after right-side AVL deletion 177 
			y->balance--;
			if(y->balance == -1)
				break;
			else if(y->balance == -2) {
				Node *x = y->links[0];
				if(x->balance == +1) {
					Node *w ;
					
					// Rotate left at x then right at y in AVL tree 156
					w = x->links[1];
					x->links[1] = w->links[0];
					w->links[0] = x;
					y->links[0] = w->links[1];
					w->links[1] = y;
					if(w->balance == -1)
						x->balance = 0, y->balance = +1;
					else if(w->balance == 0)
						x->balance = y->balance = 0;
					else
						x->balance = -1, y->balance = 0;
					w->balance = 0;						
					
					pa[k-1]->links[da[k - 1]] = w ;
				}
				else {
					y->links[0] = x->links[1];
					x->links[1] = y;
					pa[k - 1]->links[da[k - 1]] = x;
					if(x->balance == 0) {
						x->balance = +1;
						y->balance = -1;
						break;
					}
					else
						x->balance = y->balance = 0;
				}
			}
		}
	}
}


/**
 * Count the number of nodes.
 * @return	Number of nodes.
 */
int AbstractTree::count(void) const {
	int cnt = 0;
	genstruct::Vector<Node *> s;
	s.push(root);
	while(s) {
		Node *node = s.pop();
		cnt++;
		if(node->links[0])
			s.push(node->links[0]);
		if(node->links[1])
			s.push(node->links[1]);
	}
	return cnt;
}


/**
 * @class GenTree
 * This class implements an AVL tree collection based on C++ templates as provided by:
 * Ben Pfaff, "An Introduction to
 * Binary Search Trees and Balanced Trees",
 * Libavl Binary Search Tree Library, Volume 1: Source Code, Version 2.0.2.
 * @par
 * This class is rarely used as is but used as a base class for @ref elm::avl::Set or @ref elm::avl::Map.
 * 
 * @param T		Type of contained items.
 * @param C		Comparator for T items (default to @ref elm::Comparator<T>).
 * @see			@ref elm::avl::Set, @ref elm::avl::Map
 * 
 * @par Implemented concepts
 * @li @ref elm::concept::Collection<T>
 * @li @ref elm::concept::MutableCollection<T>
 */

/**
 * @fn int GenTree::count(void) const;
 * Get the count of items in the tree.
 * @return	Item count.
 * @notice	This function call is fast as the item count is maintained
 *			during each insertion and removal.
 */

/**
 * @fn bool GenTree::contains(const T& item) const;
 * Test if the tree contains the given item.
 * @param item	Item to look for.
 * @return		True if it is contained, false else.
 * @notice		Access time in log2(item number).
 */

/**
 * @fn bool GenTree::containsAll(const Co<T>& coll) const;
 * Test if the given collection is contained in the current one.
 * @param coll	Collection to test.
 * @return		True if the collection is containted, false else.
 * @param C		Type of the collection.
 */

/**
 * @fn bool GenTree::isEmpty(void) const;
 * Test if the tree is empty.
 * @return	True if the tree is empty, false else.
 */

/**
 * @class GenTree::Iterator
 * Iterator on items of the tree. No assumption may be made on the order of traversal
 * of the items of the tree.
 */

/**
 * @fn void GenTree::clear(void);
 * Remove all items from the tree and let it cleared.
 */

/**
 * @fn void GenTree::add(const T& item);
 * Add an item to the tree. Note that the item is still added even if it is already
 * contained in the tree, leading to duplicates.
 * @param item	Item to add.
 */

/**
 * @fn void GenTree::addAll(const Co<T>& coll);
 * Add a collection to this tree.
 * @param coll	Collection to add.
 * @param C		Type of the collection.
 */

/**
 * @fn void GenTree::remove(const T& item);
 * Remove an item from a tree. Notice that if the tree contains duplicates of the item,
 * only the first duplicate is removed.
 * @param item	Item to remove.
 * @warning	Attempting to remove an item not contained in the tree is an error.
 */

/**
 * @fn void GenTree::removeAll(const Co<T>& coll);
 * Remove a collection from this tree.
 * @param coll	Collection to remove.
 * @param C		Type of the collection.
 */

/**
 * @fn void GenTree::remove(const Iterator& iter);
 * Remove the item pointed by the iterator.
 * @param	Iterator pointing to the item to remove.
 */


/**
 * @class Set
 * Implements a set collection based on an AVL tree, that is, supporting access and modifications
 * with a O(log n) complexity.
 * @par Implemented concepts
 * @li @ref elm::concept::Collection<T>
 * @li @ref elm::concept::MutableCollection<T>
 * @li @ref elm::concept::Set<T>
 *
 * @param T		Type of stored items.
 * @param C		Comparator used to sort the items (must implements the @ref elm::concept::Comparator<T> concept,
 * 				as a default @ref elm::Comparator<T>).
 * @see			@ref elm::avl::GenTree
 */


/**
 * @class Map
 * Implements a map based on AVL tree, that is, a map supporting O(log n) accesses.
 * @par Implemented concepts
 * @li @ref elm::concept::Collection<T>
 * @li @ref elm::concept::Map<K, T>
 *
 * @param K		Type of keys of the map.
 * @param T		Type of stored items.
 * @param C		Comparator used to sort the items (must implements the @ref elm::concept::Comparator<K> concept,
 * 				as a default @ref elm::Comparator<K>).
 * @see			@ref elm::avl::GenTree
 */

}	// avl

namespace genstruct {

/**
 * @class AbstractAVLTree
 * @deprecated	Only for compilation compatibility. Use @ref elm::avl::AbstractTree.
 */

/**
 * @class GenAVLTree
 * @deprecated	Only for compilation compatibility. Use @ref elm::avl::GenTree.
 */

/**
 * @class AVLTree
 * @deprecated	Only for compilation compatibility. Use @ref elm::avl::Set.
 */

} }	// elm::genstruct
