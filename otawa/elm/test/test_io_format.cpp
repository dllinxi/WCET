/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_io_format.cpp -- formatted io classes test.
 */

#include <elm/util/test.h>
#include <elm/io.h>
#include <elm/string.h>
#include <elm/io/BlockInStream.h>

using namespace elm;
using namespace elm::io;

inline String do_test(IntFormat fmt) {
	StringBuffer buf;
	buf << fmt;
	return buf.toString();
}

// Entry point
TEST_BEGIN(io_format)
	
	// Testing the base
	CHECK_EQUAL(do_test(666), String("666"));
	CHECK_EQUAL(do_test(hex(666)), String("29a"));
	CHECK_EQUAL(do_test(bin(666)), String("1010011010"));
		
	// Testing width
	CHECK_EQUAL(do_test(width(8, 666)), String("666     "));
	CHECK_EQUAL(do_test(width(2, 666)), String("666"));
	
	// Testing alignment
	CHECK_EQUAL(do_test(left(width(8, 666))), String("666     "));
	CHECK_EQUAL(do_test(right(width(8, 666))), String("     666"));
	CHECK_EQUAL(do_test(center(width(8, 666))), String("  666   "));
	CHECK_EQUAL(do_test(left(666)), String("666"));
	CHECK_EQUAL(do_test(right(666)), String("666"));
	CHECK_EQUAL(do_test(center(666)), String("666"));
	
	// Testing padding
	CHECK_EQUAL(do_test(left(width(8, pad('!', 666)))), String("666!!!!!"));
	CHECK_EQUAL(do_test(right(width(8, pad('!', 666)))), String("!!!!!666"));
	CHECK_EQUAL(do_test(center(width(8, pad('!', 666)))), String("!!666!!!"));
	
	// Test special numbers
	CHECK_EQUAL(do_test(center(width(8, pad('!', -666)))), String("!!-666!!"));
	CHECK_EQUAL(do_test(center(width(8, pad('!', 0)))), String("!!!0!!!!"));
	CHECK_EQUAL(do_test(hex(t::uint32(-1))), String("ffffffff"));
	CHECK_EQUAL(do_test(hex(t::int32(-1))), String("ffffffff"));
	
	// Test upper case
	CHECK_EQUAL(do_test(hex(uppercase(666))), String("29A"));
	
	// Type test
	CHECK_EQUAL(do_test(hex((char)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((signed char)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((unsigned char)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((short)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((signed short)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((unsigned short)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((long)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((signed long)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((unsigned long)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((int)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((signed int)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((unsigned int)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((signed)111)), String("6f"));
	CHECK_EQUAL(do_test(hex((unsigned)111)), String("6f"));
	
	BlockInStream instream("666", 3);
	Input in(instream);
	int x;
	FAIL_ON_EXCEPTION(IOException, in >> x);
	CHECK_EQUAL(x, 666);

	{
		string r = _ << fmt(666);
		CHECK_EQUAL(r, string("666"));
		string rr = _ << fmt(1.2);
		CHECK_EQUAL(rr, string("1.2"));
		string rrr = _ << fmt("ok");
		CHECK_EQUAL(rrr, string("ok"));
	}

TEST_END

