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

#include <elm/assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <elm/io/io.h>
#include <elm/compare.h>

namespace elm { namespace io {

/**
 * @class Output
 * Formatted output class.
 *
 * @par Formatting Integers
 *
 * Formatting integers is maded using the class @ref IntFormat that embeds
 * the value to display and the format configuration. Then the result object
 * is passed to the @ref Output object that will perform the actual formatting
 * of the output as in the example below.
 *
 * @code
 * 	void *p;
 * 	cout << io::right(io::pad('0', io::width(32, io::hex(o)))) << io::endl;
 * @endcode
 *
 *
 *
 * @ingroup ios
 */

/**
 * Convert an integer to character using the horner method.
 * @param p			Pointer on top of buffer.
 * @param val		Integer value to convert.
 * @param base		Base of the conversion.
 * @param enc		Base character for encoding digits upper than 10.
 * @return				First character.
 */
char *Output::horner(char *p, t::uint64 val, int base, char enc) {

	// Special case of 0
	if(!val)
		*--p = '0';

	// Horner method
	else
		while(val) {

			// Compute the digit
			int digit = val % base;
			val /= base;

			// Put the character
			if(digit < 10)
				*--p = '0' + digit;
			else
				*--p = enc + digit - 10;
		}

	// Return the first character position
	return p;
}


/**
 * @fn Output::Output(void);
 * Build a formatted output on the standard output.
 */

/**
 * @fn Output::Output(OutStream& stream);
 * Build a formatted output on the given stream.
 */

/**
 * @fn OutStream& Output::stream(void) const;
 * Get the stream used by the output.
 * @return Output stream.
 */

/**
 * Print a boolean value, 'true' or 'false'.
 * @param value	Value to write.
 */
void Output::print(bool value) {
	print(value ? "true" : "false");
}

/**
 * Print a character.
 * @param chr	Character to print.
 */
void Output::print(char chr) {
	if(strm->write(chr) < 0)
		throw IOException(strm->lastErrorMessage());
}

/**
 * Print an integer.
 * @param value	Integer to print.
 * @deprecated
 */
void Output::print(t::int32 value) {
	char buffer[16];
	bool neg = false;
	t::uint32 uval;

	// Process sign
	if(value < 0) {
		neg = true;
		uval = -value;
	}
	else
		uval = value;

	// Write the digits
	char *p = horner(buffer + sizeof(buffer), uval, 10);

	// Add the sign
	if(neg)
		*--p = '-';

	// Write it
	if(strm->write(p, buffer + sizeof(buffer) - p) < 0)
		throw IOException(strm->lastErrorMessage());
}


/**
 * Print a long long integer.
 * @param value	Long long integer to print.
 * @deprecated
 */
void Output::print(t::int64 value) {
	char buffer[64];
	bool neg = false;
	t::uint64 uval;

	// Process sign
	if(value < 0) {
		neg = true;
		uval = -value;
	}
	else
		uval = value;

	// Write the digits
	char *p = horner(buffer + sizeof(buffer), uval, 10);

	// Add the sign
	if(neg)
		*--p = '-';

	// Write it
	if(strm->write(p, buffer + sizeof(buffer) - p) < 0)
		throw IOException(strm->lastErrorMessage());
}


/**
 * Print an unsigned integer.
 * @param value	Integer to print.
 * @deprecated
 */
void Output::print(t::uint32 value) {
	char buffer[16];

	// Write the digits
	char *p = horner(buffer + sizeof(buffer), value, 10);

	// Write it
	if(strm->write(p, buffer + sizeof(buffer) - p) < 0)
		throw IOException(strm->lastErrorMessage());
}


/**
 * Print an unsigned long long integer.
 * @param value	Integer to print.
 * @deprecated
 */
void Output::print(t::uint64 value) {
	char buffer[32];

	// Write the digits
	char *p = horner(buffer + sizeof(buffer), value, 10);

	// Write it
	if(strm->write(p, buffer + sizeof(buffer) - p) < 0)
		throw IOException(strm->lastErrorMessage());
}


/**
 * Print a double value.
 * @param value	Double value to print.
 */
void Output::print(double value) {
	// !!TODO!!
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "%g", value);
	if(strm->write(buffer, strlen(buffer)) < 0)
		throw IOException(strm->lastErrorMessage());
}

/**
 * Print a pointer.
 * @param value	Pointer value.
 */
void Output::print(void *value) {
	if(!value)
		print("<null>");
	else {
		char buffer[sizeof(void *) * 2];
		char *p = horner(buffer + sizeof(buffer), t::intptr(value), 16);
		while(p != buffer)
			*--p = '0';
		if(strm->write(buffer, sizeof(buffer)) < 0)
			throw IOException(strm->lastErrorMessage());
	}
}

/**
 * Print a C string.
 * @param str	C string to print.
 */
void Output::print(const CString str) {
	if(strm->write(str.chars(), str.length()) < 0)
		throw IOException(strm->lastErrorMessage());
}

/**
 * Print a string.
 * @param str	String to print.
 */
void Output::print(const String& str) {
	if(strm->write(str.chars(), str.length()) < 0)
		throw IOException(strm->lastErrorMessage());
}

/**
 * Flush the underlying stream.
 * @throw IOException	If there is a stream error.
 */
void Output::flush(void) {
	if(strm->flush() < 0)
		throw IOException(strm->lastErrorMessage());
}


/**
 * Print a formatted string a-la C prinf().
 * @param fmt	Format string.
 * @param ...	Other arguments.
 */
void Output::format(CString fmt, ...) {
	VARARG_BEGIN(args, fmt)
		format(fmt, args);
	VARARG_END
}


/**
 * Print a formatted string a-la C prinf().
 * @param fmt	Format string.
 * @param args	Other arguments.
 */
void Output::format(CString fmt, VarArg& args) {
	char buf[256];

	// Allocate remaining memory
	int size = vsnprintf(buf, sizeof(buf), &fmt, args.args());

	// It's ok
	if(size <= (int)sizeof(buf))
		strm->write(buf, size);

	// Else use a bigger one
	else {
		char newbuf[size + 1];
		size = vsnprintf(newbuf, sizeof(newbuf), &fmt, args.args());
		ASSERT(size <= (int)sizeof(newbuf));
		strm->write(newbuf, size);
	}
}


/**
 * Print a formatted long value.
 * @param fmt	Formatted value to display.
 */
void Output::print(const IntFormat& fmt) {

	// To horner
	t::uint64 uval;
	if(fmt._sign && fmt._val < 0)
		uval = -fmt._val;
	else
		uval = fmt._val;
	if(!fmt._sign && fmt._size != 8)
		uval &= (1ULL << (fmt._size * 8)) - 1;
	char buffer[33];
	char *res = horner(buffer + 32, uval, fmt._base, fmt._upper ? 'A' : 'a');
	if(fmt._sign && fmt._val < 0)
		*(--res) = '-';
        if (fmt._displaySign && fmt._val > 0)
                *(--res) = '+';
	int size = buffer + 32 - res;

	// Compute pads
	int lpad = 0, rpad = 0;
	if(fmt._width)
		switch(fmt._align) {
		case NONE:
		case LEFT:
			rpad = fmt._width - size;
			break;
		case RIGHT:
			lpad = fmt._width - size;
			break;
		case CENTER:
			lpad = (fmt._width - size) / 2;
			rpad = fmt._width - size - lpad;
			break;
		default:
			ASSERTP(0, "unknown alignment constant");
			break;
		}

	// Perform the display
	for(int i = 0; i < lpad; i++)
		strm->write(fmt._pad);
	strm->write(res, size);
	for(int i = 0; i < rpad; i++)
		strm->write(fmt._pad);
}


/**
 * Print a float with the given format.
 * @param fmt	Float format to print.
 */
void Output::print(const FloatFormat& fmt) {
	static const int maxint = 350, maxflt = 350, maxexp = 3;
	static const int initpt = maxint + 1;
	char buf[maxint + maxflt + maxexp + 1 /* . */ + 1 /* - */ + 2 /* e- */ ];
	char
		*ip = &buf[initpt],		// ip points to first integral digit
		*fp = &buf[initpt],		// fp points to last fractional digit
		*cp = 0,				// compacted pointer, e10 > 0 -> [ip, cp], e10 < 0 -> [cp, fp]
		*pp = &buf[initpt];		// position of point
	int e10 = 0;
	bool neg = false;
	int decw = fmt._decw;
	*pp = '.';

	// simple special case
	double x = fmt._val;
	if(isnan(x)) {
		ip -= 3;
		memcpy(ip, "NaN", 3);
		decw = 0;
	}
	else if(isinf(x) == 1) {
		ip -= 4;
		memcpy(ip, "+inf", 4);
		decw = 0;
	}
	else if(isinf(x) == -1) {
		ip -= 4;
		memcpy(ip, "-inf", 4);
		decw = 0;
	}
	else if(x == 0) {
		*ip = '.';
		*--ip = '0';
	}

	// non-zero case
	else {

		// take absolute value
		double x =fmt._val;
		if(x < 0) {
			x = -x;
			neg = true;
		}

		// separate integral / fractional part
		double frac, intp;
		frac = modf(x, &intp);

		// generate the integral part
		while(intp != 0) {
			double digit = modf(intp / 10, &intp);
			*--ip = int((digit + .03) * 10) + '0';
		}

		// generate the fractional part
		while(frac > 0) {
			if(fp > pp + maxflt)
				break;
			frac *= 10;
			frac = modf(frac, &intp);
			*++fp = int(intp) + '0';
		}

		// compact if possible
		if(fmt._style == FloatFormat::SHORTEST
		|| fmt._style == FloatFormat::SCIENTIFIC) {
			if(ip == pp)
				for(cp = ip + 1; *cp == '0'; cp++) e10++;
			else if(fp == pp)
				for(cp = ip - 1; *cp =='0'; cp--) e10--;
		}
	}

	// select a style for SHORTEST
	int style = fmt._style;
	if(style == FloatFormat::SHORTEST) {
		if(!cp)
			style = FloatFormat::DECIMAL;

		// no fractional width
		else if(!decw) {
			int size = log10(abs(e10)) + 1 + (e10 < 0 ? 1 : 0);
			if(e10 > 0)
				size = cp - ip + 1;
			else
				size = fp - cp;
			if(fp - ip <= size)
				style = FloatFormat::DECIMAL;
			else
				style = FloatFormat::SCIENTIFIC;
		}

		// with fractional width
		else {
			int dsize = &buf[initpt] - ip + decw;
			int ssize = log10(abs(e10)) + 1 + (e10 < 0 ? 1 : 0) + 1 + decw;
			if(dsize < ssize)
				style = FloatFormat::DECIMAL;
			else
				style = FloatFormat::SCIENTIFIC;
		}
	}

	// fix for scientific
	if(style == FloatFormat::SCIENTIFIC) {
		if(cp) {
			if(e10 > 0) {
				ip[-1] = *ip;
				pp = ip;
				*ip-- = '.';
				fp = cp;
			}
			else {
				ip = cp - 1;
				*ip = *cp;
				*cp = '.';
				pp = cp;
			}
		}
		else {
			e10 += pp - ip - 1;
			memmove(ip + 2, ip + 1, pp - ip - 1);
			pp = ip + 1;
			*pp = '.';
		}
	}

	// round according to the fractional width
	if(fp > pp + fmt._decw) {
		char *p = pp + fmt._decw + 1;
		*p += 5;
		bool cont = false;
		while(*p > '9') {
			*p-- = '0';
			if(p == pp) {
				cont = true;
				break;
			}
			(*p)++;
		}
		if(fmt._style == FloatFormat::SHORTEST)
			fp = p;
		if(cont) {
			p = pp - 1;
			(*p)++;
			while(*p > '9') {
				*p-- = '0';
				if(p < ip) {
					*--ip = '1';
					break;
				}
				(*p)++;
			}
		}
		if(fmt._style != FloatFormat::SHORTEST)
			fp = pp + fmt._decw;
	}

	// put the minus
	if(neg)
		*--ip = '-';

	// generate lacking zeroes
	if(fmt._style != FloatFormat::SHORTEST)
		while(fp < pp + decw )
			*++fp = '0';

	// generate the exponent if required
	if(e10) {
		*++fp = 'e';
		if(e10 < 0)
			*++fp = '-';
		char *ep = fp + 1;
		while(e10) {
			*++fp = '0' + e10 % 10;
			e10 /= 10;
		}
		for(char *p = fp; p > ep; p--, ep++) {
			char t = *p;
			*p = *ep;
			*ep = t;
		}
	}

	// perform the display
	int nblank = fmt._width - (fp - ip);
	char padding[max(0, nblank) + 1];
	if(fmt._width > fp - ip) {
		int size = fmt._align == CENTER ? nblank / 2 : nblank;
		for(int i = 0; i <= size; i++)
			padding[i] = fmt._pad;
		if(fmt._align != RIGHT)
			stream().write(padding, fmt._align == CENTER ? nblank / 2 : nblank);
	}
	stream().write(ip, fp - ip + 1);
	if(fmt._width > fp - ip && fmt._align != LEFT)
		stream().write(padding, fmt._align == CENTER ? nblank - nblank / 2 : nblank);
}


/**
 * Print a formatted string.
 * @param fmt	Format to print.
 */
void Output::print(const StringFormat& fmt) {
	if(!fmt._width)
		*this << fmt.s;
	else if(fmt.s.length() >= fmt._width)
		*this << fmt.s.substring(0, fmt._width);
	else {

		// compute padding size
		int b, a;
		switch(fmt._align) {
		case LEFT:
			b = 0;
			a = fmt._width - fmt.s.length();
			break;
		case CENTER:
			b = (fmt._width - fmt.s.length()) / 2;
			a = fmt._width - b - fmt.s.length();
			break;
		case RIGHT:
			b = fmt._width - fmt.s.length();
			a = 0;
			break;
		default:
			ASSERT(false);
			break;
		}

		// perform the display
		for(int i = 0; i < b; i++)
			*this << char(fmt._pad);
		*this << fmt.s;
		for(int i = 0; i < a; i++)
			*this << char(fmt._pad);
	}
}


/**
 * @class IntFormat
 * This class is used to perform formatting on integer passed to the @ref Output
 * class.
 *
 * It is rarely used as-is but with some inlines functions performing formatting:
 * @ref io::base, @ref io::bin, @ref io::hex, @ref op::width, @ref io::align,
 * @ref io::left, @ref io::center, @ref io::right, @ref io::pad,
 * @ref io::uppercase, @ref io::lowercase.
 * @ingroup ios
 */

/**
 * @var IntFormat IntFormat::_base;
 * Numeric base used to display the integer (default to 10).
 */

/**
 * @var unsigned char IntFormat::_width;
 * Width of the field where the integer will be displayed. Default 0 for
 * no field width constraint. If the displayed integer size is less than the
 * width, it will be aligned according the @ref IntFormat::align attribute
 * and padded according the @ref IntFormat::pad attribute.
 */

/**
 * @var unsigned IntFormat::_align;
 * Alignment of integer in the field. One of LEFT, CENTER or RIGHT.
 */

/**
 * @var unsigned IntFormat::_upper;
 * If true, upper case characters will be used to display integer whose base
 * is greater than 10. If false (default), lower case characters will be used.
 */

/**
 * @var unsigned IntFormat::_sign;
 * If true, ever display the sign. If false (default), only display negative
 * sign.
 */

/**
 * @var char IntFormat::_pad;
 * Character used to pad the displayed integer in the field (default '0').
 */

/**
 * @var char IntFormat::_displaySign;
 * If true, the sign of signed value is always displayed. Else only the minus is displayed when needed.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::int8	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::uint8	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::int16	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::uint16	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::int32	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::uint32	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::int64	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::operator()(t::uint64	value);
 * Instantiate the format for the given integer.
 * @param value	Integer to format.
 * @return		New integer format.
 */

/**
 * @fn IntFormat IntFormat::base(int b);
 * Set the base.
 * @param b	Base of the format.
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::bin(void);
 * Set the base to binary.
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::oct(void);
 * Set the base to octal.
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::dec(void);
 * Set the base to decimal.
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::hex(void);
 * Set the base to hexadecimal.
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::width(int w);
 * Set the width of the format.
 * @param w		Width in characters.
 * @return		Built format.
 */

/**
 * @fn IntFormat IntFormat::align(alignment_t a);
 * Set the alignment of the format.
 * @param a	Alignment (one of io::LEFT, io::CENTER or io::RIGHT).
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::left(void);
 * Set the format to left alignment.
 * @return Built format.
 */

/**
 * @fn IntFormat IntFormat::center(void);
 * Set the format to centered alignment.
 * @return Built format.
 */

/**
 * @fn IntFormat IntFormat::right(void);
 * Set the format to right alignment.
 * @return Built format.
 */

/**
 * @fn IntFormat IntFormat::upper(void);
 * Use upper case letters for base greater than 10.
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::lower(void);
 * Use lower case letters for base greater than 10.
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::sign(void);
 * Ever display the sign of the integer (even for positives).
 * @return	Built format.
 */

/**
 * @fn IntFormat IntFormat::pad(char p);
 * Set the padding character.
 * @param p	Padding characrer.
 * @return	Built format.
 */


/**
 * @fn IntFormat base(int base, IntFormat fmt);
 * Format an integer with the given base.
 * @param base	Numeric base.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat bin(IntFormat fmt);
 * Used a binary base to display an integer.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat hex(IntFormat fmt);
 * Used an hexadecimal base to display an integer.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat width(int width, IntFormat fmt);
 * Select the width of field where the integer will be displayed to.
 * @param width	Field width.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat align(alignment_t align, IntFormat fmt);
 * Used the given alignment to display the integer in its field.
 * @param align		Alignment.
 * @param fmt		Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat left(IntFormat fmt);
 * Align the integer to the left in its field.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat right(IntFormat fmt);
 * Align the integer to the right in its field.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat center(IntFormat fmt);
 * Center the integer in its field.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat pad(char pad, IntFormat fmt);
 * Select the padding character.
 * @param pad	Padding character.
 * @param fmt	Displayed integer.
  * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat uppercase(IntFormat fmt);
 * Select uppercase characters for digits bigger than 10.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */

/**
 * @fn IntFormat lowercase(IntFormat fmt);
 * Select lowercase characters for digits bigger than 10.
 * @param fmt	Displayed integer.
 * @deprecated
 * @ingroup ios
 */


/**
 * Format a pointer for display.
 * @param p		Pointer to format.
 * @return		Formatted pointer.
 */
IntFormat pointer(const void *p) {
	return f(t::intptr(p)).hex().width(sizeof(t::intptr) * 2).pad('0');
}


/**
 * Format an integer to display it as an hexadecimal byte.
 * @param b		Byte to format.
 * @return		Formatted byte.
 */
IntFormat byte(t::uint8 b) {
	return f(b).hex().width(2).pad('0').right();
}

/**
 * @class StringFormat
 * Store formatting information for character string.
 * To automatically build a string format, uses the @ref fmt() function.
 * @ingroup ios
 */

/**
 * @fn StringFormat StringFormat::width(int w);
 * Select width of the display. As a default, there is o width
 * and the full string is displayed.
 * @param w		Width in character.
 */

/**
 * @fn StringFormat StringFormat::align(alignment_t a);
 * Select alignment position when string is bigger than the selected width.
 * Default alignment value is left.
 * @param a		One of LEFT, CENTER or RIGHT.
 */

/**
 * @fn StringFormat StringFormat::left(void);
 * Align string display to left.
 */

/**
 * @fn StringFormat StringFormat::right(void);
 * Align string display to right.
 */

/**
 * @fn StringFormat StringFormat::center(void);
 * center string display.
 */

/**
 * @fn StringFormat StringFormat::pad(char p);
 * Select the padding character when a width is selected.
 * As a default, it is a space.
 * @param p		Padding character.
 */

/**
 * @class Tag
 * This convenient class allow controlling the way an object
 * is displayed, usually when the display must not conform to
 * the default display method.
 *
 * Its template parameter P must be a class defining a type t,
 * the type of the object to display and static function, print,
 * that will be called to display the object of type t.
 *
 * What does @ref Tag class is just to record an instance of t
 * and call P::print method on it at display time. This let
 * the user to specialize the way the oibject is displayed at this
 * point of the program.
 *
 * @ingroup ios
 */

} // io

/**
 * Standard output.
 * @ingroup ios
 */
io::Output cout(io::out);

/**
 * Standard error output.
 * @ingroup ios
 */
io::Output cerr(io::err);

} // elm
