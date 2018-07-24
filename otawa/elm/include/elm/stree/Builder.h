/*
 *	stree::Builder class interface
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
#ifndef ELM_STREE_BUILDER_H_
#define ELM_STREE_BUILDER_H_

#include <elm/stree/Tree.h>

namespace elm { namespace stree {

template <class K, class T, class C = Comparator<K> >
class Builder {
protected:
	typedef typename Tree<K, T, C>::node_t node_t;

	node_t *allocate(t::uint32 n) {
		int s = n + leastUpperPowerOf2(n) - 1;
		return new node_t[s];
	}

	int make(node_t *nodes, int& s, int start, int end) {
		if(start == end)
			return start;
		else {
			int m = (start + end) / 2,
				l = make(nodes, s, start, m),
				u = make(nodes, s, m + 1, end);
			int p = s;
			nodes[s++] = node_t(nodes, l, u);
			return p;
		}
	}
};

} }		// elm::stree

#endif /* ELM_STREE_BUILDER_H_ */
