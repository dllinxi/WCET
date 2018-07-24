/*
 *	WAHVector class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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
#ifndef ELM_UTIL_WAHVECTOR_H_
#define ELM_UTIL_WAHVECTOR_H_

#include <elm/types.h>
#include <elm/io.h>

namespace elm {
	
class WAHVector {
public:
	typedef t::uint32 word_t;
	
	inline WAHVector(void): rem(0), _size(0), words(0) { }
	WAHVector(int _size, bool init = false);
	WAHVector(const WAHVector& v);
	inline ~WAHVector(void) { if(words) delete [] words; }
	
	bool bit(int index) const;
	bool isEmpty(void) const;
	bool isFull(void) const;
	bool equals(const WAHVector& vec) const;
	bool includes(const WAHVector& vec) const;
	bool includesStrictly(const WAHVector &vec) const;
	int size(void) const;
	inline int countBits(void) const { return countOnes(); }
	int countOnes(void) const;
	inline int countZeroes(void) const { return size() - countOnes(); }

	inline void set(int index, bool value) { if(value) set(index); else clear(index); }
	void set(int index);
	void clear(int index);

	void set(void);
	void clear(void);
	void copy(const WAHVector& v);
	inline void applyAnd(const WAHVector& v) { doAnd(*this, *this, v); }
	inline WAHVector makeAnd(const WAHVector& v) const { WAHVector r; r.doAnd(r, *this, v); return r; }
	inline void applyOr(const WAHVector& v) { doOr(*this, *this, v); }
	inline WAHVector makeOr(const WAHVector& v) const { WAHVector r; r.doOr(r, *this, v); return r; }
	inline void applyReset(const WAHVector& v) { doReset(*this, *this, v); }
	inline WAHVector makeReset(const WAHVector& v) const { WAHVector r; r.doReset(r, *this, v); return r; }
	inline void applyNot(void) { doNot(*this, *this); }
	inline WAHVector makeNot(void) const { WAHVector r; r.doNot(r, *this); return r; }

	inline bool operator[](int i) const { return bit(i); }
	class Bit {
	public:
		inline Bit(WAHVector& v, int i): _v(&v), _i(i) { }
		inline Bit& operator=(const Bit& b) { _v = b._v; _i = b._i; return *this; }
		inline Bit& operator=(bool b) { _v->set(_i, b); return *this; }
		inline operator bool(void) const { return _v->bit(_i); }
	private:
		WAHVector *_v;
		int _i;
	};
	inline Bit operator[](int i) { return Bit(*this, i); }

	inline WAHVector operator~(void) const { return makeNot(); }
	inline WAHVector operator|(const WAHVector &vec) const { return makeOr(vec); }
	inline WAHVector operator&(const WAHVector &vec) const { return makeAnd(vec); }
	inline WAHVector operator+(const WAHVector &vec) const { return makeOr(vec); }
	inline WAHVector operator*(const WAHVector &vec) const { return makeAnd(vec); }
	inline WAHVector operator-(const WAHVector &vec) const { return makeReset(vec); }
	inline WAHVector &operator=(const WAHVector &vec) { copy(vec); return *this; }
	inline WAHVector &operator|=(const WAHVector &vec) { applyOr(vec); return *this; }
	inline WAHVector &operator&=(const WAHVector &vec) { applyAnd(vec); return *this; }
	inline WAHVector &operator+=(const WAHVector &vec) { applyOr(vec); return *this; }
	inline WAHVector &operator*=(const WAHVector &vec) { applyAnd(vec); return *this; }
	inline WAHVector &operator-=(const WAHVector &vec) { applyReset(vec); return *this; }
	inline bool operator==(const WAHVector& v) { return equals(v); }
	inline bool operator!=(const WAHVector& v) { return !equals(v); }
	inline bool operator<(const WAHVector &vec) const { return vec.includesStrictly(*this); }
	inline bool operator<=(const WAHVector &vec) const { return vec.includes(*this); }
	inline bool operator>(const WAHVector &vec) const { return includesStrictly(vec); }
	inline bool operator>=(const WAHVector &vec) const { return includes(vec); }

#	ifndef NDEBUG
		void __dump(io::Output& out) const;
#	endif
	inline int __size(void) const { return sizeof(WAHVector) + _size * sizeof(word_t); }

private:
	static void doAnd(WAHVector& r, const WAHVector& v1, const WAHVector& v2);
	static void doOr(WAHVector& r, const WAHVector& v1, const WAHVector& v2);
	static void doReset(WAHVector& r, const WAHVector& v1, const WAHVector& v2);
	static void doNot(WAHVector& r, const WAHVector& v);
	t::uint32 rem;
	t::uint32 _size;
	word_t *words;
};
	
#ifndef NDEBUG
	inline io::Output& operator<<(io::Output& out, const WAHVector& v) { v.__dump(out); return out; }
#endif

};	// elm

#endif	// ELM_UTIL_WAHVECTOR_H_
