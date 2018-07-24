/*
 *	Vector class interface
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
#include <elm/util/test.h>
#include <elm/data/Vector.h>

using namespace elm;

// test_vector()
TEST_BEGIN(vector)
	
	// Simple test
	{
		Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(2);
		v.add(3);
		CHECK(v.count() == 4);
		CHECK(v[0] == 0);
		CHECK(v[1] == 1);
		CHECK(v[2] == 2);
		CHECK(v[3] == 3);
	}
	
	// Grow test
	{
		Vector<int> v;
		for(int i = 0; i < 32; i++)
			v.add(i);
		bool ok = true;
		for(int i = 0; i < 32; i++)
			if(v[i] != i)
				ok = false;
		CHECK(ok);
	}
	
	// Insertion
	{
		Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(3);
		v.insert(2, 2);
		CHECK(v[0] == 0);
		CHECK(v[1] == 1);
		CHECK(v[2] == 2);
		CHECK(v[3] == 3);
		v.insert(4, 4);
		CHECK(v[3] == 3);
		CHECK(v[4] == 4);
	}
	
	// Contains and index
	{
		Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(0);
		v.add(1);
		v.add(0);
		v.add(1);
		CHECK_EQUAL(v.contains(1), true);
		CHECK_EQUAL(v.contains(2), false);
		CHECK_EQUAL(v.indexOf(1), 1);
		CHECK(v.indexOf(2) < 0);
		CHECK_EQUAL(v.indexOf(1, 2), 3);
		CHECK(v.indexOf(1, 6) < 0);
		CHECK_EQUAL(v.lastIndexOf(1), 5);
		CHECK(v.lastIndexOf(2) < 0);
		CHECK_EQUAL(v.lastIndexOf(1, 4), 3);
		CHECK(v.lastIndexOf(1, 1) < 0);
	}
	
	// Removal test
	{
		Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(2);
		v.add(3);
		v.add(4);
		v.removeAt(4);
		CHECK_EQUAL(v.count(), 4);
		CHECK_EQUAL(v[0], 0);
		CHECK_EQUAL(v[1], 1);
		CHECK_EQUAL(v[2], 2);
		CHECK_EQUAL(v[3], 3);
		v.removeAt(1);
		CHECK_EQUAL(v.count(), 3);
		CHECK_EQUAL(v[0], 0);
		CHECK_EQUAL(v[1], 2);
		CHECK_EQUAL(v[2], 3);
		v.remove(3);
		CHECK_EQUAL(v.count(), 2);
		CHECK_EQUAL(v[0], 0);
		CHECK_EQUAL(v[1], 2);
		v.clear();
		CHECK(v.count() == 0);
	}
	
	// Stack behaviour of the genstruct::Vector
	{
		Vector<int> v;
		v.push(0);
		CHECK(v.top() == 0);
		v.push(1);
		CHECK(v.top() == 1);
		v.push(2);
		CHECK(v.top() == 2);
		CHECK(v.pop() == 2);
		CHECK(v.pop() == 1);
		v.push(3);
		CHECK(v.top() == 3);
		CHECK(v.pop() == 3);
		CHECK(v.pop() == 0);
		CHECK(v.length() == 0);
	}
	
	// addAll test
	{
		Vector<int> v1, v2;
		for(int i = 0; i < 10; i++)
			v1.add(i);
		v2.addAll(v1);
		for(int i = 0; i < 10; i++)
			CHECK(v2[i] == i);
	}
	
	// remove from iterator
	{
		Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(2);
		v.add(3);
		Vector<int>::Iter i(v);
		i++;
		v.remove(i);
		CHECK_EQUAL(v.length(), 3);
		CHECK(v[0] == 0 && v[1] == 2 && v[2] == 3);
	}

	// vector of class type
	{
		Vector<string> v;
		v.add("1");
		v.add("2");
		v.add("3");
		Vector<string>::Iter i(v);
		CHECK(!i.ended());
		CHECK_EQUAL(*i, string("1"));
		i++;
		CHECK(!i.ended());
		CHECK_EQUAL(*i, string("2"));
		i++;
		CHECK(!i.ended());
		CHECK_EQUAL(*i, string("3"));
		i++;
		CHECK(i.ended());
	}

TEST_END


