/*
 * $Id$
 * Copyright (c) 2004-07, IRIT - UPS
 *
 * string module interface
 */
#ifndef ELM_STRING_H
#define ELM_STRING_H

#include <string.h>
#include <elm/string/Char.h>
#include <elm/string/CString.h>
#include <elm/string/String.h>
#include <elm/string/StringBuffer.h>
#include <elm/string/AutoString.h>
#include <elm/string/Split.h>

namespace elm {

// Back C Strings
inline CString CString::substring(int off) const { return CString(buf + off); };
inline String CString::substring(int off, int len) const { return String(buf + off, len); };
inline String CString::concat(const CString str) const { return String::concat(chars(), length(), str.chars(), str.length()); };
inline String CString::concat(const String& str) const { return String::concat(chars(), length(), str.chars(), str.length()); };
inline bool CString::startsWith(const char *str) const { return startsWith(CString(str)); };
inline bool CString::startsWith(const CString str) const { return !strncmp(buf, str.chars(), str.length()); };
inline bool CString::startsWith(const String& str) const { return !strncmp(buf, str.chars(), str.length()); };
inline bool CString::endsWith(const CString str) const
	{ int l1 = length(), l2 = str.length(); return l1 >= l2 && !memcmp(buf + l1 - l2, str.chars(), l2); };
inline bool CString::endsWith(const String& str) const
	{ int l1 = length(), l2 = str.length(); return l1 >= l2 && !memcmp(buf + l1 - l2, str.chars(), l2); };

// Comparison inlines
inline bool operator==(const CString& s1, const CString& s2) { return s1.compare(s2) == 0; };
inline bool operator!=(const CString& s1, const CString& s2) { return s1.compare(s2) != 0; };
inline bool operator<(const CString s1, const CString s2) { return s1.compare(s2) < 0; };
inline bool operator<=(const CString s1, const CString s2) { return s1.compare(s2) <= 0; };
inline bool operator>(const CString s1, const CString s2) { return s1.compare(s2) > 0; };
inline bool operator>=(const CString s1, const CString s2) { return s1.compare(s2) >= 0; };

inline bool operator==(const String& s1, const CString s2) { return s1.compare(s2) == 0; };
inline bool operator!=(const String& s1, const CString s2) { return s1.compare(s2) != 0; };
inline bool operator<(const String& s1, const CString s2) { return s1.compare(s2) < 0; };
inline bool operator<=(const String& s1, const CString s2) { return s1.compare(s2) <= 0; };
inline bool operator>(const String& s1, const CString s2) { return s1.compare(s2) > 0; };
inline bool operator>=(const String& s1, const CString s2) { return s1.compare(s2) >= 0; };

inline bool operator==(const String& s1, const String& s2) { return s1.compare(s2) == 0; };
inline bool operator!=(const String& s1, const String& s2) { return s1.compare(s2) != 0; };
inline bool operator<(const String& s1, const String& s2) { return s1.compare(s2) < 0; };
inline bool operator<=(const String& s1, const String& s2) { return s1.compare(s2) <= 0; };
inline bool operator>(const String& s1, const String& s2) { return s1.compare(s2) > 0; };
inline bool operator>=(const String& s1, const String& s2) { return s1.compare(s2) >= 0; };

inline bool operator==(const CString s1, const String& s2) { return s2.compare(s1) == 0; };
inline bool operator!=(const CString s1, const String& s2) { return s2.compare(s1) != 0; };
inline bool operator<(const CString s1, const String& s2) { return s2.compare(s1) >= 0; };
inline bool operator<=(const CString s1, const String& s2) { return s2.compare(s1) > 0; };
inline bool operator>(const CString s1, const String& s2) { return s2.compare(s1) <= 0; };
inline bool operator>=(const CString s1, const String& s2) { return s2.compare(s1) < 0; };

inline bool operator==(const String& s1, const char *s2) { return s1.compare(CString(s2)) == 0; };
inline bool operator!=(const String& s1, const char *s2) { return s1.compare(CString(s2)) != 0; };
inline bool operator<(const String& s1, const char *s2) { return s1.compare(CString(s2)) < 0; };
inline bool operator<=(const String& s1, const char *s2) { return s1.compare(CString(s2)) <= 0; };
inline bool operator>(const String& s1, const char *s2) { return s1.compare(CString(s2)) > 0; };
inline bool operator>=(const String& s1, const char *s2) { return s1.compare(CString(s2)) >= 0; };

inline bool operator==(const CString s1, const char *s2) { return s1.compare(CString(s2)) == 0; };
inline bool operator!=(const CString s1, const char *s2) { return s1.compare(CString(s2)) != 0; };
inline bool operator<(const CString s1, const char *s2) { return s1.compare(CString(s2)) < 0; };
inline bool operator<=(const CString s1, const char *s2) { return s1.compare(CString(s2)) <= 0; };
inline bool operator>(const CString s1, const char *s2) { return s1.compare(CString(s2)) > 0; };
inline bool operator>=(const CString s1, const char *s2) { return s1.compare(CString(s2)) >= 0; };

inline bool operator==(const char *s1, const CString s2) { return s2.compare(CString(s1)) == 0; };
inline bool operator!=(const char *s1, const CString s2) { return s2.compare(CString(s1)) != 0; };
inline bool operator<(const char *s1, const CString s2) { return s2.compare(CString(s1)) >= 0; };
inline bool operator<=(const char *s1, const CString s2) { return s2.compare(CString(s1)) > 0; };
inline bool operator>(const char *s1, const CString s2) { return s2.compare(CString(s1)) <= 0; };
inline bool operator>=(const char *s1, const CString s2) { return s2.compare(CString(s1)) < 0; };

inline bool operator==(const char *s1, const String& s2) { return s2.compare(CString(s1)) == 0; };
inline bool operator!=(const char *s1, const String& s2) { return s2.compare(CString(s1)) != 0; };
inline bool operator<(const char *s1, const String& s2) { return s2.compare(CString(s1)) >= 0; };
inline bool operator<=(const char *s1, const String& s2) { return s2.compare(CString(s1)) > 0; };
inline bool operator>(const char *s1, const String& s2) { return s2.compare(CString(s1)) <= 0; };
inline bool operator>=(const char *s1, const String& s2) { return s2.compare(CString(s1)) < 0; };


// Concatenation
inline String operator+(const CString s1, const CString s2) { return s1.concat(s2); }
inline String operator+(const CString s1, const char *s2) { return s1.concat(CString(s2)); };
inline String operator+(const CString s1, const String& s2) { return s1.concat(s2); };
inline String operator+(const String& s1, const CString s2) { return s1.concat(s2); };
inline String operator+(const String& s1, const char *s2) { return s1.concat(CString(s2)); };
inline String operator+(const String& s1, const String& s2) { return s1.concat(s2); };
inline String operator+(const char *s1, const CString s2) { return CString(s1).concat(s2); };
inline String operator+(const char *s1, const String& s2) { return CString(s1).concat(s2); };
};

#endif // ELM_STRING_H
