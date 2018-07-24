/*
 *	Process class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-12, IRIT UPS.
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
#ifndef ELM_SYS_PROCESS_H
#define ELM_SYS_PROCESS_H

namespace elm { namespace sys {

// Process class
class Process {
public:
	virtual ~Process(void);
	virtual bool isAlive(void) = 0;
	virtual int returnCode(void) = 0;
	virtual void kill(void) = 0;
	virtual void wait(void) = 0;
};

} } // elm::system

#endif // ELM_SYS_PROCESS_H
