/*
 * $Id$
 * Copyright (c) 2004-07, IRIT - UPS
 *
 * Debugging macros
 */
#ifndef ELM_DEBUG_H
#define ELM_DEBUG_H

#include <elm/io.h>

namespace elm {

// Debugging macros
#ifdef NDEBUG
#	define TRACE
#	define BREAK(cond)
#	define HERE
#	define SPY(c, v)	(v)
#else
#	define TRACE elm::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#	define BREAK(cond)	if(cond)
#	define HERE { int x = 0; };
#	define SPY(c, v)	elm_spy(c, v)
#endif

// Inlines
inline void trace(CString file, int line, CString fun) {
	elm::cerr << "TRACE(" << file << ':' << line << ":" << fun << ")\n";
	elm::cerr.flush();
}

template <class T>
inline T elm_spy(cstring comment, T value) {
	cerr << "DEBUG: " << comment << ": " << value << io::endl;
	return value;
}

}	// elm

#endif	// ELM_DEBUG_H
