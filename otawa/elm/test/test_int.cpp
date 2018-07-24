/*
 * test_int.cpp
 *
 *  Created on: 6 janv. 2011
 *      Author: casse
 */

#include <elm/util/test.h>
#include <elm/int.h>
#include <elm/compare.h>

using namespace elm;

TEST_BEGIN(int)
	for(int i = 0; i < 32; i++)
		CHECK_EQUAL(msb(t::uint32(1) << i), i);
	for(int i = 1; i < 32; i++)
		CHECK_EQUAL(msb((t::uint(1) << i) | (t::uint32(1) << (i - 1))), i);

	CHECK_EQUAL(type_info<int8_t>::max,	 int8_t(0x7f));
	CHECK_EQUAL(type_info<int16_t>::max, int16_t(0x7fff));
	CHECK_EQUAL(type_info<int32_t>::max, int32_t(0x7fffffff));
	CHECK_EQUAL(type_info<int64_t>::max, int64_t(0x7fffffffffffffffLL));

	CHECK_EQUAL(type_info<int8_t>::min,	 int8_t(0x80));
	CHECK_EQUAL(type_info<int16_t>::min, int16_t(0x8000));
	CHECK_EQUAL(type_info<int32_t>::min, int32_t(0x80000000));
	CHECK_EQUAL(type_info<int64_t>::min, int64_t(0x8000000000000000LL));

	t::uint32 r;
	bool over;
	r = mult(t::uint32(10), t::uint32(10), over);
	CHECK(!over && r == 100);
	r = mult(t::uint32(0x00ffffff), t::uint32(0x100), over);
	CHECK(!over && r == 0xffffff00);
	r = mult(t::uint32(0xffffff00), t::uint32(0xff), over);
	CHECK(over);

	// test max
	{
		int x = 10, y = 20;
		CHECK_EQUAL(elm::max(x, y), 20);
		CHECK_EQUAL(max(x, 5), 10);
	}

TEST_END

