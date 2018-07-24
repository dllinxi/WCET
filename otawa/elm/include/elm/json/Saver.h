/*
 *	json::Saver class interface
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
#ifndef ELM_JSON_SAVER_H_
#define ELM_JSON_SAVER_H_

#include <elm/genstruct/Vector.h>
#include <elm/io.h>
#include <elm/io/BufferedOutStream.h>
#include <elm/string/utf8.h>
#include <elm/sys/Path.h>
#include "common.h"

namespace elm { namespace json {

class Saver {
public:
	Saver(io::OutStream& out = io::out);
	Saver(StringBuffer& buf);
	Saver(sys::Path& path) throw(io::IOException);
	~Saver(void) throw(io::IOException, Exception);
	void close(void);

	inline bool isReadable(void) const { return readable; }
	inline void setReadable(bool read) { readable = read; }
	inline string getIndent(void) const { return indent; }
	inline void setIndent(string i) { indent = i; }

	void beginObject(void) throw(io::IOException);
	void endObject(void) throw(io::IOException);
	void beginArray(void) throw(io::IOException);
	void endArray(void) throw(io::IOException);
	void addField(string id) throw(io::IOException);

	void put(void) throw(io::IOException);
	inline void put(const char *val) throw(io::IOException) { put(cstring(val)); }
	void put(cstring val) throw(io::IOException);
	void put(string val) throw(io::IOException);
	void put(t::uint64 val) throw(io::IOException);
	void put(t::int64 val) throw(io::IOException);
	inline void put(int val) throw(io::IOException) { put(t::int64(val)); }
	void put(double val) throw(io::IOException);
	void put(bool val) throw(io::IOException);

private:
	typedef enum {
		BEGIN,
		OBJECT,
		IN_OBJECT,
		FIELD,
		ARRAY,
		IN_ARRAY,
		END
	} state_t;

	void doIndent(bool close = false) throw(io::IOException);
	static state_t next(state_t s);
	static bool isObject(state_t s);
	static bool isArray(state_t s);
	void escape(utf8::char_t c) throw(io::IOException);

	state_t state;
	genstruct::Vector<state_t> stack;
	io::Output _out;
	bool readable;
	string indent;
	io::BufferedOutStream *buf;
	io::OutStream *str;
};

} }	// elm::json

#endif /* ELM_JSON_SAVER_H_ */
