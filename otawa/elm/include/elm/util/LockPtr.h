/*
 *	Lock and LockPtr classes
 *
 *	This file is part of ELM
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
#ifndef ELM_UTIL_LOCKPTR_H
#define ELM_UTIL_LOCKPTR_H

#include <elm/assert.h>

namespace elm {

// Lock class
class Lock {
	int u;
public:
	inline Lock(int usage = 0): u(usage) { }
	inline void lock(void) { u++; }
	inline void unlock(void) { u--; }
	inline int usage(void) const { return u; }
};


// AutoPtr class
template <class T> class LockPtr {
public:
	inline LockPtr(T *p = 0): ptr(p ? p : null_lock()) { ptr->lock(); }
	inline LockPtr(const LockPtr& l): ptr(l.ptr) { ptr->lock(); }
	inline ~LockPtr(void) { unlock(); }
	static LockPtr<T> null;

	inline LockPtr& operator=(const LockPtr& lock)
		{ unlock(); ptr = lock.ptr; ptr->lock(); return *this;}
	inline LockPtr& operator=(T *p)
		{ unlock(); ptr = p ? p : null_lock(); ptr->lock(); return *this; }

	inline T *operator->(void) const
		{ ASSERTP(!isNull(), "accessing null pointer"); return (T *)ptr; }
	inline T& operator*(void) const
		{ ASSERTP(!isNull(), "accessing null pointer"); return *(T *)ptr; }
	inline T *operator&(void) const
		{ return isNull() ? 0 : (T *)ptr; }

	inline bool isNull(void) const
		{ return ptr == null_lock(); }
	inline operator bool(void) const { return !isNull(); };

	inline bool operator==(const LockPtr<T>& ap) const
		{ return ptr == ap.ptr; }
	inline bool operator!=(const LockPtr<T>& ap) const
		{ return ptr != ap.ptr; }
	inline bool operator>(const LockPtr<T>& ap) const
		{ return ptr > ap.ptr; }
	inline bool operator>=(const LockPtr<T>& ap) const
		{ return ptr >= ap.ptr; }
	inline bool operator<(const LockPtr<T>& ap) const
		{ return ptr < ap.ptr; }
	inline bool operator<=(const LockPtr<T>& ap) const
		{ return ptr <= ap.ptr; }

private:
	void unlock(void) { ptr->unlock(); if(!ptr->usage()) { delete ptr; ptr = null_lock(); } }
	T *ptr;
	static T *null_lock(void) { static Lock n(1); return (T *)&n; }
};

}	// elm

#endif // ELM_UTIL_AUTOPTR_H
