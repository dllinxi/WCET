/*
 * $Id$
 * Copyright (c) 2007, IRIT - UPS
 *
 * Debugging classes and functions
 */

#if defined(__WIN32) || defined(__WIN64) || defined(__APPLE__)
#	include <stdlib.h>
#else
#	include <elm/debug/CrashHandler.h>
#endif

namespace elm {

/**
 * @degroup debug	Debugging Support
 *
 * ELM provides several facilities to debug your own application.
 *
 * @p crash Automatic Crash Support
 *
 * As it is a tedious task to restart a program from a debugger after a crash,
 * ELM provides @ref CrashHandler class to associate an action with such an event.
 *
 * Basically, the behavior of this handler depends on the configuration of the environment.
 * Two environment variable are taken in account :
 * @li if ELM_DEBUG=yes, the handler is invoked when an error signal is sent,
 * @li if ELM_INT=yes, the handler is invoked when the program is interrupted (usually by Control-C on Unix platform).
 *
 * The used handler depends on the configuration of the ELM library. The handler to use is defined
 * by the CRASH_HANDLER definition variable and may be changed in the config.h file at the source root
 * of ELM. As a defaut, the @ref GDBCrashHandler::DEFAULT handler is used on Unix systems. For the time being,
 * there is no other crash handler but the user may provide its own crash handler by calling the method
 * @ref CrashHandler::set().
 *
 * The @ref GDBCrashHandler invokes the GDB debugger on the current process when a crash arises.
 *
 * @p debug_helper	Debug Helper Macros
 *
 * These macros help obtaining log during debugging phase. They are all de-activated as soon as the macro
 * NDEBUG is defined (as for standard C assert macro).
 * @li TRACE -- print current source file, line and container function.
 * @li HERE -- put an empty instruction making easier for the debugger to mark break lines.
 * @li BREAK(c) -- if condition c evaluates to false, executes the following code (HERE for example to get conditional breakpoint).
 * @li SPY(c, v) -- displays string comment followed by the given value and returns this one. Useful to spy values in an expression.
 */

/**
 * This function cause a crash of the program. According the system, it may
 * cause a core dump, GDB invocation or any other debugging solution.
 */
void crash(void) {
#	if defined(__WIN32) || defined(__WIN64) || defined(__APPLE__)
		abort();
#	else
		CrashHandler::crash();
#	endif
}

} // elm
