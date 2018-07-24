/*
 *	$Id$
 *	Collection class interface
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
#ifndef ELM_DATASTRUCT_COLLECTION_H
#define ELM_DATASTRUCT_COLLECTION_H

#include <elm/datastruct/AbstractCollection.h>

namespace elm { namespace datastruct {

// Collection class
template <class T, template <class _> class C>
class Collection: public virtual AbstractCollection<T> {
public:
	inline C<T>& collection(void) { return coll; }
	
	virtual ~Collection(void) { }
	virtual int count(void) { return coll.count(); }
	virtual bool contains(const T& item) const { return coll.contains(item); }
	virtual bool isEmpty(void) const { return coll.isEmpty(); }
	virtual IteratorInst<const T&> *iterator(void) const { return new Iterator(coll); }

protected:
	C<T> coll;
	class Iterator: public IteratorInst<const T&> {
	public:
		inline Iterator(const C<T>& coll): iter(coll) { }
		virtual bool ended(void) const { return iter.ended(); }
		virtual const T& item(void) const { return iter.item(); }
		virtual void next(void) { iter.next(); }
		typename C<T>::Iterator iter;
	};
};

// MutableCollection class
template <class T, template <class _> class C>
class MutableCollection: public Collection<T, C>, public MutableAbstractCollection<T> {
public:
	virtual void clear(void) { Collection<T, C>::coll.clear(); }
	virtual void add(const T& item) { Collection<T, C>::coll.add(item); }
	virtual void addAll(const AbstractCollection<T>& items)
		{ for(Iterator<const T&> iter(items.iterator()); iter; iter++) add(iter); }
	virtual void remove(const T& item) { Collection<T, C>::coll.remove(item); }
	virtual void removeAll(const AbstractCollection<T>& items)
		{ for(Iterator<const T&> iter(items.iterator()); iter; iter++) remove(iter); }
	virtual void remove(const Iterator<const T&>& iter)
		{ Collection<T, C>::coll.remove(
			static_cast<typename Collection<T, C>::Iterator *>(iter.instance())->iter); }
};

} } // elm::datastruct

#endif	// ELM_DATASTRUCT_COLLECTION_H
