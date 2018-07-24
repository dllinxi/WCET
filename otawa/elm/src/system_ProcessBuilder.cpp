/*
 *	$Id$
 *	ProcessBuilder class implementation
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

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <elm/sys/ProcessBuilder.h>
#include <elm/sys/SystemException.h>
#if defined(__WIN32) || defined(WIN64)
#	include <windows.h>
#endif

namespace elm { namespace sys {

#if defined(__unix) || defined(__APPLE__)
	Process *makeProcess(int pid);
#elif defined(__WIN32) || defined(__WIN64)
	Process *makeProcess(const PROCESS_INFORMATION& pi);
#else
#	error "Unsupported System"
#endif


/**
 * @class ProcessBuilder
 * This class is used to build a new process by launching a command line.
 * @ingroup system_inter
 */


/**
 * Construct a process builder.
 * @param command	Command to use.
 */
/*ProcessBuilder::ProcessBuilder(string command)
:	in(&io::in),
 	out(&io::out),
 	err(&io::err)
{
	args.add(command);
}*/


/**
 * Construct a process builder.
 * @param command	Command to use.
 */
ProcessBuilder::ProcessBuilder(sys::Path command)
:	in(&io::in),
 	out(&io::out),
 	err(&io::err),
	new_session(false)
{
	args.add(command);
}


/**
 * If the argument is true, the built process will run in its
 * own session and is separated from the creator process.
 * Practically, this means that the end of the parent process
 * will not cause the end of the child process.
 * @param enabled	True to get a new session, false else.
 */
void ProcessBuilder::setNewSession(bool enabled) {
	new_session = enabled;
}


/**
 * Add an argument to the command line.
 * @param argument	Argument to add.
 */	
void ProcessBuilder::addArgument(string argument) {
	args.add(argument);
}


/**
 * Set the input stream of the built process.
 * @param _in	New process input.
 */
void ProcessBuilder::setInput(SystemInStream *_in) {
	in = _in;
}


/**
 * Set the output stream of the built process.
 * @param _out	New process output.
 */
void ProcessBuilder::setOutput(SystemOutStream *_out) {
	out = _out;
}


/**
 * Set the error stream of the built process.
 * @param _out	New process error.
 */
void ProcessBuilder::setError(SystemOutStream *_err) {
	err = _err;
}


/**
 * Run the built process.
 * @return	The built process.
 * @throws SystemException	Thrown if there is an error during the build.
 */
Process *ProcessBuilder::run(void) throw(SystemException) {
#if defined(__unix) || defined(__APPLE__)
	int error = 0;
	int old_in = -1, old_out = -1, old_err = -1;

	Process *process = 0;

	// Prepare the streams
	if(in->fd() != io::in.fd()) {
		old_in = dup(0);
		if(old_in < 0)
			error = errno;
		else
			dup2(in->fd(), 0);
	}
	if(!errno && out->fd() != io::out.fd()) {
		old_out = dup(1);
		if(old_out < 0)
			error = errno;
		else
			dup2(out->fd(), 1);
	}
	if(!errno && err->fd() != io::err.fd()) {
		old_err = dup(2);
		if(old_err < 0)
			error = errno;
		else
			dup2(err->fd(), 2);
	}

	// Create the process
	if(!error) {
		int pid = fork();
		// error
		if(pid < 0)
			error = errno;
		// father
		else if(pid != 0)
			process = makeProcess(pid);
		// son
		else {

			// if needed, create new session
			if(new_session) {
				int r = setsid();
				if(r < 0) {
					cerr << "ERROR: cannot create new session!\n";
					exit(1);
				}
			}

			// build arguments
			char *tab[args.count() + 1];
			for(int i = 0; i < args.count(); i++)
				tab[i] = (char *)&args[i].toCString();
			tab[args.count()] = 0;

			// Launch the command
			execvp(tab[0], tab);
			exit(1);
		}
	}

	// Reset the streams
	if(old_in >= 0) {
		close(0);
		dup2(old_in, 0);
		close(old_in);
	}
	if(old_out >= 0) {
		close(1);
		dup2(old_out, 1);
		close(old_out);
	}
	if(old_err >= 0) {
		close(2);
		dup2(old_err, 2);
		close(old_err);
	}

#elif defined(__WIN32) || defined(__WIN64)
	// no  need to redirect output, if bInheritHandles is set to false when creating process
	// it uses standard input, output and error output

	int error = 0;
	Process *process;

	if (!error) {
		
		// build arguments
		StringBuffer tab;
		for(int i = 0; i < args.count() ; i++)
			tab << args[i] << " ";
		char tabtemp[tab.length() +1];
		strcpy(tabtemp, tab.toString().chars());

		// process information
		PROCESS_INFORMATION proc_info;
		ZeroMemory(&proc_info, sizeof(PROCESS_INFORMATION));
		
		// startup information
		STARTUPINFO start_info;
		ZeroMemory(&start_info, sizeof(STARTUPINFO));
		start_info.cb = sizeof(STARTUPINFO);
		start_info.hStdError = err->fd();
		start_info.hStdOutput = out->fd();
		start_info.hStdInput = in->fd();
		start_info.dwFlags |= STARTF_USESTDHANDLES;

		// launch process
		if(CreateProcess(
			NULL,
			tabtemp,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&start_info,
			&proc_info
		) == 0)
			error = GetLastError();
		process = makeProcess(proc_info);
	}

	// Return the result
	if(error){
		cout << "error detected " << GetLastError() << io::endl;
		throw new SystemException(error, "process building");
	}
	else
		return process;


#else
#error "System Unsupported"
#endif


// Return the result
if(error)
	throw new SystemException(error, "process building");
else
	return process;
}

} } // elm::sys
