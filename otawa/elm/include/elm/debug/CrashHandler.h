/*
 * $Id$
 * Copyright (c) 2007, IRIT - UPS
 *
 * CrashHandler class interface
 */
#ifndef ELM_DEBUG_CRASH_HANDLER_H
#define ELM_DEBUG_CRASH_HANDLER_H

#include <elm/types.h>

namespace elm {
	
// CrashHandler class
class CrashHandler {
public:
	static const t::uint32
		DEBUG = 0x01,
		INT = 0x02;
	typedef t::uint32 mode_t;

	static void set(CrashHandler *handler, mode_t mode);
	static inline CrashHandler *get(void) { return current_handler; }
	static inline mode_t mode(void) { return _mode; }
	static void crash(void);
	static CrashHandler DEFAULT;

	virtual void setup(void);
	virtual void handle(void);
	virtual void cleanup(void);

protected:
	inline CrashHandler(void) { }
	virtual ~CrashHandler(void) { }

private:
	static mode_t _mode;
	static CrashHandler *current_handler;
};

// NoCrashHandler class
class NoCrashHandler: public CrashHandler {
public:
	virtual void setup(void) { }
	virtual void handle(void) { }
	virtual void cleanup(void) { }
};

} // elm

#endif // ELM_DEBUG_CRASH_HANDLER_H
