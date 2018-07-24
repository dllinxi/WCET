/*
 * $Id$
 * Copyright (c) 2004-06, IRIT - UPS.
 *
 * elm/io/OutStream.h -- interface for OutStream class.
 */
#ifndef ELM_IO_UNIX_OUTSTREAM_H
#define ELM_IO_UNIX_OUTSTREAM_H

#include <elm/string/CString.h>
#include <elm/io/OutStream.h>

namespace elm { namespace io {

// WinOutStream class
class WinOutStream: public OutStream {
protected:
	void* _fd;
public:
	WinOutStream(void* fd);
	inline void* fd(void) const { return _fd; };
	virtual int write(const char *buffer, int size);
	virtual int flush(void);
	virtual CString lastErrorMessage(void);
};

} } // elm::io

#endif	// ELM_IO_UNIX_OUTSTREAM_H
