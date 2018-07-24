/*
 * StringBuffer class interface
 * Copyright (c) 2016, university of Toulouse.
 *
 * elm/string/StringBuffer.h -- interface for StringBuffer class.
 */
#ifndef ELM_STRING_STRING_BUFFER_H
#define ELM_STRING_STRING_BUFFER_H

#include <elm/string/CString.h>
#include <elm/string/String.h>
#include <elm/io/BlockOutStream.h>
#include <elm/io/Output.h>

namespace elm {

// StringBuffer class
class StringBuffer: public io::Output {
public:

	inline StringBuffer(int capacity = 64, int increment = 32)
		: io::Output(_stream), _stream(capacity, increment)
		{ init(); }

	inline String toString(void)
		{ int len = length(); _stream.write('\0');
		return String((String::buffer_t *)_stream.detach(), sizeof(short), len); }

	inline String copyString(void)
		{ return String( _stream.block() + sizeof(short), _stream.size() - sizeof(short)); }
	inline int length(void) const { return _stream.size() - sizeof(short); }
	inline void reset(void) { _stream.clear(); init(); }
	inline io::OutStream& stream(void) { return _stream; }

private:
	inline void init(void) { String::buffer_t str = { 0 }; _stream.write((char *)&str, sizeof(short)); }
	io::BlockOutStream _stream;
};

} // elm

#endif // ELM_STRING_STRING_BUFFER_H
