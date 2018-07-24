/*
 *	json::Parser class interface
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
#ifndef ELM_JSON_PARSER_H_
#define ELM_JSON_PARSER_H_

#include "common.h"
#include <elm/io.h>
#include <elm/sys/Path.h>

//Resolve conflict between a enum constant defined here and macro _NULL defined in mingw GCC
#undef _NULL

namespace elm { namespace json {

class Maker {
public:
	virtual ~Maker(void);
	virtual void beginObject(void) throw(json::Exception);
	virtual void endObject(void) throw(json::Exception);
	virtual void beginArray(void) throw(json::Exception);
	virtual void endArray(void) throw(json::Exception);
	virtual void onField(string name) throw(json::Exception);
	virtual void onNull(void) throw(json::Exception);
	virtual void onValue(bool value) throw(json::Exception);
	virtual void onValue(int value) throw(json::Exception);
	virtual void onValue(double value) throw(json::Exception);
	virtual void onValue(string value) throw(json::Exception);
};

class Parser {
public:
	Parser(Maker& maker);
	void parse(string s) throw(json::Exception);
	inline void parse(cstring s) throw(json::Exception) { parse(string(s)); }
	inline void parse(const char *s) throw(json::Exception) { parse(string(s)); }
	void parse(io::InStream& in) throw(json::Exception);
	void parse(sys::Path path) throw(json::Exception);

private:
	typedef enum {
		NONE = 0,
		LBRACE,
		RBRACE,
		LBRACK,
		RBRACK,
		_NULL,
		TRUE,
		FALSE,
		INT,
		FLOAT,
		STRING,
		COMMA,
		COLON
	} token_t;

	void doParsing(io::InStream& in) throw(json::Exception);
	token_t parseNumber(io::InStream& in, char c, int base = 10) throw(json::Exception);
	void parseObject(io::InStream& in) throw(json::Exception);
	void parseArray(io::InStream& in) throw(json::Exception);
	void parseValue(io::InStream& in, token_t t) throw(json::Exception);
	void parseString(io::InStream& in, char c) throw(json::Exception);
	void parseLitt(io::InStream& in, cstring litt) throw(json::Exception);
	void parseComment(io::InStream& in);
	token_t parseBasedNumber(io::InStream& in);

	void error(string message) throw(json::Exception);
	token_t next(io::InStream& in) throw(json::Exception);
	char nextChar(io::InStream& in);
	void pushBack(char c);

	Maker& m;
	int line, col;
	String text;
	char prev;
};

} }		// elm::json

#endif /* ELM_JSON_PARSER_H_ */
