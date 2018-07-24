/*
 *	$Id$
 *	Test for SortedSLList class.
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

#include <elm/genstruct/AssocList.h>
#include <elm/util/test.h>

using namespace elm;
using namespace elm::genstruct;

int main(void) {
	CHECK_BEGIN("inhstruct_Tree")

	AssocList<string, int> list;
	
	// Fill it
	CHECK(list.isEmpty());
	list.put("b", 111);
	CHECK(!list.isEmpty());
	CHECK(list.hasKey("b"));
	CHECK(list.get("b", -1) == 111);
	list.put("c", 222);
	CHECK(list.hasKey("c"));
	CHECK(list.get("c", -1) == 222);
	list.put("a", 333);
	CHECK(list.hasKey("a"));
	CHECK(list.get("a", -1) == 333);
	
	// Check all
	CHECK(list.get("a", -1) == 333);
	CHECK(list.get("b", -1) == 111);
	CHECK(list.get("c", -1) == 222);
	CHECK(list.get("d", -1) == -1);
	Option<int> res = list.get("b");
	CHECK(res.isOne());
	CHECK(res == 111);
	res = list.get("d");
	CHECK(res.isNone());
	CHECK(list.count() == 3);
	
	// Iterator class
	{
		AssocList<string, int>::Iterator iter(list);
		CHECK(iter);
		CHECK(*iter == 333);
		iter++;
		CHECK(iter);
		CHECK(*iter == 111);
		iter++;
		CHECK(iter);
		CHECK(*iter == 222);
		iter++;
		CHECK(!iter);
	}
	
	// KeyIterator class
	{
		AssocList<string, int>::KeyIterator iter(list);
		CHECK(iter);
		CHECK(*iter == "a");
		iter++;
		CHECK(iter);
		CHECK(*iter == "b");
		iter++;
		CHECK(iter);
		CHECK(*iter == "c");
		iter++;
		CHECK(!iter);
	}

	CHECK_END
}
