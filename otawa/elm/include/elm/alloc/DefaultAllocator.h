/*
 *	$Id$
 *	DefaultAllocator class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
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
#ifndef ELM_ALLOC_DEFAULTALLOCATOR_H_
#define ELM_ALLOC_DEFAULTALLOCATOR_H_

#include <elm/types.h>
#include <elm/util/Exception.h>

namespace elm {

// BadAlloc exception
class BadAlloc: public Exception {
public:
	virtual ~BadAlloc(void);
	virtual String 	message(void);
};

// DefaultAllocator class
class DefaultAllocator {
public:
	static DefaultAllocator DEFAULT;
	virtual void *allocate(t::size size) throw(BadAlloc);
	virtual bool mark(void *data, t::size size);
	inline void free(void *block) { delete [] (char *)block; }
	virtual ~DefaultAllocator() { }
};

}	// elm

#endif /* ELM_ALLOC_DEFAULTALLOCATOR_H_ */
