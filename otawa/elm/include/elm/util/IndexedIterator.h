/*
 *	$Id$
 *	IndexedIterator class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007-08, IRIT UPS.
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
#ifndef ELM_INDEXED_ITERATOR_H
#define ELM_INDEXED_ITERATOR_H

#include <elm/PreIterator.h>

namespace elm {

// IndexedIterator class
template <class I, class T, class C>
class IndexedIterator: public PreIterator<I, const T&> {
	typedef IndexedIterator<I, T, C> this_t;
public:

	inline IndexedIterator(void): c(0), i(0) { }
	inline IndexedIterator(const C& collection): c(&collection), i(0) { }
	inline IndexedIterator(const this_t& iter): c(iter.c), i(iter.i) { }
	inline this_t& operator=(const this_t& iter) { c = iter.c; i = iter.i; return *this; }

	inline bool ended(void) const { return !c || i >= c->size(); }
	inline const T& item(void) const { return (*c)[i]; }
	inline void next(void) { i++; }

private:
	const C *c;
	int i;
};

} // elm

#endif	// ELM_INDEXED_ITERATOR_H
