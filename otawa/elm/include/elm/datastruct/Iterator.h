/*
 *	$Id$
 *	Iterator class interface
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
#ifndef ELM_DATASTRUCT_ITERATOR_H
#define ELM_DATASTRUCT_ITERATOR_H

#include <elm/PreIterator.h>

namespace elm { namespace datastruct {

// IteratorInst class
template <class T>
class IteratorInst {
public:
	virtual ~IteratorInst(void) { }
	virtual bool ended(void) const = 0;
	virtual T item(void) const = 0;
	virtual void next(void) = 0;
};


// Iterator class
template <class T>
class Iterator: public PreIterator<Iterator<T>, T> {
public:
	inline Iterator(IteratorInst<T> *_iter): iter(_iter) { }
	inline IteratorInst<T> *instance(void) const { return iter; }
	inline ~Iterator(void) { delete iter; }
	inline bool ended(void) const { return iter->ended(); }
	inline T item(void) const { return iter->item(); }
	inline void next(void) { iter->next(); }
protected:
	IteratorInst<T> *iter;
};

} } // elm::datastruct

#endif // ELM_DATASTRUCT_ITERATOR_H
