/*
 *	$Id$
 *	Comparator class interface
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
#ifndef ELM_COMPARATOR_H_
#define ELM_COMPARATOR_H_

#include <elm/string.h>
#include <elm/type_info.h>
#include <elm/util/Pair.h>
#include <elm/util/Equiv.h>

namespace elm {

// Comparator class
template <class T>
class Comparator {
public:
	static inline int compare(const T& v1, const T& v2)
		{ if(v1 == v2) return 0; else if(v1 > v2) return 1; else return -1; }
	int doCompare(const T& v1, const T& v2) const { return compare(v1, v2); }
	static Comparator<T> def;
};
template <class T> Comparator<T> Comparator<T>::def;

// DelegateComparator class
template <class T, class C>
class DelegateComparator {
public:
	static inline int compare(const T& v1, const T& v2) { return C::compare(v1, v2); }
	inline int doCompare(const T& v1, const T& v2) const { return compare(v1, v2); }
	static DelegateComparator<T, C> def;
};
template <class T, class C> DelegateComparator<T, C> DelegateComparator<T, C>::def;

// CompareComparator class
template <class T>
class CompareComparator {
public:
	static inline int compare(const T& v1, const T& v2) { return v1.compare(v2); }
};
template <> class Comparator<cstring>: public CompareComparator<cstring> { };
template <> class Comparator<string>: public CompareComparator<string> { };


// AssocComparator class
template <class K, class T, class C = Comparator<K> >
class AssocComparator {
public:
	typedef Pair<K, T> pair_t;
	static inline int compare(const pair_t& v1, const pair_t& v2)
		{ return C::compare(v1.fst, v2. fst); }
};
template <class K, class T> class Comparator<Pair<K, T> >
	: public AssocComparator<K, T> { };


// ReverseComparator class
template <class T, class C>
class ReverseComparator {
public:
	static inline int compare(const T& v1, const T& v2)
		{ return -C::compare(v1, v2); }
};


// CompareEquiv class
template <class T, class C = Comparator<T> >
class CompareEquiv {
public:
	static inline bool equals(const T& v1, const T& v2)
		{ return C::compare(v1, v2) == 0; }
};


// CompareInst class
template <class T, class C = Comparator<T> >
class DefaultCompare {
public:
	static inline int compare(const T& v1, const T& v2)
		{ return C::compare(v1, v2); }
};


// Useful inlines
template <class T> inline const T& min(const T& x, const T& y)
	{ if(Comparator<T>::compare(x, y) >= 0) return y; else return x; }
template <class T> inline const T& max(const T& x, const T& y)
	{ if(Comparator<T>::compare(x, y) >= 0) return x; else return y; }

} // elm

#endif /* ELM_COMPARATOR_H_ */
