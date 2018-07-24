/*
 *	elm::Monitor class implementation
 *	OutFileStream class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011, IRIT UPS.
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
#include <elm/io/Monitor.h>
#include <elm/sys/SystemIO.h>

namespace elm { namespace io {

/**
 * @class Monitor
 * The monitor is a standard way to provide user interface that may be adapted to
 * the current environment of an application. In its default implementation,
 * it provides access to standard input/output to communicate with the user
 * but it may be customized in any way without need of customization of classes
 * based on this class.
 */


/**
 * Default configuration monitor constructor.
 */
Monitor::Monitor(void): out(elm::io::out), err(elm::io::err), log(elm::io::err) {
}


/**
 * Cloning constructor.
 * @param monitor	Monitor to clone.
 */
Monitor::Monitor(const Monitor& monitor): out(monitor.out), err(monitor.err), log(monitor.log) {

}


/**
 */
Monitor::~Monitor(void) {
}


/**
 * Display an information to the user (on error output).
 * @param message	Message to display.
 */
void Monitor::info(const string& message) {
	err << "INFO: " << message << io::endl;
}


/**
 * Display an error to the user (on error output).
 * @param message	Message to display.
 */
void Monitor::error(const string& message) {
	err << "ERROR: " << message << io::endl;
}


/**
 * Display a warning to the user (on error output).
 * @param message	Message to display.
 */
void Monitor::warn(const string& message) {
	err << "WARNING: " << message << io::endl;
}


/**
 * Change output stream.
 * @param out	New output stream.
 */
void Monitor::setOut(OutStream& out) {
	this->out.setStream(out);
}

/**
 * Change error stream.
 * @param err	New error stream.
 */
void Monitor::setErr(OutStream& err) {
	this->err.setStream(err);
}

/**
 * Change log stream.
 * @param log	New log stream.
 */
void Monitor::setLog(OutStream& log) {
	this->log.setStream(log);
}

}	// io

io::Monitor monitor;

} // elm
