/*
 *	$Id$
 *	 class implementation
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

#include <elm/util/strong_type.h>

namespace elm {
	
/**
 * @def STRONG_TYPE(N, T)
 * As type defined with "typedef" command just create aliases to types, they
 * cannot be used to resolve overloading. To circumvent this problem, this macro
 * provide a wrapper around the defined type that is usable as the original type
 * and that provide overload facilities.
 * 
 * This class is usually replace this kind of declaration:
 * @code
 * typedef type_exp type_name;
 * @endcode
 * by :
 * @code
 * typedef StrongType<type_exp> type_name;
 * @endcode
 * 
 * @param N	Name of the new type.
 * @param T	Encapsulated type.
 * @ingroup utility
 */

} // elm

