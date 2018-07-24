/*
 *	cleaner module test
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

#include <elm/util/test.h>
#include <elm/io.h>
#include <elm/util/Cleaner.h>

using namespace elm;
using namespace elm::io;

class ToClean {
public:
	ToClean(string _name, bool& _cleaned): name(_name), cleaned(_cleaned)
		{ cleaned = false; }
	~ToClean(void) { cleaned = true; cout << "cleaned " << name << io::endl; }
private:
	string name;
	bool& cleaned;
};


class LockedClean: public Lock, public ToClean {
public:
	LockedClean(string _name, bool& _cleaned): ToClean(_name, _cleaned) { }
};

// Entry point
int main(void) {	
	CHECK_BEGIN("cleaner");
	
	// Simple deletor
	bool b1, b2;
	{
		CleanList cl;
		ToClean *c1 = cl(new ToClean("1", b1));
		ToClean *c2 = cl(new ToClean("2", b2));
		CHECK(b1 == false);
		CHECK(b2 == false);
		c1 = c2;
	}
	CHECK(b1 == true);
	CHECK(b2 == true);
	
	// AutoCleaner test
	bool b3;
	{
		AutoPtr<LockedClean> p = new LockedClean("3", b3);
		CHECK(b3 == false);
		CleanList cl;
		cl(p);
	}
	CHECK(b3 = true);
	
	// AutoCleaner test
	bool b4;
	AutoPtr<LockedClean> p = new LockedClean("4", b4);
	{
		CHECK(b4 == false);
		CleanList cl;
		cl(p);
	}
	CHECK(b4 == false);

	CHECK_END
	return 0;
}
