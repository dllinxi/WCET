/*
 *	$Id$
 *	VectorQueue class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-08, IRIT UPS.
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
#ifndef ELM_GENSTRUCT_VECTORQUEUE_H
#define ELM_GENSTRUCT_VECTORQUEUE_H

#include <elm/assert.h>
#include <elm/util/Equiv.h>

namespace elm { namespace genstruct {
	
// VectorQueue class
template <class T, class E = Equiv<T> >
class VectorQueue {
	int hd, tl, cap;
	T *buffer;
	void enlarge(void);
public:
	inline VectorQueue(int capacity = 4);
	inline ~VectorQueue(void);
	
	inline int capacity(void) const;
	inline int size(void) const;
	inline bool isEmpty(void) const;
	
	inline bool contains(const T& val) const {
		for(int i = hd; i != tl; i = (i + 1) & (cap - 1))
			if(E::equals(buffer[i], val))
				return true;
		return false;
	}
	
	inline void put(const T& value);
	inline const T& get(void);
	inline T& head(void) const;
	inline void reset(void);
	
	// Operators
	inline operator bool(void) const;
	inline T *operator->(void) const;
	inline T& operator*(void) const;
};

// VectorQueue implementation

template <class T, class E> void VectorQueue<T, E>::enlarge(void) {
	int new_cap = cap * 2, off = 0;
	T *new_buffer = new T[new_cap];
	if( hd > tl) {
		off = cap - hd;
		for(int i = 0; i  < off; i++)
			new_buffer[i] = buffer[hd + i];
		hd = 0;
	}
	for(int i = hd; i < tl; i++)
		new_buffer[off + i - hd] = buffer[i];
	delete [] buffer;
	tl = off + tl - hd;
	cap = new_cap;
	buffer = new_buffer;
}

template <class T, class E> inline VectorQueue<T, E>::VectorQueue(int capacity)
: hd(0), tl(0), cap(1 << capacity), buffer(new T[cap]) {
	ASSERTP(cap >= 0, "capacity must be positive");
}

template <class T, class E> inline VectorQueue<T, E>::~VectorQueue(void) {
	delete [] buffer;
}

template <class T, class E> inline int VectorQueue<T, E>::capacity(void) const {
	return cap;
}

template <class T, class E> inline int VectorQueue<T, E>::size(void) const {
	if(hd < tl)
		return tl - hd;
	else
		return (cap - hd) + tl;
}

template <class T, class E> inline bool VectorQueue<T, E>::isEmpty(void) const {
	return hd == tl;
}
	
template <class T, class E> inline void VectorQueue<T, E>::put(const T& value) {
	int new_tl = (tl + 1) & (cap - 1);
	if(new_tl == hd) {
		enlarge();
		new_tl = tl + 1;
	}
	buffer[tl] = value;
	tl = new_tl;
}

template <class T, class E> inline const T& VectorQueue<T, E>::get(void) {
	ASSERTP(hd != tl, "queue empty");
	int res = hd;
	hd = (hd + 1) & (cap - 1);
	return buffer[res];
}

template <class T, class E> inline T& VectorQueue<T, E>::head(void) const {
	ASSERTP(hd != tl, "queue empty");
	return buffer[hd];
}

template <class T, class E> inline void VectorQueue<T, E>::reset(void) {
	hd = 0;
	tl = 0;
}

template <class T, class E> inline VectorQueue<T, E>::operator bool(void) const {
	return !isEmpty();
}

template <class T, class E> inline T *VectorQueue<T, E>::operator->(void) const {
	return &head();
}

template <class T, class E> inline T& VectorQueue<T, E>::operator*(void) const {
	return head();
}

} } // elm::genstruct

#endif // ELM_GENSTRUCT_VECTORQUEUE_H

