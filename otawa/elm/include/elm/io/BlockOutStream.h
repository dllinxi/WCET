/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS <casse@irit.fr>
 *
 * elm/io/BlockOutStream.h -- BlockOutStream class interface.
 */
#ifndef ELM_IO_BLOCK_OUT_STREAM_H
#define ELM_IO_BLOCK_OUT_STREAM_H

#include <elm/io/OutStream.h>
#include <elm/block/DynBlock.h>
#include <elm/string/CString.h>
#include <elm/string/String.h>

namespace elm { namespace io {

// BlockOutStream class
class BlockOutStream: public OutStream {
public:
	inline BlockOutStream(int size = 4096, int inc = 256): _block(size, inc) { }
	inline const char *block(void) const { return _block.base(); }
	inline int size(void) const { return _block.size(); }
	inline char *detach(void) { return _block.detach(); }
	inline void clear(void) { _block.reset(); }
	inline void setSize(int size) { _block.setSize(size); }
	CString toCString(void);
	String toString(void);
	
	// OutStream overload
	virtual int write(const char *buffer, int size);
	virtual int write(char byte);
	virtual int flush(void);

private:
	block::DynBlock _block;
};

} } // elm::io

#endif	// ELM_IO_BLOCK_OUT_STREAM_H
