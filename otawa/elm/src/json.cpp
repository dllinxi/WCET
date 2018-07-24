/*
 *	json module implementation
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
#include <elm/string/utf8.h>
#include <elm/sys/System.h>

namespace elm { namespace json {

/**
 * @defgroup json JSON Module
 * This module provides simple implementation of generic
 * JSON file format. Refer to http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
 * or to http://json.org/ for more details.
 */

/**
 * @class Saver
 * Object dedicated to write a JSON file. When this object is created, a first level object must be created
 * where the user can add pairs of (key, value) with addField() and put methods. A value may be null, a boolean,
 * an integer, a float, a string, an array or a new object. An object is started with beginObject() and
 * anded by endObject(). An array is made of several values (possibly of different types), is started
 * by beginArray() and ended by endArray().
 *
 */

/**
 */
Saver::Saver(io::OutStream& out)
: state(BEGIN), readable(false), indent("\t"), buf(0), str(0) {
	buf = new io::BufferedOutStream(out);
	_out.setStream(*buf);
}

/**
 */
Saver::Saver(StringBuffer& sbuf)
: state(BEGIN), readable(false), indent("\t"), buf(0), str(0) {
	_out.setStream(sbuf.stream());
}

/**
 */
Saver::Saver(sys::Path& path) throw(io::IOException)
: state(BEGIN), readable(false), indent("\t"), buf(0), str(0) {
	str = sys::System::createFile(path);
	buf = new io::BufferedOutStream(*str);
	_out.setStream(*buf);
}

/**
 */
Saver::~Saver(void) throw(io::IOException, Exception) {
	close();
	if(buf)
		delete buf;
	if(str)
		delete str;
}

/**
 * Close the JSON output.
 */
void Saver::close(void) {
	ASSERTP(state == END, "json: unended output!");
	_out.flush();
}

/**
 * @fn bool Saver::isReadable(void) const;
 * Test if the saver will generate human readable JSON output.
 * @return	True if human readable output is generate, false else.
 */

/**
 * @fn void Saver::setReadable(bool read);
 * Set the human readable output option.
 * @param read	True for human readable output, false else.
 */

/**
 * @fn string Saver::getIndent(void) const;
 * Get the indentation string.
 * @return	Indentation string.
 */

/**
 * @fn void Saver::setIndent(string i);
 * Set the indentation string.
 * @param i	Identation string (must be only composed of JSON blank characters).
 */

/**
 * Display indentation corresponding to the readibility activation
 * using the current indentation.
 * @param close		Set to true to close an object or an array.
 */
void Saver::doIndent(bool close) throw(io::IOException) {
	if(!close && (state == IN_ARRAY || state == IN_OBJECT))
		_out << ",";
	if(readable && state != FIELD) {
		_out << io::endl;
		for(int i = 0; i < stack.length(); i++)
			_out << indent;
	}
}

/**
 * Change the state to reflect the fact that it contains at least one item.
 * @param s		State to get next.
 * @return		Next state.
 */
Saver::state_t Saver::next(state_t s) {
	if(s == OBJECT)
		return IN_OBJECT;
	else if(s == ARRAY)
		return IN_ARRAY;
	else
		return s;
}

/**
 * Test if the given state concerns an object.
 * @param s	State to test.
 * @return	True if it is an object state, false else.
 */
bool Saver::isObject(state_t s) {
	return s == OBJECT || s == IN_OBJECT;
}

/**
 * Test if the given state concerns an array.
 * @param s	State to test.
 * @return	True if it is an array state, false else.
 */
bool Saver::isArray(state_t s) {
	return s == ARRAY || s == IN_ARRAY;
}

/**
 * Begin an object. Only allowed at the beginning of the output,
 * after adding an object or inside an array.
 */
void Saver::beginObject(void) throw(io::IOException) {
	ASSERTP(state != END, "json: ended output!");
	ASSERTP(!isObject(state), "json: object creation only allowed in a field or an array");
	doIndent();
	_out << "{";
	if(isArray(state))
		stack.push(state);
	state = OBJECT;
}

/**
 * End an object. Only allowed inside an object.
 */
void Saver::endObject(void) throw(io::IOException) {
	ASSERTP(isObject(state), "json: not inside an object!");
	if(!stack)
		state = END;
	else
		state = next(stack.pop());
	doIndent(true);
	_out << "}";
}

/**
 * Begin an array. Only allowed inside an array or in a field.
 */
void Saver::beginArray(void) throw(io::IOException) {
	ASSERTP(state != END, "json: ended output!");
	ASSERTP(state == FIELD || isArray(state), "json: array only allowed in a field or in an array");
	doIndent();
	_out << "[";
	if(state != FIELD)
		stack.push(state);
	state = ARRAY;
}

/**
 * End an array. Only allowed inside an array.
 */
void Saver::endArray(void) throw(io::IOException) {
	ASSERTP(isArray(state), "json: not inside an array!");
	state_t new_state = next(stack.pop());
	state = ARRAY;
	doIndent(true);
	_out << "]";
	state = new_state;
}

/**
 * Add a field. Only allowed inside an object.
 */
void Saver::addField(string id) throw(io::IOException) {
	ASSERTP(isObject(state), "json: field only allowed inside an object!");
	doIndent();
	_out << '"';
	try {
		for(utf8::Iter i(id); i; i++)
			escape(*i);
	}
	catch(utf8::Exception& e) {
		ASSERTP(false, _ << "json: bad utf8 string: \"" << id << "\"");
	}
	_out << '"';
	if(isReadable())
		_out << ": ";
	else
		_out << ":";
	stack.push(state);
	state = FIELD;
}

/**
 * Escape the given character if required.
 * @param c		Character to escape.
 * @return		Empty string if escape if not useful, escaped string else.
 */
void Saver::escape(utf8::char_t c) throw(io::IOException) {
	switch(c) {
	case '"' :	_out << "\\\""; break;
	case '\\':	_out << "\\\\"; break;
	case '\b':	_out << "\\b";  break;
	case '\f':	_out << "\\f";  break;
	case '\n':	_out << "\\n";  break;
	case '\r':	_out << "\\r";  break;
	case '\t':	_out << "\\t";  break;
	default:
		if(c >= 32 && c < 128)
			_out << char(c);
		else
			_out << "\\u" << io::fmt(c).hex().width(4).pad('0').right();
		break;
	}
}

/**
 * Put a null value.
 */
void Saver::put(void) throw(io::IOException) {
	ASSERTP(state == FIELD || isArray(state), "json: cannot put a value out of a field or an array!");
	doIndent();
	_out << "null";
	if(state == FIELD)
		state = stack.pop();
	state = next(state);
}

/**
 * Put a string value.
 * @param str	String to put.
 */
void Saver::put(cstring str) throw(io::IOException) {
	ASSERTP(state == FIELD || isArray(state), "json: cannot put a value out of a field or an array!");
	doIndent();
	_out << '"';
	for(utf8::Iter i(str); i; i++)
		escape(*i);
	_out << '"';
	if(state == FIELD)
		state = stack.pop();
	state = next(state);
}

/**
 * Put a string value.
 * @param val	String to put.
 */
void Saver::put(string val) throw(io::IOException) {
	ASSERTP(state == FIELD || isArray(state), "json: cannot put a value out of a field or an array!");
	doIndent();
	_out << '"';
	for(utf8::Iter i(val); i; i++)
		escape(*i);
	_out << '"';
	if(state == FIELD)
		state = stack.pop();
	state = next(state);
}

/**
 * Put an integer value.
 * @param val	Value to put.
 */
void Saver::put(t::uint64 val) throw(io::IOException) {
	ASSERTP(state == FIELD || isArray(state), "json: cannot put a value out of a field or an array!");
	doIndent();
	_out << val;
	if(state == FIELD)
		state = stack.pop();
	state = next(state);
}

/**
 * Put an integer value.
 * @param val	Value to put.
 */
void Saver::put(t::int64 val) throw(io::IOException) {
	ASSERTP(state == FIELD || isArray(state), "json: cannot put a value out of a field or an array!");
	doIndent();
	_out << val;
	if(state == FIELD)
		state = stack.pop();
	state = next(state);
}

/**
 * Put a double value.
 * @param val	Value to put.
 */
void Saver::put(double val) throw(io::IOException) {
	ASSERTP(state == FIELD || isArray(state), "json: cannot put a value out of a field or an array!");
	doIndent();
	_out << val;
	if(state == FIELD)
		state = stack.pop();
	state = next(state);
}

/**
 * Put a boolean value.
 * @param val	Value to put.
 */
void Saver::put(bool val) throw(io::IOException) {
	ASSERTP(state == FIELD || isArray(state), "json: cannot put a value out of a field or an array!");
	doIndent();
	if(val)
		_out << "true";
	else
		_out << "false";
	if(state == FIELD)
		state = stack.pop();
	state = next(state);
}

} }		// json
