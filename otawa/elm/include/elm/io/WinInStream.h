/*
 * $Id$
 *
 * elm/io/InStream.h -- interface for InStream class.
 */
#ifndef ELM_IO_UNIX_INSTREAM_H
#define ELM_IO_UNIX_INSTREAM_H

#include <elm/string/CString.h>
#include <elm/io/InStream.h>

namespace elm { namespace io {

// WinInStream class
class WinInStream: public InStream {
protected:
	void* _fd;
public:
	WinInStream(void* fd);
	inline void* fd(void) const { return _fd; };
	virtual int read(void *buffer, int size);
	virtual int read(void) { return InStream::read(); };
	virtual CString lastErrorMessage(void);
};

} } // elm::io

#endif	// ELM_IO_UNIX_INSTREAM_H
