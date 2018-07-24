/*
 *	$Id$
 *	cleaner module interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008-10, IRIT UPS.
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
#include <elm/util/Cleaner.h>

namespace elm {

/**
 * @class Cleaner
 * Cleaner is an interface shared by object needed some kind of release at some
 * program point (like the end of a program). Actually, Cleaner is used in
 * @ref CleanList to record a list of clean-up that may be invoked together.
 * @p
 * ELM provides a set of ready-to-use cleaner like:
 * @li @ref Deletor -- invoke delete on the parameter pointer,
 * @li @ref AutoCleaner -- accepts pointer as @ref AutoPtr. 
 */


/**
 * @class Deletor
 * A deletor is a cleaner that invoke delete on a passed pointer.
 * @param T		Type of the deleted object.
 */


/**
 * @fn Deletor::Deletor(T *object);
 * Build a deletor on the given object.
 * @param object	Object to delete at cleanup.
 * @seealso deletor()
 */


/**
 * @class AutoCleaner
 * A cleaner for @ref AutoPtr objects.
 * @param T	Auto-pointed object type.
 */


/**
 * @fn AutoCleaner::AutoCleaner(T *p = 0);
 * Build an AutoCleaner on the given object.
 * @param p	Pointed object.
 */
 
 
/**
 * @class AutoCleaner::AutoCleaner(const AutoPtr& locked);
 * Clone an auto-cleaner object.
 * @param locked	Cloned object.
 */


/**
 * @class CleanList
 * A CleanList is an easy way to not forget or to delegate clean-up in complex
 * program. A CleanList receives a list of @ref Cleaner object that records
 * some cleanup to perform.
 * @p
 * When the CleanList is deleted or when the method clean() is called,
 * clean-up actions of all stored object is invoked ensuring to not forget
 * clean-up to perform.
 * @p
 * @code
 * {
 * 	CleanList to_clean;
 *  int *p = to_clean(new int);
 *	...
 * }	// p deletion automatically performed at clean deletion
 * @endcode
 */
 

/**
 * Add a cleaner to the list.
 * @param cleaner	Cleaner to add.
 */
void CleanList::add(Cleaner *cleaner) {
	ASSERTP(cleaner, "null cleaner passed");
	list.add(cleaner);
}


/**
 * Clean the recorded cleaners.
 */
void CleanList::clean(void) {
	for(list_t::Iterator cleaner(list); cleaner; cleaner++) {
		cleaner->clean();
		delete cleaner;
	}
	list.clear();
}

} // elm
