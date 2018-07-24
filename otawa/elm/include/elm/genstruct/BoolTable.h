/*
 *	$Id$
 *	Plugin class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007, IRIT UPS.
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
#ifndef ELM_GENSTRUCT_BOOLTABLE_H
#define ELM_GENSTRUCT_BOOLTABLE_H

#include <elm/util/BitVector.h>
#include <elm/genstruct/Table.h>
#include <elm/util/delegate.h>

namespace elm { namespace genstruct {


// Table<bool> class
template <>
class Table<bool> {
public:
	static Table<bool> EMPTY;

	inline Table(bool *table, int count): bvec(count)
		{ for(int i = 0; i < count; i++) if(table[i]) bvec.set(i); }
	inline Table(const Table<bool>& table): bvec(table.bvec.size())
		{ copy(table); }
	
	// Accessors
	typedef ArrayDelegate<Table<bool>, int, bool> delegate_t; 
	inline int count(void) const { return bvec.size(); }
	inline bool get(int index) const { return bvec.bit(index); }
	inline delegate_t get(int index) { return delegate_t(*this, index); } 
	inline void set(int index, bool value) { bvec.set(index, value); }
	inline bool isEmpty(void) const { return bvec.isEmpty(); }
	
	// Mutators
	inline void copy(const Table<bool>& table) { bvec = table.bvec; }

	// Operators
	inline bool operator[](int index) const { return get(index); } 
	inline delegate_t operator[](int index) { return get(index); }
	inline Table<bool>& operator=(const Table<bool>& table)
		{ copy(table); return *this; }
	inline operator bool(void) const { return !isEmpty(); }

protected:
	BitVector bvec;
	inline Table(void) { }
};


// AllocatedTable<bool> class
template <>
class AllocatedTable<bool>: public Table<bool> {
public:
	inline AllocatedTable(void) { }
	inline AllocatedTable(int count) { allocate(count); }
	inline AllocatedTable(const Table<bool>& table): Table<bool>(table) { }

	inline void allocate(int count) { bvec.resize(count); }
	inline void free(void) { }
	inline void copy(const AllocatedTable<bool>& table)
		{ bvec.copy(table.bvec); }
	inline AllocatedTable<bool>& operator=(const AllocatedTable<bool>& table)
		{ copy(table); return *this; }
};

} } // elm::genstruct

#endif /* ELM_GENSTRUCT_BOOLTABLE_H */
