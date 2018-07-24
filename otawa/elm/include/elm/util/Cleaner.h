/*
 *	$Id$
 *	cleaner module interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008-10, IRIT UPS.
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
#ifndef ELM_UTIL_CLEANER_H
#define ELM_UTIL_CLEANER_H

#include <elm/util/LockPtr.h>
#include <elm/genstruct/SLList.h>

namespace elm {

// Cleaner class
class Cleaner {
public:
	virtual void clean(void) { }
	virtual ~Cleaner(void) { }
};


// Deletor class
template <class T>
class Deletor: public Cleaner {
public:
	inline Deletor(T *object): obj(object) { }
	virtual ~Deletor(void) { }
	virtual void clean(void) { delete obj; }
private:
	T *obj;	
};


// AutoCleaner class 
template <class T>
class AutoCleaner: public LockPtr<T>, public Cleaner {
public:
	inline AutoCleaner(T *p = 0): LockPtr<T>(p) { }
	inline AutoCleaner(const LockPtr<T>& locked): LockPtr<T>(locked) { }
};


// CleanList class
class CleanList {
public:
	inline ~CleanList(void) { clean(); }
	void add(Cleaner *cleaner);
	void clean(void);
	
	inline Cleaner *operator()(Cleaner *cleaner) { add(cleaner); return cleaner; }
	template <class T> inline const LockPtr<T>& operator()(const LockPtr<T>& object)
		{ add(new AutoCleaner<T>(object)); return object; } 
	template <class T> inline T *operator()(T *object)
		{ add(new Deletor<T>(object)); return object; } 

private:
	typedef genstruct::SLList<Cleaner *> list_t;
	list_t list;
};

} // elm

#endif	/* ELM_UTIL_CLEANER_H */
