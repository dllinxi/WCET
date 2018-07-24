/*
 *	utf8 module test
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2016, IRIT UPS.
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

#include <elm/string/utf8.h>
#include <elm/util/test.h>

using namespace elm;
using namespace elm::utf8;

TEST_BEGIN(utf8)

	{
		char s1[] = "abcd";

		Iter i(s1, 4);
		CHECK(i);
		CHECK_EQUAL(*i, char_t('a'));
		i++;
		CHECK(i);
		CHECK_EQUAL(*i, char_t('b'));
		i++;
		CHECK(i);
		CHECK_EQUAL(*i, char_t('c'));
		i++;
		CHECK(i);
		CHECK_EQUAL(*i, char_t('d'));
		i++;
		CHECK(!i);


	}

#define CODE2(x) char(0b11000000|(x >> 6)), char(0b10000000|(x & 0b111111))
	{
		char s[] = { 'a', CODE2(0x654) };
		Iter i(s, 3);
		CHECK(i);
		CHECK_EQUAL(*i, char_t('a'));
		i++;
		CHECK(i);
		CHECK_EQUAL(*i, char_t(0x654));
		i++;
		CHECK(!i);
	}

TEST_END
