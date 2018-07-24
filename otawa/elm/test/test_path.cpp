/*
 *	Path class test
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
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
#include <elm/sys/Path.h>

using namespace elm;

TEST_BEGIN(path)

// parent test
{
	sys::Path p = sys::Path("a") / "b" / "c";
	CHECK_EQUAL(p.namePart(), string("c"));
	p = p.parent();
	CHECK_EQUAL(p.namePart(), string("b"));
	p = p.parent();
	CHECK_EQUAL(p.namePart(), string("a"));
	p = p.parent();
	CHECK(!p);
}

// subPathOf/prefixedBy test
{
	sys::Path b = sys::Path("a") / "b";
	sys::Path f = b / "c";
	CHECK(!b.subPathOf(f));
	CHECK(f.subPathOf(b));
	CHECK(f.prefixedBy(b));
	CHECK(!b.prefixedBy(f));
	CHECK(!f.isPrefixOf(b));
	CHECK(b.isPrefixOf(f));

	sys::Path g = sys::Path("a") / "bb";
	CHECK(!b.isPrefixOf(g));
}

// relativeTo test
{
	sys::Path b = sys::Path("a") / "b";
	sys::Path f = sys::Path("a") / "b" / "c";
	sys::Path g = b / "d";
	CHECK_EQUAL(f.relativeTo(b).toString(), string("c"));
	CHECK_EQUAL(b.relativeTo(f).toString(), string(".."));
	CHECK_EQUAL(g.relativeTo(f).toString(), string("../d"));
}

TEST_END
