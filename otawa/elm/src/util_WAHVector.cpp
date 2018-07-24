/*
 *	WAHVector class implementation
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

#include <elm/assert.h>
#include <elm/util/array.h>
#include <elm/compare.h>
#include "elm/util/WAHVector.h"

namespace elm {

typedef WAHVector::word_t word_t;

/*
 * ENCODING
 * literal	b[31] = 0, b[30..0] 31-bit word of data
 * fill		b[31] = 1,	b[30] = 0,	b[29..0] x 31-bits words to 0
 * 						b[30] = 1,	b[29..0] x 31-bits words to 1
 */

static const word_t
	FILL		= 0x80000000,
	ONES		= 0x40000000,
	ALL_ONES	= 0x7fffffff,
	ALL_ZEROES	= 0x00000000;
static int WORD_SIZE = 31;	// if this value is changed, fix times() and div() functions!

inline bool isFill(word_t w) { return w & FILL; }
inline bool isLite(word_t w) { return !(w & FILL); }
inline bool isOnes(word_t w) { return w & ONES; }
inline bool isZeroes(word_t w) { return !(w & ONES); }
inline word_t fillCount(word_t w) { return w & ~(FILL | ONES); }
inline word_t mask(int n) { return (1 << n) - 1; }

inline word_t lite(word_t w) { return w; }
inline word_t fill(int size, bool init) { return FILL | (init ? ONES : 0) | size; }

inline int times(int v) { return (v << 5) - v; }
// refer to http://stackoverflow.com/questions/8141802/efficient-modulo-3-operation
inline int div(int v) { return ((t::uint64((1ULL) << 32) / 31 + 30) * v) >> 32; }
inline int mod(int v) { return v - times(div(v)); }

class builder {
public:
	inline builder(t::uint32 capacity): size(0), cap(capacity), words(new word_t[capacity]) { }

	inline void pushLite(word_t w) {
		//cerr << "DEBUG: pushLite(" << io::hex(w) << ")\n";
		if(!w)
			pushFill(fill(1, false));
		else if(w == ALL_ONES)
			pushFill(fill(1, true));
		else
			pushRaw(w);
	}

	inline void pushFill(word_t w) {
		//cerr << "DEBUG: pushFill(" << io::hex(w) << ")\n";
		if(size != 0 && (w & (FILL | ONES)) == (words[size - 1] & (FILL | ONES)))
			words[size - 1] += fillCount(w);
		else
			pushRaw(w);
	}

	inline void push(word_t w) {
		//cerr << "DEBUG: push(" << io::hex(w) << ")\n";
		if(w & FILL)
			pushFill(w);
		else
			pushLite(w);
	}

	inline void pushRaw(word_t w) {
		//cerr << "DEBUG: pushRaw(" << io::hex(w) << ")\n";
		if(size == cap)
			allocate(cap * 2);
		words[size] = w;
		size++;
		/*cerr << "DEBUG: ";
		for(int i = 0; i < size; i++)
			cerr << io::hex(words[i]) << ' ';
		cerr << io::endl;*/
	}

	inline void copy(word_t w) {
		if(size == cap)
			allocate(cap * 2);
		words[size++] = w;
	}

	void copy(word_t *wwords, int wsize) {
		if(size + wsize > cap)
			allocate(size + wsize);
		array::copy(words + size, wwords, wsize);
		size += wsize;
	}

	inline t::uint32 getSize(void) const { return size; }
	inline word_t *getWords(void) const { return words; }

	inline void fix(word_t *& w, t::uint32& s, int rem) {
		delete [] w;
		w = words;
		s = size;
		if(rem && isLite(words[size - 1])) {
			words[size - 1] &= mask(rem);
			if(words[size - 1] == lite(0)) {
				if(size >= 2 && isZeroes(words[size - 2])) {
					words[size - 2]++;
					s--;
				}
				else
					words[size - 1] = fill(1, false);
			}
			else if(words[size - 1] == lite(mask(rem))) {
				if(size >= 2 && isOnes(words[size - 2])) {
					words[size - 2]++;
					s--;
				}
				else
					words[size - 1] = fill(1, true);
			}
		}
	}

private:
	void allocate(int new_cap) {
		cap = new_cap;
		word_t *nwords = new word_t[cap];
		array::copy(nwords, words, size);
		delete [] words;
		words = nwords;
	}

	t::uint32 size, cap;
	word_t *words;
};


class iter {
public:
	inline iter(word_t *current, t::uint32 size): ii(0), cur(current), end(current + size) { step(); }
	inline word_t value(void) const { return val; }
	inline bool ended(void) const { return cur == end; }
	inline void next(void) { cur++; if(cur != end) step(); }
	inline operator bool(void) const { return !ended(); }
	inline iter& operator++(int) { next(); return *this; }
	inline iter& operator++(void) { next(); return *this; }
	inline int index(void) const { return i; }
	inline int top(void) const { return ii; }
	inline bool isFill(void) const { return is_fill; }
	inline void consume(word_t c = 1) { ASSERT(c <= cnt); cnt -= c; i += times(c); if(!cnt) next(); }
	inline int count(void) const { return cnt; }
	inline iter& operator-=(int c) { consume(c); return *this; }
	inline void copy(builder& b) { b.copy(*cur); }
	inline void pushTail(builder& b) {
		// cerr << "DEBUG: pushTail\n";
		cur++;
		if(cur < end) {
			b.push(*cur++);
			b.copy(cur, end - cur);
		}
	}
	inline bool isLast(void) const { return cur + 1 == end; }

private:
	inline void step(void) {
		i = ii;
		word_t w = *cur;
		is_fill = elm::isFill(w);
		if(is_fill) {
			cnt = fillCount(w);
			if(isOnes(w))
				val = ALL_ONES;
			else
				val = ALL_ZEROES;
			ii += times(cnt);
		}
		else {
			cnt = 1;
			val = w;
			ii += times(1);
		};
	}
	bool is_fill;
	word_t val;
	word_t cnt;
	int i, ii;
	word_t *cur, *end;
};


/**
 * @class WAHVector
 * WAHVector is an implementation of bit vector (like @ref elm::BitVector)
 * whose tradeoff has been moved towards a more compact internal representation.
 * Algorithms of WAHVector are a C++ adaptation of:
 *
 * Wu, K., Otoo, E. J., & Shoshani, A. (2002). Compressing bitmap indexes for faster search operations.
 * Proceedings. 14th International Conference on Scientific and Statistical Database Management (pp. 99-108), IEEE, 2002.
 */

/**
 * @fn WAHBEctor::WAHVector(void);
 * Build an empty WAH vector.
 */


/**
 * Build a bit of the given size bits.
 * @param size		Size in bits of the vector.
 * @param init		Initial value.
 */
WAHVector::WAHVector(int size, bool init): rem(mod(size)), _size(1) {
	words = new t::uint32[_size];
	words[0] = fill(div(size) + (rem ? 1 : 0), init);
}


/**
 * Build a WAHVector by cloning the given one.
 * @param v		Vector to clone.
 */
WAHVector::WAHVector(const WAHVector& v) {
	_size = v._size;
	rem = v.rem;
	if(!_size)
		words = 0;
	else {
		words = new word_t[_size];
		array::copy(words, v.words, _size);
	}
}


/**
 * Test if a bit has for value one.
 * @param index	Index of the tested bit.
 * @return		Value of the bit.
 */
bool WAHVector::bit(int index) const {
	for(iter i(words, _size); i; i++) {
		if(i.index() <= index && index < i.top())
			return i.value() & (1 << mod(index));
	}
	ASSERTP(false, "index out of bounds");
	return false;
}


/**
 * Test if all bits are at 0
 * @return	True if all bits are at 0, false else.
 */
bool WAHVector::isEmpty(void) const {
	ASSERT(words);
	if(!isZeroes(words[0]))
		return false;
	else if(_size == 1)
		return true;
	else
		return _size == 2 && words[1] == 0;
}



/**
 * Test if all bits are at 1
 * @return	True if all bits are at 1, false else.
 */
bool WAHVector::isFull(void) const {
	ASSERT(words);
	if(!isOnes(words[0]))
		return false;
	else if(_size == 1)
		return true;
	else
		return _size == 2 && words[1] == word_t((1 << rem) - 1);
}


/**
 * Test if both vectors are equal.
 * @param v	Vector to compare with.
 * @return	True if both vectors are equal, false else.
 */
bool WAHVector::equals(const WAHVector& vec) const {
	if(_size != vec._size)
		return false;
	for(t::uint32 i = 0; i < _size; i++)
		if(words[i] != vec.words[i])
			return false;
	return true;
}


/**
 * Test if the current vector includes the given one.
 * @param vec	Vector to include.
 * @return		True if the current vector includes the given one.
 */
bool WAHVector::includes(const WAHVector& vec) const {
	iter i(words, _size), j(vec.words, vec._size);
	while(i && j) {

		// both filling: fast
		if(i.isFill() && j.isFill()) {
			if(!i.value() && j.value())
				return false;
			int m = min(i.count(), j.count());
			i.consume(m);
			j.consume(m);
		}

		// both literal: process literal
		else {
			if(~i.value() & j.value())
				return false;
			i.consume(1);
			j.consume(1);
		}
	}
	ASSERTP(!i && !j, "vector of different size");
	return true;
}


/**
 * Test if the current vector includes strictly the given one.
 * @param vec	Vector to include.
 * @return		True if the current vector includes the given one.
 */
bool WAHVector::includesStrictly(const WAHVector &vec) const {
	return !equals(vec) && includes(vec);
}


/**
 * Compute the number of bits in the vector.
 * @return	Number of bits.
 */
int WAHVector::size(void) const {
	int r = 0;
	for(iter i(words, _size); i; i++)
		r += i.count();
	if(rem)
		return times(r) - (31 - rem);
	else
		return times(r);
}


/**
 * Count the number of bits to 1.
 * @return	Number of bits to 1.
 */
int WAHVector::countOnes(void) const {
	int r = 0;
	for(iter i(words, _size); i; i++) {
		if(i.isFill()) {
			if(i.value()) {
				r += times(i.count());
				if(rem && i.isLast())
					r -= 31 - rem;
			}
		}
		else
			r += ones((rem && i.isLast()) ? (i.value() & mask(rem)) : i.value());
	}
	return r;
}


/**
 * @fn int WAHVector::countBits(void) const;
 * Same as @ref countBits().
 */


/**
 * int WAHVector::countZeroes(void) const;
 * Count the number of zeroes in the bit vector.
 * @return	Number of zeroes.
 */


/**
 * Set a bit.
 * @param index		Index of the bit.
 */
void WAHVector::set(int index) {
	builder b(_size + 2);	// worst-case

	// copy to the position
	for(iter i(words, _size); i; i++) {
		//cerr << "DEBUG: " << i.index() << " < " << index << " < " << i.top() << " (" << value << ")\n";

		// not in this word
		if(index < i.index() || index >= i.top())
			i.copy(b);

		// in this word
		else {

			// literal case
			if(!i.isFill())
				b.pushLite(lite(i.value() | (1 << mod(index))));

			// fill case
			else {

				// one-filled
				if(i.value())
					i.copy(b);

				// zero-filled: split it
				else {
					int before = div(index - i.index()),
						after = i.count() - before - 1;
					if(before)
						b.pushRaw(fill(before, i.value()));
					//cerr << "DEBUG: before = " << before << ", after = " << after << io::endl;
					b.pushLite(lite(i.value() | (1 << mod(index))));
					if(after)
						b.pushRaw(fill(after, i.value()));
				}
			}

			// copy the remaining
			//cerr << "DEBUG: pushTail()\n";
			i.pushTail(b);
			b.fix(words, _size, rem);
			return;
		}
	}
	ASSERTP(false, "index out of bounds");
}


/**
 * Clear a bit.
 * @param index		Index of the bit.
 */
void WAHVector::clear(int index) {
	builder b(_size + 2);	// worst-case

	// copy to the position
	for(iter i(words, _size); i; i++) {
		//cerr << "DEBUG: " << i.index() << " < " << index << " < " << i.top() << " (" << value << ")\n";

		// not in this word
		if(index < i.index() || index >= i.top())
			i.copy(b);

		// in this word
		else {

			// literal case
			if(!i.isFill())
				b.pushLite(lite(i.value() & ~(1 << mod(index))));

			// fill case
			else {

				// zero-filled
				if(!i.value())
					i.copy(b);

				// one-filled: must be split
				else {
					int before = div(index - i.index()),
						after = i.count() - before - 1;
					if(before)
						b.pushRaw(fill(before, i.value()));
					//cerr << "DEBUG: before = " << before << ", after = " << after << io::endl;
					b.pushLite(lite(i.value() & ~(1 << mod(index))));
					if(after)
						b.pushRaw(fill(after, i.value()));
				}
			}

			// copy the remaining
			//cerr << "DEBUG: pushTail()\n";
			i.pushTail(b);
			b.fix(words, _size, rem);
			return;
		}
	}
	ASSERTP(false, "index out of bounds");
}


/**
 * Set all vector bits to 0.
 */
void WAHVector::clear(void) {
	words[0] = fill(div(_size) + (rem ? 1 : 0), false);
}


/**
 * Set all vector bits to 1.
 */
void WAHVector::set(void) {
	words[0] = fill(div(_size) + (rem ? 1 : 0), true);
}


/**
 * Copy the given vector into the current one.
 * @param v	Vector to copy.
 */
void WAHVector::copy(const WAHVector& v) {

	// assign empty vector
	if(!v._size) {
		if(words)
			delete [] words;
	}

	// ensure enough place
	else if(_size < v._size) {
		if(words)
			delete [] words;
		words = new word_t[v._size];
	}

	// perform the copy
	if(v._size)
		array::copy(words, v.words, v._size);
	_size = v._size;
	rem = v.rem;
}


/**
 * Perform AND operation between v1 and v2.
 * @param v1	WAH vector to perform AND with.
 * @param v2	WAH vector to perform AND with.
 * @param r		WAH vector to store result in (current vector and v may be the same).
 */
void WAHVector::doAnd(WAHVector& r, const WAHVector& v1, const WAHVector& v2) {

	// initialization
	iter i1(v1.words, v1._size), i2(v2.words, v2._size);
	builder b(max(v1._size, v2._size));

	// traverse all iterators
	while(i1) {
		ASSERT(i2);

		// both fills
		if(i1.isFill() && i2.isFill()) {
			int n = min(i1.count(), i2.count());
			b.pushFill(fill(n, i1.value() & i2.value()));
			i1.consume(n);
			i2.consume(n);
		}

		// work just on words
		else {
			b.pushLite(lite(i1.value() & i2.value()));
			i1.consume();
			i2.consume();
		}
	}

	// assign result
	b.fix(r.words, r._size, v1.rem);
}


/**
 * Perform OR operation between v1 and v2.
 * @param v1	WAH vector to perform AND with.
 * @param v2	WAH vector to perform AND with.
 * @param r		WAH vector to store result in (current vector and v may be the same).
 */
void WAHVector::doOr(WAHVector& r, const WAHVector& v1, const WAHVector& v2) {

	// initialization
	iter i1(v1.words, v1._size), i2(v2.words, v2._size);
	builder b(max(v1._size, v2._size));

	// traverse all iterators
	while(i1) {
		ASSERT(i2);

		// both fills
		if(i1.isFill() && i2.isFill()) {
			int n = min(i1.count(), i2.count());
			b.pushFill(fill(n, i1.value() | i2.value()));
			i1.consume(n);
			i2.consume(n);
		}

		// work just on words
		else {
			b.pushLite(lite(i1.value() | i2.value()));
			i1.consume();
			i2.consume();
		}
	}

	// assign result
	b.fix(r.words, r._size, v1.rem);
}


/**
 * Perform reset operation between v1 and v2.
 * @param v1	WAH vector to perform reset with.
 * @param v2	WAH vector to perform reset with.
 * @param r		WAH vector to store result in (current vector and v may be the same).
 */
void WAHVector::doReset(WAHVector& r, const WAHVector& v1, const WAHVector& v2) {

	// initialization
	iter i1(v1.words, v1._size), i2(v2.words, v2._size);
	builder b(max(v1._size, v2._size));

	// traverse all iterators
	while(i1) {
		ASSERT(i2);

		// both fills
		if(i1.isFill() && i2.isFill()) {
			int n = min(i1.count(), i2.count());
			b.pushFill(fill(n, i1.value() & ~i2.value()));
			i1.consume(n);
			i2.consume(n);
		}

		// work just on words
		else {
			b.pushLite(lite(i1.value() & ~i2.value()));
			i1.consume();
			i2.consume();
		}
	}

	// assign result
	b.fix(r.words, r._size, v1.rem);
}


/**
 * Perform not operation on v.
 * @param v		WAH vector to perform not on.
 * @param r		WAH vector to store result in (result vector and v may be the same).
 */
void WAHVector::doNot(WAHVector& r, const WAHVector& v) {
	builder b(v._size);

	// traverse all iterators
	for(iter i(v.words, v._size); i; i++) {

		// fill case
		if(i.isFill())
			b.pushFill(fill(i.count(), !i.value()));

		// work just on words
		else
			b.pushLite(lite(~i.value() & ((1 << WORD_SIZE) - 1)));
	}

	// assign result
	b.fix(r.words, r._size, v.rem);
}


#ifndef NDEBUG
void WAHVector::__dump(io::Output& out) const {
	out << "[";
	bool fst = true;
	for(iter i(words, _size); i; i.next()) {
		if(fst)
			fst = false;
		else
			out << " ";
		if(i.isFill())
			out << (i.value() ? '1' : '0') << "*" << i.count();
		else
			out << "L" << io::hex(i.value());
	}
	out << "]";
}
#endif

}	// elm
