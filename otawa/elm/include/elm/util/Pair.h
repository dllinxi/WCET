/*
 * $Id$
 * Copyright (c) 2006 - IRIT-UPS <casse@irit.fr>
 *
 * elm/util/Pair.h -- interface of Pair class.
 */
#ifndef ELM_UTIL_PAIR_H
#define ELM_UTIL_PAIR_H

namespace elm {

namespace io { class Output; }

// Pair class
template <class T1, class T2>
class Pair {
public:
	T1 fst;
	T2 snd;
	inline Pair(void) { }
	inline Pair(const T1& _fst, const T2& _snd): fst(_fst), snd(_snd) { }
	inline Pair(const Pair<T1, T2>& pair): fst(pair.fst), snd(pair.snd) { }
	inline Pair<T1, T2>& operator=(const Pair<T1, T2>& pair) { fst = pair.fst; snd = pair.snd; return *this; }
	inline bool operator==(const Pair<T1, T2>& pair) const { return ((fst== pair.fst) && (snd == pair.snd)); }
};


// RefPair class
template <class T1, class T2>
class RefPair {
public:
	inline RefPair(T1& r1, T2& r2): v1(r1), v2(r2) { }
	RefPair<T1, T2>& operator=(const Pair<T1, T2>& p) { v1 = p.fst; v2 = p.snd; return *this; }
private:
	T1& v1;
	T2& v2;
};


// Shortcuts
template <class T1, class T2> inline Pair<T1, T2> pair(const T1& v1, const T2& v2) { return Pair<T1, T2>(v1, v2); }
template <class T1, class T2> io::Output& operator<<(io::Output& out, Pair<T1, T2>& p) { out << p.fst << ", " << p.snd; return out; }
template <class T1, class T2> inline RefPair<T1, T2> let(T1& v1, T2& v2) { return RefPair<T1, T2>(v1, v2); }

} // elm

#endif /* ELM_UTIL_PAIR_H */
