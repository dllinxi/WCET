/*
 *	avl::Map class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011, IRIT UPS.
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
#ifndef ELM_AVL_MAP_CPP_
#define ELM_AVL_MAP_CPP_

#include <elm/util/Option.h>
#include <elm/avl/GenTree.h>

namespace elm { namespace avl {

// Map class
template <class K, class T, class C = Comparator<K> >
class Map {
	typedef Pair<typename ti<K>::embed_t, typename ti<T>::embed_t> pair_t;
	typedef GenTree<pair_t, PairAdapter<K, T>, C > tree_t;
	typedef typename ti<K>::in_t key_t;
	typedef typename ti<T>::in_t val_t;
	typedef typename ti<T>::out_t out_t;
public:

	// Map concept
	inline Option<T> get(key_t key) const
		{ const pair_t *p = tree.get(key); if(!p) return none; else return some(ti<T>::get(p->snd)); }
	inline val_t get(key_t key, val_t def) const
		{ const pair_t *p = tree.get(key); if(!p) return def; else return ti<T>::get(p->snd); }
	inline bool hasKey (key_t key) const
		{ const pair_t *p = tree.get(key); return p; }
	inline int count(void) const { return tree.count(); }
	inline bool isEmpty(void) const { return tree.isEmpty(); }
	inline bool equals(const Map<K, T, C>& map) const { return tree.equals(map.tree); }
	inline bool operator==(const Map<K, T, C>& map) const { return equals(map); }
	inline bool operator!=(const Map<K, T, C>& map) const { return !equals(map); }

	// KeyIterator class
	class KeyIterator: public PreIterator<KeyIterator, K> {
	public:
		inline KeyIterator(const Map<K, T, C>& map): it(map.tree) { }
		inline KeyIterator(const KeyIterator& iter): it(iter.it) { }
		inline KeyIterator& operator=(const KeyIterator& iter) { it = iter; return *this; }
		inline bool ended(void) const { return it.ended(); }
		inline void next(void) { it.next(); }
		inline key_t item(void) const { return it.item().fst; }
	private:
		typename tree_t::Iterator it;
	};

	// PairIterator class
	class PairIterator: public tree_t::Iterator {
	public:
		inline PairIterator(const Map<K, T, C>& map): tree_t::Iterator(map.tree) { }
		inline PairIterator(const PairIterator& iter): tree_t::Iterator(iter) { }
		inline PairIterator& operator=(const PairIterator& iter) { tree_t::Iterator::operand=(iter) ; return *this; }
	};

	// MutableIterator class
	class MutableIterator: public PairIterator {
	public:
		inline MutableIterator(Map<K, T, C>& map): PairIterator(map) { }
		inline MutableIterator(const MutableIterator& iter): PairIterator(iter) { }
		inline MutableIterator& operator=(const MutableIterator& iter) { PairIterator::operator=(iter); return *this; }
		inline void set(const T& value) { PairIterator::data().snd = value; }
	};

	// Iterator class
	/*class Iterator: public PairIterator {
	public:
		inline Iterator(const Map<K, T, C>& map): PairIterator(map) { }
		inline Iterator(const Iterator& iter): PairIterator(iter) { }
		inline typename ti<T>::val_t item(void) const { return ti<T>::get(PairIterator::item().snd); }
	};*/


	// KeyIterator class
	class Iterator: public PreIterator<Iterator, T> {
	public:
		inline Iterator(const Map<K, T, C>& map): it(map.tree) { }
		inline Iterator(const Iterator& iter): it(iter.it) { }
		inline Iterator& operator=(const Iterator& iter) { it = iter; return *this; }
		inline bool ended(void) const { return it.ended(); }
		inline void next(void) { it.next(); }
		inline val_t item(void) const { return it.item().snd; }
	private:
		typename tree_t::Iterator it;
	};


	// MutableMap concept
	inline Option<T> get(key_t key)
		{ pair_t *p = tree.get(key); if(!p) return none; else return some(ti<T>::get(p->snd)); }
	inline T & get(const K &key, T& def)
		{ const pair_t *p = tree.get(key); if(!p) return def; else return one(p->snd); }
	inline void put(const K &key, const T &value) { tree.set(pair_t(key, value)); }
	inline void remove(const K &key) { tree.remove(key); }
	inline void remove(const PairIterator &iter) { tree.remove(iter.item().fst); }
	inline void clear(void) { tree.clear(); }
	inline void copy(const Map<K, T, C>& map) { tree.copy(map.tree); }
	inline Map<K, T, C>& operator=(const Map<K, T, C>& map) { copy(map); return *this; }

private:
	tree_t tree;
};

} }		// elm::genstruct

#endif /* ELM_GENSTRUCT_AVLMAP_CPP_ */
