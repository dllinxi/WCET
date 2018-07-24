/*
 *	$Id$
 *	HashKey test file
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2010, IRIT UPS.
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
#include <elm/util/HashKey.h>

using namespace elm;

TEST_BEGIN(hashkey)

	// generic hash
	{
		typedef string t;
		t t1 = "012";
		t t2 = "012";
		t t3 = "210";
		typedef HashKey<t> ht;
		CHECK_EQUAL(ht::hash(t1), ht::hash(t1));
		CHECK_EQUAL(ht::hash(t1), ht::hash(t2));
		CHECK(ht::equals(t1, t1));
		CHECK(ht::equals(t1, t2));
		CHECK(!ht::equals(t1, t3));
	}

	// hasher test (equality)
	{
		t::hash h1, h2;
		{
			Hasher h;
			h.add(10);
			h.add("coucou");
			h.add(true);
			h.add(&h1);
			h1 = h.hash();
		}
		{
			Hasher h;
			h.add(10);
			h.add("coucou");
			h.add(true);
			h.add(&h1);
			h2 = h.hash();
		}
		CHECK_EQUAL(h1, h2);
	}

	// hasher test (inequality)
	{
		t::hash h1, h2;
		{
			Hasher h;
			h.add(10);
			h.add("coucou");
			h.add(true);
			h.add(&h1);
			h1 = h.hash();
		}
		{
			Hasher h;
			h.add(10);
			h.add("coucou");
			h.add(false);
			h.add(&h1);
			h2 = h.hash();
		}
		CHECK(h1 != h2);
	}

	TEST_END

