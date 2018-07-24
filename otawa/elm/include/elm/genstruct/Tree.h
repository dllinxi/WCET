/*
 *	$Id$
 *	Tree class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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
#ifndef ELM_GENSTRUCT_TREE_H
#define ELM_GENSTRUCT_TREE_H

#include <elm/PreIterator.h>
#include <elm/inhstruct/Tree.h>

namespace elm { namespace genstruct {

// Tree class
template <class T>
class Tree: private inhstruct::Tree {
public:
	inline Tree(const T& value): _value(value) { }
	
	inline const T& data(void) const { return _value; }
	inline T& data(void) { return _value; }
	
	// Accessors
	inline const Tree *children(void) const
		{ return static_cast<Tree *>(inhstruct::Tree::children()); }
	inline Tree *children(void)
		{ return static_cast<Tree *>(inhstruct::Tree::children()); }
	inline const Tree *sibling(void) const
		{ return static_cast<Tree *>(inhstruct::Tree::sibling()); }
	inline Tree *sibling(void)
		{ return static_cast<Tree *>(inhstruct::Tree::sibling()); }
	bool hasChild(Tree *tree) const
		{ return inhstruct::Tree::hasChild(tree); }
	inline bool contains(Tree *tree) const { return hasChild(tree); }
	inline int count(void) const
		{ return inhstruct::Tree::count(); }
	inline bool isEmpty(void) const
		{ return inhstruct::Tree::isEmpty(); }
	inline operator bool(void) const
		{ return inhstruct::Tree::isEmpty(); }

	// Iterator class
	class Iterator: public PreIterator<Iterator, const T&> {
	public:
		inline Iterator(const Iterator& iter): it(iter.it) { }
		inline Iterator(const Tree *tree): it(tree) { }
		inline bool ended (void) const { return it.ended(); }
		inline const T& item(void) const
			{ return static_cast<Tree *>(it.item())->_value; }
		inline void next(void) { it.next(); }
	private:
		inhstruct::Tree::Iterator it;
	};

	// Mutators
	inline void prependChild(Tree *child)
		{ inhstruct::Tree::prependChild(child); }
	inline void appendChild(Tree *child)
		{ inhstruct::Tree::appendChild(child); }
	inline void addSibling(Tree *newSibling)
		{ inhstruct::Tree::addSibling(newSibling); }
	inline void add(Tree *child)
		{ inhstruct::Tree::add(child); }
	template <class TT> void addAll(const TT& coll)
		{ inhstruct::Tree::addAll(coll); } 
	inline void removeChild(Tree *child)
		{ inhstruct::Tree::removeChild(child); }
	inline void remove(Tree *child)
		{ inhstruct::Tree::remove(child); }
	inline void remove(const Iterator& iter)
		{ removeChild(iter); }
	template <class TT> void removeAll(const TT& coll)
		{ inhstruct::Tree::removeAll(coll); } 
	inline void clear(void)
		{ inhstruct::Tree::clear(); }
	
private:
	T _value;
};

} } // elm:genstruct

#endif // ELM_GENSTRUCT_TREE_H
