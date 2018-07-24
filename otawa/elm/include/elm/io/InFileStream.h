/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * elm/io/InFileStream.h -- interface for InFilestream class.
 */
#ifndef ELM_IO_INFILESTREAM_H
#define ELM_IO_INFILESTREAM_H

#include <elm/string.h>
#include <elm/sys/SystemIO.h>

namespace elm { namespace io {

// InFileStream class
class InFileStream: public sys::SystemInStream {
public:
	InFileStream(CString path);
	virtual ~InFileStream(void);
	bool isReady(void);
	void close();
};

} } // elm::io

#endif // ELM_IO_INFILESTREAM_H
