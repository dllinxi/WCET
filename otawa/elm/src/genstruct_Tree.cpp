/*
 *	$Id$
 *	Tree class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <elm/genstruct/Tree.h>

namespace elm { namespace genstruct {

/**
 * @class Tree
 * This class implements a minimal tree whose children are linked with
 * a simple link list. It provides facilities to store a value on the tree
 * and to access either children, or closer sibling.
 * 
 * @param T	Type of stored values.
 * @ingroup gen_datastruct
 */

/**
 * @fn Tree::Tree(const T& value);
 * Build a new empty tree storing the given value.
 * @param value	Value to store in the tree.
 */

/**
 * @fn const T& Tree::data(void) const;
 * Get the value stored in the tree.
 * @return	Stored value.
 */


/**
 * @fn T& Tree::data(void);
 * Get assignable reference on the value stored in the tree.
 * @return	Stored value reference.
 */


/**
 * @fn const Tree<T> *Tree::children(void) const;
 * Get list of children of the tree, that is, the first child.
 * @return	First children.
 */


/**
 * @fn Tree<T> *Tree::children(void)
 * Get list of children of the tree, that is, the first child.
 * @return	First children.
 */


/**
 * @fn const Tree<T> *Tree::sibling(void) const;
 * Get the next sibling of the current tree.
 * @return	Next sibling.
 */


/**
 * @fn Tree<T> *Tree::sibling(void)
 * Get the next sibling of the current tree.
 * @return	Next sibling.
 */
	
} } // elm:genstruct
