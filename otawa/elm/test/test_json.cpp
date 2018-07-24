/*
 *	json module test
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

#include <elm/json.h>
#include <elm/util/test.h>

using namespace elm;

class MyMaker: public json::Maker {
public:

	enum {
		NONE,
		_NULL,
		TRUE,
		FALSE,
		INT,
		FLOAT,
		STRING,
		ARRAY,
		OBJECT
	} res;
	int i;
	double f;
	string s;
	genstruct::Vector<int> iv;

	MyMaker(void): res(NONE), i(0), f(0) { }

	virtual void beginObject(void) throw(json::Exception) 		{ res = OBJECT; }
	virtual void endObject(void) throw(json::Exception) 		{ }
	virtual void beginArray(void) throw(json::Exception)		{ res = ARRAY; }
	virtual void endArray(void) throw(json::Exception)			{ }

	virtual void onField(string name) throw(json::Exception)	{
		ASSERT(res == OBJECT);
		ASSERTP(name == "int" || name == "float" || name == "string", "got " << name);
	}

	virtual void onNull(void) throw(json::Exception)			{ res = _NULL; }
	virtual void onValue(bool value) throw(json::Exception)		{ res = value ? TRUE : FALSE; }

	virtual void onValue(int value) throw(json::Exception)		{
		if(res == ARRAY)
			iv.add(value);
		else if(res == OBJECT)
			i = value;
		else {
			i = value;
			res = INT;
		}
	}

	virtual void onValue(double value) throw(json::Exception)	{
		if(res == OBJECT)
			f = value;
		else {
			f = value;
			res = FLOAT;
		}
	}

	virtual void onValue(string value) throw(json::Exception) {
		if(res == OBJECT)
			s = value;
		else {
			s = value;
			res = STRING;
		}
	}
};

TEST_BEGIN(json)

	// empty object
	{
		StringBuffer buf;
		json::Saver save(buf);
		save.beginObject();
		save.endObject();
		save.close();
		string r = buf.toString();
		CHECK_EQUAL(r, string("{}"));
	}

	// simple pairs
	{
		StringBuffer buf;
		json::Saver save(buf);
		save.beginObject();
		save.addField("1");
		save.put(1);
		save.addField("2");
		save.put(2);
		save.endObject();
		save.close();
		string r = buf.toString();
		CHECK_EQUAL(r, string("{\"1\":1,\"2\":2}"));
	}

	// pair with an empty array
	{
		StringBuffer buf;
		json::Saver save(buf);
		save.beginObject();
		save.addField("a");
		save.beginArray();
		save.endArray();
		save.endObject();
		save.close();
		string r = buf.toString();
		CHECK_EQUAL(r, string("{\"a\":[]}"));
	}

	// pair with an array
	{
		StringBuffer buf;
		json::Saver save(buf);
		save.beginObject();
		save.addField("a");
		save.beginArray();
		save.put(0);
		save.put(1);
		save.put(2);
		save.put(3);
		save.endArray();
		save.endObject();
		save.close();
		string r = buf.toString();
		CHECK_EQUAL(r, string("{\"a\":[0,1,2,3]}"));
	}

	// parser test
	{
		MyMaker maker;
		json::Parser p(maker);
		p.parse("null");
		CHECK_EQUAL(maker.res, MyMaker::_NULL);
		p.parse("true");
		CHECK_EQUAL(maker.res, MyMaker::TRUE);
		p.parse("false");
		CHECK_EQUAL(maker.res, MyMaker::FALSE);
		p.parse("111");
		CHECK_EQUAL(maker.res, MyMaker::INT);
		CHECK_EQUAL(maker.i, 111);
		p.parse("0xff");
		CHECK_EQUAL(maker.res, MyMaker::INT);
		CHECK_EQUAL(maker.i, 255);
		p.parse("0.5e3");
		CHECK_EQUAL(maker.res, MyMaker::FLOAT);
		CHECK_EQUAL(maker.f, 0.5e3);
		p.parse("'ko'");
		CHECK_EQUAL(maker.res, MyMaker::STRING);
		CHECK_EQUAL(maker.s, string("ko"));
		p.parse("[0,1,2]");
		CHECK_EQUAL(maker.res, MyMaker::ARRAY);
		CHECK_EQUAL(maker.iv.length(), 3);
		CHECK_EQUAL(maker.iv[0], 0);
		CHECK_EQUAL(maker.iv[1], 1);
		CHECK_EQUAL(maker.iv[2], 2);
		p.parse("{'int':666,'float': /* ok */ 111.666,'string':'ok'}");
		CHECK_EQUAL(maker.res, MyMaker::OBJECT);
		CHECK_EQUAL(maker.i, 666);
		CHECK_EQUAL(maker.f, 111.666);
		CHECK_EQUAL(maker.s, string("ok"));
		p.parse("   \n\rnull  ");
		CHECK_EQUAL(maker.res, MyMaker::_NULL);
		p.parse("// ok \ntrue");
		CHECK_EQUAL(maker.res, MyMaker::TRUE);
		p.parse(" /* coucou */ null");
		CHECK_EQUAL(maker.res, MyMaker::_NULL);
	}

TEST_END


