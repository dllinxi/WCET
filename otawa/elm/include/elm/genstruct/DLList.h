/*
 *	$Id$
 *	DLList class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-08, IRIT UPS.
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
#ifndef ELM_GENSTRUCT_DLLIST_H
#define ELM_GENSTRUCT_DLLIST_H

#include <elm/util/Equiv.h>
#include <elm/PreIterator.h>
#include <elm/inhstruct/DLList.h>

namespace elm { namespace genstruct {

// DLList<T> class
template <class T, class E = Equiv<T> >
class DLList {
	
	// DLNode class
	class DLNode: public inhstruct::DLNode {
		T val;
	public:
		inline DLNode(const T value);
		inline const T& value(void) const;
		inline T& value(void) { return val; }
	};
	
	inhstruct::DLList list;
public:
	inline ~DLList(void);

	class AbstractIterator {
	public:
		inline AbstractIterator(const DLList<T>& _list, DLNode *_cur)
			: list(&_list.list), cur(_cur) { }
		inline AbstractIterator(const AbstractIterator& iter)
			: list(iter.list), cur(iter.cur) { }
		inline AbstractIterator& operator=(const AbstractIterator& iter)
			{ list = iter.list; cur = iter.cur; return *this; }
	protected:
		const inhstruct::DLList *list;
		DLNode *cur;
	};

	// Collection concept
	inline int count(void) const;
	inline bool contains(const T& value) const;
	inline bool isEmpty(void) const;
	inline operator bool(void) const { return !isEmpty(); }
	
	class Iterator: public PreIterator<Iterator, const T&>, public AbstractIterator {
	public:
		inline Iterator(const DLList& _list)
			: AbstractIterator(_list, (DLNode *)_list.list.first()) { }
		inline Iterator(const AbstractIterator& iter)
			: AbstractIterator(iter) { }
		inline Iterator& operator=(const AbstractIterator& iter)
			{ AbstractIterator::operator=(iter); return *this; }
		
		inline bool ended(void) const { return this->cur->atEnd(); }
		inline const T& item(void) const { return this->cur->value(); }
		inline void next(void) { this->cur = (DLNode *)this->cur->next(); }
	};
	
	// MutableCollection concept
	inline void clear(void);
	inline void add(const T& item) { addLast(item); }
	template <template <class _> class C> inline void addAll(const C<T>& items);
	inline void remove(const T& value);
	template <template <class _> class C> inline void removeAll(const C<T>& items);
	void remove(const AbstractIterator &iter);
	
	// List concept
	inline const T& first(void) const;
	inline const T& last(void) const;
	inline Iterator find(const T& item) const {
		Iterator iter(*this);
		for(; iter; iter++) if(E::equals(item, iter)) break;
		return iter;
	}
	inline Iterator find(const T& item, const Iterator& iter) const {
		for(iter++; iter; iter++) if(E::equals(item, iter)) break;
		return iter;
	}
	
	// MutableList concept
	inline void addFirst(const T& value);
	inline void addLast(const T& value);
	inline void removeFirst(void);
	inline void removeLast(void);
	inline void addAfter(const AbstractIterator &pos, const T& item)
		{ ASSERTP(!pos.cur->atEnd(), "insert after end");
		pos.cur->insertAfter(new DLNode(item)); }
	inline void addBefore(const AbstractIterator &pos, const T& item)
		{ ASSERTP(!pos.cur->atBegin(), "insert before begin");
		pos.cur->insertBefore(new DLNode(item)); }
	inline void set(const AbstractIterator &pos, const T& item)
		{ ASSERTP(!pos.cur->atBegin() && !pos.cur->atEnd(), "bad position");
		pos.cur->val = item; }

	// BiDiList concept
	class BackIterator: public PreIterator<Iterator, const T&>, public AbstractIterator {
	public:
		inline BackIterator(const DLList& _list)
			: AbstractIterator(_list,	(DLNode *)_list.list.last()) { }
		inline BackIterator(const AbstractIterator& iter)
			: AbstractIterator(iter) { }
		inline BackIterator& operator=(const BackIterator& iter)
			{ AbstractIterator::operator=(iter); return *this; }
		
		inline bool ended(void) const { return this->cur->atBegin(); }
		inline const T& item(void) const { return this->cur->value(); }
		inline void next(void) { this->cur = (DLNode *)this->cur->previous(); }
	};

	// Queue concept
	inline const T &head(void) const { return first(); }
	inline T get(void) { T res = first(); removeFirst(); return res; }
	inline void put(const T &item) { addLast(item); }
	inline void reset(void) { clear(); }
};


// DLList<T>::DLNode methods
template <class T, class E>
DLList<T, E>::DLNode::DLNode(const T value): val(value) { }
template <class T, class E>
const T& DLList<T, E>::DLNode::value(void) const { return val; }


// DLList<T> methods
template <class T, class E> inline DLList<T, E>::~DLList(void) { clear(); }

template <class T, class E> const T& DLList<T, E>::first(void) const
	{ return ((DLNode *)list.first())->value(); }

template <class T, class E> const T& DLList<T, E>::last(void) const
	{ return ((DLNode *)list.last())->value(); }

template <class T, class E> bool DLList<T, E>::isEmpty(void) const
	{ return list.isEmpty(); }

template <class T, class E> int DLList<T, E>::count(void) const {
	return list.count();
}
template <class T, class E> void DLList<T, E>::addFirst(const T& value)
	{ list.addFirst(new DLNode(value)); }

template <class T, class E> void DLList<T, E>::addLast(const T& value)
	{ list.addLast(new DLNode(value)); }

template <class T, class E> void DLList<T, E>::removeFirst(void)
	{ list.removeFirst(); }

template <class T, class E> void DLList<T, E>::removeLast(void)
	{ list.removeLast(); }

template <class T, class E> void DLList<T, E>::remove(const T& value) {
	for(DLNode *cur = (DLNode *)list.first(); !cur->atEnd(); cur = (DLNode *)cur->next())
		if(E::equals(cur->value(), value)) {
			cur->remove();
			delete cur;
			break;
		}
}

template <class T, class E> bool DLList<T, E>::contains(const T& value) const {
	for(DLNode *cur = (DLNode *)list.first(); !cur->atEnd(); cur = (DLNode *)cur->next())
		if(E::equals(cur->value(), value))
			return true;
	return false;
}

template <class T, class E> void DLList<T, E>::clear(void) {
	while(!list.isEmpty()) {
		DLNode *node = (DLNode *)list.first();
		list.removeFirst();
		delete node;
	}
}

template <class T, class E> template <template <class _> class C>
inline void DLList<T, E>::addAll(const C<T>& items) {
	for(typename C<T>::Iterator iter(items); iter; iter++)
		add(iter);
}

template <class T, class E> template <template <class _> class C>
inline void DLList<T, E>::removeAll(const C<T>& items) {
	for(typename C<T>::Iterator iter(items); iter; iter++)
		remove(iter);	
}

template <class T, class E>
void DLList<T, E>::remove(const AbstractIterator &iter)
	{ iter.cur.remove(); }

} }	// elm::genstruct


#endif	// ELM_GENSTRUCT_DLLIST_H
