/*
 *	$Id$
 *	Option class implementation
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

#include <elm/util/Option.h>

namespace elm {

/**
 * @class Option
 * This class is very handly to manage parameters or return value that may be
 * optional. This class records there is no value or there is a value and the
 * instance of the value.
 * @par
 * To build it, you may be easier to use the special value @ref elm::none or the
 * special function @ref elm::some() instead of the class constructors.
 * @code
 * 	if(cannot_compute)
 * 		return none;
 *	else
 * 		return some(result);
 * @endcode
 * @par
 * Please consider the conversion operator allowing to test of the availability
 * of a value by putting it straight-forward in a condition or accessing the
 * value by putting the optional value in place where the value may be used.
 * @code
 * 		Option<int> val = f();
 * 		if(val)
 * 			x += val;
 * @endcode
 * @param T	Type of the stored value.
 * @ingroup utility
 */


/**
 * 	@fn Option::Option(void);
 * Build an empty optional value recording there is no value.
 */


/**
 * @fn Option::Option(const OptionalNone& none);
 * For nternal use only.
 */


/**
 * @fn Option::Option(const T& value);
 * Build an optional value containing the given one.
 * @param value	Value to store.
 */


/**
 * @fn Option::Option(const Option<T> &opt);
 * Build an optional value by copying an existing one.
 * @param opt	Optional value to clone.
 */


/**
 * @fn bool Option::isOne(void) const;
 * Test if a value is available.
 * @return	True if a value is available, false else.
 */


/**
 * @fn bool Option::isNone(void) const;
 * Test if no value is available.
 * @return	True if no value is available, false else.
 */
 

/**
 * @fn T Option<T>::value(void) const;
 * Get the value.
 * @return The stored value.
 * @warning It is an error to call this method if no value is available.
 */


/**
 * @fn Option::operator bool(void) const;
 * @return Evaluates to true if a value is available, false else.
 */


/**
 * @fn T Option::operator*(void) const;
 * @return The stored value.
 */


/**
 * @fn Option::operator T(void) const;
 * Convert to the stored value.
 */


/**
 * @fn Option<T>& Option::operator=(const Option<T>& opt);
 * Provides assignment support.
 * @param opt	Optional value to assign.
 * @return		Current object.
 */



/**
 * This special value cause the construction of an optional value without
 * value available.
 */
const OptionalNone none = OptionalNone();


/**
 * @fn Option<T> some(const T& val);
 * This function makes easier the construction of optional value with
 * an available value.
 */
 
 
 /**
  * @fn Option<T>& Option::operator=(const T& value);
  * Assignment of a value.
  * @param value	Value to assign.
  * @return 		Current object.
  */

} // elm
