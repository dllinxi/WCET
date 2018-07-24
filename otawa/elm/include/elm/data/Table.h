/*
 *	Table classes interface
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
#ifndef ELM_DATA_TABLE_H_
#define ELM_DATA_TABLE_H_

#include <elm/assert.h>
#include <elm/PreIterator.h>
#include <elm/util/array.h>

namespace elm {

template <class T>
class Table {
public:
	inline Table(void): cnt(0), buf(0) { }
	inline Table(int count, T *buffer): cnt(count), buf(buffer) { }

	inline int count(void) const { return cnt; }
	inline const T *buffer(void) const { return buf; }
	inline T *buffer(void) { return buf; }

	class Iter: public PreIterator<Iter, T> {
	public:
		inline Iter(void): p(0), t(0) { }
		inline Iter(const Table<T>& table): p(table.buffer()), t(table.buffer() + table.count()) { }
		inline bool ended(void) const { return p >= t; }
		inline const T& item(void) const { return *p; }
		inline void next(void) { p++; }
	private:
		const T *p, *t;
	};
	inline Iter items(void) const { return Iter(*this); }
	inline void set(const Iter& i, const T& val) { ASSERT(buf <= i.p && i.p < buf + cnt); *i.p = val; }

	inline void set(int count, T *buffer) { cnt = count; buf = buffer; }
	inline void set(const Table<T>& t) { cnt = t.cnt; buf = t.buf; }
	inline void copy(const Table& t) { cnt = min(cnt, t.cnt); array::copy(buf, t.buf, cnt); }
	inline void fill(const T& val) { array::set(buf, cnt, val); }
	inline const T& get(int idx) const { ASSERT(0 <= idx && idx < cnt); return buf[idx]; }
	inline T& get(int idx) { ASSERT(0 <= idx && idx < cnt); return buf[idx]; }
	inline void set(int idx, const T& val) { ASSERT(0 <= idx && idx < cnt); buf[idx] = val; }

	inline const T *operator()(void) const { return buffer(); }
	inline T *operator()(void) { return buffer(); }
	inline const T& operator[](int idx) const { return get(idx); }
	inline T& operator[](int idx) { return get(idx); }
	inline Table<T>& operator=(const Table<T>& t) { set(t); return *this; }
	inline operator Iter(void) const { return items(); }
	inline Iter operator*(void) const { return items(); }

protected:
	int cnt;
	T *buf;
};

template <class T>
class AllocTable: public Table<T> {
public:
	inline AllocTable(void) { }
	inline AllocTable(int count, T *buffer): Table<T>(count, buffer) { }
	inline AllocTable(int count): Table<T>(count, new T[count]) { }
	inline AllocTable(int count, const T& val): Table<T>(count, new T[count]) { fill(val); }
	inline AllocTable(const Table<T>& t): Table<T>(t.cnt, new T[t.cnt]) { Table<T>::copy(t); }
	inline AllocTable(const AllocTable<T>& t): Table<T>(t.cnt, new T[t.cnt]) { Table<T>::copy(t); }
	inline ~AllocTable(void) { if(this->buf) delete [] this->buf; }

	inline void copy(const Table<T>& t)
		{ if(this->count() < t.count()) { if(this->buf) delete [] this->buf;
		  this->set(t.count(), new T[t.count()]); } Table<T>::copy(t); }
	inline void set(int cnt, T *buffer) { if(this->buf) delete [] this->buf; Table<T>::set(cnt, buffer); }

	inline AllocTable<T>& operator=(const Table<T>& t) { copy(t); return *this; }
	inline AllocTable<T>& operator=(const AllocTable<T>& t) { copy(t); return *this; }
};

} // elm

#endif /* ELM_DATA_TABLE_H_ */
