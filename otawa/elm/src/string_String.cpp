/*
 *	$Id$
 *	String class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2003-08, IRIT UPS.
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

#include <ctype.h>
#include <elm/string.h>
#include <elm/assert.h>
 
 namespace elm {
 
 
 /**
  * @defgroup string Character String
  * 
  * This module provides classes making easier the use of character string.
  * There is basically two classes: @ref String and @ref CString. Other classes
  * provides more services like @ref StringBuffer.
  * 
  * Notice that typedefs "string" and "cstring" are only scalar-type-like
  * shortcuts to @ref String and @ref CString classes.
  * 
  * @par Base Classes
  * 
  * The @ref CString is a simple wrapper around the original C "char *" 
  * null-terminated C string. It
  * provides an interface very similar to the @ref String class as possible.
  * The operator "&" may be used to get the hidden "const char *" buffer.
  * When @ref CString operation requires memory allocation, a @ref String
  * object is instead returned as this class has automatic memory management
  * facilities. The @ref CString is either useful to interact with the OS,
  * or to handle constant litteral strings.
  * 
  * The @ref String provides a lot of facilities to handle strings as any other
  * scalar type:
  * @li the memory allocation is automatically handled (counter based),
  * @li the memory allocation is only performed when needed (for concatenation
  * time or for @ref CString conversion),
  * @li the substring building is fast (inducing no memory movement).
  *
  * @par String Operations
  * 
  * The string classes implements the concept @ref concept::Array<char>.
  * 
  * The following operators are also available:
  * @li "==", "!=", "<", "<=", ">", ">=" for comparisons,
  * @li "+" for concatenation,
  * @li "=" for assignment,
  * @li "&" to get the character buffer (note that, for @ref String class, this
  * array not ever ended by a null character).
  * 
  * Some automatic conversions are also available:
  * @li @ref CString to @ref String,
  * @li string to boolean (to false for an empty string, to true else).
  * @li String::toCString() provides a way to convert a @ref String to a
  *		@ref CString (ensuring a terminating null character),
  * @li litteral C++ string to any string.
  * 
  * Finally, there some methods to perform tests or retrieval of strings:
  * @li indexOf() and lastIndexOf() to retrieve a single character,
  * @li startsWith() and endsWith() to test prefixes and suffixes.
  * 
  * And some methods to build strings:
  * @li substring() to retrieve a sub-string by its position and length,
  * @li concat() to perform concanetation.
  * 
  * @par StringBuffer Class
  * 
  * String building is very costly because it requires a memory copy of both
  * involved strings. Usually, a string building operation involves many
  * concatenation. The @ref StringBuffer class may be used to reduce this cost.
  * 
  * The string is built in a large buffer that is expanded (inducing a copy
  * operation) less often than the original @ref String class. To makes things
  * easier, this class provides also the same interface as IO @ref io::Output
  * class. To get the completed string, one has only to call the toString()
  * method as in the example below.
  * 
  * @code
  * for(int i = 0; i < 10; i++) {
  * 	StringBuffer buffer;
  * 	buffer << "Hello world: " << i << io:endl;
  * 	string str = buffer.toString();
  * 	my_function(str);
  * }
  * @endcode
  * 
  * To make things even easier, the ELM library provides an auto-builder
  * and frier of string buffer called @ref elm::_ 
  * that avoids the need to declare the string buffer. The previous example
  * uses below this feature:
  * 
  * @code
  * for(int i = 0; i < 10; i++)
  * 	my_function(_ << "Hello world: " << i << io:endl);
  * @endcode
  */


/**
 * @class String
 * An immutable implementation of the string data type. Refer to
 * @ref StringBuffer for long concatenation string building.
 * @ingroup string
 * @warning This string class is bound to strings of less than 65535 characters.
 */

 /* Empty buffer */
 String::buffer_t String::empty_buf = { 1, { 0 } };
 
 /**
  * Make a string by copying the given character array.
  * @param str	Character array address.
  * @param _len	Character array length.
  */
 void String::copy(const char *str, int _len) {
 	
 	// empty string?
 	if(!_len) {
 		buf = (char *)&empty_buf;
 		off = zero_off;
 		len = 0;
 		lock();
 	}
 	
 	// Create the buffer
 	else {
		buf = new char[sizeof(buffer_t) + _len]; 
		buffer_t *desc = (buffer_t *)buf;
		desc->use = 1;
		memcpy(desc->buf, str, _len);
		desc->buf[_len] = '\0';
		off = zero_off;
		len = _len; 
 	}
}

/**
 * Build a string with a single character.
 * @param chr	Character containing the string.
 * @return		Built string.
 */
String String::make(char chr) {
	char buf[2] = { chr, '\0' };
	return String(buf);
}

/**
 * Build a new string by concatenation of two character arrays.
 * @param s1	First character array address.
 * @param l1	First character array length.
 * @param s2	Second character array address.
 * @param l2	Second character array length.
 */
String String::concat(const char *s1, int l1, const char *s2, int l2) {
	buffer_t *sbuf = (buffer_t *)new char[sizeof(String::buffer_t) + l1 + l2];
	sbuf->use = 0;
	memcpy(sbuf->buf, s1, l1);
	memcpy(sbuf->buf + l1, s2, l2);
	sbuf->buf[l1 + l2] = '\0';
	return String((char *)sbuf, zero_off, l1 + l2);
}


/**
 * Transform the buffer such that a '\0' character is found at end of the
 * current string. If the buffer is only owned by the current string, the '\0'
 * is simply added. Else a new buffer is created.
 */
void String::toc(void) const {
	
	// Only one owner
	buffer_t *sbuf = (buffer_t *)buf;
	if(sbuf->use <= 1)
		sbuf->buf[len] = '\0';
	
	// Build a new buffer
	else {
		char *nbuf = new char[sizeof(buffer_t) + len]; 
		buffer_t *nsbuf = (buffer_t *)nbuf;
		nsbuf->use = 1;
		memcpy(nsbuf->buf, chars(), len);
		unlock();
		nsbuf->buf[len] = '\0';
		off = zero_off;
		buf = nbuf;
	}
}


/**
 * @fn String::String(void)
 * Build an empty string.
 */


/**
 * @fn String::String(const char *str, int _len)
 * Build a string from a character array.
 * @param str	Character array base.
 * @param _len	Character array length.
 */


/**
 * @fn String::String(const char *str);
 * Build an ELM string from a C string.
 * @param str	C string.
 */


/**
 * @fn String::String(const CString str);
 * Build a string from a @ref CString.
 * @param str	CString object to use.
 */


/**
 * @fn String::String(const String& str);
 * Clone an existing string.
 * @param str	String to clone.
 */


/**
 * @fn String& String::operator=(const String& str);
 * Assign a string to the current one.
 * @param str	String to assign.
 * @return		Current string.
 */


/**
 * @fn String& String::operator=(const CString str);
 * Assign a CString object to the current string.
 * @param str	CString object to assign.
 * @return		Current string.
 */
 
 
/**
 * @fn String& String::operator=(const char *str)
 * Assign a C string to the current string.
 * @param str	C string to assign.
 * @return		Current string.
 */


/**
 * @fn int String::length(void) const;
 * Get the length of the string.
 * @return	String length.
 */


/**
 * @fn const char *String::chars(void) const;
 * Get access to internal representation of the string. Use it with caution and
 * recall that the got buffer may be shared by many different strings.
 * @return	Raw access to character buffer.
 */

 
/**
 * @fn int String::compare(const String& str) const;
 * Compare two strings with ASCII order.
 * @param str	String to compare with.
 * @return		0 for equality, <0 if the current string is less than given one,
 * 				>0 if the current string is greater than given one.
 */


/**
 * @fn int String::compare(const CString str) const;
 * Compare this string with a CString object using ASCII order.
 * @param str	String to compare with.
 * @return		0 for equality, <0 if the current string is less than given one,
 * 				>0 if the current string is greater than given one.
 */


/**
 * @fn bool String::isEmpty(void) const;
 * Test if string is empty.
 * @return	True if the string is empty, false else.
 */


/**
 * @fn String::operator bool(void) const;
 * Same as isEmpty().
 */


/**
 * @fn CString String::toCString(void);
 * Convert the current string to a CString object.
 * @return	Matching CString object.
 * @warning The returned CString contains a pointer to a buffer owned by the
 * current string. This buffer is only valid while the current is living.
 */


/**
 * @fn const char *String::operator&(void);
 * Convert to C string.
 * @return	Matching C string.
 */

		
/**
 * @fn char String::charAt(int index) const;
 * Get a character at some position in the string. 
 * @param index Index of the character to get (It is an error to give
 * a negative index or an index greater or equal to the string length).
 * @return Matching character.
 */


/**
 * @fn char String::operator[](int index) const;
 * Same as @ref charAt().
 */


/**
 * @fn String String::substring(int _off);
 * Get a substring starting at the given offset to the
 * end of the string.
 * @param _off	Offset to start from (It is an error to give
 * a negative index or an index greater than the string length).
 * @return		Matching substring.
 */


/**
 * @fn String String::substring(int _off, int _len);
 * Get a substring starting at the given offset and continuing to the given
 * length.
 * @param _off	Offset to start from (It is an error to give
 * a negative index or an index greater than the string length).
 * @param _len	Length of the substring (it is an error to have a length
 * out of the current string).
 * @return		Matching substring.
 */


/**
 * @fn String String::concat(const CString str) const;
 * Concatenate a string with a C string.
 * @param str	C string to concatenate after.
 * @return		Result of concatenation.
 */


/**
 * @fn String String::concat(const String& str) const;
 * Concatenate two strings.
 * @param str	String to concatenate after.
 * @return		Result of concatenation.
 */


/**
 * @fn int String::indexOf(char chr) const;
 * Get the first index of a character in the string.
 * @param chr	Character to look for.
 * @param index	Index of the character in the string or -1 if not
 * found.
 */


/**
 * @fn int String::indexOf(char chr, int pos) const;
 * Get the first index of a character in the string from the given position.
 * @param chr	Character to look for.
 * @param pos	Position to start to look for (it is an error to pass a negative
 * position or a position greater or equal to the string length).
 * @param index	Index of the character in the string or -1 if not
 * found.
 */


/**
 * @fn int String::lastIndexOf(char chr) const;
 * Get the last index of a character in the string.
 * @param chr	Character to look for.
 * @param index	Index of the character in the string or -1 if not
 * found.
 */


/**
 * @fn int String::lastIndexOf(char chr, int pos) const;
 * Get the last index of a character in the string from the given position.
 * @param chr	Character to look for.
 * @param pos	Position to start to look for (it is an error to pass a negative
 * or null position or a position greater to the string length).
 * @param index	Index of the character in the string or -1 if not
 * found.
 */


/**
 * @fn bool String::startsWith(const char *str) const;
 * Test if the string starts with the given C string.
 * @param str	C string to compare.
 * @return		True if the string starts with, false else.
 */
 

/**
 * @fn bool String::startsWith(const CString str) const;
 * Test if the string starts with the given C string.
 * @param str	C string to compare.
 * @return		True if the string starts with, false else.
 */


/**
 * @fn bool String::startsWith(const String& str) const;
 * Test if the string starts with the given string.
 * @param str	String to compare.
 * @return		True if the string starts with, false else.
 */


/**
 * @fn bool String::endsWith(const char *str) const;
 * Test if the string ends with the given C string.
 * @param str	C string to compare.
 * @return		True if the string ends with, false else.
 */


/**
 * @fn bool String::endsWith(const CString str) const;
 * Test if the string ends with the given string.
 * @param str	String to compare.
 * @return		True if the string ends with, false else.
 */


/**
 * @fn bool String::endsWith(const String& str) const;
 * Test if the string ends with the given string.
 * @param str	String to compare.
 * @return		True if the string ends with, false else.
 */


/**
 * Find the first occurrence of a substring.
 * @param string	String to look for.
 * @param pos		Start position.
 */
int String::indexOf(const String& string, int pos) {
	ASSERTP(string, "cannot look for an empty string");
	while(pos >= 0) {
		pos = indexOf(string[0], pos);
		if(pos >= 0) {
			if(substring(pos, string.length()) == string) 
				return pos;
			else
				pos++;
		}
	}
	return -1;
}


/**
 * Find the last occurrence of a substring.
 * @param string	String to look for.
 * @param pos		Position to start to look before.
 */
int String::lastIndexOf(const String& string, int pos) {
	ASSERTP(string, "cannot look for an empty string");
	while(pos >= 0) {
		pos = lastIndexOf(string[0], pos);
		if(pos >= 0 && substring(pos, string.length()) == string)
				return pos;
	}
	return -1;
}


/**
 * Remove blanks at left and right of the current string.
 * @return	Trimmed string.
 */
string String::trim(void) const {
	return ltrim().rtrim();
}


/**
 * Remove blanks at left of the current string.
 * @return	Left-trimmed string.
 */
string String::ltrim(void) const {
	int i = 0;
	while(i < length() && isblank(charAt(i)))
		i++;
	return substring(i);
}


/**
 * Remove blanks at right of the current string.
 * @return	Right-trimmed string.
 */
string String::rtrim(void) const {
	int i = length() - 1;
	while(i >= 0 && isblank(charAt(i)))
		i--;
	return substring(0, i + 1);
}


/**
 * @class CString
 * Wrapper around C usual string.
 * @ingroup string
 */

namespace str {

/**
 * @class Split
 * This is an helper class to split a string in sub-parts
 * using a separator (character or string). At each iteration,
 * the next separated part is provided.
 *
 * Notice that the split of an empty string iterates exactly
 * once and its unique value is the empty string.
 *
 * @ingroup string
 */

}	// str
}	// elm
