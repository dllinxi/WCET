/*
 *	$Id$
 *	Process class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <elm/assert.h>
#include <sys/types.h>
#if defined(__unix) || defined(__APPLE__)
#include <sys/errno.h>
#include <sys/wait.h>
#elif defined(__MINGW__) || defined(__MINGW32__)
#include <errno.h>
#endif
#if defined(__WIN32) || defined(__WIN64)
#	include <windows.h>
#	undef min
#	undef max
#endif
#include <signal.h>
#include <elm/sys/Process.h>
#include <elm/sys/SystemException.h>

#include <stdio.h>

namespace elm { namespace sys {

/**
 * @class Process
 * An interface with the processes of the OS.
 * @ingroup system_inter
 */


/**
 */
Process::~Process(void) {
}


/**
 * @fn bool Process::isAlive(void);
 * Test if the current process is still alive.
 * @return	True if it is alive, false else.
 * @throws SystemException	On process system error.
 */


/**
 * @fn int Process::returnCode(void);
 * If the process is terminated, get the return code.
 * @return	Process termination return code.
 * @throws SystemException	On process system error.
 */


/**
 * @fn void Process::kill(void);
 * Kill the given thread.
 * @throws SystemException	On process system error.
 */


/**
 * @fn void Process::wait(void);
 * Wait the termination of the process.
 */


/*** Unix Process Implementation ***/
#if defined(__unix) || defined(__APPLE__)

	class ActualProcess: public Process {
	public:
		ActualProcess(int _pid): pid(_pid) {
			ASSERTP(pid >= 0, "negative PID");
		}

		virtual bool isAlive(void) {
			if(pid < 0)
				return false;
			int result = waitpid(pid, &rcode, WNOHANG);
			if(result == 0)
				return true;
			else if(result > 0) {
				pid = -1;
				rcode = WEXITSTATUS(rcode);
				return false;
			}
			else
				throw SystemException(errno, "process information");
		}

		virtual int returnCode(void) {
			if(pid >= 0)
				wait();
			return rcode;
		}

		virtual void kill(void) {
			ASSERT(pid >= 0);
			if(::kill(pid, SIGKILL) < 0)
				throw SystemException(errno, "process kill");
		}

		virtual void wait(void) {
			if(pid < 0)
				return;
			if(waitpid(pid, &rcode, 0) >= 0) {
				rcode = WEXITSTATUS(rcode);
				pid = -1;
				return;
			}
			else
				throw new SystemException(errno, "process wait");
		}

	private:
		int pid, rcode;
	};

	Process *makeProcess(int pid) { return new ActualProcess(pid); }


/*** Windows Process Implementation ***/
#elif defined(__WIN32) || defined(__WIN64)
	}

	namespace win {
		void setError(int code);
		int getError(void);
		string getErrorMessage(void);
	}

	namespace sys {


	class ActualProcess: public Process {
	public:
		ActualProcess(const PROCESS_INFORMATION& _pi): ended(false), rcode(0) {
			pi = _pi;
		}

		virtual bool isAlive(void) {
			if(GetExitCodeProcess(pi.hProcess, &rcode ) != 0){
				if(rcode != STILL_ACTIVE)
					return false;
				else
					return true;
			}
			else {
				win::setError(GetLastError());
				throw SystemException(win::getError(), win::getErrorMessage());
			}
		}

		int returnCode(void) {
			if(ended)
				return rcode;
			else if(GetExitCodeProcess(pi.hProcess, &rcode) != 0){
				wait();
				return rcode;
			}
			else {
				win::setError(GetLastError());
				throw SystemException(win::getError(), win::getErrorMessage());
			}
		}

		void kill(void) {
			if(::TerminateProcess(pi.hProcess, 0) == 0) {
				win::setError(GetLastError());
				throw SystemException(win::getError(), win::getErrorMessage());
			}
		}

		void wait(void) {
			if(GetExitCodeProcess(pi.hProcess, &rcode) == 0 && rcode != STILL_ACTIVE) {
				ended = true;
				return;
			}
			else if(WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_FAILED){
				GetExitCodeProcess(pi.hProcess, &rcode);
				ended = true;
				return;
			}
			else {
				win::setError(GetLastError());
				throw new SystemException(win::getError(), win::getErrorMessage());
			}
		}

	private:
		PROCESS_INFORMATION pi;
		bool ended;
		unsigned long  rcode;
	};

	Process *makeProcess(const PROCESS_INFORMATION& pi) { return new ActualProcess(pi); }

#else
#	error "Unsupported System"
#endif

} } // elm::sys
