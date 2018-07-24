/*
 *	$Id$
 *	AVLMap class interface
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
#ifndef ELM_GENSTRUCT_AVLMAP_CPP_
#define ELM_GENSTRUCT_AVLMAP_CPP_

#include <elm/type_info.h>
#include <elm/PreIterator.h>
#include <elm/genstruct/AVLTree.h>

namespace elm { namespace genstruct {

// AVLMap class
template <class K, class T, class C = Comparator<K> >
class AVLMap {
	typedef Pair<K, T> pair_t;
	typedef GenAVLTree<pair_t, PairAdapter<K, T>, C > tree_t;
public:

	// Map concept
	inline Option<typename type_info<T>::in_t> get(const K &key) const
		{ const pair_t *p = tree.get(key); if(!p) return none; else return some(p->snd); }
	inline const T &get(const K &key, const T &def) const
		{ const pair_t *p = tree.get(key); if(!p) return def; else return p->snd; }
	inline bool hasKey (const K &key) const
		{ const pair_t *p = tree.get(key); return p; }
	inline int count(void) const { return tree.count(); }

	// KeyIterator class
	class KeyIterator: public PreIterator<KeyIterator, const K&> {
	public:
		inline KeyIterator(const AVLMap<K, T, C>& map): it(map.tree) { }
		inline KeyIterator(const KeyIterator& iter): it(iter) { }
		inline KeyIterator& operator=(const KeyIterator& iter) { it = iter; return *this; }
		inline bool ended(void) const { return it.ended(); }
		inline void next(void) { it.next(); }
		inline const K& item(void) const { return it.item().fst; }
	private:
		typename tree_t::Iterator it;
	};

	// PairIterator class
	class PairIterator: public PreIterator<PairIterator, pair_t> {
	public:
		inline PairIterator(const AVLMap<K, T, C>& map): it(map.tree) { }
		inline PairIterator(const PairIterator& iter): it(iter) { }
		inline PairIterator& operator=(const PairIterator& iter) { it = iter; return *this; }
		inline bool ended(void) const { return it.ended(); }
		inline void next(void) { it.next(); }
		inline pair_t item(void) const { return *it; }
	private:
		typename tree_t::Iterator it;
	};

	// Iterator class
	class Iterator: public PairIterator {
	public:
		inline Iterator(const AVLMap<K, T, C>& map): PairIterator(map) { }
		inline Iterator(const Iterator& iter): PairIterator(iter) { }
	};

	// MutableMap concept
	inline Option<T> get(const K &key)
		{ pair_t *p = tree.get(key); if(!p) return none; else return some(p->snd); }
	inline T & get(const K &key, T& def)
		{ const pair_t *p = tree.get(key); if(!p) return def; else return one(p->snd); }
	inline void put(const K &key, const T &value) { tree.add(pair_t(key, value)); }
	inline void remove(const K &key) { tree.remove(key); }
	inline void remove(const PairIterator &iter) { tree.remove(iter.item().fst); }

private:
	tree_t tree;
};

} }		// elm::genstruct

#endif /* ELM_GENSTRUCT_AVLMAP_CPP_ */
