/*
 *	$Id$
 *	inhstruct::Tree class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007, IRIT UPS.
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

#include <elm/inhstruct/Tree.h>

namespace elm { namespace inhstruct {

/**
 * @class Tree
 * This class provides a simple generic implementation of trees storing children
 * in a simple list.
 *  
 * @par {Implemented concepts}:
 * @li @ref elm::concept::MutableCollection
 * 
 * @param T	Type of the top of the user @ref Tree.
 */ 


/**
 * @fn Tree::Tree(void);
 * Build an empty tree.
 */


/**
 * @fn Tree *Tree::children(void) const;
 * Get the first child.
 * @return	First child.
 */


/**
 * @fn Tree *Tree::sibling(void) const;
 * Get the next sibling.
 * @return	Next sibling.
 */


/**
 * @fn bool Tree::hasChild(Tree *tree) const;
 * Test if the given tree is a child of the current one.
 * @param	True if it is a child, false else.
 */


/**
 * @fn bool Tree::contains(Tree *tree) const;
 * Same as hasChild().
 */


/**
 * Count the children of the tree.
 * @return	Children count.
 */
int Tree::count(void) const {
	int cnt = 0;
	for(Iterator cur(this); cur; cur++)
		cnt++;
	return cnt;
}


/**
 * @fn bool Tree::isEmpty(void) const;
 * Test if the tree has no children.
 * @return	True if there is no children, false else.
 */


/**
 * @fn Tree::operator bool(void) const;
 * Same as !isEmpty().
 */


/**
 * @class Tree::Iterator;
 * Iterator on the children of a tree.
 * @b{Implemented concepts}: @ref elm::concept::Iterator.
 */


/**
 * @fn Tree::prependChild(Tree *child);
 * Add a child at the start of the children list.
 * @param child	Child to add.
 */


/**
 * Add a child at the end of the children list.
 * @param child	Child to add.
 */
void Tree::appendChild(Tree *child) {
	ASSERTP(child, "null child argument");
	if(!_children)
		_children = child;
	else
		for(Iterator cur(this); cur; cur++)
			if(!cur->sibling()) {
				cur->_sibling = child;
				break;
			}
}


/**
 * @fn void Tree::add(Tree *child);
 * Same as prepend().
 */


/**
 * @fn void Tree::addAll(const TT& coll);
 * Add a collcection of children to the tree.
 * @param coll	Collection to add (must implements @ref elm::concept::Collection).
 */


/**
 * Remove a child from the list.
 * @param child	Child to remove.
 */
void Tree::removeChild(Tree *child) {
	ASSERTP(child, "null child argument");
	if(_children == child) {
		_children = _children->_sibling;
			return;		
	}
	for(Iterator cur(this); cur; cur++)
		if(cur->sibling() == child) {
			cur->_sibling = child->_sibling;
			return;
		}
	ASSERTP(false, "child not in this tree");
}


/**
 * @fn void Tree::remove(Tree *child);
 * Same as removeChild().
 */


/**
 * @fn void Tree::remove(const Iterator& iter);
 * Remove a child pointed by an iterator.
 * @param iter	Iterator pointing to the child.
 */


/**
 * @fn void Tree::removeAll(const TT& coll);
 * Remove a collection of children.
 * @param coll	Collection to remove. Must implement @ref elm::concept::Collection.
 */


/**
 * @fn void Tree::clear(void);
 * Remove all children of the tree.
 */

} } // elm::inhstruct
