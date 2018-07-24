/*
 * $Id$
 * Copyright (c) 2005, IRIT UPS.
 *
 * test/test_bitvector.cpp -- test program for BitVector class.
 */

#include <elm/util/test.h>
#include <elm/util/BitVector.h>

using namespace elm;

BitVector makeBitVector(int size, unsigned long v) {
	BitVector vec(size);
	for(int i = 0; i < size; i++, v >>= 1)
		if(v & 0x00000001)
			vec.set(i);
	return vec;
}

bool equalsBitVector(const BitVector& vec, unsigned long v) {
	for(int i = 0; i < vec.size(); i++, v >>= 1)
		if(vec.bit(i) != (v & 0x00000001))
			return false;
	return true;
}

TEST_BEGIN(bitvector)
	
	// Initial test
	BitVector v1(13);
	CHECK(v1.isEmpty());
	CHECK(v1.size() == 13);
	
	// Bit Access
	v1.set(2);
	CHECK(v1.bit(2));
	CHECK(!v1.isEmpty());
	v1.set(11);
	CHECK(v1.bit(11));
	v1.clear(2);
	CHECK(!v1.bit(2));
	v1.clear(11);
	CHECK(!v1.bit(11));
	CHECK(v1.isEmpty());
	v1.set(2);
	v1.set(4);
	CHECK(v1.bit(2) && v1.bit(4));
	v1.clear(2);
	CHECK(!v1.bit(2) && v1.bit(4));
	v1.clear();
	CHECK(v1.isEmpty());
	
	// Copy and equality
	BitVector v2(13);
	v2.set(1);
	v2.set(5);
	v2.set(6);
	v2.set(12);
	BitVector v3(13);
	v3.copy(v2);
	CHECK(v2.equals(v3));
	CHECK(!v1.equals(v2));

	// Check inclusion
	CHECK(v2.includes(v3));
	CHECK(!v2.includesStrictly(v3));
	v3.clear(5);
	CHECK(v2.includes(v3));
	CHECK(v2.includesStrictly(v3));
	CHECK(!v3.includes(v2));
	CHECK(!v3.includesStrictly(v2));

	BitVector v4 = makeBitVector(13, 0x0303);
	BitVector v5 = makeBitVector(13, 0x0505);

	// Check applies
	BitVector v9 = v4;
	v9.applyAnd(v5);
	CHECK(equalsBitVector(v9, 0x0101));	
	BitVector v10 = v4;
	v10.applyOr(v5);
	CHECK(equalsBitVector(v10, 0x0707));	
	BitVector v11 = v4;
	v11.applyReset(v5);
	CHECK(equalsBitVector(v11, 0x0202));	
	BitVector v13 = v4;
	v13.applyNot();
	CHECK(equalsBitVector(v13, 0xfcfc));	
	
	// Check makes
	BitVector v6 = v4.makeAnd(v5);
	CHECK(equalsBitVector(v6, 0x0101));
	BitVector v7 = v4.makeOr(v5);
	CHECK(equalsBitVector(v7, 0x0707));
	BitVector v8 = v4.makeReset(v5);
	CHECK(equalsBitVector(v8, 0x0202));
	BitVector v12 = v4.makeNot();
	CHECK(equalsBitVector(v12, 0xfcfc));
	
TEST_END

