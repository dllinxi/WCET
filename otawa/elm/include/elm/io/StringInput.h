/*
 *	StringInput class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2012, IRIT UPS.
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
#ifndef ELM_IO_STRINGINPUT_H_
#define ELM_IO_STRINGINPUT_H_

#include <elm/io/Input.h>
#include <elm/io/BlockInStream.h>

namespace elm { namespace io {

// StringInput class
class StringInput: public Input {
public:
	inline StringInput(const char *str): Input(bin), bin(str) { }
	inline StringInput(const cstring& str): Input(bin), bin(str) { }
	inline StringInput(const string& str): Input(bin), bin(str) { }
private:
	BlockInStream bin;
};

} }		// elm::io

#endif /* ELM_IO_STRINGINPUT_H_ */
