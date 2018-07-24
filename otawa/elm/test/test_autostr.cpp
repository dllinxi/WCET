/*
 * $Id$
 * Copyright (c) 2007, IRIT-UPS.
 *
 * AutoStr tester
 */

//#define ELM_TEST_AUTOSTR
#include <elm/util/test.h>
#include <elm/io.h>
#include <elm/string.h>
 
using namespace elm;

void by_param(const String& str) {
	cout << "by param = \"" << str << "\"\n";
}

// Entry point
TEST_BEGIN(autostr)
	String str1 = _ << "ok " << 666 << " " << true;
	cout << str1 << io::endl;
	by_param(_ << "ok " << 666 << " " << true);
TEST_END
