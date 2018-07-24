/*
 *	$Id$
 *	IOException class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-07, IRIT UPS.
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

#include <string.h>
#include <elm/io/io.h>

namespace elm { namespace io {

/**
 * @defgroup ios Input / Output System
 * 
 * The input/output of ELM is a merge of the C++ standard library system and of
 * the Java standard library. From the former, it uses the "<<" and ">>"
 * operators overload. From the latter, it implements the two IO access:
 * low-level classes manage streams of bytes and the high-level provides
 * formatting facilities.
 * 
 * @section high_level High-Level Input / Output
 * 
 * This level provides formatted input/output as in C++ standard library, that
 * is, through the "<<" and ">>" operators. The formatted output class is
 * @ref Output and the formatted input class is @ref Input.
 * 
 * Three global variables give access to the standard input/output:
 * @li cin -- standard input,
 * @li cout -- standard output,
 * @li cerr -- standard error.
 * 
 * Most basic types are supported (including @ref String and @ref CString).
 * Some constants are provided for special output:
 * @li @ref elm::io::endl -- new line.
 * 
 * Unlike the C++ standard library, the output format is managed through special
 * formatting objects. For integer, the work is performed by @ref IntFormat
 * that provides display features like representation base, field width,
 * alignment, uppercase letters for base greater than 10, signess, padding.
 * For float values, it exists the class @ref FloatFormat and for strings,
 * @ref StringFormat can be used.
 *
 * This format objects may easily be built using @ref io::fmt() primitives
 * and using internal constructors as in the example below:
 * @code
 * t::uint32 x;
 * cout << fmt(x).width(8).hex().right().width(8).pad('0');
 * @endcode
 *
 * A format can be built and used latter:
 * @code
 * IntFormat address_fmt = IntFormat().width(8).hex().right().width(8).pad('0');
 *
 * cout << address_fmt(x);
 * @endcode
 *
 *
 * The errors are managed here using exception objects derived from the
 * @ref IOException class.
 *
 *
 *
 * @section int_format Formatting
 *
 * The class @ref IntFormat embeds the value of an integer and
 * its format configuration. Such is usually passed to @ref Output object
 * and is built from a list of usual inline functions (described below):
 *
 * @code
 * void *p;
 * cout << pointer(p) << io::endl;
 * @endcode
 *
 * The basic to display a formatted integer is to use a constructor inline
 * function as io::f(). This builds an @ref IntFormat object to display
 * the passed integer. The format can then be modified by calling specific
 * functions as listed below:
 * @li IntFormat::base() -- select a specific base,
 * @li IntFormat::bin() -- binary base,
 * @li IntFormat::dec() -- decimal base,
 * @li IntFormat::hex() -- hexadecimal base,
 * @li IntFormat::width() -- field with (in characters),
 * @li IntFormat::align() -- alignment (one of io::LEFT, io::CENTER or io::RIGHT),
 * @li IntFormat::left -- left alignment,
 * @li IntFormat::center -- centered alignment,
 * @li IntFormat::right -- right alignment,
 * @li IntFormat::pad -- padding character (if the number does not occupy the full width),
 * @li IntFormat::uppercase -- uppercase character for digits greater than 10,
 * @li IntFormat::lowercase -- lowercase character for digits greater than 10.
 * 
 * One way to implement the pointer of the above example is to define an inline function as this:
 * @code
 * inline IntFormat pointer(void *p) {
 * 	return io::f(t::uint64(p)).width(16).pad('0').right().hex();
 * }
 * @endcode
 *
 * Notice the function io::f() that is a shortcut to create an @ref IntFormat object.
 *
 * Another way to define this format is to declare an IntFormat variable and to benefit
 * from the overlaod of operator() of this class to pass the actual value to the format:
 * @code
 * 	IntFormat pointer = IntFormat().width(16).pad('0').right().hex();
 * @endcode
 *
 * A last facility to specialize the way a data is formatted is the use of @ref Tag
 * class. This template class takes a class as parameter that will be called
 * to perform the actual display. Using tags, it is possible to specialize
 * the display of particular object inside the usual flow of "<<" operators.
 * The example below displays a string escaping the special characters "<", ">"
 * and "&" in order, for example, to output HTML text:
 * <code c++>
 * class Escape {
 * public:
 * 		typedef string t;
 * 		static void print(io::Output& out, t s) {
 * 			for(int i = 0; i < s.length(); i++)
 * 				switch(s[i]) {
 * 				case '>':	out << "&gt;"; break;
 * 				case '<':	out << "&lt;"; break;
 * 				case '&':	out << "&amp;"; break;
 * 				default:	out << s[i]; break;
 * 				}
 * 		}
 * };
 *
 * string my_string;
 * cout << Tag<Escape>(my_string) << io::endl;
 * </code>
 * 
 * @section low_level Byte Streams
 * 
 * The low-level IO system is only responsible for exchanging streams of bytes.
 * 
 * All input streams must inherit from the @ref InStream class and defines the
 * following functions:
 * @li read(buffer) -- read some bytes into the given buffer,
 * @li read() -- read one byte.
 * ELM provides byte input streams for files (@ref InFileStream), for memory
 * blocks (@ref BlockInStream) or system pipes (@ref PipeInStream).
 * 
 * The errors are returned by the called functions. Either a positive value,
 * or a value of @ref InStream::FAILED (error on the media), or
 * @ref InStream::ENDED (end of the media reached). Information about the
 * errors may be obtained by the lastErrorMessage() method.
 * 
 * All output streams must inherit from the @ref OutStream class and defines the
 * following functions:
 * @li write(buffer) -- write some bytes from the given buffer,
 * @li write() -- write one byte,
 * @li @ref flush() -- ensures that all written btes have been transferred to
 * the media.
 * ELM provides byte output streams for files (@ref OutFileStream), for memory
 * blocks (@ref BlockOutStream) or system pipes (@ref PipeOutStream).
 * 
 * The errors are returned as negative value by this functions. Information
 * about the errors may be obtained by the lastErrorMessage() method.
 */


/**
 * @class IOException
 * This exception is thrown when an IO error occurs.
 * @ingroup ios
 */


/**
 *@fn  IOException::IOException(CString message, ...);
 * Build a new IOException with the given message.
 * @param message	Message formatted a-la printf.
 * @param ...		Arguments used in the message.
 */


/**
 *@fn  IOException::IOException(CString message, VarArg& args);
 * Build a new IOException with the given message.
 * @param message	Message formatted a-la printf.
 * @param args		Arguments used in the message.
 */

} } // elm::io
