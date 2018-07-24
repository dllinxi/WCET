/*
 *	ListQueue class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2015, IRIT UPS.
 *
 *	ELM is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	ELM is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with ELM; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef ELM_DATA_LISTQUEUE_H_
#define ELM_DATA_LISTQUEUE_H_

#include <elm/assert.h>
#include <elm/data/Manager.h>

namespace elm {

template <class T, class M = EquivManager<T> >
class ListQueue {

	class Node  {
	public:
		inline Node(const T& v, Node *n = 0): next(n), val(v) { }
		Node *next;
		T val;
		void *operator new(size_t s, M& m) { return m.alloc.allocate(s); }
		inline void free(M& m) { this->~Node(); m.alloc.free(this); }
	private:
		inline ~Node(void) { }
	};

public:
	inline ListQueue(void): h(0), t(0), _man(Single<M>::_) { }
	inline ~ListQueue(void) { reset(); }

	inline bool isEmpty(void) const { return !h; }
	inline const T &head(void) const { ASSERTP(h, "empty queue"); return h->val; }
	inline T get(void)
		{ ASSERTP(h, "empty queue"); T r = h->val; Node *n = h; h = h->next; if(!h) t = 0; n->free(_man); return r; }
	inline bool contains(const T& val)
		{ for(Node *n = h; n; n = n->next) if(_man.eq.equals(n->val, val)) return true; return false; }

	inline void put(const T &item)
		{ Node *n = new(_man) Node(item); (h ? t->next : h) = n; t = n; }
	inline void reset(void)
		{ for(Node *n = h, *nn; n; n = nn) { nn = n->next; n->free(_man); } }

	inline operator bool(void) const { return !isEmpty(); }
	inline ListQueue& operator<<(const T& v) { put(v); return *this; }
	inline ListQueue& operator>>(T& v) { v = get(); return *this; }

private:
	Node *h, *t;
	M& _man;
};

}	// elm

#endif /* ELM_DATA_LISTQUEUE_H_ */
