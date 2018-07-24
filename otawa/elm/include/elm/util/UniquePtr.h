/*
 *	UniquePtr class
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
#ifndef ELM_UTIL_UNIQUEPTR_H
#define ELM_UTIL_UNIQUEPTR_H

namespace elm {

// AutoDestructor class
template <class T>
class UniquePtr {
public:
	inline UniquePtr(void): p(0) { }
	inline UniquePtr(T *ptr): p(ptr) { };
	inline UniquePtr(UniquePtr<T>& ptr): p(ptr.p) { ptr.p = 0; }
	inline ~UniquePtr(void) { clean(); }

	inline bool isNull(void) const { return !p; }
	inline void clean(void) { if(p) delete p; p = 0; }
	inline T *detach(void) { T *res = p; p = 0; return res; }
	inline void set(T *ptr) { clean(); p = ptr; }
	inline T *get(void) const { return p; }

	inline UniquePtr<T>& operator=(T *ptr) { set(ptr); return *this; }
	inline UniquePtr<T>& operator=(UniquePtr<T>& ptr) { clean(); p = ptr.p; ptr.p = 0; return *this; }
	inline operator T *(void) const { return get(); }
	inline T *operator->(void) const { return get(); }
	inline operator bool(void) const { return !isNull(); }
	inline T& operator*(void) const { return *get(); }

private:
	T *p;
};

};

#endif // ELM_UTIL_UNIQUEPTR_H

