/*
 *	$Id$
 *	inhstruct::Tree class interface
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
#ifndef ELM_INHSTRUCT_TREE_H
#define ELM_INHSTRUCT_TREE_H

#include <elm/PreIterator.h>
#include <elm/assert.h>

namespace elm { namespace inhstruct {

// Tree class
class Tree {
public:

	inline Tree(void): _children(0), _sibling(0) { }
	
	// Accessors
	inline Tree *children(void) const { return _children; }
	inline Tree *sibling(void) const { return _sibling; }
	bool hasChild(Tree *tree) const { return _children; }
	inline bool contains(Tree *tree) const { return hasChild(tree); }
	int count(void) const;
	inline bool isEmpty(void) const { return !_children; }
	inline operator bool(void) const { return !isEmpty(); }
	
	// Iterator class
	class Iterator: public PreIterator<Iterator, Tree *> {
	public:
		inline Iterator(const Iterator& iter): cur(iter.cur) { }
		inline Iterator(const Tree *tree): cur(tree->children()) { }
		inline bool ended (void) const { return !cur; }
		inline Tree *item (void) const { return cur; }
		inline void next(void) { cur = cur->sibling(); }
	private:
		Tree *cur;
	};

	// Mutators
	inline void prependChild(Tree *child) {
		ASSERTP(child, "null child argument");
		child->_sibling = _children;
		_children = child;
	}

	void appendChild(Tree *child);
	inline void addSibling(Tree *newSibling) {
		Tree *oldSibling = _sibling;	
		newSibling->_sibling = oldSibling;
		_sibling = newSibling;			
	}
	
	inline void add(Tree *child) { prependChild(child); }
	template <class TT> void addAll(const TT& coll)
		{ for(typename TT::Iterator iter(coll); iter; iter++) add(*iter); } 
	void removeChild(Tree *child);
	inline void remove(Tree *child) { removeChild(child); }
	inline void remove(const Iterator& iter) { removeChild(iter); }
	template <class TT> void removeAll(const TT& coll)
		{ for(typename TT::Iterator iter(coll); iter; iter++) remove(*iter); } 
	inline void clear(void) { _children = 0; }
	
private:
	Tree *_children, *_sibling;
};

} } // elm::inhstruct

#endif // ELM_INHSTRUCT_TREE_H
