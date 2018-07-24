/*
 *	BiDiList class test
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
#include <elm/data/BiDiList.h>
#include <elm/genstruct/Vector.h>

using namespace elm;

class Eq {
public:
	bool isEqual(int x, int y) const { return x == y; }
};

TEST_BEGIN(bidilist)

	// Test initial
	{
		BiDiList<int> list;
		CHECK(list.count() == 0);
	}

	// Test addFirst
	{
		BiDiList<int> list;
		for(int i = 0; i < 10; i++)
			list.addFirst(i);
		CHECK(list.count() == 10);
		int i = 9;
		bool good = true;
		for(BiDiList<int>::Iter iter(list); iter; iter++, i--)
			if(iter.item() != i) {
				good = false;
				break;
			}
		CHECK(good);
	}


	// List copy test
	{
		BiDiList<int> list1, list2;
		list1.add(1);
		list1.add(2);
		list1.add(3);
		CHECK(list2.isEmpty());
		list2 = list1;
		CHECK(!list2.isEmpty());
		CHECK(list2 & 1);
		CHECK(list2.contains(2));
		CHECK(list2.contains(3));
		CHECK(!list1.isEmpty());
		CHECK(list1.contains(1));
		CHECK(list1.contains(2));
		CHECK(list1.contains(3));
		list2.remove(2);
		CHECK(!list2.contains(2));
		CHECK(list1.contains(2));
		list2.remove(3);
		CHECK(!list2.contains(3));
		CHECK(list1.contains(3));
		list2.remove(1);
		CHECK(!list2.contains(1));
		CHECK(list1.contains(1));
	}

	// test addAll, removeAll
	{
		/*genstruct::Vector<int> v;
		v.add(1);
		v.add(2);
		v.add(3);
		BiDiList<int> l;
		l.addAll(v);
		CHECK(l.contains(1));
		CHECK(l.contains(2));
		CHECK(l.contains(3));*/
	}

	// test remove with iterator
	{
		BiDiList<int> l;
		for(int i = 0; i < 10; i++)
			l += i;
		for(BiDiList<int>::Iter i(l); i;) {
			if(*i % 2 == 0)
				l.remove(i);
			else
				i++;
		}
		int pairs = 0;
		for(BiDiList<int>::Iter i(l); i; i++)
			if(*i % 2 == 0)
				pairs++;
		CHECK_EQUAL(pairs, 0);
	}

	// test for null
	{
		CHECK(BiDiList<int>::null.isEmpty());
	}

	// equality and inclusion test
	{
		BiDiList<int> l1, l2;
		for(int i = 0; i < 10; i++) {
			l1 += i;
			if(i % 2 == 0 || i == 6)
				l2 += i;
		}
		CHECK(l1 == l1);
		CHECK(l1 != l2);
		CHECK(l1 >= l1);
		CHECK(!(l1 > l1));
		CHECK(l1 >= l2);
		CHECK(l1 > l2);
	}

	// special equivalence
	{
		Eq e;
		typedef EquivManager<int, Eq> man_t;
		man_t m(e);
		BiDiList<int, man_t> l(m);
	}
	{
		BiDiList<int, EquivManager<Eq> > l;
	}

TEST_END

