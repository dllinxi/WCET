/*
 *	avl::GenTree class interface
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
#ifndef ELM_AVL_GENTREE_H
#define ELM_AVL_GENTREE_H

#include <elm/utility.h>
#include <elm/PreIterator.h>
#include <elm/adapter.h>
#include <elm/util/array.h>
#include <elm/genstruct/Vector.h>

namespace elm { namespace avl {

// Private class
class AbstractTree {
protected:
	inline AbstractTree(void): root(0), cnt(0) { }

	class Node {
	public:
		inline Node(void): balance(0) { links[0] = links[1] = 0; }
		Node *links[2];
		signed char balance;
	};

	void insert(unsigned char da[], int dir, Node *node, Node *q, Node *y, Node *z);
	void remove(Node *pa[], unsigned char da[], int k, Node *p);
	int count(void) const;

	Node *root;
	int cnt;
};

// GenAVLTree class
template <class T, class K = IdAdapter<T>, class C = elm::Comparator<typename K::key_t> >
class GenTree: public AbstractTree {
	static const int MAX_HEIGHT = 32;
protected:
	class Node: public AbstractTree::Node {
	public:
		inline Node(const T& item): data(item) { }
		inline Node(const Node *node): data(node->data) { }
		T data;
		inline Node *left(void) const { return (Node *)links[0]; }
		inline Node *right(void) const { return (Node *)links[1]; }
		inline Node *succ(int dir) const { return (Node *)links[dir]; }
#		ifdef ELM_DEBUG_AVL
			void dump(io::Output& out, int t) {
				for(int i = 0; i < t; i++) out << "    ";
				out << data;
				if(left()) left()->dump(out, t + 1);
				if(right()) right()->dump(out, t + 1);
			}
#		endif
	};

	Node *find(const typename K::key_t& key) const {
		for(Node *p = (Node *)root; p;) {
			int cmp = C::compare(key, K::key(p->data));
			if(cmp < 0)
				p = p->left();
			else if(cmp > 0)
				p = p->right();
			else
				return p;
		}
		return 0;
    }

public:

	GenTree(void) { }
	GenTree(const GenTree<T>& tree) { copy(tree); }
	~GenTree(void) { clear(); }

#	ifdef ELM_DEBUG_AVL
		void dump(io::Output& out) { if(root) static_cast<Node *>(root)->dump(out, 0); else out << "<empty>"; }
#	endif
	inline T *get(const typename K::key_t& key)
		{ Node *node = find(key); if(!node) return 0; else return &node->data; }
	inline const T *get(const typename K::key_t& key) const
		{ const Node *node = find(key); if(!node) return 0; else return &node->data; }

	// Collection concept
	inline int count(void) const { return cnt; }
	inline bool contains(const typename K::key_t& item) const { return find(item) != 0; }
	inline bool isEmpty(void) const { return cnt == 0; }
	inline operator bool(void) const { return !isEmpty(); }

	template <template <class _> class Co>
	inline bool containsAll(const Co<T>& coll) const
		{ for(typename Co<T>::Iterator iter(coll); iter; iter++)
			if(!contains(iter)) return false; return true; }

	// Iterator class
	class Iterator: public PreIterator<Iterator, const T&> {
	public:
		inline Iterator(const GenTree<T, K, C>& tree): sp(stack)
			{ if(tree.root) visit((Node *)tree.root); }
		inline Iterator(const Iterator& iter): sp(stack + (iter.sp - iter.stack))
			{ array::copy(stack, iter.stack, iter.sp - iter.stack); }
		inline bool ended(void) const { return sp == stack; }
		void next(void) {
			while(true) {
				Node *last = pop();
				if(ended()) return;
				if(last == top()->left()) { push(top()); break; }
				else if(last == top() && top()->right()) { visit(top()->right()); break; }
			}
		}
		inline const T& item(void) const { return top()->data; }

	protected:
		inline T& data(void) { return top()->data; }

	private:
		inline void push(Node *node) { *sp++ = node; }
		inline Node *pop(void) { return *--sp; }
		inline Node *top(void) const { return sp[-1]; }
		void visit(Node *node) {
			if(!node) return;
			push(node);
			while(top()->left()) push(top()->left());
			push(top());
		}
		Node *stack[MAX_HEIGHT + 1], **sp;
	};

	// MutableCollection concept
	void clear(void) {
		Node *stack[MAX_HEIGHT + 1];
		Node **sp = stack;
		if(root)
			*sp++ = static_cast<Node *>(root);
		while(sp != stack) {
			Node *node = *--sp;
			if(node->left()) *sp++ = node->left();
			if(node->right()) *sp++ = node->right();
			delete node;
		}
		root = 0;
		cnt = 0;
	}

	void copy(const GenTree<T, K, C>& tree) {
		clear();
		Pair<Node *, AbstractTree::Node **> stack[MAX_HEIGHT + 1], *sp = stack;
		if(tree.root)
			*sp++ = pair(static_cast<Node *>(tree.root), &root);
		while(sp != stack) {
			Pair<Node *, AbstractTree::Node **> c = *--sp;
			Node *node = new Node(c.fst);
			*c.snd = node;
			if(c.fst->left()) *sp++ = pair(c.fst->left(), &node->links[0]);
			if(c.fst->right()) *sp++ = pair(c.fst->right(), &node->links[1]);
		}
		cnt = tree.cnt;
	}
	inline GenTree<T, K, C>& operator=(const GenTree<T, K, C>& tree) { copy(tree); return *this; }

	void add(const T& item) {
		unsigned char da[MAX_HEIGHT];
		int k = 0;
		Node *z = (Node *)&root;
		Node *y = (Node *)root, *q, *p;

		// finding the insertion position
		unsigned char dir = 0;
		for(q = z, p = y; p != 0; q = p, p = p->succ(dir)) {
			int cmp = C::compare(K::key(item), K::key(p->data));
			if(p->balance != 0) {
     			z = q;
     			y = p;
     			k = 0;
			}
			dir = cmp > 0;
			da[k++] = dir;
		}

		// node creation
		Node *node = new Node(item);
		AbstractTree::insert(da, dir, node, q, y, z);
	}

	void set(const T& item) {
		unsigned char da[MAX_HEIGHT];
		int k = 0;
		Node *z = (Node *)&root;
		Node *y = (Node *)root, *q, *p;

		// finding the insertion position
		unsigned char dir = 0;
		for(q = z, p = y; p != 0; q = p, p = p->succ(dir)) {
			int cmp = C::compare(K::key(item), K::key(p->data));
			if(cmp == 0) {
				p->data = item;
				return;
			}
			if(p->balance != 0) {
     			z = q;
     			y = p;
     			k = 0;
			}
			dir = cmp > 0;
			da[k++] = dir;
		}

		// node creation
		Node *node = new Node(item);
		AbstractTree::insert(da, dir, node, q, y, z);
	}

	void remove(const typename K::key_t& item) {
		AbstractTree::Node *pa[MAX_HEIGHT];
		unsigned char da[MAX_HEIGHT];
		int k;

		// find the item
		k = 0;
		Node *p = static_cast<Node *>(root);
		for(int cmp = C::compare(item, K::key(p->data));
		cmp != 0;
		cmp = C::compare(item, K::key(p->data))) {
			int dir = cmp > 0;
			pa[k] = p;
			da[k++] = dir ;
			p = p->succ(dir);
			ASSERTP(!p, "removed item not in the tree");
		}

		// remove the item
		AbstractTree::remove(pa, da, k, p);
		delete p;
		cnt--;
	}

	inline void remove(const Iterator& iter) { remove(iter.item()); }
	template <class CC> inline void addAll(const CC& coll)
		{ for(typename CC::Iterator iter(coll); iter; iter++) add(iter); }
	template <class CC> inline void removeAll(const CC& coll)
		{ for(typename CC::Iterator iter(coll); iter; iter++) remove(iter); }

	bool equals(const GenTree<T, K, C>& tree) const {
		GenTree<T, K, C>:: Iterator ai(*this), bi(tree);
		for(; ai && bi; ai++, bi++)
			if(!(C::compare(*ai, *bi) == 0))
				return false;
		return !ai && !bi;
	}
	inline bool operator==(const GenTree<T, K, C>& tree) const { return equals(tree); }
	inline bool operator!=(const GenTree<T, K, C>& tree) const { return !equals(tree); }
};

} }	// elm::avl

#endif	// ELM_AVL_GENTREE_H
