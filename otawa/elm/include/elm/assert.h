/*
 *	$Id$
 *	assert module interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007-09, IRIT UPS.
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
#ifndef ELM_ASSERT_H
#define ELM_ASSERT_H

// Useful functions
namespace elm {

#ifdef __GNUC__
	void crash(void) __attribute__ ((noreturn));
#else
	void crash(void);
#endif


} // elm

// Macros
#ifndef NDEBUG
#	include <elm/io.h>
#	define ELM_ASSERT_MSG(msg) \
		elm::cerr << "ASSERT:" << __FILE__ << ":" << __LINE__ << ": " << msg << elm::io::endl
#	define ELM_ASSERT(cond) \
		{ if(!(cond)) { ELM_ASSERT_MSG("assert failure: " #cond); elm::crash(); } }
#	define ELM_ASSERTP(cond, msg) \
		{ if(!(cond)) { ELM_ASSERT_MSG(msg); elm::crash(); } }
#else
#	define ELM_ASSERT(cond)	;
#	define ELM_CHECK(cond) ;
// I added that, seems like it was missing
#   define ELM_ASSERTP(cond, msg);
#endif

// Macros without prefix
#ifndef ELM_NO_ASSERT_SHORTCUT
#	define ASSERT(cond) ELM_ASSERT(cond)
#	define ASSERTP(cond, msg) ELM_ASSERTP(cond, msg)
#endif

#endif // ELM_ASSERT_H
