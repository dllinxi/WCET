/*
 *	$Id$
 *	Test for type_info class.
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

#include <iostream>
#include <elm/type_info.h>
#include <elm/util/test.h>
#include <elm/util/Option.h>
#include <elm/genstruct/HashTable.h>
#include <elm/genstruct/Vector.h>

using namespace elm;

typedef enum enm_t {
	A,
	B,
	C
} enm_t;

class MyClass { };

TEST_BEGIN(type_info)

	CHECK(type_info<t::int8>::null == 0);
	CHECK(type_info<t::int8>::is_signed == true);
	CHECK(type_info<t::int8>::min == -0x80);
	CHECK(type_info<t::int8>::max == 0x7f);

	CHECK(type_info<t::uint8>::null == 0);
	CHECK(type_info<t::uint8>::is_signed == false);
	CHECK(type_info<t::uint8>::min == 0);
	CHECK(type_info<t::uint8>::max == 0xff);

	CHECK(type_info<t::int16>::null == 0);
	CHECK(type_info<t::int16>::is_signed == true);
	CHECK(type_info<t::int16>::min == t::int16(-32768));
	CHECK(type_info<t::int16>::max == 0x7fff);

	CHECK(type_info<t::uint16>::null == 0);
	CHECK(type_info<t::uint16>::is_signed == false);
	CHECK(type_info<t::uint16>::min == 0);
	CHECK(type_info<t::uint16>::max == 0xffff);

	CHECK(type_info<int>::null == 0);
	CHECK(type_info<int>::is_signed == true);
	CHECK(type_info<int>::min == (int)-0x80000000);
	CHECK(type_info<int>::max == 0x7fffffff);

	CHECK(type_info<unsigned>::null == 0);
	CHECK(type_info<unsigned>::is_signed == false);
	CHECK(type_info<unsigned>::min == 0);
	CHECK(type_info<unsigned>::max == 0xffffffff);

	CHECK(type_info<t::int32>::null == 0);
	CHECK(type_info<t::int32>::is_signed == true);
	CHECK(type_info<t::int32>::min == t::int32(-0x80000000));
	CHECK(type_info<t::int32>::max == 0x7fffffff);

	CHECK(type_info<t::uint32>::null == 0);
	CHECK(type_info<t::uint32>::is_signed == false);
	CHECK(type_info<t::uint32>::min == 0);
	CHECK(type_info<t::uint32>::max == 0xffffffff);

	CHECK(type_info<t::int64>::null == 0);
	CHECK(type_info<t::int64>::is_signed == true);
	CHECK(type_info<t::int64>::min == t::int64(-0x8000000000000000LL));
	CHECK(type_info<t::int64>::max == 0x7fffffffffffffffLL);

	CHECK(type_info<t::uint64>::null == 0);
	CHECK(type_info<t::uint64>::is_signed == false);
	CHECK(type_info<t::uint64>::min == 0LL);
	CHECK(type_info<t::uint64>::max == 0xffffffffffffffffLL);

	CHECK(type_info<enm_t>::is_ptr == 0);
	//cerr << io::hex(type_info<unsigned long long>::max) << io::endl;

	// embed test
	{
		type_info<bool>::embed_t v;
		type_info<bool>::put(v, true);
		CHECK_EQUAL(type_info<bool>::get(v), true);

		int i = 666;
		type_info<int&>::embed_t ir;
		type_info<int&>::put(ir, i);
		CHECK_EQUAL(type_info<int&>::get(ir), 666);

		const int ci = 111;
		type_info<const int&>::embed_t cir;
		type_info<const int&>::put(cir, ci);
		CHECK_EQUAL(type_info<const int&>::get(cir), 111);
	}

	// embed in option test
	{
		Option<int&> o = none;
		int i = 123;
		int& ir = i;
		Option<int&> o2(some<int&>(ir));
		CHECK_EQUAL(o2.isNone(), false);
		CHECK_EQUAL(o2.value(), 123);
		o = o2;
		CHECK_EQUAL(o.isNone(), false);
		CHECK_EQUAL(o.value(), 123);
	}

	// embed in hashtable (const reference)
	/*{
		genstruct::HashTable<const int&, int> h;
		int a = 0, b = 1, c = 2, d = 3;
		h.put(a, a);
		h.put(b, b);
		h.put(c, c);
		CHECK_EQUAL(a, h.get(a, d));
		CHECK_EQUAL(b, h.get(b, d));
		CHECK_EQUAL(c, h.get(c, d));
	}

	// embed in hashtable (reference)
	{
		genstruct::HashTable<int&, int> h;
		int a = 0, b = 1, c = 2, d = 3;
		h.put(a, a);
		h.put(b, b);
		h.put(c, c);
		CHECK_EQUAL(a, h.get(a, d));
		CHECK_EQUAL(b, h.get(b, d));
		CHECK_EQUAL(c, h.get(c, d));
	}*/

	// embed a class
	{
		typedef genstruct::Vector<int> v_t;
		genstruct::HashTable<int, v_t> h;
		v_t v1;
		v1.add(0);
		h.put(0, v1);
		Option<v_t> v = h.get(0);
		CHECK(v);
		CHECK_EQUAL(*v, v1);
		v = h.get(1);
		CHECK(!v);
	}

	// parameter/result passing
	{
		cout << "T (in, out, val, ref)\n";
		cout << "bool: "
			 << type_info<type_info<bool>::in_t>::name() << ", "
			 << type_info<type_info<bool>::out_t>::name() << ", "
			 << type_info<type_info<bool>::val_t>::name() << ", "
			 << type_info<type_info<bool>::ref_t>::name()<< io::endl;
		cout << "int: "
			 << type_info<type_info<int>::in_t>::name() << ", "
			 << type_info<type_info<int>::out_t>::name() << ", "
			 << type_info<type_info<int>::val_t>::name() << ", "
			 << type_info<type_info<int>::ref_t>::name()<< io::endl;
		cout << "float: "
			 << type_info<type_info<float>::in_t>::name() << ", "
			 << type_info<type_info<float>::out_t>::name() << ", "
			 << type_info<type_info<float>::val_t>::name() << ", "
			 << type_info<type_info<float>::ref_t>::name()<< io::endl;
		cout << "int *: "
			 << type_info<type_info<int *>::in_t>::name() << ", "
			 << type_info<type_info<int *>::out_t>::name() << ", "
			 << type_info<type_info<int *>::val_t>::name() << ", "
			 << type_info<type_info<int *>::ref_t>::name()<< io::endl;
		cout << "C: "
			 << type_info<type_info<MyClass>::in_t>::name() << ", "
			 << type_info<type_info<MyClass>::out_t>::name() << ", "
			 << type_info<type_info<MyClass>::val_t>::name() << ", "
			 << type_info<type_info<MyClass>::ref_t>::name() << io::endl;
	}

TEST_END
