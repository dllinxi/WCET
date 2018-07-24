/*
 *	$Id$
 *	SortedBinMap class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-07, IRIT UPS.
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
#ifndef ELM_GENSTRUCT_SORTEDBINMAP_H
#define ELM_GENSTRUCT_SORTEDBINMAP_H

#include <elm/utility.h>
#include <elm/util/Pair.h>
#include <elm/genstruct/SortedBinTree.h>

namespace elm { namespace genstruct {

// SortedBinMap class
template <class K, class T, class C = Comparator<K> >
class SortedBinMap {
	typedef Pair<K, T> value_t;
	typedef genstruct::GenSortedBinTree<value_t, PairAdapter<K, T>, C> tree_t;
public:
	inline SortedBinMap(void) { }
	inline SortedBinMap(const SortedBinMap& map): tree(map.tree) { }
	
	// Collection concept
	inline int count(void) const { return tree.count(); }
	inline bool contains(const K &item) const { return tree.look(item); }
	inline bool isEmpty(void) const { return tree.isEmpty(); }
 	inline operator bool(void) const { return !isEmpty(); }

	// Iterator class
	class Iterator: public PreIterator<Iterator, const T&> {
	public:
		inline Iterator(const SortedBinMap& map): iter(map.tree) { }
		inline Iterator(const Iterator& _): iter(_) { }
		inline bool ended(void) const { return iter.ended(); }
		inline void next(void) { iter.next(); }
		const T &item(void) const { return iter.item().snd; }
	private:
		typename tree_t::Iterator iter;
	};
	
	// Map concept
	inline const T& get(const K &key, const T &def) const {
		const value_t *val = tree.look(key);
		return val ? val->snd : def;
	}
	inline Option<T> get(const K &key) const {
		const value_t *res = tree.look(key);
		return res ? Option<T>(res->snd) : none;
	}
	inline bool hasKey(const K &key) const { return tree.look(key); }

	// KeyIterator class
	class KeyIterator: public PreIterator<KeyIterator, const K&> {
	public:
		inline KeyIterator(const SortedBinMap& map): iter(map.tree) { }
		inline KeyIterator(const KeyIterator& _): iter(_) { }
		inline bool ended(void) const { return iter.ended(); }
		inline void next(void) { iter.next(); }
		const K &item(void) const { return iter.item().fst; }
	private:
		typename tree_t::Iterator iter;
	};
	
	// PairIterator class
	class PairIterator: public PreIterator<PairIterator, const value_t&> {
	public:
		inline PairIterator(const SortedBinMap& map): iter(map.tree) { }
		inline PairIterator(const PairIterator& _): iter(_.iter) { }
		inline bool ended(void) const { return iter.ended(); }
		inline void next(void) { iter.next(); }
		const value_t &item(void) const { return iter.item(); }
	private:
		typename tree_t::Iterator iter;
	};
	
	// MutableMap concept
	inline void put(const K& key, const T& value)
		{ tree.add(value_t(key, value)); }
	inline void remove(const K& key)
		{ value_t *val = tree.look(key); if(val) tree.remove(*val); }
	inline void remove(const PairIterator& iter)
		{ tree.remove(iter.iter); }

private:
	tree_t tree;
};

} } // elm::genstruct

#endif // ELM_GENSTRUCT_SORTEDBINMAP_H
