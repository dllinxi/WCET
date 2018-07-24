/*
 *	$Id$
 *	delegate classes interface
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
#ifndef ELM_UTIL_DELEGATE_H
#define ELM_UTIL_DELEGATE_H

namespace elm {

// ArrayDelegate class
template <class C, class I, class T>
class ArrayDelegate {
public:
	inline ArrayDelegate(C& container, const I& identifier)
		: cont(&container), id(identifier) { }
	inline ArrayDelegate(const ArrayDelegate& delegate)
		: cont(delegate.cont), id(delegate.id) { }
	
	inline operator T(void) const { return ((const C *)cont)->get(id); }
	inline const T& operator*(void) const { return ((const C *)cont)->get(id); }
	inline ArrayDelegate<C, I, T>& operator=(const T& value)
		{ cont->set(id, value); return *this; }
	inline ArrayDelegate<C, I, T>& operator=(const ArrayDelegate<C, I, T>& delegate)
		{ cont = delegate.cont; id = delegate.id; return *this; }
	
private:
	C *cont;
	I id;
};


// Default value
template <class T>
class Default {
public:
	static T def; 
};
template <class T> T Default<T>::def = 0;


// MapDelegate class
template <class C, class I, class T, class D = Default<T> >
class MapDelegate {
public:
	inline MapDelegate(C& container, const I& identifier)
		: cont(&container), id(identifier) { }
	inline MapDelegate(const MapDelegate& delegate)
		: cont(delegate.cont), id(delegate.id) { }
	
	inline operator T(void) const
		{ return ((const C *)cont)->get(id, D::def); }
	inline const T& operator*(void) const
		{ return ((const C *)cont)->get(id, D::def); }
	inline MapDelegate<C, I, T>& operator=(const T& value)
		{ cont->put(id, value); return *this; }
	inline MapDelegate<C, I, T>& operator=(const MapDelegate<C, I, T>& delegate)
		{ cont = delegate.cont; id = delegate.id; return *this; }
	
private:
	C *cont;
	I id;
};

} // elm

#endif /* ELM_UTIL_DELEGATE_H */
