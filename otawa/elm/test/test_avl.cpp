/*
 * $Id$
 * Copyright (c) 2004, IRIT-UPS.
 *
 * test/test_avl.cpp -- AVLTree class test.
 */

#define ELM_DEBUG_AVL

#include <elm/util/test.h>
#include <elm/genstruct/Vector.h>
#include <elm/avl/Set.h>
#include <elm/avl/Map.h>
#include <elm/genstruct/HashTable.h>
#include <elm/system/System.h>
#include <elm/io.h>
 
using namespace elm;
using namespace elm::avl;

#ifdef ELM_DEBUG_AVL
#	define LOG(cmd)	// cmd
#endif

static const int maxv = 10000;
static const int _count = 100000;

// Entry point
TEST_BEGIN(avl)
	
	// unit test
	{
		Set<int> tree;
		CHECK(tree.isEmpty());
		tree.add(100);
		CHECK(!tree.isEmpty());
		CHECK(tree.contains(100));
		CHECK(!tree.contains(0));
	}

	// copy and clear
	{
		Set<int> tree;
		tree.add(10);
		tree.add(15);
		tree.add(0);
		tree.add(-10);
		tree.add(8);
		tree.add(20);
		CHECK(tree.contains(10));
		CHECK(tree.contains(15));
		CHECK(tree.contains(0));
		CHECK(tree.contains(-10));
		CHECK(tree.contains(8));
		CHECK(tree.contains(20));
		Set<int> tree2;
		tree2 = tree;
		CHECK(!tree2.isEmpty());
		CHECK(tree2.contains(10));
		CHECK(tree2.contains(15));
		CHECK(tree2.contains(0));
		CHECK(tree2.contains(-10));
		CHECK(tree2.contains(8));
		CHECK(tree2.contains(20));
		CHECK(tree == tree2);
		tree.clear();
		CHECK(tree.isEmpty());

	}

	// check AVLTree
	{
		genstruct::Vector<int> ints;
		Set<int> tree;
		bool intensive = true;
		
		int rem_cnt = 0,
			ins_cnt = 0,
			chk_cnt = 0;
		for(int i = 0; intensive && i < _count; i++) {
			LOG(tree.dump(cerr); cerr << io::endl;)
			int a = system::System::random(maxv * 3);

			// remove
			if(a < 2 * maxv && ints.count()) {
				int n = ints[a % ints.count()];
				LOG(cerr << "removing " << ints[a % ints._count()] << io::endl;)
				ints.remove(n);
				tree.remove(n);
				intensive = !tree.contains(n);
				rem_cnt++;
			}

			// insert
			else if(a < maxv) {
				int n = a % maxv;
				LOG(cerr << "adding " << n << io::endl;)
				if(!ints.contains(n))
					ints.add(n);
				tree.add(n);
				intensive = tree.contains(n);
				ins_cnt++;
			}

			// check
			else {
				bool ok = true;
				for(int i = 0; i < ints.count(); i++)
					if(!tree.contains(ints[i]))
						ok = false;
				intensive = ok;
				chk_cnt++;
			}
		}
		cerr << "INFO: rem_cnt = " << rem_cnt
			 <<     ", ins_cnt = " << ins_cnt
			 <<     ", chk_cnt = " << chk_cnt << io::endl;
		CHECK(intensive);
	}
	
	// AVLMap
	{
		Map<string, int> map;
		Option<int> r = map.get("ok");
		CHECK(!r);

		genstruct::HashTable<string, int> htab;

		bool map_intensive = true;
		for(int i = 0; map_intensive && i < _count; i++) {
			int a = system::System::random(maxv * 4);

			// remove
			if(a < 2 * maxv && !htab.isEmpty()) {
				int n = a % htab.count();
				string kk;
				for(genstruct::HashTable<string, int>::KeyIterator k(htab); n >= 0; k++, n--)
					kk = k;
				map.remove(kk);
				htab.remove(kk);
				map_intensive = !map.hasKey(kk);
			}

			// insert
			else if(a < maxv) {
				int n = a % maxv;
				string k = _ << n;
				htab.put(k, n);
				map.put(k, n);
				map_intensive = map.get(k) == n;
			}

			// contains
			else if(a < 2 * maxv) {
				int n = a % maxv;
				string k = _ << n;
				map_intensive = htab.get(k, -1) == map.get(k, -1);
			}

			// check
			else {
				bool ok = true;
				for(genstruct::HashTable<string, int>::KeyIterator k(htab); k; k++)
					if(htab.get(k, -1) != map.get(k, -1)) {
						ok = false;
						break;
					}
				map_intensive = ok;
			}
		}
		CHECK(map_intensive);
	}

	// Map::PairIterator test
	{
		avl::Map<int, int> map;
		map.put(0, 0);
		map.put(1, 1);
		map.put(2, 2);
		map.put(3, 3);
		map.put(4, 4);
		bool equals = true;
		for(avl::Map<int, int>::PairIterator i(map); i; i++)
			if((*i).fst != (*i).snd)
				equals = false;
		CHECK(equals);
	}

	// map with setting
	{
		avl::Map<int, int> map;
		map.put(0, 0);
		map.put(1, 1);
		map.put(2, 2);
		map.put(3, 3);
		map.put(4, 4);
		for(avl::Map<int, int>::MutableIterator i(map); i; i++)
			if((*i).fst % 2 == 0)
				i.set(0);
		CHECK(map.get(0) == 0);
		CHECK(map.get(1) == 1);
		CHECK(map.get(2) == 0);
		CHECK(map.get(3) == 3);
		CHECK(map.get(4) == 0);
	}

TEST_END

