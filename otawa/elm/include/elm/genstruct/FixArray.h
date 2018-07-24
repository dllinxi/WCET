/*
 *	$Id$
 *	FixArray class interface
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
#ifndef ELM_GENSTRUCT_FIXARRAY_H_
#define ELM_GENSTRUCT_FIXARRAY_H_

#include <elm/util/Equiv.h>
#include <elm/util/array.h>

template <class T, int S, class E = Equiv<T> >
class FixArray {
public:

	inline FixArray(void) { }
	inline FixArray(const FixArray& array) { }

	// Iterator class
	class Iterator: public PreIterator<Iterator, const T&> {
	public:
		inline Iterator(const FixArray<T, S, E>& array): a(&array), i(0) { }
		inline Iterator(const Iterator& it): a(it.a), i(it.i) { }
		inline Iterator& operator=(const Iterator& it) { a = it.a; i = it.i; return *this; }

		inline bool ended(void) const { return i >= S; }
		inline const T& item(void) const { return a->get(i); }
		inline void next(void) { i++; }

	private:
		const FixArray<T, S, E> *a;
		int i;
	};

	// Array concept
	inline int length(void) { return S; }
	inline const T& get(int i) const { ASSERT(i < S); return t[i]; }
	inline const T& operator[](int i) const { return get(i); }
	inline int indexOf(const T& v, int i = 0) const
		{ for(; i < S; i++) if(E::equals(t[i], v)) return i; return -1; }
	inline int lastIndexOf(const T& v, int i = S) const
		{ for(i--; i >= 0; i--) if(E::equals(t[i], v)) return i; return -1; }

	// Mutable array concept
	inline void set(int i, const T& v) { ASSERT(i < S); t[i] = v; }
	inline void set(const Iterator& i, const T& v) { set(i.i, v); }
	inline T& get(int i) { ASSERT(i < S); return t[i]; }
	inline T& operator[](int i) { return get(i); }
	inline void insert(int i, const T& v)
		{ ASSERT(i < S); Array::copy(t + i + 1, t + i, S - i - 1); t[i] = v; }
	inline void insert(const Iterator& i, const T& v) { insert(i.i, v); }
	inline void removeAt(int i)
		{ ASSERT(i < S); Array::copy(t + i, t + i + 1, S - i - 1); }
	inline void removeAt(const Iterator& i) { removeAt(i.i); }

	// Collection concept
	inline int count(void) const { return S; }
	inline bool contains(const T& v) const { return indexOf(v) >= 0; }
	inline bool isEmpty(void) const { return false; }
	inline operator bool(void) const { return true; }
	template<template< class _> class C> bool containsAll(const C<T> &collection) {
		for(typename C<T>::Iterator it(collection); it; it++)
			if(!contains(it))
				return false;
		return true;
	}

private:
	T t[S];
};

#endif /* FIXARRAY_H_ */
