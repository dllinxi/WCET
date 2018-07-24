/*
 *	Char class interface
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
#ifndef ELM_STRING_CHAR_H_
#define ELM_STRING_CHAR_H_

namespace elm {

class Char {
public:
	inline Char(void): c('\0') { }
	inline Char(char ch): c(ch) { }
	inline operator char(void) const { return c; }
	inline Char& operator=(char ch) { c = ch; return *this; }
	inline Char& operator=(Char ch) { c = ch.c; return *this; }

	inline int compare(char cc) const { return (unsigned char)c - (unsigned char)cc; }
	inline int compare(Char ch) const { return (unsigned char)c - (unsigned char)ch.c; }

	static Char bin(int v);
	static Char dec(int v);
	inline static Char hex(int v) { return lowerHex(v); }
	static Char lowerHex(int v);
	static Char upperHex(int v);

	bool isBin(void) const;
	bool isDec(void) const;
	bool isHex(void) const;
	bool isSpace(void) const;
	bool isPrintable(void) const;
	bool isLetter(void) const;
	bool isLowerCase(void) const;
	bool isUpperCase(void) const;

	int asBin(void) const;
	int asDec(void) const;
	int asHex(void) const;

	inline bool operator==(char ch) { return compare(ch) == 0; }
	inline bool operator!=(char ch) { return compare(ch) != 0; }
	inline bool operator<(char ch) { return compare(ch) < 0; }
	inline bool operator<=(char ch) { return compare(ch) <= 0; }
	inline bool operator>(char ch) { return compare(ch) > 0; }
	inline bool operator>=(char ch) { return compare(ch) >= 0; }

	inline bool operator==(Char ch) { return compare(ch) == 0; }
	inline bool operator!=(Char ch) { return compare(ch) != 0; }
	inline bool operator<(Char ch) { return compare(ch) < 0; }
	inline bool operator<=(Char ch) { return compare(ch) <= 0; }
	inline bool operator>(Char ch) { return compare(ch) > 0; }
	inline bool operator>=(Char ch) { return compare(ch) >= 0; }

private:
	char c;
};

}	// elm

#endif /* ELM_STRING_CHAR_H_ */
