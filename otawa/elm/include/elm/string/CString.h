/*
 * $Id$
 * Copyright (c) 2004-07, IRIT - UPS.
 *
 * CString class
 */
#ifndef ELM_STRING_CSTRING_H
#define ELM_STRING_CSTRING_H

#include <string.h>

namespace elm {

class String;
	
// CString class
class CString {
protected:
	const char *buf;
public:
	inline CString(void): buf("") { };
	inline CString(const char *str): buf(str ? str : "") { };
	inline CString(const CString& str): buf(str.buf) { };
	inline CString& operator=(const CString& str) { buf = str.buf; return *this; };
	
	inline int length(void) const { return strlen(buf); };
	inline const char *chars(void) const { return buf; };
	inline int compare(const CString& str) const { return strcmp(buf, str.buf); };
	inline const char *operator&(void) const { return buf; };
	
	inline bool isEmpty(void) const { return !*buf; };
	inline operator bool(void) const { return !isEmpty(); };
	
	inline char charAt(int index) const { return buf[index]; };
	inline char operator[](int index) const { return charAt(index); };
	inline CString substring(int pos) const;
	inline String substring(int pos, int len) const;
	
	inline String concat(const CString str) const;
	inline String concat(const String& str) const;
	
	inline int indexOf(char chr) const { return indexOf(chr, 0); };
	inline int indexOf(char chr, int pos) const
		{ for(const char *p = buf + pos; *p; p++) if(*p == chr) return p - buf; return -1; };
	inline int lastIndexOf(char chr) const { return lastIndexOf(chr, length()); };
	inline int lastIndexOf(char chr, int pos) const
		{ for(const char *p = buf + pos - 1; p >= buf; p--) if(*p == chr) return p - buf; return -1; };
	
	// Suffix and prefix
	inline bool startsWith(const char *str) const;
	inline bool startsWith(const CString str) const;
	inline bool startsWith(const String& str) const;
	inline bool endsWith(const CString str) const;
	inline bool endsWith(const String& str) const;
	
	// Automatic conversion
	inline operator const char *(void) const { return buf; };
};

// Type shortcut
#ifndef ELM_NO_STRING_SHORTCUT
	typedef CString cstring;
#endif

} // elm

#endif	// ELM_STRING_CSTRING_H
