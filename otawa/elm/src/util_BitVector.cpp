/*
 *	$Id$
 *	BitVector class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
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

#include <elm/util/BitVector.h>
#include <elm/int.h>
#include <elm/compare.h>

namespace elm {

/**
 * Class for building the bit counter array.
 * @ingroup utility
 */
class BitCounter {
public:
	char counts[256];
	inline BitCounter(void) {
		for(int i = 0; i < 256; i++) {
			counts[i] = 0;
			for(int j = 1; j < 256; j <<= 1)
				if(i & j)
					counts[i]++;		
		}
	};
};
static BitCounter bit_counter;


/**
 * @class BitVector
 * <p>This class provides facilities for managing vector of bits in an optimized
 * way.</p>
 * <p>Notice that vector is represented as a contiguous block of memory. This
 * bit vector representation is clearly not performant for sparse vectors.</p>
 * @ingroup utility
 */


/**
 * @fn BitVector::BitVector(int size);
 * Build a bit vector of the given size.
 * @param size	Count of bits in the vector.
 */


/**
 * @fn BitVector::BitVector(const BitVector& vec);
 * Build a vector by copying the given one.
 * @param vec	Vector to copy.
 */


/**
 * Build a vector by extending or reducing the given one.
 * @param vec		Vector to extend or to reduce.
 * @param new_size	Size of the built vector.
 */
BitVector::BitVector(const BitVector& vec, int new_size): _size(new_size) {
	ASSERTP(new_size > 0, "size must be positive");
	bits = new unsigned char[bytes()];
	if(new_size >= vec._size) {
		memcpy(bits, vec.bits, vec._size);
		memset(bits + vec.bytes(), 0, bytes() - vec.bytes());
	}
	else {
		memcpy(bits, vec.bits, bytes());
		mask();
	}
}


/**
 * Same as copy().
 */
BitVector& BitVector::operator=(const BitVector& vec) {
	if(bytes() && bytes() == vec.bytes())
		copy(vec);
	else {
		if(bits)
			delete bits;
		if(!vec.bits)
			bits = 0;
		else {
			_size = vec._size;
			bits = new unsigned char[bytes()];
			copy(vec);
		}
	}
	return *this;
}



/**
 * @fn int BitVector::size(void) const;
 * Get the size in bits of the vector.
 * @return	Vector size.
 */


/**
 * @fn bool BitVector::bit(int index) const;
 * Get the value of a bit. It is an error to pass index higher than vector size.
 * @param index	Bit index.
 * @return		Value of the bit.
 */


/**
 * @fn bool BitVector::isEmpty(void) const;
 * Test if the vector is empty.
 * @return	True if no bit is set, false else.
 */


/**
 * @fn bool BitVector::includes(const BitVector& vec) const;
 * Test if the current vector includes the given one.
 * @param vec	Vector to use.
 * @return		True if the inclusion holds, false else.
 */


/**
 * @fn bool BitVector::includesStrictly(const BitVector &vec) const;
 * Test if the current vector includes strictly the given one.
 * @param vec	Vector to use.
 * @return		True if the strict inclusion holds, false else.
 */


/**
 * @fn bool BitVector::equals(const BitVector& vec) const;
 * Test if two vectors are equals.
 * @param vec	Second vector to test with.
 * @return		True if the vectors are equal, false else.
 */


/**
 * @fn void BitVector::set(int index) const;
 * Set to true a bit.
 * @param index	Index of the bit to set. It must be higher or equal to vector
 * size.
 */


/**
 * @fn void BitVector::set(int index, bool value) const;
 * Set the value of a bit.
 * @param index	Index of the bit to set. It must be higher or equal to vector
 * size.
 * @param value	Value of the bit.
 */


/**
 * @fn void BitVector::clear(int index) const;
 * Set a bit to false.
 * @param index Index of the bit to set. It must be higher or equal to vector
 * size.
 */


/**
 * @fn void BitVector::copy(const BitVector& vec) const;
 * Copy the giuven vector in the current vector.
 * @param vec	Vector to copy from.
 */


/**
 * @fn void BitVector::clear(void);
 * Set to false all bits in the vector.
 */


/**
 * @fn void BitVector::applyNot(void);	
 * Apply NOT-operator on the current vector.
 */


/**
 * @fn void BitVector::applyOr(const BitVector& vec);
 * Apply the OR-operation on this vector with given one.
 * @param vec	Vector to process with.
 */


/**
 * @fn void BitVector::applyAnd(const BitVector& vec);
 * Apply the AND-operation on this vector with given one.
 * @param vec	Vector to process with.
 */

 
/**
 * @fn void BitVector::applyReset(const BitVector& vec);
 * Apply the RESET-operation (current & ~vec) on this vector with given one.
 * @param vec	Vector to process with.
 */
 

/**
 * @fn BitVector BitVector::makeNot(void) const;
 * Build a new vector as the result of operation NOT of the current vector.
 * @return	Result of NOT on current vector.
 */


/**
 * @fn BitVector BitVector::makeOr(const BitVector& vec) const;
 * Build a new vector as the result of operation OR on the current vector and
 * the given one.
 * @param vec	Second vector of operation.
 * @return		Result of OR operation.
 */


/**
 * @fn BitVector BitVector::makeAnd(const BitVector& vec) const;
 * Build a new vector as the result of operation AND on the current vector and
 * the given one.
 * @param vec	Second vector of operation.
 * @return		Result of AND operation.
 */


/**
 * @fn BitVector BitVector::makeReset(const BitVector& vec) const;
 * Build a new vector as the result of operation RESET on the current vector and
 * the given one.
 * @param vec	Second vector of operation.
 * @return		Result of RESET operation.
 */


/**
 * @fn BitVector::operator bool(void) const;
 * Same as NOT isEmpty().
 */


/**
 * @fn bool BitVector::operator[](int index) const;
 * Same as bit().
 */


/**
 * @fn BitVector BitVector::operator~(void) const;
 * Same as makeNot().
 */


/**
 * @fn BitVector BitVector::operator|(const BitVector& vec) const;
 * Same as makeOr().
 */


/**
 * @fn BitVector BitVector::operator&(const BitVector& vec) const;
 * Same as makeAnd().
 */


/**
 * @fn BitVector BitVector::operator+(const BitVector& vec) const;
 * Same makeOr().
 */


/**
 * @fn BitVector BitVector::operator-(const BitVector& vec) const;
 * Same as makeReset().
 */


/**
 * @fn BitVector& BitVector::operator|=(const BitVector& vec);
 * Same as applyOr().
 */


/**
 * @fn BitVector& BitVector::operator&=(const BitVector& vec);
 * Same as applyAnd().
 */


/**
 * @fn BitVector& BitVector::operator+=(const BitVector& vec);
 * Same as applyOr().
 */


/**
 * @fn BitVector& BitVector::operator-=(const BitVector& vec);
 * Same makeReset().
 */


/**
 * @fn bool BitVector::operator==(const BitVector& vec) const;
 * Same as equals().
 */


/**
 * @fn bool BitVector::operator!=(const BitVector& vec) const;
 * Same as NOT equals().
 */


/**
 * @fn bool BitVector::operator<(const BitVector& vec) const;
 * Same as includesStrictly().
 */


/**
 * @fn bool BitVector::operator<=(const BitVector& vec) const;
 * Same as includes().
 */


/**
 * @fn bool BitVector::operator>(const BitVector& vec) const;
 * Reverse form of includesStricly().
 */


/**
 * @fn bool BitVector::operator>=(const BitVector& vec) const;
 * Reverse form of includes().
 */


/**
 * Print the bit vector in a binary form.
 * @param out	Output to use.
 */
void BitVector::print(io::Output& out) const {
	for(int i = 0; i < size(); i++)
		out << (bit(i) ? '1' : '0');
}


/**
 * Count the number of bits whose value is 1.
 */
int BitVector::countBits(void) const {
	int result = 0;
	for(int i = 0; i < bytes(); i++)
		result += bit_counter.counts[bits[i]];
	return result;
}


/**
 * Test if vectors have some bit to 1 in common.
 * @param bv	Bit vector to compare with.
 * @return		True if there is something common, false else.
 */
bool BitVector::meets(const BitVector& bv) {
	int s = min(bytes(), bv.bytes());
	for(int i = 0; i < s; i++)
		if(bits[i] & bv.bits[i])
			return true;
	return false;
}



/**
 * @class Iterator::OneIterator
 * This class represents an iterator on the bits containing a one in a bit
 * vector. As a value, it returns the bit vector positions containing a 1.
 * @note May be fully improved by benefiting from the test a full byte
 * for presence of at least a one inside.
 */


/**
 * @fn BitVector::OneIterator::OneIterator(const BitVector& bit_vector);
 * Build a new bit-to-one iterator.
 * @param bit_vector	Bit vector to iterate on.
 */


/**
 * @fn int BitVector::countOnes(void) const;
 * Count the number of ones in the vector.
 * @return	Number of ones.
 */
int BitVector::countOnes(void) const {
	int cnt = 0;
	for(int i = 0; i < bytes(); i++)
		cnt += ones(bits[i]);
	return cnt;
}


/**
 * @fn int BitVector::countZeroes(void) const;
 * Count the number of zeroes in the vector.
 * @return	Number of zeroes.
 */

} // elm
