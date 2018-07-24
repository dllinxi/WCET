/*
 * $Id$
 * Copyright (c) 2005, IRIT UPS.
 *
 * test/test_SLList/test_SLList.cpp -- test program for SLList class.
 */

#include <elm/util/test.h>
#include <elm/data/List.h>
#include <elm/data/SortedList.h>
#include <elm/genstruct/AssocList.h>
#include <elm/genstruct/Vector.h>

using namespace elm;

class Eq {
public:
	bool isEqual(int x, int y) const { return x == y; }
};

TEST_BEGIN(list)

	// Test initial
	{
		List<int> list;
		CHECK(list.count() == 0);
	}

	// Test addFirst
	{
		List<int> list;
		for(int i = 0; i < 10; i++)
			list.addFirst(i);
		CHECK(list.count() == 10);
		int i = 9;
		bool good = true;
		for(List<int>::Iter iter(list); iter; iter++, i--)
			if(iter.item() != i) {
				good = false;
				break;
			}
		CHECK(good);
	}


	// List copy test
	{
		List<int> list1, list2;
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
		genstruct::Vector<int> v;
		v.add(1);
		v.add(2);
		v.add(3);
		genstruct::SLList<int> l;
		l.addAll(v);
		CHECK(l.contains(1));
		CHECK(l.contains(2));
		CHECK(l.contains(3));
	}

	// test remove with iterator
	{
		List<int> l;
		for(int i = 0; i < 10; i++)
			l += i;
		for(List<int>::PrecIter i(l); i;) {
			if(*i % 2 == 0)
				l.remove(i);
			else
				i++;
		}
		int pairs = 0;
		for(List<int>::Iter i(l); i; i++)
			if(*i % 2 == 0)
				pairs++;
		CHECK_EQUAL(pairs, 0);
	}

	// test for null
	{
		CHECK(genstruct::SLList<int>::null.isEmpty());
	}

	// sub-list iterator
	{
		List<int> l;
		for(int i = 9; i >= 0; i--)
			l += i;
		List<int>::Iter b, e;
		for(List<int>::Iter i = l;
		i;
		i++) {
			if(i == 3)
				b = i;
			else if(i == 8)
				e = i;
		}
		CHECK(b);
		CHECK(e);
		List<int>::SubIter s(b, e);
		for(int i = 3; i < 8; i++) {
			CHECK(s);
			CHECK(*s == i);
			s++;
		}
		CHECK(!s);
	}

	// equality and inclusion test
	{
		List<int> l1, l2;
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
		List<int, man_t> l(m);
	}
	{
		List<int, EquivManager<Eq> > l;
	}

	// sorted list
	{
		SortedList<int> s;
		s.add(0);
		s.add(10);
		s.add(5);
		SortedList<int>::Iter i(s);
		CHECK_EQUAL(*i, 0);
		i++;
		CHECK_EQUAL(*i, 5);
		i++;
		CHECK_EQUAL(*i, 10);
		i++;
		CHECK(i.ended());
	}

	// set test
	{
		ListSet<int> l;
	}

	// test map
	{
		/*ListMap<int, int> m;
		m.put(0, 0);
		m.put(1, 1);
		CHECK_EQUAL(m.get(0, -1), 0);*/
	}

	// compatibility test
	{
		genstruct::AssocList<int, int> a;
		a.put(0, 0);
		for(typename genstruct::AssocList<int, int>::Iterator i(a); i; i++)
			;
	}

TEST_END

