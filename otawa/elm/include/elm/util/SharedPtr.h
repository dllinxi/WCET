/*
 *	SharedPtr class
 *
 *	This file is part of OTAWA
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
#ifndef ELM_UTIL_SHAREDPTR_H_
#define ELM_UTIL_SHAREDPTR_H_

#include <elm/io.h>
#include <elm/types.h>

namespace elm {

template <class T>
class SharedPtr {
	typedef struct cell_t {
		inline cell_t(T *p): cnt(1), ptr(p) { }
		t::uint32 cnt;
		T *ptr;
	} cell_t;

public:
	inline SharedPtr(void): c(&null) { lock(); }
	inline SharedPtr(T *p): c(new cell_t(p)) { }
	inline SharedPtr(const SharedPtr<T>& p): c(p.c) { lock(); }
	inline ~SharedPtr(void) { unlock(); }

	inline operator bool(void) const { return c->ptr; }
	inline operator T *(void) const { return c->ptr; }
	inline T *operator->(void) const { return c->ptr; }
	inline T& operator*(void) const { return *(c->ptr); }
	inline T *operator&(void) const { return c->ptr; }
	inline SharedPtr<T>& operator=(const SharedPtr<T>& p) { unlock(); c = p.c; lock(); return *this; }
	inline SharedPtr<T>& operator=(T *p) { unlock(); c = new cell_t(p); return *this; }

	inline bool operator==(const SharedPtr<T>& p) const { return c->ptr == p.c->ptr; }
	inline bool operator!=(const SharedPtr<T>& p) const { return c->ptr != p.c->ptr; }
	inline bool operator==(T *p) const { return c->ptr == p; }
	inline bool operator!=(T *p) const { return c->ptr != p; }

private:
	inline void lock(void) { c->cnt++; }
	inline void unlock(void) { c->cnt--; if(c->cnt == 0) clear(); }
	void clear(void) { if(c->ptr) delete c->ptr; delete c; }

	cell_t *c;
	static cell_t null;
};

template <class T> typename SharedPtr<T>::cell_t SharedPtr<T>::null(0);

}	// elm

#endif /* ELM_UTIL_SHAREDPTR_H_ */
