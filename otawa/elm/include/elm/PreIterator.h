/*
 *	$Id$
 *	Comparator class interface
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
#ifndef ELM_PREITERATOR_H
#define ELM_PREITERATOR_H

namespace elm {

// PreIterator class
template <class I, class T>
class PreIterator {
public:

	// operators
	inline operator bool(void) const
		{ return !((I *)this)->ended(); }
	inline operator T(void) const
		{ return ((I *)this)->item(); }
	inline I& operator++(void)
		{ ((I *)this)->next(); return *(I *)this; }
	inline I& operator++(int)
		{ ((I *)this)->next(); return *(I *)this; }
	inline T operator*(void) const
		{ return ((I *)this)->item(); }
	inline T operator->(void) const
		{ return ((I *)this)->item(); }
};

template <class I>
inline int count(I i)
	{ int c = 0; for(; i; i++) c++; return c; }

template <class I, class P>
inline bool forall(I i, P p)
	{ for(; i; i++) if(!p(*i)) return false; return true; }

template <class I, class P, class A>
inline bool forall(I i, P p, A a)
	{ for(; i; i++) if(!p(*i, a)) return false; return true; }

template <class I, class P>
inline bool exists(I i, P p)
	{ for(; i; i++) if(p.test(*i)) return true; return false; }

template <class I, class P, class A>
inline bool exists(I i, P p, A a)
	{ for(; i; i++) if(p.test(*i, a)) return true; return false; }

} // elm

#endif	// ELM_PREITERATOR_H
