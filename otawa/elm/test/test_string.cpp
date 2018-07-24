/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_string.cpp -- unit tests for String class.
 */

#include <elm/util/test.h>
#include <elm/io/BlockInStream.h>

using namespace elm;

TEST_BEGIN(string)
	
	// Comparison overload test
	{
		elm::String s1 = "ok", s2 = "ko";
		elm::CString c1 = "ok", c2 = "ko";
		
		CHECK(s1 != s2);
		CHECK(s1 == c1);
		CHECK(s1 == "ok");
//		cout << "inside test_string" << io::endl;
		CHECK(c1 == s1);
		CHECK(c1 != s2);
		CHECK(c1 != c2);
		CHECK(c1 == "ok");
		
		CHECK("ok" == s1);
		CHECK("ok" == c1);
		CHECK("ok" != s2);
		CHECK("ok" != c2);
	}
	
	// Basic information
	{
		elm::String s("0123");
		CHECK(s.length() == 4);
		CHECK(s.chars()[0] == '0');
		CHECK(s.chars()[1] == '1');
		CHECK(s.chars()[2] == '2');
		CHECK(s.chars()[3] == '3');
	}
	{
		elm::CString s("0123");
		CHECK(s.length() == 4);
		CHECK(s.chars()[0] == '0');
		CHECK(s.chars()[1] == '1');
		CHECK(s.chars()[2] == '2');
		CHECK(s.chars()[3] == '3');
	}
	
	// charAt test
	{
		elm::String s("01234");
		CHECK(s.charAt(0) == '0');
		CHECK(s.charAt(1) == '1');
		CHECK(s.charAt(2) == '2');
		CHECK(s.charAt(3) == '3');
	}
	{
		elm::CString s("01234");
		CHECK(s.charAt(0) == '0');
		CHECK(s.charAt(1) == '1');
		CHECK(s.charAt(2) == '2');
		CHECK(s.charAt(3) == '3');
	}

	// Comparisons
	{
		elm::String s1("ok"), s2("ko"), s3("oko"), s4("ok");
		CHECK(s1.compare(s1) == 0);
		CHECK(s1.compare(s2) > 0);
		CHECK(s1.compare(s3) < 0);
		CHECK(s1.compare(s4) == 0);
	}
	{
		elm::String s1("ok");
		elm::CString s2("ko"), s3("oko"), s4("ok");
		CHECK(s1.compare(s2) > 0);
		CHECK(s1.compare(s3) < 0);
		CHECK(s1.compare(s4) == 0);
	}
	{
		elm::CString s1("ok");
		elm::CString s2("ko"), s3("oko"), s4("ok");
		CHECK(s1.compare(s2) > 0);
		CHECK(s1.compare(s3) < 0);
		CHECK(s1.compare(s4) == 0);
	}
	
	// Comparison with substring
	 {
		elm::String s("okoko");
		{
			elm::String s1 = s.substring(2, 2);
			elm::String s2("ko"), s3("oko"), s4("ok");
			CHECK(s1.compare(s1) == 0);
			CHECK(s1.compare(s2) > 0);
			CHECK(s1.compare(s3) < 0);
			CHECK(s1.compare(s4) == 0);
		}
		{
			elm::String s1 = s.substring(2, 2);
			elm::CString s2("ko"), s3("oko"), s4("ok");
			CHECK(s1.compare(s2) > 0);
			CHECK(s1.compare(s3) < 0);
			CHECK(s1.compare(s4) == 0);
		}
	}
	
	// Assignment
	{
		elm::String lhs, rhs("ok");
		lhs = rhs;
		CHECK(lhs == rhs);
	}
	{
		elm::CString lhs, rhs("ok");
		lhs = rhs;
		CHECK(lhs == rhs);
	}
	{
		elm::String lhs;
		elm::CString rhs("ok");
		lhs = rhs;
		CHECK(lhs == rhs);
	}
	
	// Substrings
	{
		elm::String s("0123456789"), r;
		CHECK((r = s.substring(0)) == s);
		CHECK((r = s.substring(4)) == "456789");
		CHECK((r = s.substring(0, 4)) == "0123");
		CHECK((r = s.substring(4, 4)) == "4567");
		CHECK((r = s.substring(4, 0)) == "");
	}
	{
		elm::CString s("0123456789");
		elm::String r;
		CHECK((r = s.substring(0)) == s);
		CHECK((r = s.substring(4)) == "456789");
		CHECK((r = s.substring(0, 4)) == "0123");
		CHECK((r = s.substring(4, 4)) == "4567");
		CHECK((r = s.substring(4, 0)) == "");
	}
	
	// Concatenation
	{
		elm::String s1("0123"), s2("45"), s3("");
		CHECK(s1.concat(s1) == "01230123");
		CHECK(s1.concat(s2) == "012345");
		CHECK(s1.concat(s3) == s1);
		CHECK(s2.concat(s1) == "450123");
		CHECK(s3.concat(s1) == s1);
	}
	{
		elm::CString s1("0123"), s2("45"), s3("");
		CHECK(s1.concat(s1) == "01230123");
		CHECK(s1.concat(s2) == "012345");
		CHECK(s1.concat(s3) == s1);
		CHECK(s2.concat(s1) == "450123");
		CHECK(s3.concat(s1) == s1);
	}
	{
		elm::CString s1("0123");
		elm::String s2("45"), s3("");
		CHECK(s1.concat(s1) == "01230123");
		CHECK(s1.concat(s2) == "012345");
		CHECK(s1.concat(s3) == s1);
		CHECK(s2.concat(s1) == "450123");
		CHECK(s3.concat(s1) == s1);
	}
	{
		elm::String s1("0123");
		elm::CString s2("45"), s3("");
		CHECK(s1.concat(s1) == "01230123");
		CHECK(s1.concat(s2) == "012345");
		CHECK(s1.concat(s3) == s1);
		CHECK(s2.concat(s1) == "450123");
		CHECK(s3.concat(s1) == s1);
	}
	
	// Index of
	{
		elm::String s("00010010010");
		CHECK(s.indexOf('1') == 3);
		CHECK(s.indexOf('2') < 0);
		CHECK(s.indexOf('1', 4) == 6);
		CHECK(s.indexOf('2', 4) < 0);
		CHECK(s.indexOf('1', 10) < 0);
		CHECK(s.indexOf('2', 10) < 0);
	}
	{
		elm::CString s("00010010010");
		CHECK(s.indexOf('1') == 3);
		CHECK(s.indexOf('2') < 0);
		CHECK(s.indexOf('1', 4) == 6);
		CHECK(s.indexOf('2', 4) < 0);
		CHECK(s.indexOf('1', 10) < 0);
		CHECK(s.indexOf('2', 10) < 0);
	}
	{
		elm::String s("00010010010");
		CHECK(s.lastIndexOf('1') == 9);
		CHECK(s.lastIndexOf('2') < 0);
		CHECK(s.lastIndexOf('1', 8) == 6);
		CHECK(s.lastIndexOf('2', 8) < 0);
		CHECK(s.lastIndexOf('1', 3) < 0);
		CHECK(s.lastIndexOf('2', 3) < 0);
	}
	{
		elm::CString s("00010010010");
		CHECK(s.lastIndexOf('1') == 9);
		CHECK(s.lastIndexOf('2') < 0);
		CHECK(s.lastIndexOf('1', 8) == 6);
		CHECK(s.lastIndexOf('2', 8) < 0);
		CHECK(s.lastIndexOf('1', 3) < 0);
		CHECK(s.lastIndexOf('2', 3) < 0);
	}
	
	// startsWith tests
	{
		elm::String s("0123456789"), s1("0123"), s2("ok"), s3("");
		CHECK(s.startsWith(s) == true);
		CHECK(s.startsWith(s1) == true);
		CHECK(s.startsWith(s2) == false);
		CHECK(s.startsWith(s3) == true);
		CHECK(s1.startsWith(s) == false);
		CHECK(s2.startsWith(s) == false);
		CHECK(s3.startsWith(s) == false);
	}
	{
		elm::CString s("0123456789"), s1("0123"), s2("ok"), s3("");
		CHECK(s.startsWith(s) == true);
		CHECK(s.startsWith(s1) == true);
		CHECK(s.startsWith(s2) == false);
		CHECK(s.startsWith(s3) == true);
		CHECK(s1.startsWith(s) == false);
		CHECK(s2.startsWith(s) == false);
		CHECK(s3.startsWith(s) == false);
	}
	{
		elm::CString s("0123456789");
		elm::String s1("0123"), s2("ok"), s3("");
		CHECK(s.startsWith(s) == true);
		CHECK(s.startsWith(s1) == true);
		CHECK(s.startsWith(s2) == false);
		CHECK(s.startsWith(s3) == true);
		CHECK(s1.startsWith(s) == false);
		CHECK(s2.startsWith(s) == false);
		CHECK(s3.startsWith(s) == false);
	}
	{
		elm::String s("0123456789");
		elm::CString s1("0123"), s2("ok"), s3("");
		CHECK(s.startsWith(s) == true);
		CHECK(s.startsWith(s1) == true);
		CHECK(s.startsWith(s2) == false);
		CHECK(s.startsWith(s3) == true);
		CHECK(s1.startsWith(s) == false);
		CHECK(s2.startsWith(s) == false);
		CHECK(s3.startsWith(s) == false);
	}
	
	// endsWith tests
	{
		elm::String s("0123456789"), s1("6789"), s2("ok"), s3("");
		CHECK(s.endsWith(s) == true);
		CHECK(s.endsWith(s1) == true);
		CHECK(s.endsWith(s2) == false);
		CHECK(s.endsWith(s3) == true);
		CHECK(s1.endsWith(s) == false);
		CHECK(s2.endsWith(s) == false);
		CHECK(s3.endsWith(s) == false);
	}
	{
		elm::CString s("0123456789"), s1("6789"), s2("ok"), s3("");
		CHECK(s.endsWith(s) == true);
		CHECK(s.endsWith(s1) == true);
		CHECK(s.endsWith(s2) == false);
		CHECK(s.endsWith(s3) == true);
		CHECK(s1.endsWith(s) == false);
		CHECK(s2.endsWith(s) == false);
		CHECK(s3.endsWith(s) == false);
	}
	{
		elm::String s("0123456789");
		elm::CString s1("6789"), s2("ok"), s3("");
		CHECK(s.endsWith(s) == true);
		CHECK(s.endsWith(s1) == true);
		CHECK(s.endsWith(s2) == false);
		CHECK(s.endsWith(s3) == true);
		CHECK(s1.endsWith(s) == false);
		CHECK(s2.endsWith(s) == false);
		CHECK(s3.endsWith(s) == false);
	}
	{
		elm::CString s("0123456789");
		elm::String s1("6789"), s2("ok"), s3("");
		CHECK(s.endsWith(s) == true);
		CHECK(s.endsWith(s1) == true);
		CHECK(s.endsWith(s2) == false);
		CHECK(s.endsWith(s3) == true);
		CHECK(s1.endsWith(s) == false);
		CHECK(s2.endsWith(s) == false);
		CHECK(s3.endsWith(s) == false);
	}
	
	// (last)indexOf tests
	{
		elm::String s = "12345\n---\nfsdmlkjsmflk";
		elm::String l = "\n---\n";
		CHECK(s.indexOf(l) == 5);
		CHECK(s.indexOf("bof") < 0);
		CHECK(s.lastIndexOf(l) == 5);
		CHECK(s.lastIndexOf("bof") < 0);
	}
	
	// simple StringInput
	{
		string str("123");
		int x;
		str >> x;
		CHECK(x == 123);
	}

	// double StringInput
	{
		string str("123 456");
		int x, y;
		str >> x >> " " >> y;
		CHECK(x == 123);
		CHECK(y == 456);
	}

	// simple hexadecimal
	{
		string str("0xff");
		int x;
		str >> x;
		CHECK(x == 0xff);
	}

	// shared buffer
	{
		string s = "34";
		s = s;
		CHECK_EQUAL(s, string("34"));
	}

	// trim test
	{
		string s = "  123  ", e = "   ", t = "  1 2 3  ";
		CHECK_EQUAL(s.ltrim(), string("123  "));
		CHECK_EQUAL(s.rtrim(), string("  123"));
		CHECK_EQUAL(s.trim(), string("123"));
		CHECK_EQUAL(e.ltrim(), string(""));
		CHECK_EQUAL(e.rtrim(), string(""));
		CHECK_EQUAL(e.trim(), string(""));
		CHECK_EQUAL(t.ltrim(), string("1 2 3  "));
		CHECK_EQUAL(t.rtrim(), string("  1 2 3"));
		CHECK_EQUAL(t.trim(), string("1 2 3"));
	}

	// character builder
	{
		string s = string::make('?');
		CHECK_EQUAL(s, string("?"));
	}

	// check splitter
	{
		string s = "otawa.hard.Platform";
		str::Split sp(s, '.');
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string("otawa"));
		sp.next();
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string("hard"));
		sp.next();
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string("Platform"));
		sp.next();
		CHECK(sp.ended());
	}
	{
		string s = "otawa";
		str::Split sp(s, '.');
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string("otawa"));
		sp.next();
		CHECK(sp.ended());
	}
	{
		string s = "";
		str::Split sp(s, '.');
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string());
		sp.next();
		CHECK(sp.ended());
	}
	{
		string s = "otawa.hard.";
		str::Split sp(s, '.');
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string("otawa"));
		sp.next();
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string("hard"));
		sp.next();
		CHECK(!sp.ended());
		CHECK_EQUAL(*sp, string(""));
		sp.next();
		CHECK(sp.ended());
	}
TEST_END

