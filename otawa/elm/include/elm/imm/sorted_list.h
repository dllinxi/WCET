/*
 *	imm::sorted_list class interface
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
#ifndef ELM_IMM_SORTED_LIST_H_
#define ELM_IMM_SORTED_LIST_H_

#include <elm/compare.h>
#include <elm/imm/list.h>
#include <elm/adapter.h>

namespace elm { namespace imm {

template <class T, class K = IdAdapter<T>, class C = Comparator<K> >
class sorted_list: public list<T> {
public:

	inline bool contains(const typename K::t& k) const {
		sorted_list<T, K, C> l = *this;
		while(l) {
			int r = C::compare(k, K::key(l.hd()));
			if(r == 0) return true;
			else if(r < 0) break;
			else l = l.tl(); };
		return false;
	}

	/*inline sorted_list<T, K, C> remove(const typename K::t& k) const {
		if(isEmpty()) return *this;
	}*/
	/*inline bool remove(const K::t& k) const
		{ if(list<T>::isEmpty()) return *this; }*/
		/*if(C::compare(k, hd()) == 0) return tl(); }
		sorted_list<T, K, C> t = tl().remove(k); if(t.node == tl().node) return *this; else return cons(hd(), t); }*/

};

} }		// elm::imm

#endif /* ELM_IMM_SORTED_LIST_H_ */
