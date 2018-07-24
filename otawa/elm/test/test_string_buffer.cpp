/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test_string_buffer.cpp -- unit tests for StringBuffer..
 */

#include <elm/util/test.h>

using namespace elm;

TEST_BEGIN(string_buffer)
	
	// Empty string
	{
		StringBuffer buffer;
		CHECK(buffer.length() == 0);
		String str = buffer.toString();
		CHECK(str == "");
	}
	
	// C String puts
	{
		StringBuffer buffer;
		buffer.print("0123");
		CHECK(buffer.length() == 4);
		buffer.print("");
		CHECK(buffer.length() == 4);
		buffer.print("4567");
		CHECK(buffer.length() == 8);
		String str = buffer.toString();
		CHECK(str == "01234567");
	}
	
	// Different put types
	{
		StringBuffer buffer;
		buffer.print('0');
		buffer.print("1");
		CString cstr("2");
		buffer.print(cstr);
		String str("3");
		buffer.print(str);
		String res = buffer.toString();
		CHECK(res == "0123");
	}
	
	// Formatted test
	{
		StringBuffer buffer;
		buffer.format("%s%s%s%s", "0", "1", "2", "3");
		String str = buffer.toString();
		CHECK(str == "0123");
	}
	
TEST_END


