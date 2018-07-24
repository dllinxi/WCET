/*
 *	$Id$
 *	xom::Elements class interface
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

#include <elm/assert.h>
#include <elm/xom/Elements.h>
#include <elm/xom/Element.h>
#include "xom_macros.h"

namespace elm { namespace xom {

/**
 * @class Elements
 * A read-only list of elements for traversal purposes. Changes to the document
 * from which this list was generated are not reflected in this list. Changes to
 * the individual Element objects in the list are reflected.
 * @ingroup xom
 */


/**
 * @fn Element *Elements::get(int index) const;
 * Returns the indexth element in the list. The first element has index 0. The
 * last element has index size() - 1.
 * @param index the element to return 
 * @return 		the element at the specified position.
 */


/**
 * @fn int Elements::size(void) const;
 * Returns the number of elements in the list. This is guaranteed non-negative.
 * @return the number of elements in the list.
 */


/**
 * @fn Element *Elements::operator[](int index) const;
 * Shortcut to @ref get().
 */

} } // elm::xom
