/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * TextSerializer class implementation.
 */

#include <elm/assert.h>
#include <elm/io.h>
#include <elm/serial2/serial.h>
#include <elm/serial2/TextSerializer.h>

namespace elm { namespace serial2 {

/**
 * @class TextSerializer
 * Serialize the given data to a character output stream, readable by
 * human being.
 * @ingroup serial
 */


/**
 * Display prefix information.
 * @param type		Type of the object.
 * @param object	Current object.
 */
void TextSerializer::prefix(AbstractType& type, const void *object) {
	_out << '*' << t::intptr(object);
	objects.put(object, true);
	if(!level)
		_out << ':' << type.name();
	_out << '=';
	level++;
}


/**
 * Display suffix information.
 */
void TextSerializer::suffix(void) {
	level--;
}


/**
 */
void TextSerializer::flush(void) {
	while(to_process) {
		delay_t delayed = to_process.get();
		if(!objects.get(delayed.fst, false))
			delayed.snd->serialize(*this, delayed.fst);
	}
	objects.clear();
}


/**
 */
void TextSerializer::beginObject(AbstractType& type, const void *object) {
	prefix(type, object);
	_out << "{";
}


/**
 */
void TextSerializer::endObject(AbstractType& type, const void *object) {
	suffix();
	_out << "}";
}


/**
 */
void TextSerializer::beginField(CString name) {
	_out << name << "->";
}


/**
 */
void TextSerializer::endField(void) {
}


/**
 */
void TextSerializer::onPointer(AbstractType& type, const void *object) {
	_out << "&" << t::intptr(object) << ';';
	if(!objects.exists(object)) {
		to_process.put(pair(object, &type));
		objects.put(object, false);
	}
}


/**
 */
void TextSerializer::onValue(const bool& v) {
	prefix(type<bool>(), &v);
	_out << (v ? "true" : "false") << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const signed int& v) {
	prefix(type<signed int>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const unsigned int& v) {
	prefix(type<unsigned int>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const signed char& v) {
	prefix(type<signed char>(), &v);
	_out << (signed int)v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const unsigned char& v) {
	prefix(type<unsigned char>(), &v);
	_out << (unsigned int)v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const signed short& v) {
	prefix(type<signed short>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const unsigned short& v) {
	prefix(type<unsigned short>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const signed long& v) {
	prefix(type<signed long>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const unsigned long& v) {
	prefix(type<unsigned long>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const signed long long& v) {
	prefix(type<signed long long>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const unsigned long long& v) {
	prefix(type<unsigned long long>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const float& v) {
	prefix(type<float>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const double& v) {
	prefix(type<double>(), &v);
	_out << v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const long double& v) {
	prefix(type<long double>(), &v);
	/* !! TODO !! */
	_out << (double)v << ';';
	suffix();
}


/**
 */
void TextSerializer::onValue(const CString& v) {
	prefix(type<signed int>(), &v);
	_out << '"' << v << "\";";
	suffix();
}


/**
 */
void TextSerializer::onValue(const String& v) {
	prefix(type<signed int>(), &v);
	_out << '"' << v << "\";";
	suffix();
}


/**
 */
void TextSerializer::onEnum(const void *address, int value, AbstractEnum& clazz)
{
	prefix(clazz, address);
	CString name = clazz.nameOf(value);
	ASSERTP(name, "empty class name");
	_out << name;
	suffix(); 
}


/**
 */
void TextSerializer::beginCompound(const void *object) {
	level++;
	_out << '{';
}


/**
 */
void TextSerializer::onItem(void) {
}


/**
 */
void TextSerializer::endCompound(const void *object) {
	_out << '}';
	level--;
}

} } // elm::serial2
