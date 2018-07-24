/*
 *	$Id$
 *	Ref class implementation
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

#include <elm/util/Ref.h>

namespace elm {

/**
 * @class Ref
 * This class allows handling pointer as references but they provide
 * assignment operators.
 * @param T	Pointe type.
 * @ingroup utility
 */ 


/**
 * @fn Ref::Ref(T *_ptr);
 * Build a new reference.
 * @param _ptr	Pointer to store in.
 */


/**
 * @fn Ref::Ref(const Ref<T>& ref);
 * Cloning constructor.
 * @param ref	Cloned reference.
 */


/**
 * @fn Ref::operator T& (void) const;
 * Automatic conversion to reference.
 * @return	Reference on pointed object.
 */


/**
 * @fn T *Ref::operator&(void) const;
 * Get the stored pointer.
 * @return Stored pointer.
 */


/**
 * @fn Ref& Ref::operator=(T *_ptr);
 * Assignment with a pointer.
 * @param _ptr	Assigned pointer.
 * @return		Current object.
 */


/**
 * @fn Ref& Ref::operator=(const Ref<T>& ref);
 * Assignment with a reference.
 * @param ref	Assigned reference.
 * @return 		Current object.
 */


/**
 * @fn T *Ref::operator->(void) const;
 * Only way to perform unreference with structure and objects.
 * @return	Pointer on object.
 */


/**
 * @fn Ref<T>& Ref::operator=(const T& val);
 * Assignment of the pointed value.
 * @param val	Assigned value.
 * @return		Current object.
 */

} // elm
