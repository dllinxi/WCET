/*
 *	ini::Loader class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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
#ifndef ELM_INI_H_
#define ELM_INI_H_

#include <elm/string.h>
#include <elm/genstruct/AssocList.h>
#include <elm/genstruct/Vector.h>
#include <elm/sys/Path.h>

namespace elm { namespace ini {

class Exception: public MessageException {
public:
	inline Exception(const string& msg): MessageException(msg) { }
};

class Section {
	friend class File;
	typedef genstruct::AssocList<string, string> map_t;
	inline Section(const string& name): _name(name) { }

public:
	inline const string& name(void) const { return _name; }
	inline string get(const string& key) const { return values.get(key, ""); }
	inline string operator[](const string& key) const { return get(key); }
	inline bool isDefined(const string& key) const { return values.hasKey(key); }
	string get(const string& key, const string& def) const;
	int getInt(const string& key, int def);
	void getList(const string& key, genstruct::Vector<string>& list);

	class Iterator: public map_t::PairIterator {
	public:
		inline Iterator(Section *s): map_t::PairIterator(s->values) { }
		inline Iterator(const Iterator& i): map_t::PairIterator(i) { }
		inline Iterator& operator=(const Iterator& i) { map_t::PairIterator::operator=(i); return *this; }
		inline const string& key(void) const { return item().fst; }
		inline const string& value(void) const { return item().snd; }
	};

private:
	string _name;
	map_t values;
};

class File {
	File(void);
	typedef genstruct::AssocList<string, Section *> map_t;

public:
	static File *load(const sys::Path& path) throw(Exception);
	static File *load(io::InStream *in) throw (Exception);
	~File(void);
	inline Section *defaultSection(void) const { return def; }
	inline Section *get(const string& name) const { return sects.get(name, 0); }
	inline Section *operator[](const string& name) const { return get(name); }

	class Iterator: public map_t::Iterator {
	public:
		inline Iterator(File *file): map_t::Iterator(file->sects) { }
		inline Iterator(const Iterator& i): map_t::Iterator(i) { }
		inline Iterator operator=(const Iterator& i) { map_t::Iterator::operator=(i); return *this; }
	};

private:
	Section *def;
	map_t sects;
};

} }	// elm::ini

#endif /* ELM_INI_H_ */
