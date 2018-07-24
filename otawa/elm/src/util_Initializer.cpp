/*
 *	$Id$
 *	Initializer class implementation
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

#include <elm/util/Initializer.h>

namespace elm {

/**
 * @class Initializer
 * This class is a partial solution to the problem of the initialization
 * order of static variables at the start time of a program in C++.
 * This class solve the following problem at initialization. A set of
 * static object embedded in different sources shares a common datastructure
 * whose initialization time may be happen after some of these objects.
 * @par
 * Whatever, the static object initialization is split in two phases.
 * First, the object initialize itself but not the part using the shared data.
 * Instead, they record themselves to the initializer that will, when the shared
 * data will be initialized, call the "void initialize(void)" method of each
 * recorded object ensuring final initialization after the shared data item.
 * @par
 * To achieve this goal, either the shared data item must inherit from the
 * Initializer class or an Initializer static instance must be declared after
 * the shared data item.
 * @par
 * This class has very small footprint on the memory and, except the Initializer
 * object itself, all memory used will be returned to the system after the
 * initialization stage.
 * 
 * @param T	Type of the objects to give an order to. Must contain a method named initialize()
 * that is called to perform the real initialization of the object.
 * @ingroup utility
 */


/**
 * @fn Initializer::Initializer(bool start);
 * Simple constructor.
 * @param start	If set to true, the initialization takes place at construction
 * 				else the initialization is delayed until the @ref startup()
 * 				method call from an external source.
 */


/**
 * @fn void Initializer::record(T *object);
 * Record the given object to be initialized after the shared data item.
 * @param object	Object to initialize.
 */


/**
 * @fn void Initializer::startup(void);
 * Cause the initializer to start to initializer recorded objects.
 */

} // elm
