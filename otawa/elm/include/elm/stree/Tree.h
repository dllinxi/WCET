/*
 *	stree::Tree class interface
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
#ifndef ELM_STREE_TREE_H_
#define ELM_STREE_TREE_H_

#include <elm/compare.h>
#include <elm/genstruct/AVLTree.h>

namespace elm { namespace stree {

// Tree class
template <class K, class T, class C = Comparator<K> >
class Tree {
	static const int null = -1;

public:
	typedef struct node_t {
		inline node_t(void) { }
		inline node_t(struct node_t s[], int _ll, int _rl)
			: lb(s[_ll].lb), ub(s[_rl].ub), ll(_ll), rl(_rl) { }
		inline node_t(const K& _lb, const K& _ub)
			: lb(_lb), ub(_ub), ll(null), rl(null) { }
		inline bool isLeaf(void) const { return ll == null; }
		inline int left(void) const { return ll; }
		inline int right(void) const { return rl; }
		inline const K& lowerBound(void) const { return lb; }
		inline const K& upperBound(void) const { return ub; }
		K lb, ub;
		int ll, rl;
		T data;
	} node_t;

	inline Tree(void): root(-1), nodes(0) { }
	inline Tree(int _root, node_t *_nodes): root(_root), nodes(_nodes) { }
	inline void set(int _root, node_t *_nodes) { root = _root; nodes = _nodes; }

	~Tree(void) { delete [] nodes; }

	inline const T& get(const K& key, const T& def) const
		{ T *val = find(key); if(!val) return def; else return *val; }
	inline const T& get(const K& key) const
		{ T *val = find(key); ASSERTP(val, "out of tree"); return *val; }
	inline T& get(const K& key)
		{ T *val = find(key); ASSERTP(val, "out of tree"); return *val; }
	inline bool contains(const K& key) const
		{ return C::compare(key, nodes[root].lowerBound()) >= 0 && C::compare(key, nodes[root].upperBound()) <= 0; }

#	ifdef ELM_STREE_DEBUG
		void dump(io::Output& out = cout, int i = -1, int t = 0) {
			if(i < 0) i = root;
			for(int j = 0; j < t; j++) out << "| ";
			out << "|- ";
			out << nodes[i].lowerBound() << " - " << nodes[i].upperBound();
			if(nodes[i].isLeaf())
				out << " -> " << nodes[i].data << io::endl;
			else {
				out << io::endl;
				dump(out, nodes[i].left(), t + 1);
				dump(out, nodes[i].right(), t + 1);
			}
		}
#	endif

protected:
	T *find(const K& key) const {
		ASSERTP(nodes && root >= 0, "uninitialized stree");
		int i = root;
		if(!contains(key))
			return 0;
		while(!nodes[i].isLeaf()) {
			if(C::compare(key, nodes[nodes[i].right()].lowerBound()) < 0)
				i = nodes[i].left();
			else
				i = nodes[i].right();
		}
		return &nodes[i].data;
	}

private:
	int root;
	node_t *nodes;
};

} }	// elm::stree

#endif /* ELM_STREE_TREE_H_ */
