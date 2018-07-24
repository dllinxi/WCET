/*
 * $Id$
 * Copyright (c) 2003-07 IRIT-UPS <casse@irit.fr>
 *
 * Unit tests
 */

#include <elm/io.h>
#include <elm/util/test.h>
#include <elm/genstruct/Vector.h>

using namespace elm;

// Entry point
int main(int argc, char *argv[]) {

	// process the help
	for(int i = 1; i < argc; i++)
		if(string("-h") == argv[i] || string("--help") == argv[i]) {
			cerr << "Modules:\n";
			for(TestSet::Iterator test(TestSet::def); test; test++)
				cerr << "\t" << test->name() << io::endl;
			return 0;
		}

	// process the tests
	genstruct::Vector<TestCase *> tests;
	for(int i = 1; i < argc; i++) {
		bool found = false;

		// look in the structure
		for(TestSet::Iterator test(TestSet::def); test; test++)
			if(test->name() == argv[i]) {
				tests.add(test);
				found = true;
				break;
			}

		// not found: error
		if(!found) {
			cerr << "ERROR: no test called \"" << argv[i] << "\"\n";
			return 1;
		}
	}

	// if none selected, test all
	if(!tests)
		for(TestSet::Iterator test(TestSet::def); test; test++)
			tests.add(test);

	// perform tests
	bool failed = false;
	for(genstruct::Vector<TestCase *>::Iterator test(tests); test; test++) {
		try {
			test->perform();
		}
		catch(Exception& e) {
			cerr << "FATAL: " << e.message() << io::endl;
			return 2;
		}
		if(test->hasFailed())
			failed = true;
	}

	// display summary
	if(!failed) {
		cerr << "SUCCESS: all tests successfully passed!\n";
		return 0;
	}
	else {
		cerr << "ERROR: some tests failed:\n";
		for(genstruct::Vector<TestCase *>::Iterator test(tests); test; test++)
			if(test->hasFailed())
				cerr << "  * " << test->name() << io::endl;
		return 1;
	}

	// success
	return 0;
}
