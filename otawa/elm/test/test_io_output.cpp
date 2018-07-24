/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_io_output.cpp -- io output classes test.
 */

#include <elm/util/test.h>
#include <elm/io/OutFileStream.h>
#include <elm/io.h>
#include <elm/string.h>

using namespace elm;
using namespace elm::io;

// Entry point
int main(void) {
	
	// Standard output
	{
		Output &out = cout;
		out << "---------------------------------------------\n";
		
	}
	
	// Created file
	{
		elm::io::OutFileStream stream("log.txt");
		elm::io::Output out(stream);	
		out << "+++++++++++++++++++++++++++++++++++++++++++++\n";
	}
	
	// IntFormat test
	{
		cout << "p = " << pointer((void *)main) << io::endl;
	}

	// Test float
	{
		cout << "0 = " << FloatFormat(0.).decimal() << io::endl;
		cout << "decimal   : 1234.1234 = " << FloatFormat(1234.1234).decimal() << io::endl;
		cout << "scientific: 1234.1234 = " << FloatFormat(1234.1234).scientific() << io::endl;
		cout << "shortest  : 1234.1234 = " << FloatFormat(1234.1234).shortest() << io::endl;
	}

	return 0;
}
