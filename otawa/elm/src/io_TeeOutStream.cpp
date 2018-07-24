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

#include <elm/io/TeeOutStream.h>

namespace elm { namespace io {

/**
 * @class TeeOutStream: public OutStream
 * This class allows to divert the byte stream to two different outputs.
 * This may be useful to perform an output while performing a parallel
 * processing of the streamed data: size computation, checksumming, etc.
 */

/**
 */
TeeOutStream::TeeOutStream(OutStream& out1, OutStream& out2)
: _out1(out1), _out2(out2), state(OK) {
}

/**
 */
int TeeOutStream::write (const char *buffer, int size) {
	int r = _out1.write(buffer, size);
	if(r < 0) {
		state = OUT1_ERROR;
		return -1;
	}
	r = _out2.write(buffer, size);
	if(r < 0) {
		state = OUT2_ERROR;
		return -1;
	}
	return r;
}


/**
 */
int TeeOutStream::flush(void) {
	int r = _out1.flush();
	if(r < 0) {
		state = OUT1_ERROR;
		return -1;
	}
	r = _out2.flush();
	if(r < 0) {
		state = OUT2_ERROR;
		return -1;
	}
	return r;
}


/**
 */
cstring TeeOutStream::lastErrorMessage(void) {
	switch(state) {
	case OUT1_ERROR:	return _out1.lastErrorMessage();
	case OUT2_ERROR:	return _out2.lastErrorMessage();
	default:			return "success";
	}
}

} }	// elm::io
