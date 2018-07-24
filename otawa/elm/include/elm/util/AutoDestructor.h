/*
 *  AutoDestructor class
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
#ifndef ELM_UTIL_AUTODESTRUCTOR_H
#define ELM_UTIL_AUTODESTRUCTOR_H

#warning "DEPRECATED: use UniquePtr instead."

#include "UniquePtr.h"

namespace elm {

// AutoDestructor class
template <class T>
class AutoDestructor: public UniquePtr<T> {
public:
	inline AutoDestructor(void) { }
	inline AutoDestructor(T *ptr): UniquePtr<T>(ptr) { };

	inline AutoDestructor& operator=(T *ptr) { UniquePtr<T>::operator=(ptr); return *this; }
	inline AutoDestructor& operator=(AutoDestructor& ad) { UniquePtr<T>::operator=(ad); return *this; }
};

};

#endif // ELM_UTIL_AUTODESTRUCTOR_H

