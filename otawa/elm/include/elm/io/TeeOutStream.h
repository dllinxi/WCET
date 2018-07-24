/*
 *	io::TeeOutStream class interface
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

#ifndef ELM_IO_TEEOUTSTREAM_H_
#define ELM_IO_TEEOUTSTREAM_H_

#include <elm/io/OutStream.h>

namespace elm { namespace io {

class TeeOutStream: public OutStream {
public:
	TeeOutStream(OutStream& out1, OutStream& out2);
	virtual int write (const char *buffer, int size);
	virtual int flush (void);
	virtual cstring lastErrorMessage(void);
private:
	OutStream& _out1;
	OutStream& _out2;
	enum {
		OK = 0,
		OUT1_ERROR,
		OUT2_ERROR
	} state;
};

} }	// elm::io

#endif /* TEEOUTSTREAM_H_ */
