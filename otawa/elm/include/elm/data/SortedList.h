/*
 *	SortedList class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2012, IRIT UPS.
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
#ifndef ELM_DATA_SORTEDLIST_H_
#define ELM_DATA_SORTEDLIST_H_

#include "Adapter.h"
#include "List.h"
#include "Manager.h"
#include <elm/util/Option.h>

namespace elm {

// SortedList class
template <class T, class M = CompareManager<T>, class A = IdAdapter<T> >
class SortedList {
	typedef List<T, M> list_t;
public:
	typedef SortedList<T, M, A> self_t;
	typedef typename A::key_t key_t;
	typedef typename A::val_t val_t;

	inline SortedList(void): _man(Single<M>::_) { }
	inline SortedList(M& man): _man(man) { }
	SortedList(const SortedList &l): list(l.list), _man(l._man) { }

	inline void removeFirst(void) { list.removeFirst(); }
	inline void removeLast(void) { list.removeLast(); }

	// Collection concept
	inline int count (void) const { return list.count(); }

	bool contains(const key_t &item) const {
		for(typename list_t::Iter current(list); current; current++) {
			int cmp = _man.cmp.doCompare(item,  A::key(*current));
			if(cmp > 0) continue; else if(!cmp) return true; else break;
		}
		return false;
	}

	inline bool isEmpty(void) const { return list.isEmpty(); }
	inline operator bool(void) const { return !list.isEmpty(); }

	class Iter: public list_t::Iter {
	public:
		inline Iter(void) { }
		inline Iter(const self_t& _list): list_t::Iter(_list.list) { }
	};

	// MutableCollection concept
	inline void clear(void) { list.clear(); }

	void add(const T &value) {
		for(typename list_t::PrecIter current(list); current; current++)
			if(_man.cmp.doCompare(A::key(value),  A::key(*current)) < 0) {
				list.addBefore(current, value);
				return;
			}
		list.addLast(value);
	}

	template <template <class _> class CC> inline void addAll (const CC<T> &items)
		{ for(typename CC<T>::Iterator item(items); item; item++) add(item); }
	inline void remove(const T &item) { list.remove(item); }
	template <template <class _> class CC> inline void removeAll(const CC<T> &items)
		{ list.removeAll(items); }
	void remove(const Iter &iter) { list.remove(iter); }

	// List concept
	inline const T& first(void) const { return list.first(); }
	inline const T& last(void) const { return list.last(); }
	inline Iter find(const T& item) const
		{ return Iter(list.find(item)); }
	inline Iter find(const T& item, const Iter& iter) const
		{ return list.find(item, iter); }

private:
	list_t list;
	M& _man;
};

template <class T, class M = CompareManager<T> >
class ListSet: public SortedList<T, M> {
public:

};

template <class K, class T, class M = CompareManager<T> >
class ListMap: public SortedList<Pair<K, T>, M, PairAdapter<K, T> > {
public:
	typedef SortedList<Pair<K, T>, M, PairAdapter<K, T> > base_t;
	typedef ListMap<K, T, M> self_t;

	inline ListMap(void) { }
	inline ListMap(const ListMap<K, T, M>& l): base_t(l) { }

	class KeyIter: public PreIterator<KeyIter, K> {
	public:
		inline KeyIter(const self_t& m): i(m) { }
	private:
		typename base_t::Iter i;
	};

	inline Option<T> get(const K& k)
		{ typename base_t::Iter i = base_t::find(k); if(i) return some(*i); else return none; }
	inline const T& get(const K& k, const T& d)
		{ typename base_t::Iter i = base_t::find(k);
		if(i) return *i; else return d; }
	inline bool hasKey(const K& k)
		{ typename base_t::Iter i = base_t::find(k); return !i.ended(); }

	inline void put(const K& k, const T& v)
		{ base_t::add(pair(k, v)); }
	inline void remove(const K& k)
		{ typename base_t::Iter i = find(k); if(i) remove(*i); }
};

}	// elm

#endif /* ELM_DATA_SORTEDLIST_H_ */
