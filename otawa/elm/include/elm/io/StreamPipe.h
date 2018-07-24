/*
 *	io::StreamPipe class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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

#ifndef ELM_IO_STREAMPIPE_H_
#define ELM_IO_STREAMPIPE_H_

#include <elm/string.h>

namespace elm { namespace io {

class InStream;
class OutStream;

class StreamPipe {
public:
	StreamPipe(InStream& in, OutStream& out, int buffer_size = 1 << 16);
	~StreamPipe(void);
	int proceed(void);
	string lastErrorMessage(void) const;
private:
	InStream& _in;
	OutStream& _out;
	enum {
		OK = 0,
		IN_ERROR,
		OUT_ERROR
	} state;
	int bufs;
	char *buf;
};

} }	// elm::io

#endif /* ELM_IO_STREAMPIPE_H_ */
