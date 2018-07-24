/*
 *	ini::Loader class implementation
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
#include <elm/ini.h>
#include <elm/io.h>
#include <elm/sys/System.h>
#include <elm/io/BufferedInStream.h>

namespace elm { namespace ini {

/**
 * @defgroup ini	.INI file support
 * This module provides minimal implementation for loading ".INI" files as found
 * on MS systems. A @ref File content is obtained using one of the @ref elm::ini::load() function.
 * They can be explored by @ref Section and in each section, the values associated with key
 * can be examined and possibly converted in integer or lists.
 *
 * Notice that key/values outside of a section is put in a section named "".
 */

/**
 * @class Exception;
 * Used to report an error during an IO activity on ".INI" file.
 * @ingroup ini
 */


/**
 * @class Section
 * Data in ".INI" files are grouped by sections containing a set if key/value pairs.
 * @ingroup ini
 */


/**
 * @fn Section::Section(const string& _name);
 * Build a section.
 */

/**
 * @fn const string& Section::name(void) const;
 * Get the name of a section.
 * @return	Section name.
 */


/**
 * string Section::get(const string& key) const;
 * Get the value associated with the given key or an empty string if it is not defined.
 * @param key	Key of the looked value.
 * @return		Value associated with the key or an empty string.
 */


/**
 * @fn string Section::operator[](const string& key);
 * Alias to get().
 */


/**
 * @fn bool Section::isDefined(const string& key) const;
 * Test if a key/value pair is defined.
 * @param key	Key to test.
 * @return		True if the key exists, false else.
 */


/**
 * Get a value and transform it to integer.
 * @param key			Looked key.
 * @param def			Default value if the key is not defined or if the conversion fails.
 * @return				Value transformed to integer.
 */
int Section::getInt(const string& key, int def) {
	string v = get(key);
	if(!v)
		return def;
	int r;
	try {
		v >> r;
	}
	catch(io::IOException& e) {
		return def;
	}
	return r;
}


/*
 * Get the value associated with a key as a list of strings (separated by ";").
 * @param key	Looked key.
 * @param list	Vector to store items of the list inside.
 */
void Section::getList(const string& key, genstruct::Vector<string>& list) {
	string buf = get(key);
	int p = buf.indexOf(';');
	while(p >= 0) {
		list.add(buf.substring(0, p));
		buf = buf.substring(p + 1);
		p = buf.indexOf(';');
	}
	if(buf)
		list.add(buf);
}


/**
 * Get a value and possibly a default value if not defined.
 * @param key	Key to look for.
 * @param def	Default value.
 * @return		Found value or default value.
 */
string Section::get(const string& key, const string& def) const {
	if(values.hasKey(key))
		return get(key);
	else
		return def;
}


/**
 * @class File
 * Repressents a ".INI" file made of sections. To get the default section, either use
 * the @ref defaultSection() method or get the section wit an empty name.
 * @ingroup ini
 */


/**
 * Private.
 */
File::File(void) {
	def = new Section("");
	sects.put("", def);
}


/**
 */
File::~File(void) {
	for(genstruct::AssocList<string, Section *>::Iterator s(sects); s; s++)
		delete *s;
}


/**
 * Load the ".INI" file from the given path.
 * @param path		Path to load ".INI" from.
 * @return			Created INI file.
 * @throw Exception	For any error (IO or format).
 */
File *File::load(const sys::Path& path) throw(Exception) {
	try {
		io::InStream *in = sys::System::readFile(path);
		File *r = load(in);
		delete in;
		return r;
	}
	catch(sys::SystemException& e) {
		throw Exception(e.message());
	}
	return 0;
}


/**
 * Load the ".INI" file from the given input stream.
 * @param in		stream to read file from.
 * @return			Created INI file.
 * @throw Exception	For any error (IO or format).
 */
File *File::load(io::InStream *in) throw (Exception) {
	File *file = new File();
	Section *sect = file->defaultSection();
	io::BufferedInStream bufin(*in);
	io::Input input(bufin);
	try {

		// read all lines
		int num = 1;
		while(true) {

			// read the line
			bool ended = false;
			string line;
			while(true) {
				line = line + input.scanLine();
				num++;
				if(!line) {
					ended = true;
					break;
				}
				while(line[line.length() - 1] == '\n' || line[line.length() - 1] == '\r')
					line = line.substring(0, line.length() - 1);
				if(line[line.length() - 1] != '\\')
					break;
				line = line.substring(0, line.length() - 1);
			}
			if(ended)
				break;

			// empty line?
			if(!line)
				continue;

			// is it a comment?
			if(line[0] == ';')
				continue;

			// is it a section?
			if(line[0] == '[') {
				if(line[line.length() - 1] != ']')
					throw Exception(_ << num << ": malformed section name");
				sect = new Section(line.substring(1, line.length() - 2));
				file->sects.put(sect->name(), sect);
				continue;
			}

			// is it a key/value pair?
			int p = line.indexOf('=');
			if(p >= 0) {
				sect->values.put(line.substring(0, p), line.substring(p + 1));
				continue;
			}

			// syntax error?
			throw Exception(_ << num << ": garbage here");
		}
	}
	catch(io::IOException& e) {
		delete file;
		throw Exception(e.message());
	}
	return file;
}


/**
 * @fn Section *File::defaultSection(void) const;
 */


/**
 * @fn Section *Section::get(const string& name) const;
 * @param name	Name of looked section.
 * @return		Found section or null.
 */


/**
 * @fn Section *Section::operator[](const string& name) const;
 * Alias to @ref get().
 */

} }	// elm::ini
