/*
 *	avl::Set class interface
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
#ifndef ELM_AVL_SET_H_
#define ELM_AVL_SET_H_

#include <elm/avl/GenTree.h>

namespace elm { namespace avl {

template <class T, class C = elm::Comparator<T> >
class Set: public GenTree<T, IdAdapter<T>, C> {
public:
	static const Set<T, C> null;
	inline void add(const T& value) { GenTree<T, IdAdapter<T>, C>::set(value); }
	template <class CC> inline void addAll(const CC& coll)
		{ for(typename CC::Iterator iter(coll); iter; iter++) GenTree<T, IdAdapter<T>, C>::set(iter); }
};
template <class T, class C> const Set<T, C> Set<T, C>::null;
template <class K, class C> inline Set<K, C>& operator+=(Set<K, C> &t, const K& h) { t.add(h); return t; }
template <class K, class C> inline Set<K, C>& operator+=(Set<K, C> &t, const Set<K, C>& s) { t.addAll(s); return t; }

} }	// elm::avl

#endif /* ELM_AVL_SET_H_ */
