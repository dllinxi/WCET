/*
 *	imm::list class interface
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
#ifndef ELM_IMM_LIST_H_
#define ELM_IMM_LIST_H_

#include <elm/assert.h>
#include <elm/alloc/DefaultAllocator.h>
#include <elm/alloc/BlockAllocatorWithGC.h>
#include <elm/compare.h>
#include <elm/genstruct/SLList.h>

namespace elm { namespace imm {

template <class T>
class list {
	typedef struct node_t {
		inline node_t(const T& h, node_t *t): hd(h), tl(t) { }
		T hd;
		node_t *tl;
	} node_t;
	class GC;
public:
	class Collector {
		friend class GC;
	public:
		virtual ~Collector(void);
		inline void mark(list<T> l) { node_t *n = l.node; while(n && !gc->mark(n)) n = n->tl; }
		virtual void collect(void) = 0;
	private:
		GC *gc;
	};

private:
	class GC: public BlockAllocatorWithGC<node_t> {
	public:
		virtual void collect(void)
			{ for(typename genstruct::SLList<Collector *>::Iterator coll(colls); coll; coll++) coll->collect(); }
		inline void add(Collector& coll) { colls.add(&coll); coll.gc = this; }
		inline void remove(Collector& coll) { colls.remove(&coll); }
		inline bool mark(node_t *node) { return BlockAllocatorWithGC<node_t>::mark(node); }
	private:
		genstruct::SLList<Collector *> colls;
	};
	static GC gc;

	inline list(node_t *n): node(n) { }
public:
	static inline void add(Collector& coll) { gc.add(coll); }
	static inline void remove(Collector& coll) { gc.remove(coll); }

	static list<T> null;
	inline list(void): node(0) { }
	inline list(const list<T>& l): node(l.node) { }
	inline list<T>& operator=(list<T> l) { node = l.node; return *this; }
	static list<T> cons(const T& h, list<T> t)
		{ node_t *n = list<T>::gc.allocate(); n->hd = h; n->tl = t.node; return list<T>(n); }

	inline const T& hd(void) const { ASSERT(node); return node->hd; }
	inline list<T> tl(void) const { ASSERT(node); return node->tl; }
	inline const T& operator*(void) const { return hd(); }
	inline bool isEmpty(void) const { return !node; }
	inline operator bool(void) const { return !isEmpty(); }

	inline int length(void) const
		{ int c = 0; for(node_t *n = node; n; n = n->tl) c++; return c; }
	inline bool contains(const T& v)
		{ for(node_t *n = node; n; n = n->tl) if(Equiv<int>::equals(n->hd, v)) return true; return false; }
	inline bool equals(list<T> l) const
		{ if(node == l.node) return true; if(!node || !l.node || !Equiv<T>::equals(hd(), l.hd())) return false; return tl().equals(l.tl()); }
	inline bool operator==(list<T> l) const { return equals(l); }
	inline bool operator!=(list<T> l) const { return !equals(l); }

	inline list<T> concat(list<T> l)
		{ if(isEmpty()) return l; else return cons(hd(), tl().concat(l)); }
	inline list<T> remove(const T& h)
		{ if(!node) return *this; if(Equiv<T>::equals(h, hd())) return tl();
		list<T> t = tl().remove(h); if(node == t.node) return *this; else return cons(hd(), t); }

private:
	node_t *node;
};
template <class T> typename list<T>::GC list<T>::gc;
template <class T> list<T> list<T>::null(0);

template <class T> inline list<T> cons(const T& h, list<T> t) { return list<T>::cons(h, t); }
template <class T> inline list<T> operator+(const T& h, list<T> t) { return cons(h, t); }
template <class T> inline list<T> operator+(list<T> l1, list<T> l2) { return l1.concat(l2); }
template <class T> inline list<T> make(T a[], int n)
	{ if(!n) return list<T>::null; else return cons(*a, make(a + 1, n - 1)); }

template <class T>
io::Output& operator<<(io::Output& out, list<T> l)
	{ bool f = true; out << "[ "; for(; l; l = l.tl()) { if(f) f = false; else out << ", "; out << l.hd(); } out << " ]"; return out; }

} }	// elm::imm

#endif /* ELM_IMM_LIST_H_ */
