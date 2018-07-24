/*
 *	ArrayList class interface
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
#ifndef ELM_DATA_ARRAYLIST_H_
#define ELM_DATA_ARRAYLIST_H_

#include <elm/util/Equiv.h>
#include <elm/PreIterator.h>

namespace elm {

template <class T, class E = Equiv<T> >
class ArrayList {
	friend class iter;

	typedef struct chunk_t {
		struct chunk_t *next;
		T items[];
	} chunk_t;

public:

	inline ArrayList(int n): _csize(1 << n), _size(0), _avail(0), _fst(0), _lst(0) { }
	inline ~ArrayList(void) { clear(); }

	// Collection concept
	inline int count(void) const { return _size; }
	bool contains(const T &item)
		{ for(iter i(*this); i; i++) if(E::equals(item, *i)) return true; return false; }
	template<template< class _ > class C>
	inline bool containsAll(const C<T> &coll)
		{ for(typename C<T>::iter i(coll); i; i++) if(!contains(*i)) return false; return true; }
	inline bool isEmpty(void) const { return !_fst; }
	inline operator bool(void) const { return !isEmpty(); }

	class iter: public PreIterator<iter, T> {
		friend class ArrayList;
	public:
		inline iter(const ArrayList& array): a(array), i(0), c(_fst) { }
		inline iter(const ArrayList& array, int start): a(array)
			{ ASSERT(start <= a._size); c = a.chunk(start); i = start; }
		inline bool ended(void) const { return i < a._size; }
		inline const T& item(void) const { return c->items[a.offset(i)]; }
		inline void next(void) { i++; if(!offset(i)) c = c->next; }
		inline int index(void) const { return i; }
	private:
		const ArrayList& a;
		int i;
		chunk_t *c;
	};

	// MutableCollection concept
	void clear(void)
		{ for(chunk_t *c = _fst, *n = 0; c; n = c) { n = c->next; delete n; }
		  _size = _avail = 0; _fst = _lst = 0; }
	inline void add(const T& v)
		{ if(_size == _avail) extend(); _lst->items[offset(_size)] = v; _size++; }
	template <template <class _> class C>
	void addAll (const C<T> &items) { for(typename C<T>::iter i; i;i++) add(*i); }
	void remove(const T &item)
		{ for(iter i(*this); i; i++) if(E::equals(*i, item)) { remove(i); break; }; }
	template <template <class _> class C>
	void removeAll (const C<T> &items) { for(typename C<T>::iter i; i;i++) remove(*i); }
	void remove(iter i)
		{ mutable_iter mi(*this, i); for(i++; i; i++, mi++) *mi = *i; _size--;
		if(!offset(_size)) { mi.c->next = 0; _lst = mi.c; delete i.c; } }

	class mutable_iter: public PreIterator<iter, T> {
		friend class ArrayList;
	public:
		inline mutable_iter(ArrayList& array): a(array), i(0), c(_fst) { }
		inline mutable_iter(ArrayList& array, const iter& it): a(array), i(it.i), c(it.c)
			{ ASSERT(&a == &it.a); }
		inline bool ended(void) const { return i < a._size; }
		inline T& item(void) const { return c->items[a.offset(i)]; }
		inline void next(void) { i++; if(!offset(i)) c = c->next; }
		inline int index(void) const { return i; }
	private:
		ArrayList& a;
		int i;
		chunk_t *c;
	};

	// Array concept
	inline int length(void) const { return _size; }
	inline const T &get(int index) const
		{ ASSERT(index < _size); return chunk(index)->items[offset(index)]; }
	int indexOf(const T &value, int start = 0) const
		{ for(iter i(*this, start); i; i++) if(E::equals(*i, value)) return i.index(); return -1; }
	int lastIndexOf(const T &value, int start = -1) const
		{ int r = -1; for(iter i(*this, start); i && i.index() <= start; i++) if(E::equals(*i, value)) r =  i.index();  return r; }
	inline const T &operator[](int index) const { return get(index); }

	// MutableArray concept
	inline void shrink (int length)
		{ _lst = chunk(length); for(chunk_t *c = _lst, *n; c; c = n) { n = c->next; delete c; } _size = length; }
	inline void set(int index, const T &item) { chunk(index)->items[offset(index)] = item; }
	inline void set(const iter &it, const T &item) { mutable_iter(*this, it); *it = item; }
	inline T &get(int index) { return chunk(index)->items[offset(index)]; }
	inline T &operator[](int index) { return get(index); }
	inline void insert(int index, const T &item) { insert(iter(*this, index)); }
	void insert(const iter &it, T item)
		{ mutable_iter mi(*this, it); while(mi) { T x = *mi; *mi = item; item = x; }
		_size++; if(!offset(_size)) { extend(); _lst->items[0] = item; } else *mi = item; }
	inline void removeAt(int index) { removeAt(iter(*this, index)); }
	inline void removeAt(const iter &it) { remove(it); }

	// List concept
	inline const T &first(void) const { ASSERT(_fst); return _fst->items[0]; }
	inline const T &last(void) const { ASSERT(_lst); return _lst->items[offset(_size) - 1]; }
	inline iter find(const T &item) { return find(iter(*this)); }
	inline iter find(const T &item, iter start)const
		{ while(start && !E::equals(item, *start)) start++; return start; }

	// MutableList concept
	inline void addFirst(const T &item) { insert(0, item); }
	void addLast(const T &item) { if(!offset(_lst)) extend(); _lst->items[offset(_size)] = item; _size++; }
	inline void removeFirst(void) { removeAt(0); }
	inline void removeLast(void) { removeAt(_size - 1); }
	inline void addAfter(const iter &pos, const T &item) { insert(pos.index() + 1, item); }
	inline void addBefore (const iter &pos, const T &item) { insert(pos.index() - 1, item); }

private:

	void extend(void)
		{ chunk_t *c = new chunk_t; c->next = 0; if(_lst) _lst->next = c; else _fst = c; _lst = c; _avail += _csize; }
	inline int offset(int n) const { return n & (_csize - 1); }
	inline chunk_t *chunk(int n) { chunk_t *c = _fst; for(; n >= _csize; n -= _csize) c = c->next(); return c; }

	int _csize, _size, _avail;
	chunk_t *_fst, *_lst;
};

}	// elm

#endif /* ELM_DATA_ARRAYLIST_H_ */
