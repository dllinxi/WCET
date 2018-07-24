/*
 *	stree::MarkerBuilder class interface
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
#ifndef ELM_STREE_MARKERBUILDER_H_
#define ELM_STREE_MARKERBUILDER_H_

#include <elm/stree/Builder.h>
#include <elm/avl/Map.h>

namespace elm { namespace stree {

template <class K, class T, class C = Comparator<K> >
class MarkerBuilder: public Builder<K, T, C> {
	typedef typename Builder<K, T, C>::node_t node_t;
public:

	void add(const K& mark, const T& val) {
		marks.put(mark, val);
	}

	void make(stree::Tree<K, T, C>& tree) {

		// allocate the memory
		node_t *nodes = Builder<K, T, C>::allocate(marks.count());

		// insert the bounds
		int i = 0;
		typename avl::Map<K, T, C>::PairIterator iter(marks);
		Pair<K, T> l = *iter;
		for(iter++; iter; iter++) {
			Pair<K, T> u = *iter;
			nodes[i] = node_t(l.fst, u.fst);
			nodes[i++].data = l.snd;
			l = u;
		}

		// build the tree
		int root = Builder<K, T, C>::make(nodes, i, 0, i - 1);

		// perform the transfer
		tree.set(root, nodes);
	}

private:
	avl::Map<K, T, C> marks;
};

} }		// elm::stree

#endif /* ELM_STREE_MARKERBUILDER_H_ */
