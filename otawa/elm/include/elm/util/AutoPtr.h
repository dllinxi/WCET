/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS <casse@irit.fr>
 *
 * elm/util/AutoPtr.h -- AutoPtr class.
 */
#ifndef ELM_UTIL_AUTOPTR_H
#define ELM_UTIL_AUTOPTR_H

#include "LockPtr.h"

#warning "Deprecated header file. Use LockPtr instead."

namespace elm {

template <class T>
class AutoPtr: public LockPtr<T> {
public:
	inline AutoPtr(T *p = 0): LockPtr<T>(p) { }
	inline AutoPtr(const AutoPtr<T>& p): LockPtr<T>(p) { }
	inline AutoPtr(const LockPtr<T>& p): LockPtr<T>(p) { }
};

} // elm

#endif // ELM_UTIL_AUTOPTR_H

