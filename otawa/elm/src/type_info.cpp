/*
 *	$Id$
 *	type_info class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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

#include <float.h>
#include <elm/type_info.h>
#include <elm/rtti.h>

namespace elm {

/**
 * @defgroup types Type Support
 *
 * ELM provides several facilities to handle type and parametric types.
 *
 * @par Basic Types
 *
 * @code
 * #include <elm/types.h>
 * using namespace elm;
 * @endcode
 *
 * This definitions provide several facilities to work with integers.
 * To use, one has to include <elm/int.h>.
 *
 * This module provides reliable and OS-independent types to represent integers
 * together with a list of efficiently-implemented functions to process them.
 * These types are stored in the @c elm::t sub-namespace and, if you are
 * using the @c elm namespace, are quickly accessed by syntax @c t::type .
 *
 * @code
 * #include <elm/int.h>
 * using namespace elm;
 * ...
 * t::uint32 my_variable;
 * @endcode
 *
 * You can also use the @ref null<T>() function to build easily typed null pointer.
 *
 * @par Type Information
 *
 * @code
 * #include <elm/type_info.h>
 * using namespace elm;
 * @endcode
 *
 * Class @ref elm::type_info<T> allows to get from the type passed as the parametric argument.
 * This may be useful for templates using static evaluation. The common fields includes:
 * @li is_type -- true if T is not a class
 * @li is_scalar -- true if T is a simple type (neither a structure, nor union, nor class)
 * @li is_enum -- true if T is an enumerated type
 * @li is_class -- true if T is a class
 * @li is_ptr -- true if T is a pointer
 * @li is_ref -- true if T is a reference
 * @li is_deep -- true if T requires deep copy (that is, requires copy using operator = instead of
 * system fast byte-per-byte copy mechanism)
 * @li is_virtual -- true if T contains virtual functions
 * @li name() -- get the name of a type.
 * @li embed_t -- type T to converted to an assignable form.
 * @li put(embed_t& var, T val) -- assign value of type T to a variable of assigned type.
 * @li get(embed_t& var) -- convert back a variable in assigned type to T.
 *
 * Notice that ELM provides type information for basic (integer, float, string) and composed types (pointer, references)
 * but need the developer help for more complex types like classes, struct or unions. Either one can add the specialized
 * type_info structure for the defined type:
 * @code
 *	class MyClass { ... };
 *	template <> struct type_info<T> { static cstring name(void) { return "MyClass"; } }
 * @endcode
 *
 * Or just add a static member named "__type_name" in the class declaration:
 * @code
 *	class MyClass {
 *	public:
 *		static cstring __type_name(void) { return "MyClass"; }
 *		...
 *	};
 * @endcode
 *
 *
 * @par Type Selection Helper
 *
 * A big issue in C++ is the selection of type for parameters and functions results.
 * A common practice is to pass a simple type as is as parameter and to use const
 * references for bigger types. This policy is easy to apply when the type is known
 * but what to do when the parameter method is in a template class. Here, a conservative
 * approach advise to use const reference in any case in the hope that the compiler will
 * optimize the code and use only straight type for scalar types.
 *
 * ELM proposes an alternative to this approach that works with template. Using the
 * type_info<T> class (that is aware of the actual type of the generic parameter),
 * one can obtain the type declaration that fits the best its use:
 * @li type_info<T>::in_t -- for input parameter,
 * @li type_info<T>::out_t -- for output parameter,
 * @li type_info<T>::val_t -- for returning a value,
 * @li type_info<T>::ref_t -- for returning a reference,
 * @li type_info<T>::embed_t -- for storing internally the type.
 *
 * Notice type_info<T>::embed_t that allows storing references: in fact, when T is
 * a reference, it is stored automatically as a pointer. type_info<T>::put() and
 * type_info<T>::get() can then be used to store the generic value or to get it.
 *
 *
 * @par Enumeration Information
 *
 * Users may also benefit from whole facilities of input/output, serialization and like for
 * the enumerated type. First, the type_info must be specialized
 * (in the header containing the enumeration declaration):
 * @code
 *	typedef enum { V1, V2, V3, ... } my_enum;
 *	...
 *	namespace elm {
 *		template <> struct type_info<my_enum>: enum_info<my_enum> { };
 *	}
 * @endcode
 * And then to provided the missing (usually in the source file providing implementation).
 * @code
 *  namespace elm {
 *		template <> cstring enum_info<my_enum>::name(void) { return "my_enum"; }
 *		template <> enum_info<my_enum>::value_t enum_info<my_enum>::values[] = {
 *			value("V1", V1),
 *			value("V2", V2),
 *			value("V3", V3),
 *			last()
 *		};
 *	}
 * @endcode
 *
 * As the primitives provided by ELM, the values may examined with such a loop:
 * @code
 *	for(type_info<my_enum>::iterator i = type_info<my_enum>::begin(); i != type_info<my_enum>::end(); i++)
 *		cout << "name = " << i.name << ", value = " << i.value << io::endl;
 * @endcode
 *
 * @par Array Copying
 *
 * The include file <otawa/util/array.h> provides functions to handling arrays:
 * @li @ref array::copy() -- copy without overlapping
 * @li @ref array::move() -- copy with overlapping
 * @li @ref array::clear() -- set to initial value
 * @li @ref array::set() -- set all items to a specific values
 *
 * According to the type of the type of the items, these functions may invokes
 * specific system functions to make faster this operation. Usually, only types
 * without pointers can only be processed in this way (scalar data) but you can
 * informs that your own class can be shallowly copied by specializing the @ref type_info
 * class:
 * @code
 * class MyClass {
 * 	...
 * };
 *
 * template <>
 * class type_info<MyClass>: public class_t<T> { enum { is_deep = 1 }; };
 *
 * @endcode
 */

/**
 * @class type_info
 * This class can not be instantiated. It provides only static information
 * about the types used in a program. Although a partial type_info instantiation
 * is provided for usual types of C++, the type_info may be specialized
 * for user types in order to let ELM classes -- like containers -- to adapt
 * to the custom types.
 *
 * According the type T, one of the following information may also be available:
 * @li const T null: null value for the types supporting one.
 * @li const T min: minimum value for the types supporting a total order.
 * @li const T max: maximum value for the types supporting a total order.
 *
 * @param T		Type to get information for.
 *
 * @ingroup types
 */


/**
 * @var type_info<T>::is_type
 * True if T is a simple type (not a class).
 */


/**
 * @var type_info<T>::is_class
 * True if T is a class type.
 */


/**
 * @var type_info<T>::is_scalar
 * True if T is a scalar type (integer, float, boolean, etc).
 */


/**
 * @var type_info<T>::is_enum
 * True if T is an enumerated type.
 */


/**
 * @var type_info<T>::is_deep
 * Does not support byte-to-byte copy.
 */


/**
 * @var type_info<T>::is_virtual
 * Contains virtual functions.
 */

/**
 * @var type_info<T>::null
 * Null value for the current type.
 */


/**
 * @fn cstring type_info<T>::name(void);
 * Get the name of the type.
 * @return	Type name.
 */

// Statics
const cstring type_info<cstring>::null = "";
const string type_info<string>::null = "";


/**
 * @class AbstractClass
 *
 * Abstract class to represent classes in RTTI. It is mainly used
 * for serialization / deserialization.
 *
 * @ingroup types
 */

/**
 * Test if the current class is base class of the given one.
 * Notice that this class is a base class of clazz even if this
 * class equals the class clazz.
 * @param clazz		Class to test.
 * @return			True if this class is a base class, false else.
 */
bool AbstractClass::baseOf(AbstractClass *clazz) {
	while(clazz) {
		if(clazz == this)
			return true;
		clazz = clazz->base();
	}
	return false;
}


// type_info<integer types>
cstring type_info<signed int>::name(void) { return "int"; }
cstring type_info<unsigned int>::name(void) { return "unsigned"; }
cstring type_info<char>::name(void) { return "char"; }
cstring type_info<signed char>::name(void) { return "signed char"; }
cstring type_info<unsigned char>::name(void) { return "unsigned char"; }
cstring type_info<signed short>::name(void) { return "short"; }
cstring type_info<unsigned short>::name(void) { return "unsigned short"; }
cstring type_info<signed long>::name(void) { return "long"; }
cstring type_info<unsigned long>::name(void) { return "unsigned long"; }
cstring type_info<signed long long>::name(void) { return "long long"; }
cstring type_info<unsigned long long>::name(void) { return "unsigned long long"; }


// type_info<float>
const float type_info<float>::min = -FLT_MAX;
const float type_info<float>::max = FLT_MAX;
const float type_info<float>::null = 0;
cstring type_info<float>::name(void) { return "float"; }

// type_info<double>
const double type_info<double>::min = -DBL_MAX;
const double type_info<double>::max = DBL_MAX;
const double type_info<double>::null = 0;
cstring type_info<double>::name(void) { return "double"; }

// type_info<long double>
const long double type_info<long double>::min = -LDBL_MAX;
const long double type_info<long double>::max = LDBL_MAX;
const long double type_info<long double>::null = 0;
cstring type_info<long double>::name(void) { return "long double"; }

// type_info<string types>
cstring type_info<cstring>::name(void) { return "cstring"; }
cstring type_info<string>::name(void) { return "string"; }


/**
 * @fn  T *null(void) const;
 * Build a typed null pointer from the template type.
 *
 * @param T	Pointed object type.
 *
 * @ingroup types
 */

/**
 * @class Single
 * Ensures a unique implementation of a singleton of type T.
 * This singleton is accessible by Single<T>::_ (no need to declare and define
 * it somewhere).
 *
 * @param T		Type of singleton. Must support no-paramater constructor.
 */

}  // elm

