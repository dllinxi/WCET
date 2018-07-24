/*
 * $Id$
 * Copyright (c) 2005-06, IRIT - UPS.
 *
 * elm/io/InStream.h -- interface for InStream class.
 */
#ifndef ELM_IO_UNIX_INSTREAM_H
#define ELM_IO_UNIX_INSTREAM_H

#include <elm/string/CString.h>
#include <elm/io/InStream.h>

namespace elm { namespace io {

// UnixInStream class
class UnixInStream: public InStream {
protected:
	int _fd;
public:
	inline UnixInStream(int fd): _fd(fd) { };
	inline int fd(void) const { return _fd; };
	virtual int read(void *buffer, int size);
	virtual int read(void) { return InStream::read(); };
	virtual CString lastErrorMessage(void);
};

} } // elm::io

#endif	// ELM_IO_UNIX_INSTREAM_H
