/*
 *	SortedSLList class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-08, IRIT UPS.
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
#ifndef ELM_GENSTRUCT_SORTEDSLLIST_H
#define ELM_GENSTRUCT_SORTEDSLLIST_H

#include <elm/genstruct/SLList.h>
#include <elm/compare.h>

namespace elm { namespace genstruct {

// SortedSLList class
template <class T, class R = Comparator<T> >
class SortedSLList {
	typedef genstruct::SLList<T, CompareEquiv<T, R> > list_t;
public:
	inline SortedSLList(void) { }
	SortedSLList(SortedSLList & source): list(source.list) { }

	inline void removeFirst(void) { list.removeFirst(); }
	inline void removeLast(void) { list.removeLast(); }

	// Collection concept
	inline int count (void) const { return list.count(); }
	bool contains(const T &item) const;
	inline bool isEmpty(void) const { return list.isEmpty(); }
	inline operator bool(void) const { return !list.isEmpty(); }

	class Iterator: public list_t::Iterator {
	public:
		inline Iterator(void) { }
		inline Iterator(const SortedSLList& _list)
			: list_t::Iterator(_list.list) { }
		inline Iterator(const Iterator& source)
			: list_t::Iterator(source) { }
	private:
		friend class SortedSLList;
		inline Iterator(const typename list_t::Iterator& iter)
			: list_t::Iterator(iter) { }
	};

	// MutableCollection concept
	inline void clear(void) { list.clear(); }
	void add (const T &item);
	template <template <class _> class C> inline void addAll (const C<T> &items)
		{ for(typename C<T>::Iterator item(items); item; item++)
			add(item); }
	inline void remove(const T &item) { list.remove(item); }
	template <template <class _> class C> inline void removeAll(const C<T> &items)
		{ list.removeAll(items); }
	void remove(const Iterator &iter) { list.remove(iter); }

	// List concept
	inline const T& first(void) const { return list.first(); }
	inline const T& last(void) const { return list.last(); }
	inline Iterator find(const T& item) const
		{ return Iterator(list.find(item)); }
	inline Iterator find(const T& item, const Iterator& iter) const
		{ return list.find(item, iter); }

private:
	list_t list;
};


// SortedSLList inlines
template <class T, class R>
void SortedSLList<T,R>::add(const T& value) {
	for(Iterator current(*this); current; current++)
		if(R::compare(value,  *current) < 0) {
			list.addBefore(current, value);
			return;
		}
	list.addLast(value);
}

template <class T, class R>
bool SortedSLList<T, R>::contains (const T &item) const {
	for (Iterator current(*this); current; current++) {
		int cmp = R::compare(item,  *current);
		if(cmp > 0)
			continue;
		else if(!cmp)
			return true;
		else
			break;
	}
	return false;
}

} } // elm::genstruct

#endif
