/*
 *	$Id$
 *	VarArg class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
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

#include <elm/util/VarArg.h>

namespace elm {

/**
 * @class VarArg
 * This is a wrapper around the <stdarg.h> macros. This class may be used:
 * - as a wrapper around stdarg (use VARARG_BEGIN and VARARG_END macros),
 * - for avoiding ambiguity in overload between VarArg (va_list) and "..."
 * ellipsis.
 * @ingroup utility
 */


/**
 * @fn VarArg::VarArg(va_list& args);
 * Constructor from an existing va_list.
 * @param args	Existing va_list.
 */


/**
 * @fn VarArg::VarArg(const VarArg& args);
 * Cloning constructor.
 * @param args	VarArg to clone.
 */


/**
 * @fn T VarArg::next(void);
 * Get the next item in the variable argument list.
 * @return	Next item.
 */


/**
 * @fn va_list& VarArg::args(void);
 * Get the argument list.
 * @return	Argument list.
 */

} // elm
