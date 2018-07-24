/*
 *	Table and AllocatedTable class interfaces
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
#ifndef ELM_GENSTRUCT_TABLE_H
#define ELM_GENSTRUCT_TABLE_H

#include <elm/assert.h>
#include <elm/deprecated.h>
#include <elm/util/IndexedIterator.h>
#include <elm/util/array.h>

namespace elm { namespace genstruct {

// Table class
template <class T>
class Table {
public:
	static Table<T> EMPTY;

	inline Table(void): tab(0), cnt(0) { }
	inline Table(T *table, int count): tab(table), cnt(count)
		{ ASSERTP(count == 0 || (count > 0 && table), "null pointer for table"); }
	inline Table(const Table<T>& table): tab(table.tab), cnt(table.cnt) { }

	// Accessors
	inline int size(void) const { return cnt; }
	inline const T& get(int index) const
		{ ASSERTP(index >= 0 && index < cnt, "index out of bounds"); return tab[index]; }
	inline T& get(int index)
	 	 { ASSERTP(index >= 0 && index < cnt, "index out of bounds"); return tab[index]; }
	inline void set(int index, const T& value)
		{ ASSERTP(index >= 0 && index < cnt, "index out of bounds"); tab[index] = value; }
	inline bool isEmpty(void) const { return cnt == 0; }

	// Mutators
	inline void copy(const Table<T>& table) { tab = table.tab; cnt = table.cnt; }

	// Operators
	inline const T& operator[](int index) const { return get(index); }
	inline T& operator[](int index) { return get(index); }
	inline Table<T>& operator=(const Table& table) { copy(table); return *this; }
	inline operator bool(void) const { return !isEmpty(); }
	inline const T *operator*(void) const { return tab; }
	inline T *operator*(void) { return tab; }

	// Iterator class
	class Iterator: public IndexedIterator<Iterator, T, Table<T> > {
	public:
		inline Iterator(void) { }
		inline Iterator(const Table<T>& table): IndexedIterator<Iterator, T, Table<T> >(table) { }
		inline Iterator(const Iterator &iter): IndexedIterator<Iterator, T, Table<T> >(iter) { }
	};

	// Deprecated
	inline int count(void) const { return cnt; }
	inline const T *table(void) const { ELM_DEPRECATED; return tab; }
	inline T *table(void) { ELM_DEPRECATED return tab; }

protected:
	T *tab;
	int cnt;
};

template <class T>
class DeletableTable: public Table<T> {
public:
	inline DeletableTable(void) { }
	inline DeletableTable(T *table, int count): Table<T>(table, count) { }
	inline DeletableTable(const Table<T>& table): Table<T>(table) { }
	inline ~DeletableTable(void) { if(Table<T>::tab) delete [] Table<T>::tab; }
	inline DeletableTable<T>& operator=(const Table<T>& table)
		{ Table<T>::copy(table); return *this; }
	inline DeletableTable<T>& operator=(const DeletableTable<T>& table)
		{ copy(table); return *this; }
	inline void copy(const Table<T>& t)
		{ clear(); if(t.size()) { Table<T>::copy(Table<T>(new T[t.count()], t.count())); array::copy(**this, *t, t.count()); } }
	inline void clear(void) { if(Table<T>::tab) { delete [] Table<T>::tab; copy(Table<T>::EMPTY); } }
};

// AllocatedTable class
template <class T>
class AllocatedTable: public Table<T> {
public:
	inline AllocatedTable(void) { }
	inline AllocatedTable(int count)
		: Table<T>(new T[count], count) { }
	inline AllocatedTable(const Table<T>& table): Table<T>(table ? new T[table.count()] : 0, table.count())
		{ copyItems(*table); }
	inline ~AllocatedTable(void) { free(); }

	inline void allocate(int count)
		{ free(); Table<T>::cnt = count; Table<T>::tab = new T[count]; }
	inline void free(void)
		{ if(Table<T>::tab) delete [] Table<T>::tab; Table<T>::tab = 0; Table<T>::cnt = 0; }

	inline AllocatedTable<T>& operator=(const AllocatedTable<T>& table)
		{ *this = (const Table<T>&)table; return *this; }	//
	inline AllocatedTable<T>& operator=(const Table<T>& table)
		{ allocate(table.count()); copyItems(*table); return *this; }

private:
	inline void copyItems(const T *tab)
		{ for(int i = 0; i < Table<T>::cnt; i++) (*this)[i] = tab[i]; }
};


// statics
template <class T>
Table<T> Table<T>::EMPTY;

}	// genstruct

// shortcuts
template <class T> struct table: public genstruct::Table<T> {
	inline table(void) { }
	inline table(T *table, int count): genstruct::Table<T>(table, count) { }
	inline table(const genstruct::Table<T>& table): genstruct::Table<T>(table) { }
};

template <class T> struct dtable: public genstruct::DeletableTable<T> {
	inline dtable(void) { }
	inline dtable(T *table, int count): genstruct::DeletableTable<T>(table, count) { }
	inline dtable(const genstruct::Table<T>& table): genstruct::DeletableTable<T>(table) { }
};

} // elm

#endif // ELM_GENSTRUCT_TABLE_H
