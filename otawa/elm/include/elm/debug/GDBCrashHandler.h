/*
 * $Id$
 * Copyright (c) 2007, IRIT - UPS
 *
 * GDBCrashHandler class interface
 */
#ifndef ELM_DEBUG_GDB_CRASH_HANDLER_H
#define ELM_DEBUG_GDB_CRASH_HANDLER_H

#include <elm/debug/CrashHandler.h>

namespace elm {
	
// GDBCrashHandler class
class GDBCrashHandler: public CrashHandler {
public:
	static GDBCrashHandler DEFAULT;

protected:
	virtual void handle(void);

private:
	void fatal(const char *msg);
};

} // elm

#endif // ELM_DEBUG_GDB_CRASH_HANDLER_H
