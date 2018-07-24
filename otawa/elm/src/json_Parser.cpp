/*
 *	json::Parser class implementation
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

#include <elm/io/BufferedInStream.h>
#include <elm/json/Parser.h>
#include <elm/string/utf16.h>
#include <elm/sys/System.h>

namespace elm { namespace json {

/**
 * @class Maker
 * A maker used by a JSON @ref Parser to make a top-level object
 * from a JSON description. During the JSON text parsing, methods
 * of maker are called according to the find entities.
 *
 * Maker are interfaces that must be overload to provide actual
 * maker from JSON description texts. Any maker method can raise
 * on @ref json::Exception if the JSON text does not match the
 * requirement of the maker.
 *
 * @ingroup json
 */

/**
 */
Maker::~Maker(void) {
}

/**
 * Called when a new object is started.
 * As a default, raise an error.
 */
void Maker::beginObject(void) throw(json::Exception) {
	throw json::Exception("unexpected object");
}

/**
 * Called when the current object is ended.
 * As a default, do nothing.
 */
void Maker::endObject(void) throw(json::Exception) {
}

/**
 * Called an array is started.
 * As a default, raise an error.
 */
void Maker::beginArray(void) throw(json::Exception) {
	throw json::Exception("unexpected array");
}

/**
 * Called when an array is ended.
 * As a default, do nothing.
 */
void Maker::endArray(void) throw(json::Exception) {
}

/**
 * Called when a field is found. Notice that calls to this methods
 * are always preceded by a beginObject() and no field called is performed
 * after the matching endObject().
 * As a default, raise an error.
 * @param name	Field name.
 */
void Maker::onField(string name) throw(json::Exception) {
	throw json::Exception("unexpected field");
}

/**
 * Called when a "null" is found in JSON file.
 * As a default, raise an exception.
 */
void Maker::onNull(void) throw(json::Exception) {
	throw json::Exception("unexpected null");
}

/**
 * Called when a boolean value, "true" or "false",
 * is found in the JSON file.
 * As a default, raise an error.
 * @param value	Found value.
 */
void Maker::onValue(bool value) throw(json::Exception) {
	throw json::Exception("unexpected boolean value");
}


/**
 * Called when an integer value is found.
 * As a default, raise an error.
 * @param value	Integer value.
 */
void Maker::onValue(int value) throw(json::Exception) {
	throw json::Exception("unexpected integer value");
}

/**
 * Called when float value if found.
 * As a default, raise an error.
 * @param value	Float value.
 */
void Maker::onValue(double value) throw(json::Exception) {
	throw json::Exception("unexpected float value");
}

/**
 * Called when a string is found.
 * As a default, raise an error.
 * @param value	String value.
 */
void Maker::onValue(string value) throw(json::Exception) {
	throw json::Exception("unexpected string value");
}


/**
 * @class Parser
 * This class provide a simple way to read JSON structured format.
 * It receive a @ref Maker as parameter and the maker methods are called
 * along the parsing of the JSOn text. This should allow top-level implementation
 * of @ref Maker to build back objects and arrays.
 *
 * Notice that this parser supports a larger set of JSON text than the standard:
 * * top-level simple values (integer, float, string) are accepted,
 * * top-level array are accepted,
 * * "//" and "/ *" ... "* /" are parsed and ignored (and doesn't cause error).
 * * hexadecimal and binary integer prefixed by "0[xX]" or "0[bB]".
 *
 * @ingroup json
 */

/**
 * Build a new parser.
 * @param maker		Maker to use.
 */
Parser::Parser(Maker& maker): m(maker), line(0), col(0), prev('\0') {

}

/**
 * Parser from a string.
 * @param s		String to parser.
 */
void Parser::parse(string s) throw(json::Exception) {
	io::BlockInStream in(s);
	doParsing(in);
}

/**
 * Parse from an input stream.
 * @param in	Input stream to use.
 */
void Parser::parse(io::InStream& in) throw(json::Exception) {
	try {
		io::BufferedInStream buf(in);
		doParsing(buf);
	}
	catch(io::IOException& e) {
		throw json::Exception(e.message());
	}
}

/**
 * Parser from a file.
 * @param path	File path.
 */
void Parser::parse(sys::Path path) throw(json::Exception) {
	try {
		io::InStream *file = sys::System::readFile(path);
		try {
			parse(*file);
			delete file;
		}
		catch(json::Exception& e) {
			delete file;
			throw;
		}
	}
	catch(sys::SystemException& e) {
		throw json::Exception(e.message());
	}
}


/**
 * Raise an error with the given message at the given position.
 * @param message	Error message.
 */
void Parser::error(string message) throw(json::Exception) {
	throw json::Exception(_ << line << ':' << col << ": " << message);
}


/**
 * Perform the parsing of the input.
 */
void Parser::doParsing(io::InStream& in) throw(json::Exception) {
	line = 1;
	col = 0;
	prev = '\0';
	parseValue(in, next(in));
}


/**
 * Parse a simple value, an array or an object.
 * @param in	Input stream.
 * @param t		Last token.
 */
void Parser::parseValue(io::InStream& in, token_t t) throw(json::Exception) {
		switch(t) {
		case LBRACE:	m.beginObject(); parseObject(in); return;
		case LBRACK:	m.beginArray(); parseArray(in); return;
		case _NULL:		m.onNull(); return;
		case TRUE:		{ m.onValue(true); return; }
		case FALSE:		{ m.onValue(false); return; }
		case INT:		{ int i; text >> i; m.onValue(i); return; }
		case FLOAT:		{ double d; text >> d; m.onValue(d); return; }
		case STRING:	{ m.onValue(text); return; }
		default:		error("unexpected symbol");
		}
}


/**
 * Parse an array (initial token has already been parsed).
 * @param in	Input stream.
 */
void Parser::parseArray(io::InStream& in) throw(json::Exception) {
	token_t t = next(in);
	while(t != RBRACK) {
		parseValue(in, t);
		t = next(in);
		if(t == COMMA)
			t = next(in);
		else if(t != RBRACK)
			error("unexpected symbol");
	}
}


/**
 * Parse an object (considering initial token has been parsed).
 * @param in	Input stream.
 */
void Parser::parseObject(io::InStream& in) throw(json::Exception) {
	token_t t = next(in);
	while(t != RBRACE) {
		if(t != STRING)
			error("expected field name here");
		m.onField(text);
		t = next(in);
		if(t != COLON)
			error("':' expected here");
		parseValue(in, next(in));
		t = next(in);
		if(t == COMMA)
			t = next(in);
		else if(t != RBRACE)
			error("',' or '}' expected here");
	}
}


/**
 * Push back an unused character.
 * @param c	Character to push back.
 */
void Parser::pushBack(char c) {
	ASSERT(!prev);
	prev = c;
}

static inline bool isNumber(char c) {
	return ('0' <= c && c <= '9')
		|| c == '.'
		|| c == 'e'
		|| c == 'E';
}

/**
 * Get next token.
 * @param in	Input stream.
 * @return		Found token.
 */
Parser::token_t Parser::next(io::InStream& in) throw(json::Exception) {
	while(true) {
		char c = nextChar(in);
		switch(c) {
		case '{':	return LBRACE;
		case '}':	return RBRACE;
		case '[':	return LBRACK;
		case ']':	return RBRACK;
		case '\'':	parseString(in, '\''); return STRING;
		case '\"':	parseString(in, '"'); return STRING;
		case ':':	return COLON;
		case ',':	return COMMA;
		case '\n':
		case '\t':
		case '\r':
		case ' ':	break;
		case '+':
		case '-':	return parseNumber(in, c);
		case 'n':	parseLitt(in, "null"); return _NULL;
		case 't':	parseLitt(in, "true"); return TRUE;
		case 'f':	parseLitt(in, "false"); return FALSE;
		case '/':	parseComment(in); break;
		case '0':	return parseBasedNumber(in);

		default:
			if(isNumber(c))
				return parseNumber(in, c);
			else
				error(_ << "bad character '" << c << "' (code = " << int(c) << ")");
		}
	}
}

/**
 * Get the next character.
 * @param in	Input stream.
 */
char Parser::nextChar(io::InStream& in) {
	if(prev != '\0') {
		char r = prev;
		prev = '\0';
		return r;
	}
	else {
		int c = in.read();
		if(c == io::InStream::FAILED)
			error(in.lastErrorMessage());
		else if(c == io::InStream::ENDED)
			return '\0';
		else if(c == '\n') {
			line++;
			col = 0;
		}
		return c;
	}
}


/**
 * Parse a number.
 * @param in	Input stream.
 * @param c		First character to parse.
 * @param base	Base for an integer parse.
 * @return		Matching token.
 */
Parser::token_t Parser::parseNumber(io::InStream& in, char c, int base)  throw(json::Exception) {
	static string dec_base = "0123456789+-.eE";
	static string hex_base = "0123456789abcdefABCEDEF";
	static string bin_base = "01";
	static string float_chars = ".eE";

	// select base
	StringBuffer buf;
	token_t t = INT;
	string chars;
	if(base == 10)
		chars = dec_base;
	else if(base == 2) {
		chars = bin_base;
		buf << "0b";
	}
	else {
		chars = hex_base;
		buf << "0x";
	}

	// scan the number
	while(chars.indexOf(c) >= 0) {
		col++;
		if(float_chars.indexOf(c) >= 0)
			t = FLOAT;
		buf << c;
		c = nextChar(in);
	}
	pushBack(c);
	text = buf.toString();
	return t;
}


/**
 * Parse a string with support of escapes.
 * @param in	Input stream.
 * @param q		First quote.
 */
void Parser::parseString(io::InStream& in, char q) throw(json::Exception) {
	static string escapes = "\"\'\\/bfnrt";
	StringBuffer buf;
	char c = nextChar(in);
	while(c != q) {
		if(c != '\\')
			buf << c;
		else {
			c = nextChar(in);
			if(escapes.indexOf(c) >= 0)
				text << c;
			else if(c != 'u')
				error("bad escape in string");
			else {
				int wc = 0;
				for(int i = 0; i < 4; i++) {
					c = nextChar(in);
					int d = Char(c).asHex();
					if(d < 0)
						error("hex digit expected here");
					wc = (wc << 4) | d;
					utf16::Char(wc).toUTF8(buf);
				}
			}
		}
		c = nextChar(in);
	}
	text = buf.toString();
}


/**
 * Parse a literal string.
 * @param litt	Literal string to parse (first character is ignored).
 */
void Parser::parseLitt(io::InStream& in, cstring litt) throw(json::Exception) {
	for(int i = 1; i < litt.length(); i++)
		if(nextChar(in) != litt[i])
			error("unknown identifier");
}


/**
 * Parse a comment.
 * @param in	Input stream.
 */
void Parser::parseComment(io::InStream& in) {
	char c = nextChar(in);
	if(c == '/')
		while(c != '\n')
			c = nextChar(in);
	else if(c == '*') {
		while(true) {
			while(nextChar(in) != '*');
			if(nextChar(in) == '/')
				break;
		}
	}
	else
		error("bad character");
}


/**
 * Parser a based number.
 * @param in	Input stream.
 */
Parser::token_t Parser::parseBasedNumber(io::InStream& in) {
	char c = nextChar(in);
	if(c == 'x' || c == 'X')
		return parseNumber(in, nextChar(in), 16);
	else if(c == 'b' || c == 'B')
		return parseNumber(in, nextChar(in), 2);
	else if(isNumber(c))
		return parseNumber(in, c);
	else {
		pushBack(c);
		text = "0";
		return INT;
	}
}

} }	// elm::json
