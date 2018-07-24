/*
 *	$Id$
 *	AutoString and InputString class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007-09, IRIT UPS.
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

#include <elm/string/AutoString.h>
#include <elm/io/BlockInStream.h>

namespace elm {

/**
 * Refer to @ref elm::_.
 * @ingroup string
 */
AutoStringStartup autostr;


/**
 * This little object make easier the construction of strings. There is no more
 * to explictely construct a string buffer, concatenates items and buid the
 * string to pass it to a parameter. Just perform the concatenation on the
 * autostr object and all work is done automatically as in the example
 * below.
 * 
 * @code
 *	void call(const String& message);
 * 
 *	for(int i = 0; i < 100; i++)
 *		call(autostr << "Hello world, " << i << " times !");
 * @endcode
 * 
 * As the @ref autostr keyword is a bit long, you may replaced it by "_".
 * 
 * @code
 *	for(int i = 0; i < 100; i++)
 *		call(_ << "Hello world, " << i << " times !");
 * @endcode
 * 
 * @ingroup string
 */
AutoStringStartup &_ = autostr;


/**
 * @class InputString
 * This class allows to quicly read formatted information from string or C string.
 * The syntax is very easy and looks like the IO syntax for formatted input:
 * @code
 * 	int x;
 * 	string str = "123";
 * 	str >> x;
 *
 *	cstring s2 = "123 456";
 *	int x, y;
 *	s2 >> x >> " " >> y;
 * @endcode
 * @ingroup string
 */


/**
 */
/*StringInput::StringInput(const string& str) {
	in.setStream(*(new io::BlockInStream(str)));
}*/


/**
 */
/*StringInput::StringInput(cstring str) {
	in.setStream(*(new io::BlockInStream(str)));
}*/


/**
 */
/*StringInput::StringInput(const char *str) {
	in.setStream(*(new io::BlockInStream(str)));
}*/


/**
 */
/*StringInput::~StringInput(void) {
	if(&in.stream() != &io::in) delete &in.stream();
}*/

} // elm
