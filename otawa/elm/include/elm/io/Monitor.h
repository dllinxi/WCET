/*
 *	elm::Monitor class interface
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
#ifndef ELM_IO_MONITOR_H_
#define ELM_IO_MONITOR_H_

#include <elm/io.h>

namespace elm { namespace io {

class Monitor {
public:
	Monitor(void);
	Monitor(const Monitor& monitor);
	virtual ~Monitor(void);

	virtual void info(const string& message);
	virtual void error(const string& message);
	virtual void warn(const string& message);

	Output out;
	Output err;
	Output log;

	void setOut(OutStream& out);
	void setErr(OutStream& err);
	void setLog(OutStream& log);
};

}	// io

extern io::Monitor monitor;

} // elm


#endif /* MONITOR_H_ */
