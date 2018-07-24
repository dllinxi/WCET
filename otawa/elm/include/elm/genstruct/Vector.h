/*
 *	$Id$
 *	Vector class interface
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
#ifndef ELM_GENSTRUCT_VECTOR_H
#define ELM_GENSTRUCT_VECTOR_H

#include <elm/assert.h>
#include <elm/genstruct/Table.h>
#include <elm/genstruct/Vector.h>
#include <elm/PreIterator.h>

namespace elm { namespace genstruct {


// EmbedVector class
template <class T>
class Vector {
public:
	inline Vector(int _cap = 8): tab(new T[_cap]), cap(_cap), cnt(0) { }
	inline Vector(const Vector<T>& vec): tab(0), cap(0), cnt(0) { copy(vec); }
	inline ~Vector(void) { if(tab) delete [] tab; }
	
	// Iterator
	class Iterator: public PreIterator<Iterator, const T&> {
	public:
		friend class Vector;
		inline Iterator(const Vector& vec): _vec(vec), i(0) { }
		inline Iterator(const Iterator& iter): _vec(iter._vec), i(iter.i) { }
		inline bool ended(void) const { return i >= _vec.length(); }
		inline const T& item(void) const { return _vec[i]; }
		inline void next(void) { i++; }
		inline int index(void) { return i; }
		const Vector<T>& vector(void) { return _vec; }
	private:
		const Vector<T>& _vec;
		int i;
	};
	
	// MutableIterator
	class MutableIterator: public PreIterator<MutableIterator, T&> {
	public:
		friend class Vector;
		inline MutableIterator(Vector& vec): _vec(vec), i(0) { }
		inline MutableIterator(const MutableIterator& iter): _vec(iter._vec), i(iter.i) { }
		inline bool ended(void) const { return i >= _vec.length(); }
		inline T& item(void) const { return _vec[i]; }
		inline void next(void) { i++; }
		inline int index(void) { return i; }
		Vector<T>& vector(void) { return _vec; }
	private:
		Vector<T>& _vec;
		int i;
	};

	// Accessors
	int count(void) const { return cnt; }
	inline int length(void) const { return count(); }
	inline int capacity(void) const;
	inline bool isEmpty(void) const { return cnt == 0; }
	inline const T& get(int index) const;
	inline T& item(int index);
	inline void set(int index, const T value);
	inline T& operator[](int index) { return item(index); }
	inline const T& operator[](int index) const { return get(index); }
	bool contains(const T& value) const;
	template <template <class _> class C> inline bool containsAll(const C<T>& items)
		{ for(typename C<T>::Iterator item(items); item; item++)
			if(!contains(item)) return false; return true; }
	int indexOf(const T& value, int start = 0) const;
	int lastIndexOf(const T& value, int start = -1) const;
	inline operator bool(void) const { return cnt != 0; }
	inline bool operator==(const Vector<T>& v) const
		{ if(length() != v.length()) return false; for(int i = 0; i < length(); i++) if(get(i) != v[i]) return false; return true; }
	inline bool operator!=(const Vector<T>& v) const { return !(*this == v); }
	inline const T& first(void) const
		{ ASSERT(cnt > 0); return tab[0]; }
	inline const T& last(void) const
		{ ASSERT(cnt > 0); return tab[cnt - 1]; }
	inline Iterator find(const T &item)
		{ Iterator i(*this); while(i && *i != item) i++; return i; }
	inline Iterator find (const T &item, const Iterator &start)
		{ Iterator i(start); while(i && *i != item) i++; return i; }
	inline const T& top(void) const
		{ ASSERTP(cnt > 0, "no more data in the stack"); return tab[cnt - 1]; }
	inline T& top(void)
		{ ASSERTP(cnt > 0, "no more data in the stack"); return tab[cnt - 1]; }

	// Mutators
	inline void add(void);
	inline void add(const T& value);
	template <template <class _> class C> inline void addAll(const C<T>& items)
		{ for(typename C<T>::Iterator item(items); item; item++) add(item); }
	void removeAt(int index);
	inline void remove(const T& value) { int i = indexOf(value); if(i >= 0) removeAt(i); }
	inline void remove(const Iterator& iter) { removeAt(iter.i); }
	inline void remove(const MutableIterator& iter) { removeAt(iter.i); }
	template <template <class _> class C> inline void removeAll(const C<T>& items)
		{ for(typename C<T>::Iterator item(items); item; item++) remove(item); }
	void insert(int index, const T& value);
	inline void clear(void) { cnt = 0; }
	void grow(int new_cap);
	void setLength(int new_length);
	inline table<T> detach(void);
	inline void copy(const Vector& vec);
	inline Vector<T>& operator=(const Vector& vec) { copy(vec); return *this; };
	inline void swallow(Vector<T>& v) { if(tab) delete [] tab; tab = v.tab; v.tab = 0; }
	inline void insert(const T &item) { add(item); }
	inline void push(const T& value)
		{ add(value); }
	inline const T pop(void)
		{ ASSERTP(cnt > 0, "no more data to pop"); return tab[--cnt]; }
	inline void addFirst(const T &item) { insert(0, item); }
	inline void addLast(const T &item) { add(item); }
	inline void removeFirst(void) { removeAt(0); }
	inline void removeLast(void) { removeAt(cnt - 1); }
	inline void set (const Iterator &pos, const T &item) { tab[pos.pos] = item; }
	inline void addAfter(const Iterator &pos, const T &item) { insert(pos.i + 1, item); }
	inline void addBefore(const Iterator &pos, const T &item) { insert(pos.i, item); }

private:
	T *tab;
	unsigned short cap, cnt;
};


// EmbedVector methods




template <class T>
inline table<T> Vector<T>::detach(void) {
	T *dtab = tab;
	tab = 0;
	return table<T>(dtab, cnt);
}

template <class T> int Vector<T>::capacity(void) const {
	return cap;
}
template <class T> T& Vector<T>::item(int index) {
	ASSERTP(index < cnt, "index out of bounds");
	return tab[index];
}
template <class T> const T& Vector<T>::get(int index) const {
	ASSERTP(index < cnt, "index out of bounds");
	return tab[index];
}
template <class T> void Vector<T>::set(int index, const T value) {
	ASSERTP(index < cnt, "index out of bounds");
	tab[index] = value;
}
template <class T> bool Vector<T>::contains(const T& value) const {
	for(int i = 0; i < cnt; i++)
		if(value == tab[i])
			return true;
	return false;
}
template <class T> int Vector<T>::indexOf(const T& value, int start) const {
	for(int i = start; i < cnt; i++)
		if(value == tab[i])
			return i;
	return -1;
}
template <class T> int Vector<T>::lastIndexOf(const T& value, int start) const {
	ASSERTP(start <= cnt, "index out of bounds");
	for(int i = (start < 0 ? cnt : start) - 1; i >= 0; i--)
		if(value == tab[i])
			return i;
	return -1;
}

template <class T> void Vector<T>::add(const T& value) {
	if(cnt >= cap)
		grow(cap * 2);
	tab[cnt++] = value;
}

template <class T>
inline void Vector<T>::add(void) {
	if(cnt >= cap)
		grow(cap * 2);
	cnt++;	
}

template <class T> void Vector<T>::removeAt(int index) {
	for(int i = index + 1; i < cnt; i++)
		tab[i - 1] = tab[i];
	cnt--;
}
template <class T> void Vector<T>::insert(int index, const T& value) {
	ASSERTP(index <= cnt, "index out of bounds");
	if(cnt >= cap)
		grow(cap * 2);
	for(int i = cnt; i > index; i--)
		tab[i] = tab[i - 1];
	tab[index] = value;
	cnt++;
}
template <class T> void Vector<T>::grow(int new_cap) {
	ASSERTP(new_cap > 0 && new_cap < 65536, "new capacity out of [1, 65535]");
	ASSERTP(new_cap >= cap, "new capacity must be bigger than old one");
	cap = new_cap;
	T *new_tab = new T[cap];
	for(int i =0; i < cnt; i++)
		new_tab[i] = tab[i];
	delete [] tab;
	tab = new_tab;
}
template <class T> void Vector<T>::setLength(int new_length) {
	int new_cap; 
	ASSERTP(new_length >= 0, "new length must be >= 0");
	
	for (new_cap = 1; new_cap < new_length; new_cap *= 2); 
	if (new_cap > cap) {
		grow(new_cap);
	}
	cnt = new_length;	
}

template <class T> inline void Vector<T>::copy(const Vector& vec) {
	if(!tab || vec.cnt > cap) {
		if(tab)
			delete [] tab;
		cap = vec.cap;
		tab = new T[vec.cap];
	}
	cnt = vec.cnt;
	for(int i = 0; i < cnt; i++)
		tab[i] = vec.tab[i];
}

} }	// elm::genstruct

#endif	// ELM_GENSTRUCT_VECTOR_H
