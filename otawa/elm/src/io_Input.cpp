/*
 *	$Id$
 *	Input class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <elm/assert.h>
#include <elm/io/io.h>
#include <elm/io/Input.h>
#include <elm/string/StringBuffer.h>
#include <math.h>
#include <ctype.h>

namespace elm { namespace io {

/**
 * @class Input
 * This class provides formatted scanning to input streams.
 * @ingroup ios
 */


/**
 * Raise an exception claiming that the IO is not supported.
 */
void Input::unsupported(void) {
	throw io::IOException("unsupported input");
}

/**
 * Get the next character.
 * @return	Next character or -1 if there is no more character available.
 */
int Input::get(void) {
	char res;
	if(buf >= 0) {
		res = buf;
		buf = -1;
	}
	else
		switch(strm->read(&res, 1)) {
		case 0:
			return -1;
		case 1:
			return (unsigned char)res;
		default:
			throw IOException(strm->lastErrorMessage());
		}
	return res;
}


/**
 * Push back a read character.
 * @param chr	Back-pushed character.
 */
void Input::back(int chr) {
	ASSERTP(buf < 0, "buffer is empty");
	buf = chr;
}


/**
 * @fn Input::Input(void);
 * Build a new input using the standard.
 */


/**
 * @fn Input::Input(InStream& stream);
 * Build a new input with the given input stream.
 * @param stream	Used input stream.
 */


/**
 * @fn InStream& Input::stream(void) const;
 * Get the current input stream.
 * @return	Current input stream.
 */


/**
 * @fn void Input::setStream(InStream& stream);
 * change the used input stream.
 */


/**
 * Read a boolean value, that is, 'true'/'false' or '1'/'0'.
 * @return	Read boolean value.
 */
bool Input::scanBool(void) {
	const char *pattern;
	bool res;

	// Look first character
	int chr = get();
	if (chr == '1') {
		pattern = "1";
		res = true;
	}
	else if (chr == '0') {
		pattern = "0";
		res = false;
	}
	else if(chr == 't') {
		pattern = "true";
		res = true;
	}
	else if(chr == 'f') {
		pattern = "false";
		res = false;
	}
	else
		pattern = "";

	// Look other characters
	while(*pattern == chr) {
		pattern++;
		if(*pattern == '\0')
			return res;
		chr = get();
	}
	throw IOException("bad format for boolean");
}


/**
 * Read the next character.
 * @return	Next character.
 */
char Input::scanChar(void) {
	int res = get();
	if(res <  0)
		throw IOException("no more character to read");
	else
		return res;
}


/**
 * Test if the character is in the base and return the matching digit.
 * @param chr	Character to test.
 * @param base	Base of the read.
 * @return		Matching digit or not (-1).
 */
static inline int test_base(char chr, int base) {
	if(base < 10) {
		if(chr >= '0' && chr < '0' + base)
			return chr - '0';
		else
			return -1;
	}
	else if(chr >= '0' && chr <= '9')
		return chr - '0';
	else if(chr >= 'a' && chr < 'a' + base - 10)
		return chr - 'a' + 10;
	else if(chr >= 'A' && chr < 'A' + base - 10)
		return chr - 'A' + 10;
	else
		return -1;
}


/**
 * Scan a based unsigned long, decimal as a default.
 * Supported base prefixes are '0', '0[xX]' or '0[bB]'.
 * @return	Integer value.
 */
t::uint32 Input::scanULong(void) {
	t::uint32 val = 0;
	int base = 10;

	// Read the base
	int chr = get();
	if(chr == '0')
		switch(chr = get()) {
		case 'x': case 'X':
			base = 16;
			chr = get();
			break;
		case 'b': case 'B':
			base = 2;
			chr = get();
			break;
		default:
			if(chr < '0' || chr > '7') {
				back(chr);
				return 0;
			}
			base = 8;
			break;
		}

	// read the digits
	bool one = false;
	int digit = test_base(chr, base);
	while(digit >= 0) {
		if(digit < 0)
			throw IOException("bad formatted integer");
		t::uint32 old = val;
		val = val * base + digit;
		one = true;
		if(val < old)
			throw IOException("32-bits value too big");
		chr = get();
		digit = test_base(chr, base);
	}
	if(chr >= 0)
		back(chr);
	if(!one)
		throw IOException("not a number");
	return val;
}


/**
 * Scan a based long, decimal as a default.
 * Supported base prefixes are '0', '0[xX]' or '0[bB]'.
 * @return	Integer value.
 */
t::int32 Input::scanLong(void) {
	t::uint32 val = 0;
	bool neg = false;

	// Read sign
	int chr = get();
	if(chr == '-')
		neg = true;
	else if(chr != '+')
		back(chr);

	// get the value
	val = scanULong();
	if(val >= (1UL << 31))
		throw IOException("32-bits value too big");
	return neg ? -t::int32(val) : val;
}


/**
 * Scan a based unsigned long long, decimal as a default.
 * Supported base prefixes are '0', '0[xX]' or '0[bB]'.
 * @return	Integer value.
 */
t::uint64 Input::scanULLong(void) {
	t::uint64 val = 0;
	int base = 10;

	// Read the base
	int chr = get();
	if(chr == '0')
		switch(chr = get()) {
		case 'x': case 'X':
			base = 16;
			chr = get();
			break;
		case 'b': case 'B':
			base = 2;
			chr = get();
			break;
		default:
			if(chr < '0' || chr > '7') {
				back(chr);
				return 0;
			}
			base = 8;
			break;
		}

	// read the digits
	int digit = test_base(chr, base);
	if(digit < 0)
		throw IOException("bad formatted integer");
	while(digit >= 0) {
		val = val * base + digit;
		chr = get();
		digit = test_base(chr, base);
	}
	if(chr >= 0)
		back(chr);
	return val;
}


/**
 * Scan a based long long, decimal as a default.
 * Supported base prefixes are '0', '0[xX]' or '0[bB]'.
 * @return	Integer value.
 */
t::int64 Input::scanLLong(void) {
	t::uint64 val = 0;
	bool neg = false;

	// Read sign
	int chr = get();
	if(chr == '-')
		neg = true;
	else if(chr != '+')
		back(chr);

	// get the value
	val = scanULLong();
	if(val >= (1ULL << 63))
		throw IOException("bad formatted signed 64-bits value");
	return neg ? -t::int64(val) : val;
}


/**
 * Scan a floating value.
 * @return	Read value.
 */
double Input::scanDouble(void) {
	double value = 0;
	bool neg = false;

	// Read sign
	int chr = get();
	if(chr == '-') {
		neg = true;
		chr = get();
	}
	else if(chr == '+')
		chr = get();

	// Read integer part
	if(chr != '.' && (chr < '0' || chr > '9'))
		throw IOException("bad formatted float value");
	while(chr >= '0' && chr <= '9') {
		value = value * 10 + (chr - '0');
		chr = get();
	}

	// Read decimal part
	if(chr == '.') {
		chr = get();
		double dec = 0.1;
		while(chr >= '0' && chr <='9') {
			value += (chr - '0') * dec;
			dec = dec / 10;
			chr = get();
		}
	}

	// Read exponent part
	if(chr == 'e' || chr == 'E') {
		int exp = 0;
		chr = get();
		if(chr == '-')
			chr = get();
		else if(chr == '+')
			chr = get();
		if(chr < '0' || chr > '9')
			throw IOException("bad formatted float value");
		while(chr >= '0' && chr <= '9') {
			exp = exp * 10 + (chr - '0');
			chr = get();
		}
		value = value * pow(10, exp);
	}

	// Return result
	back(chr);
	return neg ? -value : value;
}


/**
 * Scan a word from the text and return it.
 * @note A word is separated from other words by a blank character.
 * @return	Read word.
 */
String Input::scanWord(void) {
	StringBuffer buf;
	int chr = get();
	while(chr >= 0 && !isspace(chr)) {
		buf << (char)chr;
		chr = get();
	}
	back(chr);
	return buf.toString();
}


/**
 * Scan a full line.
 * @return	Read line (final \n, if any, is appended).
 */
String Input::scanLine(void) {
	StringBuffer buf;
	int chr = get();
	while(chr >= 0) {
		buf << (char)chr;
		if(chr == '\n')
			break;
		chr = get();
	}
	return buf.toString();
}


/**
 * Read a character if it is equal to the given one or throw @ref an IOException.
 * @param chr	Character to read.
 */
void Input::swallow(char chr) {
	int read = get();
	if((unsigned char)chr == read)
		return;
	else {
		back(chr);
		throw IOException("bad character");
	}
}


/**
 * Read the character of the given string until finding a not matching
 * character (and throw an exception in this case).
 * @param str	String to read.
 */
void Input::swallow(CString str) {
	for(int i = 0; str[i]; i++)
		swallow(str[i]);
}


/**
 * Read the character of the given string until finding a not matching
 * character (and throw an exception in this case).
 * @param str	String to read.
 */
void Input::swallow(const String& str) {
	for(int i = 0; i < str.length(); i++)
		swallow(str[i]);
}


/**
 * Swallow one or more characters.
 * @return	True if some blanks have swallowed, false else.
 */
void Input::swallowBlank(void) {
	int chr = get();
	while(isspace(chr) && chr >= 0)
		chr = get();
	back(chr);
}


/**
 * @fn Input& Input::operator>>(bool& value);
 * Shortcut @ref scanBool.
 */


/**
 * @fn Input& Input::operator>>(char& value);
 * Shortcut to @ref scanChar().
 */


/**
 * @fn Input& Input::operator>>(unsigned char& value);
 * Shortcut to @ref scanInt().
 */


/**
 * @fn Input& Input::operator>>(short& value);
 * Shortcut to @ref scanInt().
 */


/**
 * @fn Input& Input::operator>>(unsigned short& value);
 * Shortcut to @ref scanInt().
 */


/**
 * @fn Input& Input::operator>>(int& value);
 * Shortcut to @ref scanInt().
 */


/**
 * @fn Input& Input::operator>>(unsigned int& value);
 * Shortcut to @ref scanInt().
 */


/**
 * @fn Input& Input::operator>>(long& value);
 * Shortcut to @ref scanInt().
 */


/**
 * @fn Input& Input::operator>>(unsigned long& value);
 * Shortcut to @ref scanInt().
 */


/**
 * @fn Input& Input::operator>>(long long& value);
 * Shortcut to @ref scanLong().
 */


/**
 * @fn Input& Input::operator>>(unsigned long long& value);
 * Shortcut to @ref scanLong().
 */


/**
 * @fn Input& Input::operator>>(float& value);
 * Shortcut to @ref scanDouble().
 */


/**
 * @fn Input& Input::operator>>(double& value);
 * Shortcut to @ref scanDouble().
 */


/**
 * @fn Input& Input::operator>>(String& value);
 * Shortcut to @ref scanLine().
 */


/**
 * @fn Input& Input::operator>>(char value);
 * Shortcut to @ref swallow(char).
 */


/**
 * @fn Input& Input::operator>>(CString value);
 * Shortcut to @ref swallow(CString).
 */


/**
 * @fn Input& Input::operator>>(const String& value);
 * shortcut @ref swallow(String).
 */

} // io

/**
 * Formatted input.
 * @ingroup ios
 */
io::Input cin;

} // eml
