/*
 * $Id$
 * Copyright (c) 2006 - IRIT-UPS <casse@irit.fr>
 *
 * Ref class interface.
 */
#ifndef ELM_UTIL_REF_H
#define ELM_UTIL_REF_H

#include <elm/io.h>

namespace elm {

using namespace io;

// Ref class
template <class T>
class Ref {
	T *ptr;
public:
	inline Ref(T *_ptr): ptr(_ptr) { }
	inline Ref(const Ref<T>& ref): ptr(ref.ptr) { }
	inline operator T& (void) const { return *ptr; }
	inline T *operator&(void) const { return ptr; }
	inline T& operator*(void) const { return *ptr; }
	inline T *operator->(void) const { return ptr; }
	inline Ref<T>& operator=(T *_ptr)
		{ ptr = _ptr; return *this; }
	inline Ref<T>& operator=(const Ref<T>& ref)
		{ ptr = ref.ptr; return *this; }
	inline Ref<T>& operator=(const T& val)
		{ *ptr = val; return *this; }
};

// Display
template <class T>
inline Output& operator<<(Output& out, const Ref<T>& ref) {
	out << *ref;
	return out;
}

} // elm

#endif // ELM_UTIL_REF_H
