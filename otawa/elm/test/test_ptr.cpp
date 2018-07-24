/*
 *	Test for pointer classes (AutoPtr, SharedPtr).
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

#include <elm/util/SharedPtr.h>
#include <elm/util/test.h>

using namespace elm;

bool del;

class C {
public:
	C(int _x): x(_x) { }
	~C(void) { del = true; }
	int x;
};

TEST_BEGIN(ptr)

	// simple shared
	{
		del = false;
		{
			SharedPtr<C> p = new C(111);
			CHECK((bool)p == true);
			CHECK(p->x == 111);
		}
		CHECK(del == true);
	}

	// several pointers
	{
		del = false;
		{
			SharedPtr<C> p = new C(111);
			{
				SharedPtr<C> q = p;
				CHECK(q->x == 111);
				CHECK(p == q);
			}
			CHECK(del == false);
		}
		CHECK(del == true);
	}

	// array of references
	{
		del = false;
		{
			SharedPtr<C> p = new C(666);
			{
				SharedPtr<C> t[10];
				for(int i = 0; i < 10; i++)
					CHECK((bool)t[i] == false);
				for(int i = 0; i < 10; i++)
					t[i] = p;
				for(int i = 0; i < 10; i++)
					CHECK(t[i]->x == 666);
			}
			CHECK(del == false);
		}
		CHECK(del == true);
	}

TEST_END
