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
#ifndef ELM_STREE_SEGMENTBUILDER_H_
#define ELM_STREE_SEGMENTBUILDER_H_

#include <elm/stree/Builder.h>
#include <elm/avl/Map.h>

namespace elm { namespace stree {

template <class K, class T, class C = Comparator<K> >
class SegmentBuilder: public Builder<K, T, C> {
	typedef typename Builder<K, T, C>::node_t node_t;
	typedef Pair<K, K> segment_t;
	typedef struct {
		static int compare(const segment_t& v1, const segment_t& 	v2) { return C::compare(v1.fst, v2.fst); }
	} segment_comparator_t;
	typedef avl::Map<Pair<K, K>, T, segment_comparator_t> map_t;
	typedef typename map_t::PairIterator iter_t;

public:

	SegmentBuilder(const T& def): _def(def) { }

	void add(const K& low, const K& high, const T& val) {
		segs.put(pair(low, high), val);
	}

	void make(stree::Tree<K, T, C>& tree) {

		// count the nodes
		int cnt = 0;
		{
			iter_t iter(segs);
			if(!iter)
				return;
			cnt++;
			K p = iter.item().fst.snd;
			iter++;
			while(iter) {
				if(C::compare(iter.item().fst.fst, p) != 0)
					cnt++;
				p = iter.item().fst.snd;
				cnt++;
				iter++;
			}
		}

		// allocate the memory
		node_t *nodes = Builder<K, T, C>::allocate(cnt);

		// insert the bounds
		int i = 0;
		iter_t iter(segs);
		nodes[i] = node_t(iter.item().fst.fst, iter.item().fst.snd);
		nodes[i++].data = iter.item().snd;
		K p = iter.item().fst.snd;
		iter++;
		while(iter) {
			if(C::compare(iter.item().fst.fst, p) != 0) {
				nodes[i] = node_t(p, iter.item().fst.fst);
				nodes[i++].data = _def;
			}
			nodes[i] = node_t(iter.item().fst.fst, iter.item().fst.snd);
			nodes[i++].data = iter.item().snd;
			p = iter.item().fst.snd;
			iter++;
		}

		// build the tree
		int root = Builder<K, T, C>::make(nodes, i, 0, i - 1);

		// perform the transfer
		tree.set(root, nodes);
	}

private:
	map_t segs;
	T _def;
};

} }		// elm::stree


#endif /* ELM_STREE_SEGMENTBUILDER_H_ */
