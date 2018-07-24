/*
 *	$Id$
 *	Output class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-12, IRIT UPS.
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
#ifndef ELM_IO_OUTPUT_H
#define ELM_IO_OUTPUT_H

#include <elm/types.h>
#include <elm/sys/SystemIO.h>
#include <elm/util/VarArg.h>
#include <elm/string/String.h>
#include <elm/string/CString.h>
#include <elm/enum_info.h>
#include <elm/meta.h>

namespace elm { namespace io {

// alignment_t enum
typedef enum alignment_t {
	NONE = 0,
	LEFT,
	CENTER,
	RIGHT
} alignment_t;


// IntFormat class
class IntFormat {
	inline void init(bool s, int size)  {
		_base = 10;
		_width = 0;
		_align = LEFT;
		_pad = ' ';
		_upper = false;
		_sign = s;
		_displaySign = false;
		_size = size;
	}
public:
	inline IntFormat(void				)		: _val(0) 					{ init(true, 8); }
	inline IntFormat(signed char		value)	: _val(value) 				{ init(true,  1); }
	inline IntFormat(unsigned char		value)	: _val(t::uint64(value)) 	{ init(false, 1); }
	inline IntFormat(signed short		value)	: _val(value) 				{ init(true,  2); }
	inline IntFormat(unsigned short		value)	: _val(t::uint64(value)) 	{ init(false, 2); }
	inline IntFormat(signed int			value)	: _val(value) 				{ init(true,  4); }
	inline IntFormat(unsigned int		value)	: _val(t::uint64(value)) 	{ init(false, 4); }
#ifndef __LP64__
	inline IntFormat(signed long		value)	: _val(value) 				{ init(true, 4); }
	inline IntFormat(unsigned long		value)	: _val(t::uint64(value)) 	{ init(false, 4); }
#else
	inline IntFormat(signed long		value)	: _val(value) 				{ init(true, 8); }
	inline IntFormat(unsigned long		value)	: _val(t::uint64(value)) 	{ init(false, 8); }
#endif
	inline IntFormat(signed long long	value)	: _val(value)				{ init(true,  8); }
	inline IntFormat(unsigned long long	value)	: _val(value)				{ init(false, 8); }

	inline IntFormat operator()(t::int8	value) { IntFormat f = *this; f._val = value; return f; }
	inline IntFormat operator()(t::uint8	value) { IntFormat f = *this; f._val = value; return f; }
	inline IntFormat operator()(t::int16	value) { IntFormat f = *this; f._val = value; return f; }
	inline IntFormat operator()(t::uint16	value) { IntFormat f = *this; f._val = value; return f; }
	inline IntFormat operator()(t::int32	value) { IntFormat f = *this; f._val = value; return f; }
	inline IntFormat operator()(t::uint32	value) { IntFormat f = *this; f._val = value; return f; }
	inline IntFormat operator()(t::int64	value) { IntFormat f = *this; f._val = value; return f; }
	inline IntFormat operator()(t::uint64	value) { IntFormat f = *this; f._val = value; return f; }

	inline IntFormat base(int b) { _base = b; return *this; }
	inline IntFormat bin(void) { _base = 2; _sign = false; return *this; }
	inline IntFormat oct(void) { _base = 8; _sign = false; return *this; }
	inline IntFormat dec(void) { _base = 10; return *this; }
	inline IntFormat hex(void) { _base = 16; _sign = false; return *this; }
	inline IntFormat width(int w) { _width = w; return *this; }
	inline IntFormat align(alignment_t a) { _align = a; return *this; }
	inline IntFormat left(void) { _align = LEFT; return *this; }
	inline IntFormat center(void) { _align = CENTER; return *this; }
	inline IntFormat right(void) { _align = RIGHT; return *this; }
	inline IntFormat upper(void) { _upper = true; return *this; }
	inline IntFormat lower(void) { _upper = false; return *this; }
	inline IntFormat sign(void) { _displaySign = true; return *this; }
	inline IntFormat pad(char p) { _pad = p; return *this; }

	t::int64 _val;
	unsigned char _base;
	unsigned char _width;
	unsigned _align : 5;
	unsigned _upper : 1;
	unsigned _sign : 1;
	unsigned _displaySign : 1;
	char _pad;
	char _size;
};


// FloatFormat class
class FloatFormat {
public:
	typedef enum {
		SHORTEST = 0,
		DECIMAL = 1,
		SCIENTIFIC = 2
	} style_t;

	inline FloatFormat(void): _val(0) { init(); }
	inline FloatFormat(float val): _val(val) { init(); }
	inline FloatFormat(double val): _val(val) { init(); }

	inline FloatFormat operator()(float val) { FloatFormat f = *this; f._val = val; return f; }
	inline FloatFormat operator()(double val) { FloatFormat f = *this; f._val = val; return f; }

	inline FloatFormat width(int w) { _width = w; return *this; }
	inline FloatFormat width(int w, int d) { _width = w; _decw = d; return *this; }
	inline FloatFormat style(style_t s) { _style = s; return *this; }
	inline FloatFormat shortest(void) { _style = SHORTEST; return *this; }
	inline FloatFormat decimal(void) { _style = DECIMAL; return *this; }
	inline FloatFormat scientific(void) { _style = SCIENTIFIC; return *this; }
	inline FloatFormat align(alignment_t a) { _align = a; return *this; }
	inline FloatFormat left(void) { _align = LEFT; return *this; }
	inline FloatFormat center(void) { _align = CENTER; return *this; }
	inline FloatFormat right(void) { _align = RIGHT; return *this; }
	inline FloatFormat upper(void) { _upper = true; return *this; }
	inline FloatFormat lower(void) { _upper = false; return *this; }
	inline FloatFormat pad(char p) { _pad = p; return *this; }

	double _val;
	unsigned char _width;
	unsigned char _decw;
	unsigned char _style;
	unsigned char _align: 2;
	unsigned char _upper: 1;
	unsigned char _pad;

private:
	void init(void) {
		_width = 0;
		_decw = 5;
		_style = SHORTEST;
		_align = LEFT;
		_upper = true;
		_pad = ' ';
	}
};


// StringFormat class
class StringFormat {
public:
	StringFormat(void) { init(); }
	StringFormat(string str): s(str) { init(); }

	inline StringFormat& operator()(string str)	{ s = str; return *this; }
	inline StringFormat width(int w) 			{ _width = w; return *this; }
	inline StringFormat align(alignment_t a)	{ _align = a; return *this; }
	inline StringFormat left(void) 				{ _align = LEFT; return *this; }
	inline StringFormat center(void) 			{ _align = CENTER; return *this; }
	inline StringFormat right(void) 			{ _align = RIGHT; return *this; }
	inline StringFormat pad(char p) 			{ _pad = p; return *this; }

	string s;
	unsigned char _width;
	unsigned char _align: 2;
	unsigned char _pad;

private:
	void init(void) {
		_width = 0;
		_align = LEFT;
		_pad = ' ';
	}
};

// Output class
class Output {
	OutStream *strm;
	char *horner(char *p, t::uint64 val, int base, char enc = 'a');
public:
	inline Output(void): strm(&out) { };
	inline Output(OutStream& stream): strm(&stream) { };
	inline OutStream& stream(void) const { return *strm; };
	inline void setStream(OutStream& stream) { strm = &stream; };
	void flush(void);

	void print(bool value);
	void print(char chr);
	void print(double value);
	void print(void *value);
	inline void print(const char *str) { print(CString(str)); };
	void print(const CString str);
	void print(const String& str);
	void print(const IntFormat& fmt);
	void print(const FloatFormat& fmt);
	void print(const StringFormat& fmt);
	void format(CString fmt, ...);
	void format(CString fmt, VarArg& args);

	// deprecated
	void print(t::int32 value);
	void print(t::uint32 value);
	void print(t::int64 value);
	void print(t::uint64 value);
};


// operators accesses
template <class T> struct def_printer { static inline void print(Output& out, const T& v) { out.print("<not printable>"); } };
template <class T> struct enum_printer { static inline void print(Output& out, const T& v) { out.print(enum_info<T>::toString(v)); } };
template <class T> inline Output& operator<<(Output& out, const T& v)
	{ _if<type_info<T>::is_defined_enum, enum_printer<T>, def_printer<T> >::_::print(out, v); return out; }
template <class T> inline Output& operator<<(Output& out, T *v)
	{ out.print((void *)v); return out; }
inline Output& operator<<(Output& out, bool value) { out.print(value); return out; };
inline Output& operator<<(Output& out, char value) { out.print(value); return out; };
inline Output& operator<<(Output& out, unsigned char value) 	{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, signed char value) 		{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, short value) 			{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, unsigned short value)	{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, int value) 				{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, unsigned int value) 		{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, long value) 				{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, unsigned long value) 	{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, long long value) 		{ out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, unsigned long long value) { out.print(IntFormat(value)); return out; };
inline Output& operator<<(Output& out, float value) { out.print(value); return out; };
inline Output& operator<<(Output& out, double value) { out.print(value); return out; };
inline Output& operator<<(Output& out, const char *value) { out.print(value); return out; };
inline Output& operator<<(Output& out, char *value) { out.print(value); return out; };
inline Output& operator<<(Output& out, const CString value) { out.print(value); return out; };
inline Output& operator<<(Output& out, const string& value) { out.print(value); return out; };
inline Output& operator<<(Output& out, const IntFormat& value) { out.print(value); return out; };
inline Output& operator<<(Output& out, const FloatFormat& value) { out.print(value); return out; }
inline Output& operator<<(Output& out, const StringFormat& value) { out.print(value); return out; }


// End-of-line
const char endl = '\n';

// starter macro
inline IntFormat f(signed char			value)	{ return IntFormat(value); }
inline IntFormat f(unsigned char			value) 	{ return IntFormat(value); }
inline IntFormat f(signed short			value) 	{ return IntFormat(value); }
inline IntFormat f(unsigned short			value)	{ return IntFormat(value); }
inline IntFormat f(signed int				value)	{ return IntFormat(value); }
inline IntFormat f(unsigned int			value) 	{ return IntFormat(value); }
inline IntFormat f(signed long			value) 	{ return IntFormat(value); }
inline IntFormat f(unsigned long			value) 	{ return IntFormat(value); }
inline IntFormat f(signed long long		value) 	{ return IntFormat(value); }
inline IntFormat f(unsigned long long	value) 	{ return IntFormat(value); }
inline FloatFormat f(float value)					{ return FloatFormat(value); }
inline FloatFormat f(double value)				{ return FloatFormat(value); }

// predefined styles
IntFormat pointer(const void *p);
IntFormat byte(t::uint8 b);

// Tag tool
template <class P>
class Tag {
public:
	inline Tag(const typename P::t& val): v(val) { }
	inline void print(io::Output& out) const { P::print(out, v); }
private:
	const typename P::t& v;
};
template <class P>
io::Output& operator<<(io::Output& out, const Tag<P>& t) { t.print(out); return out; }


// set style macros (deprecated)
inline IntFormat base(int base, IntFormat fmt) { return fmt.base(base); }
inline IntFormat bin(IntFormat fmt) { return fmt.bin(); }
inline IntFormat oct(IntFormat fmt) { return fmt.oct(); }
inline IntFormat hex(IntFormat fmt) { return fmt.hex(); }
inline IntFormat sign(IntFormat fmt) { return fmt.sign(); }
inline IntFormat width(int width, IntFormat fmt) { return fmt.width(width); }
inline IntFormat align(alignment_t align, IntFormat fmt) { return fmt.align(align); }
inline IntFormat left(IntFormat fmt) { return fmt.left(); }
inline IntFormat right(IntFormat fmt) { return fmt.right(); }
inline IntFormat center(IntFormat fmt) { return fmt.center(); }
inline IntFormat pad(char pad, IntFormat fmt) { return fmt.pad(pad); }
inline IntFormat uppercase(IntFormat fmt) { return fmt.upper(); }
inline IntFormat lowercase(IntFormat fmt) { return fmt.lower(); }

// fmt macro
inline IntFormat 	fmt(t::int8   	i)	{ return IntFormat(i); }
inline IntFormat 	fmt(t::uint8  	i) 	{ return IntFormat(i); }
inline IntFormat 	fmt(t::int16  	i)	{ return IntFormat(i); }
inline IntFormat 	fmt(t::uint16 	i) 	{ return IntFormat(i); }
inline IntFormat 	fmt(t::int32  	i)	{ return IntFormat(i); }
inline IntFormat	fmt(t::uint32 	i) 	{ return IntFormat(i); }
inline IntFormat 	fmt(t::int64  	i)	{ return IntFormat(i); }
inline IntFormat 	fmt(t::uint64 	i) 	{ return IntFormat(i); }
inline FloatFormat	fmt(float 		f)	{ return FloatFormat(f); }
inline FloatFormat 	fmt(double 		f)	{ return FloatFormat(f); }
inline StringFormat fmt(string 		s)	{ return StringFormat(s); }
inline StringFormat fmt(cstring 	s)	{ return StringFormat(s); }
inline StringFormat fmt(const char *s)	{ return StringFormat(s); }

} } // elm::io

#endif	// ELM_IO_OUTPUT_H
