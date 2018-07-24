/*
 *	Vector class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2016, IRIT UPS.
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
#ifndef INCLUDE_ELM_DATA_VECTOR_H_
#define INCLUDE_ELM_DATA_VECTOR_H_

#include "Manager.h"
#include "Table.h"
#include <elm/util/array.h>

namespace elm {

template <class T, class M = EquivManager<T> >
class Vector {
	inline T *allocate(int size)
		{	T *t = static_cast<T *>(_man.alloc.allocate(size * sizeof(T)));
			array::init(t, size); return t; }
	inline void free(T *t) { _man.alloc.free(t); }

public:
	inline Vector(int _cap = 8, M& m = Single<M>::_): _man(m), tab(allocate(_cap)), cap(_cap), cnt(0) { }
	inline Vector(const Vector<T>& vec): _man(vec._man), tab(0), cap(0), cnt(0) { copy(vec); }
	inline ~Vector(void) { if(tab) free(tab); }

	// Iterator
	class Iter: public PreIterator<Iter, const T&> {
	public:
		friend class Vector;
		inline Iter(const Vector& vec): _vec(vec), i(0) { }
		inline bool ended(void) const { return i >= _vec.length(); }
		inline const T& item(void) const { return _vec[i]; }
		inline void next(void) { i++; }
		inline int index(void) { return i; }
	private:
		const Vector<T>& _vec;
		int i;
	};

	// specific methods
	inline int capacity(void) const { return cap; }
	void grow(int new_cap)
		{	ASSERTP(new_cap >= cap, "new capacity must be bigger than old one");
			cap = new_cap; T *new_tab = allocate(cap); array::copy(new_tab, tab, cnt); free(tab); tab = new_tab; }
	void setLength(int new_length)
		{	int new_cap; ASSERTP(new_length >= 0, "new length must be >= 0");
			for(new_cap = 1; new_cap < new_length; new_cap *= 2);
			if (new_cap > cap) grow(new_cap); cnt = new_length; }
	void copy(const Vector& vec)
		{	if(!tab || vec.cnt > cap) { if(tab) free(tab); cap = vec.cap; tab = allocate(vec.cap); }
			cnt = vec.cnt; array::copy(tab, vec.tab, cnt); }
	inline Table<T> detach(void)
		{ T *rt = tab; int rc = cnt; tab = 0; cnt = 0; return Table<T>(rt, rc); }

	// Collection concept
	static const Vector<T, M> null;
	inline int count(void) const { return cnt; }
	bool contains(const T& v) const
		{ for(Iter i(*this); i; i++) if(v == *i) return true; return false; }
	template <template <class _> class C> inline bool containsAll(const C<T>& items)
		{ for(typename C<T>::Iter item(items); item; item++) if(!contains(item)) return false; return true; }
	inline bool isEmpty(void) const { return cnt == 0; }
	inline operator bool(void) const { return cnt != 0; }
	inline Iter operator*(void) const { return Iter(*this); }

	// MutableCollection concept
	inline void clear(void) { cnt = 0; }
	void add(const T& v) { if(cnt >= cap) grow(cap * 2); tab[cnt++] = v; }
	template <class C> inline void addAll(const C& c)
		{ for(typename C::Iter i(c); i; i++) add(i); }
	inline void remove(const T& value) { int i = indexOf(value); if(i >= 0) removeAt(i); }
	template <template <class _> class C> inline void removeAll(const C<T>& items)
		{ for(typename C<T>::Iter item(items); item; item++) remove(item); }
	inline void remove(const Iter& i) { removeAt(i.i); }

	// Array concept
	inline int length(void) const { return count(); }
	inline const T& get(int i) const
		{ ASSERTP(0 <= i && i < cnt, "index out of bounds"); return tab[i]; }
	inline int indexOf(const T& v, int p = 0) const
		{ ASSERTP(0 <= p && p <= cnt, "index out of bounds");
		for(int i = p; i < cnt; i++) if(_man.eq.isEqual(v, tab[i])) return i; return -1; }
	inline int lastIndexOf(const T& v, int p = -1) const
		{	ASSERTP(p <= cnt, "index out of bounds");
			for(int i = (p < 0 ? cnt : p) - 1; i >= 0; i--) if(_man.eq.isEqual(v, tab[i])) return i; return -1; }
	inline const T & operator[](int i) const { return get(i); }

	// MutableArray concept
	inline void shrink(int l)
		{ ASSERTP(0 <= l && l < cnt, "bad shrink value"); cnt = l; }
	inline void set(int i, const T& v)
		{ ASSERTP(0 <= i && i < cnt, "index out of bounds"); tab[i] = v; }
	inline void set (const Iter &i, const T &v) { set(i.i, v); }
	inline T& get(int index)
		{ ASSERTP(index < cnt, "index out of bounds"); return tab[index]; }
	inline T & operator[](int i) { return get(i); }
	void insert(int i, const T& v)
		{	ASSERTP(0 <= i && i <= cnt, "index out of bounds");
			if(cnt >= cap) grow(cap * 2); array::move(tab + i + 1, tab + i, cnt - i);
			tab[i] = v; cnt++; }
	inline void insert(const Iter &i, const T &v) { insert(i.i, v); }
	void removeAt(int i)
		{ array::move(tab + i, tab + i + 1, cnt - i - 1); cnt--; }
	inline void removeAt(const Iter& i) { removeAt(i.i); }

	// List concept
	inline const T& first(void) const { ASSERT(cnt > 0); return tab[0]; }
	inline const T& last(void) const { ASSERT(cnt > 0); return tab[cnt - 1]; }
	inline Iter find(const T &v)
		{ Iter i(*this); while(i && !_man.eq.isEquals(*i, v)) i++; return i; }
	inline Iter find (const T &v, const Iter &p)
		{ Iter i(p); while(i && !_man.eq.isEquals(*i, v)) i++; return i; }

	// MutableList concept
	inline void addFirst(const T &v) { insert(0, v); }
	inline void addLast(const T &v) { add(v); }
	inline void removeFirst(void) { removeAt(0); }
	inline void removeLast(void) { removeAt(cnt - 1); }
	inline void addAfter(const Iter &i, const T &v) { insert(i.i + 1, v); }
	inline void addBefore(const Iter &i, const T &v) { insert(i.i, v); }

	// Stack concept
	inline const T &top(void) const { return last(); }
	inline T pop(void) { ASSERTP(cnt > 0, "no more data to pop"); cnt--; return tab[cnt]; }
	inline void push(const T &v) { add(v); }
	inline void reset(void) { clear(); }

	// operators
	inline Vector<T>& operator=(const Vector& vec) { copy(vec); return *this; };
	inline bool operator==(const Vector<T>& v) const
		{ if(length() != v.length()) return false; for(int i = 0; i < length(); i++) if(get(i) != v[i]) return false; return true; }
	inline bool operator!=(const Vector<T>& v) const { return !(*this == v); }

private:
	M& _man;
	T *tab;
	int cap, cnt;
};

template <class T, class M>
const Vector<T, M> Vector<T, M>::null;

}	// elm

#endif /* INCLUDE_ELM_DATA_VECTOR_H_ */
