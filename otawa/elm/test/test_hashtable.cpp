/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_hastable.cpp -- unit tests for elm::xxx::HashTable classes.
 */

#include <elm/util/test.h>
#include <elm/genstruct/HashTable.h>

using namespace elm;

// test_hashtable()
TEST_BEGIN(hashtable)

	// Simple key
	{
		genstruct::HashTable<int, int> htab;
		CHECK(htab.isEmpty() == true);
		CHECK(htab.count() == 0);
		htab.put(666, 111);
		CHECK(htab.isEmpty() == false);
		CHECK(htab.count() == 1);
		CHECK(htab.get(666, 0) == 111);
		CHECK(htab.get(111, 0) == 0);
		htab.put(777, 222);
		CHECK(htab.isEmpty() == false);
		CHECK(htab.count() == 2);
		CHECK(htab.get(666, 0) == 111);
		CHECK(htab.get(777, 0) == 222);
		CHECK(htab.get(111, 0) == 0);
		htab.remove(666);
		CHECK(htab.count() == 1);
		CHECK(htab.get(666, 0) == 0);
		CHECK(htab.get(777, 0) == 222);
	}

	// Complex key
	{
		genstruct::HashTable<String, int> htab;
		String k1("key1"), k2("key2"), k3("key3");
		CHECK(htab.isEmpty() == true);
		CHECK(htab.count() == 0);
		htab.put(k1, 111);
		CHECK(htab.isEmpty() == false);
		CHECK(htab.count() == 1);
		CHECK(htab.get(k1, 0) == 111);
		CHECK(htab.get(k3, 0) == 0);
		htab.put(k2, 222);
		CHECK(htab.isEmpty() == false);
		CHECK(htab.count() == 2);
		CHECK(htab.get(k1, 0) == 111);
		CHECK(htab.get(k2, 0) == 222);
		CHECK(htab.get(k3, 0) == 0);
		htab.remove(k1);
		CHECK(htab.count() == 1);
		CHECK(htab.get(k1, 0) == 0);
		CHECK(htab.get(k2, 0) == 222);
	}

	// Iterator test
	{
		genstruct::HashTable<int, int> tab;
		for(int i = 0; i < 10; i++)
			tab.put(i, 9 - i);
		int mask = 0x3ff;
		for(genstruct::HashTable<int, int>::KeyIterator key(tab); key; key++) {
			CHECK(mask & (1 << key));
			mask &= ~(1 << key);
		}
		CHECK(!mask);
		mask = 0x3ff;
		for(genstruct::HashTable<int, int>::Iterator item(tab); item; item++) {
			CHECK(mask & (1 << item));
			mask &= ~(1 << item);
		}
		CHECK(!mask);
	}

	// operator[] test
	{
		genstruct::HashTable<cstring, int> tab;
		tab["ok"] = 111;
		tab["ko"] = 666;
		CHECK_EQUAL(*(tab["ok"]), 111);
		CHECK_EQUAL(*(tab["ko"]), 666);
	}

	// copy of table
	{
		genstruct::HashTable<string, int> t, tt;
		for(int i = 0; i < 100; i++)
			t.put(_ << i << (-i), i);
		tt = t;
		bool failed = false;
		for(int i = 0; i < 100; i++) {
			string k = _ << i << (-i);
			Option<int> o = tt.get(k);
			if(!o || *o != i) {
				failed = true;
				break;
			}
		}
		CHECK(!failed);
	}

TEST_END
