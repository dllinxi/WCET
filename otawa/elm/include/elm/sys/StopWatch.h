/*
 *	StopWatch class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-12, IRIT UPS.
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
#ifndef ELM_SYS_STOP_WATCH_H
#define ELM_SYS_STOP_WATCH_H

#include <elm/types.h>

namespace elm { namespace sys {

// Time type
typedef t::uint64 time_t;

// StopWatch class
class StopWatch {
	time_t start_time, stop_time;
#if defined(__WIN32) || defined(__WIN64)
	time_t delay_time;
#endif
public:
	void start(void);
	void stop(void);
	inline time_t delay(void) const;
	inline time_t startTime(void) const;
	inline time_t stopTime(void) const;
};

// Inlines
inline time_t StopWatch::delay(void) const {
	return stop_time - start_time;
}

inline time_t StopWatch::startTime(void) const {
	return start_time;
}

inline time_t StopWatch::stopTime(void) const {
	return stop_time;
}

} } // elm::sys

#endif // ELM_SYS_STOP_WATCH_H
