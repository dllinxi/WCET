/*
 *	type_info class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-08, IRIT UPS.
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
#ifndef ELM_STRING_STRING_H
#define ELM_STRING_STRING_H

#include <elm/PreIterator.h>
#include <elm/string/CString.h>

namespace elm {

// String class
class String {
	friend class CString;
	friend class StringBuffer;

	// Data structure
	typedef struct buffer_t {
		unsigned short use;
		char buf[1];
	} buffer_t;
	static buffer_t empty_buf;
	static const int zero_off = sizeof(unsigned short);
	mutable const char *buf;
	mutable unsigned short off, len;

	// Internals
	void copy(const char *str, int _len);
	void lock(void) const { ((buffer_t *)buf)->use++; }
	void toc(void) const;
	void unlock(void) const {
		((buffer_t *)buf)->use--;
		if(!((buffer_t *)buf)->use && buf != (char *)&empty_buf)
			delete [] buf;
	}
	inline String(const char *_buf, int _off, int _len): buf(_buf), off(_off), len(_len) { lock(); };
	static String concat(const char *s1, int l1, const char *s2, int l2);
	inline String(buffer_t *buffer, int offset, int length)
		: buf((char *)buffer), off(offset), len(length) { lock(); };

public:
	inline String(void): buf((char *)&empty_buf), off(zero_off), len(0) { lock(); };
	inline String(const char *str, int _len) { copy(str, _len); };
	inline String(const char *str) { if(!str) str = ""; copy(str, strlen(str)); };
	inline String(cstring str) { copy(str.chars(), str.length()); };
	inline String(const String& str): buf(str.buf), off(str.off), len(str.len) { lock(); };
	inline ~String(void) { unlock(); };
	inline String& operator=(const String& str)
		{ str.lock(); unlock(); buf = str.buf; off = str.off; len = str.len; return *this; };
	inline String& operator=(const CString str)
		{ unlock(); copy(str.chars(), str.length()); return *this; };
	inline String& operator=(const char *str)
		{ if(!str) str = ""; unlock(); copy(str, strlen(str)); return *this; };
	static String make(char chr);

	inline int length(void) const { return len; };
	inline const char *chars(void) const { return buf + off; };
	inline int compare(const String& str) const {
		int res = memcmp(chars(), str.chars(), len > str.len ? str.len : len);
		return res ? res : len - str.len;
	};
	inline int compare(const CString str) const {
		int slen = str.length();
		int res = memcmp(chars(), str.chars(), len > slen ? slen : len);
		return res ? res : len - slen;
	};

	inline bool isEmpty(void) const { return !len; };
	inline operator bool(void) const { return !isEmpty(); };

	inline CString toCString(void) const { if(buf[off + len] != '\0') toc(); return chars(); };
	inline const char *operator&(void) const { return toCString().chars(); };

	inline char charAt(int index) const { return buf[index + off]; };
	inline char operator[](int index) const { return charAt(index); };
	inline String substring(int _off) const { return String(buf, off + _off, len - _off); };
	inline String substring(int _off, int _len) const { return String(buf, off + _off, _len); };

	inline String concat(const CString str) const { return concat(chars(), len, str.chars(), str.length()); };
	inline String concat(const String& str) const { return concat(chars(), len, str.chars(), str.length()); };

	inline int indexOf(char chr) const { return indexOf(chr, 0); };
	inline int indexOf(char chr, int pos) const
		{ for(const char *p = chars() + pos; p < chars() + len; p++) if(*p == chr) return p - chars(); return -1; };
	int indexOf(const String& str, int pos = 0);
	inline int lastIndexOf(char chr) const { return lastIndexOf(chr, length()); };
	inline int lastIndexOf(char chr, int pos) const
		{ for(const char *p = chars() + pos - 1; p >= chars(); p--) if(*p == chr) return p - chars(); return -1; };
	inline int lastIndexOf(const String& str) { return lastIndexOf(str, length()); }
	int lastIndexOf(const String& str, int pos);

	inline bool startsWith(const char *str) const
		{ return startsWith(CString(str)); }
	inline bool startsWith(const CString str) const
		{ int l = str.length(); return len >= l && !memcmp(chars(), str.chars(), l); }
	inline bool startsWith(const String& str) const
		{ return len >= str.len && !memcmp(chars(), str.chars(), str.length()); }
	inline bool endsWith(const char *str) const
		{ return endsWith(CString(str)); }
	inline bool endsWith(const CString str) const
		{ int l = str.length(); return len >= l && !memcmp(chars() + len - l, str.chars(), l); }
	inline bool endsWith(const String& str) const
		{ return len>= str.len && !memcmp(chars() + len - str.len, str.chars(), str.len); }

	String trim(void) const;
	String ltrim(void) const;
	String rtrim(void) const;
};

// Type shortcut
#ifndef ELM_NO_STRING_SHORTCUT
	typedef String string;
#endif

} // elm

#endif // ELM_STRING_STRING_H
