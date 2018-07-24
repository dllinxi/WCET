/*
 *	$Id$
 *	DLList class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-08, IRIT UPS.
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
#include <elm/genstruct/DLList.h>

using namespace elm;

// MyDLNode class
class MyDLNode: public inhstruct::DLNode {
public:
	int v;
	inline MyDLNode(int _v): v(_v) {
	};
};



// Test
TEST_BEGIN(dllist)
	
	// Base tests
	{
		genstruct::DLList<int> l;
		CHECK(l.isEmpty());
		l.addLast(666);
		CHECK(!l.isEmpty());
		CHECK(l.count() == 1);
		CHECK(l.contains(666));
		CHECK(!l.contains(111));
		l.addLast(111);
		CHECK(!l.isEmpty());
		CHECK(l.count() == 2);
		CHECK(l.contains(666));
		CHECK(l.contains(111));
	}
	
	// Removal tests
	{
		genstruct::DLList<int> l;
		l.addLast(0);
		l.addLast(1);
		l.addLast(2);
		CHECK(l.count() == 3);
		l.remove(1);
		CHECK(l.count() == 2);
		CHECK(l.contains(0));
		CHECK(!l.contains(1));
		CHECK(l.contains(2));
		l.remove(2);
		CHECK(l.count() == 1);
		CHECK(l.contains(0));
		CHECK(!l.contains(1));
		CHECK(!l.contains(2));
		l.remove(0);
		CHECK(l.count() == 0);
		CHECK(!l.contains(0));
		CHECK(!l.contains(1));
		CHECK(!l.contains(2));
	}
	
	// Iterator test
	{
		genstruct::DLList<int> l;
		l.addLast(0);
		l.addLast(1);
		l.addLast(2);
		l.addLast(3);
		l.addLast(4);
		CHECK(l.count() == 5);
		int i = 0;
		for(genstruct::DLList<int>::Iterator iter(l); iter; iter++, i++)
			CHECK(*iter == i);
	}
	
	// Bi-directionnal walk test
	{
		inhstruct::DLList list;
		list.addLast(new MyDLNode(0));
		list.addLast(new MyDLNode(1));
		list.addLast(new MyDLNode(2));
		CHECK(list.count() == 3);
		int cnt = 0;
		for(MyDLNode *node = (MyDLNode *)list.first(); !node->atEnd();
		node = (MyDLNode *)node->next(), cnt++)
			CHECK(node->v == cnt);
		CHECK(cnt == 3);
		cnt--;
		for(MyDLNode *node = (MyDLNode *)list.last(); !node->atBegin();
		node = (MyDLNode *)node->previous(), cnt--)
			CHECK(node->v == cnt);
		CHECK(cnt == -1);
		while(!list.isEmpty()) {
			MyDLNode *node = (MyDLNode *)list.first();
			list.removeFirst();
			delete node;
		}
	}
	
	// Insert before and efter tests
	{
		inhstruct::DLList list;
		list.addLast(new MyDLNode(0));
		list.addLast(new MyDLNode(2));
		list.addLast(new MyDLNode(4));
		inhstruct::DLNode *node = list.first();
		CHECK(!node->atEnd());
		node = node->next();
		CHECK(!node->atEnd());
		node->insertBefore(new MyDLNode(1));
		node->insertAfter(new MyDLNode(3));
		int cnt = 0;
		for(MyDLNode *node = (MyDLNode *)list.first(); !node->atEnd();
		node = (MyDLNode *)node->next(), cnt++)
			CHECK(node->v == cnt);
		CHECK(cnt == 5);
		cnt--;
		for(MyDLNode *node = (MyDLNode *)list.last(); !node->atBegin();
		node = (MyDLNode *)node->previous(), cnt--)
			CHECK(node->v == cnt);
		CHECK(cnt == -1);		
		while(!list.isEmpty()) {
			MyDLNode *node = (MyDLNode *)list.first();
			list.removeFirst();
			delete node;
		}
	}
	
	// Check back iteration
	{
		genstruct::DLList<int> list;
		for(int i = 0; i < 10; i++)
			list.addLast(i);
		int n = 9;
		for(genstruct::DLList<int>::BackIterator iter(list); iter;
		iter++, n--)
			CHECK_EQUAL(*iter, n); 
	}

TEST_END


