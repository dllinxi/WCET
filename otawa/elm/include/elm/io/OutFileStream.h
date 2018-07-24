/*
 *	$Id$
 *	OutFileStream class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007-8, IRIT UPS.
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
#ifndef ELM_IO_OUTFILESTREAM_H
#define ELM_IO_OUTFILESTREAM_H

#include <elm/string.h>
#include <elm/sys/SystemIO.h>
#include <elm/sys/Path.h>

namespace elm { namespace io {

// OutFileStream class
class OutFileStream: public sys::SystemOutStream {
public:
	OutFileStream(const char *path);
	OutFileStream(const Path& path);
	virtual ~OutFileStream(void);
	bool isReady(void);

	void close();
};

} } // elm::io

#endif // ELM_IO_OUTFILESTREAM_H
