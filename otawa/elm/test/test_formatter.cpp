/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_formatter.cpp -- unit tests for Formatter class.
 */

#include <elm/util/test.h>
#include <elm/util/Formatter.h>
#include <elm/io/BlockInStream.h>

using namespace elm;
using namespace elm::util;

class MyFormatter: public Formatter {
	int index;
protected:
	virtual int process(io::OutStream& out, char chr) {
		io::Output fout(out);
		switch(chr) {
		case 'd':
			fout << index;
			return DONE;
		case 'a':
		case 'A':
			fout << (char)(chr + index);
			return DONE;
		default:
			return REJECT;
		}
	}
public:
	MyFormatter(int idx): index(idx) {
	}
};

class Escape {
public:
	typedef string t;
	static void print(io::Output& out, string s) {
		for(int i = 0; i < s.length(); i++)
			switch(s[i]) {
			case '<':	out << "&lt;"; break;
			case '>':	out << "&gt;"; break;
			default:	out << s[i]; break;
			}
	}
};

TEST_BEGIN(formatter)
	
	{
		MyFormatter format(3);
		io::BlockInStream temp("r%d = %%%a");
		io::BlockOutStream buf;
		format.format(temp, buf);
		String result = buf.toString();
		CString res = buf.toCString();
		CHECK_EQUAL(res, CString("r3 = %d"));
		delete [] res.chars();
	}

	{
		StringBuffer out;
		out << io::Tag<Escape>("ok <b> ko");
		CHECK_EQUAL(out.toString(), string("ok &lt;b&gt; ko"));
	}
	
	{
		int x = 3;
		StringBuffer out;
		out << io::Tag<Escape>(_ << "ok <b> ko " << x);
		CHECK_EQUAL(out.toString(), string("ok &lt;b&gt; ko 3"));
	}

	TEST_END
