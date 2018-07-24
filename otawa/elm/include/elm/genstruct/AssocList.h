/*
 *	$Id$
 *	Comparator class interface
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
#ifndef ELM_GENSTRUCT_ASSOCLIST_H_
#define ELM_GENSTRUCT_ASSOCLIST_H_

#include <elm/PreIterator.h>
#include <elm/type_info.h>
#include <elm/util/Option.h>
#include <elm/compare.h>
#include <elm/genstruct/SortedSLList.h>

namespace elm { namespace genstruct {

// AssocList class
template <class K, class T, class C = Comparator<K>, class E = Equiv<T>, class N = type_info<T> >
class AssocList {
	typedef Pair<K, T> pair_t;
	typedef SortedSLList<pair_t, AssocComparator<K, T, C> > list_t;

public:
	inline AssocList(void) { }
	inline AssocList(const AssocList& alist): list(alist.list) { }

	class PairIterator: public list_t::Iterator {
	public:
		PairIterator(const AssocList& list): list_t::Iterator(list.list) { }
		PairIterator(const PairIterator& iter): list_t::Iterator(iter) { }
		PairIterator& operator=(const PairIterator& iter)
			{ list_t::Iterator::operator=(iter); return *this; }
	};

	// Collection concept
	inline int count(void) const { return list.count(); }
	inline bool contains(const T& item) const {
		for(Iterator iter(*this); iter; iter++)
			if(E::equals(item, iter)) return true;
		return false;
	}
	inline bool isEmpty(void) const { return list.isEmpty(); }
	inline operator bool (void) const { return !list.isEmpty(); }

	class Iterator: public PreIterator<Iterator, T> {
	public:
		inline Iterator(const AssocList& list): iter(list) { }
		inline Iterator(const Iterator& iterator): iter(iterator.iter) { }
		inline Iterator& operator=(const Iterator& iterator)
			{ iter = iterator.iter; return *this; }

		inline bool ended(void) const { return iter.ended(); }
		inline void next(void) { iter.next(); }
		inline T item(void) const { return iter.item().snd; }
	private:
		PairIterator iter;
	};

	// List concept
	inline const T& first(void) const { return list.first().snd; }
	inline const T& last (void) const { return list.last().snd; }
	inline Iterator find (const T& item) {
		Iterator iter(*this);
		for(; iter; iter++) if(E::equals(iter, item)) break;
		return iter;
	}
	inline Iterator find (const T& item, const Iterator &iter) {
		for(iter++; iter; iter++) if(E::equals(iter, item)) break;
		return iter;
	}

	// Map concept
	Option<T> get(const K &key) const
		{ typename list_t::Iterator item = list.find(pair(key, N::null));
		if(!item.ended()) return some((*item).snd); else return none; }
	T get(const K& key, const T& def) const
		{ typename list_t::Iterator item = list.find(pair(key, N::null));
		if(item) return (*item).snd; else return def; }
	bool hasKey(const K &key) const
		{ return list.contains(pair(key, N::null)); }

	class KeyIterator: public PreIterator<KeyIterator, const K&> {
	public:
		inline KeyIterator(const AssocList& list): iter(list) { }
		inline KeyIterator(const KeyIterator& iterator): iter(iterator.iter) { }
		inline KeyIterator& operator=(const KeyIterator& iterator)
			{ iter = iterator.iter; return *this; }

		inline bool ended(void) const { return iter.ended(); }
		inline void next(void) { iter.next(); }
		inline const K& item(void) const { return iter.item().fst; }
	private:
		PairIterator iter;
	};

	// MutableMap class
	inline void put(const K& key, const T& value)
		{ list.add(pair(key, value)); }
	inline void remove(const K& key)
		{ list.remove(pair(key, N::null)); }
	inline void remove(const PairIterator& iter)
		{ list.remove(iter); }

private:
	list_t list;
};

} } // elm::genstruct

#endif /* ELM_GENSTRUCT_ASSOCLIST_H_ */
