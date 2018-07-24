/*
 *	$Id$
 *	Iterator class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-08, IRIT UPS.
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

#include <elm/datastruct/Iterator.h>

namespace elm {

/**
 * @class Iterator;
 * <p>This class is used for implementing the iterators. It is declared usually
 * as embedded and receive a pointer to an IteratorInst. When the iteration is
 * ended, that is, when this class is destructed, it free the matching
 * IteratorInst object.</p>
 * <pre><code>
 * for(Iterator<type_of_item *> iter(pointer_to_IteratorInst); iter; iter++)
 * 		use_of_(iter);
 * </code></pre>
 * <p>The iterator may replace any value of the type of its generic type
 * by automatic conversion.</p>
 */


/**
 * @fn Iterator::Iterator(IteratorInst<T> *_iter);
 * Build an iterator from the IteratorInst pointer (fried at iterator
 * destruction).
 * @param _iter		IteratorInst object.
 */


/**
 * @fn Iterator::~Iterator(void);
 * Free the IteratorInst passed at construction.
 */


/**
 * @fn bool Iterator::ended(void) const;
 * Return true if iteration is at end.
 * @return	True at end, false else.
 */


/**
 * @fn T Iterator::item(void) const;
 * Get the current item of iteration. It is an error to call it when ended()
 * return true.
 * @return	Current item.
 */


/**
 * @fn void Iterator::next(void);
 * Go to the next item of the iteration. It is an error to call it when ended()
 * return true.
 */


/**
 * @fn Iterator<T>& Iterator::operator++(int _);
 * As next() function but looks nicer.
 */


/**
 * @fn Iterator::operator bool(void) const;
 * Like item() function but looks nicer.
 * @note Don't use with bool as generic type, an ambiguity will appear due to
 * both automatic conversion to bool.
 */


/**
 * @fn T Iterator::operator*(void) const;
 * Like item() function but looks nicer.
 */


/**
 * @fn T Iterator::operator->(void) const;
 * Allows using the iterator as a pointer.
 */


/**
 * @fn Iterator::operator T(void) const;
 * Allows using the iterator as the current item making easier the use of the
 * iterator.
 * @note Do not use this automatic conversion when T is bool because
 * end-of-iteration expression will make an ambiguity with this form.
 */

} // elm
