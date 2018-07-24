/*
 *	$Id$
 *	Fast array utilities.
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011, IRIT UPS.
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

#ifndef ELM_UTIL_ARRAY_H_
#define ELM_UTIL_ARRAY_H_

#include <new>
#include <string.h>
#include <elm/meta.h>
#include <elm/type_info.h>

namespace elm {

namespace array {

// fast copies
template <class T> class fast {
public:
	static inline void copy(T *target, const T *source, int size)
		{ ::memcpy(target, source, size * sizeof(T)); }
	static inline void move(T *target, const T *source, int size)
		{ ::memmove(target, source, size * sizeof(T)); }
	static inline void clear(T *target, int size)
		{ ::memset(target, 0, size * sizeof(target)); }
	static inline void init(T *t, int size) { }
};

// slow copies (cause of constructor, destructor, etc)
template <class T> class slow {
public:
	static inline void copy(T *target, const T *source, int size)
		{ for(int i = 0; i < size; i++) target[i] = source[i]; }
	static inline void copy_back(T *target, const T *source, int size)
		{ for(int i = size - 1; i >= 0; i--) target[i] = source[i]; }
	static inline void move(T *target, const T *source, int size)
		{ if(target < source) copy(target, source, size); else copy_back(target, source, size); }
	static inline void clear(T *target, int size)
		{ for(int i = 0; i < size; i++) target[i] = T(); }
	static inline void init(T *t, int size)
		{ for(int i = 0; i < size; i++) ::new((void *)(t + i)) T(); }
};

// copy definitions
template <class T> inline void copy(T *target, const T *source, int size)
	{ _if<type_info<T>::is_deep, slow<T>, fast<T> >::_::copy(target, source, size); }
template <class T> inline void move(T *target, const T *source, int size)
	{ _if<type_info<T>::is_deep, slow<T>, fast<T> >::_::move(target, source, size); }
template <class T> inline void set(T *target, int size, const T& v)
	{ for(int i = 0; i < size; i++) target[i] = v; }
template <class T> inline void clear(T *target, int size)
	{ _if<type_info<T>::is_virtual, slow<T>, fast<T> >::_::clear(target, size); }
template <class T> inline void init(T *t, int size)
	{ _if<type_info<T>::is_virtual, slow<T>, fast<T> >::_::init(t, size); }

} }	// elm::array

#endif /* ELM_ARRAY_H_ */
