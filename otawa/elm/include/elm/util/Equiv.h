/*
 *	$Id$
 *	Equiv class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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
#ifndef ELM_UTIL_EQUIV_H_
#define ELM_UTIL_EQUIV_H_

#include <elm/string.h>
#include <elm/util/Pair.h>

namespace elm {

// Equiv class
template <class T>
class Equiv {
public:
	static inline bool equals(const T& v1, const T& v2) { return v1 == v2; }
	inline bool isEqual(const T& v1, const T& v2) const { return equals(v1, v2); }
	static Equiv<T> def;
};
template <class T> Equiv<T> Equiv<T>::def;


// EqualsEquiv class
template <class T>
class EqualsEquiv {
public:
	static inline int equal(const T& v1, const T& v2) { return v1.equals(v2); }
	inline bool isEqual(const T& v1, const T& v2) const { return equals(v1, v2); }
	static EqualsEquiv<T> def;
};
template <class T> EqualsEquiv<T> EqualsEquiv<T>::def;


// AssocEquiv class
template <class K, class T, class E = Equiv<K> >
class AssocEquiv {
public:
	typedef Pair<K, T> pair_t;
	static inline bool equals(const pair_t& v1, const pair_t& v2) { return E::equals(v1.fst, v2. fst); }
	inline bool isEqual(const pair_t& v1, const pair_t& v2) { return equals(v1, v2); }
	static AssocEquiv<K, T, E> def;
};
template <class K, class T, class E> AssocEquiv<K, T, E> AssocEquiv<K, T, E>::def;
template <class K, class T> class Equiv<Pair<K, T> >
	: public AssocEquiv<K, T> { };

} // elm

#endif /* ELM_UTIL_EQUIV_H_ */
