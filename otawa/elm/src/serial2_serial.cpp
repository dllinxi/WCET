/*
 *	serial module implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006, IRIT UPS.
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
#include <elm/serial2/serial.h>
#include <elm/genstruct/HashTable.h>
#include <elm/util/Initializer.h>

using namespace elm::genstruct;

namespace elm { namespace serial2 {


/**
 * @defgroup serial Serialization / unserialization
 *
 * ELM provides support for pseudo-automatic serialization / unserialization
 * of objects.
 *
 * The following C++ features are supported in serialization:
 * @li scalar types (integer, floats)
 * @li atomic types (strings),
 * @li class type with inheritance,
 * @li pointer and reference types with circularity detection,
 * @li array types,
 * @li ELM collection types,
 * @li enumeration with user support for value names.
 *
 * In addition, the user can provides its own custom serialization code in
 * a serializer / unserializer independent format.
 *
 * @par A simple example
 *
 * The serialization or the unserialization is a very simple to perform using
 * the same interface as usual C++ input/output.
 *
 * @code
 * #include <elm/serial2/serial.h>
 * #include <elm/serial2/TextSerializer.h>
 * #include <elm/serial2/collections.h>
 *
 * using namespace elm;
 *
 * int main(void) {
 *	serial2::TextSerializer serializer;
 *	serializer << "Hello, World !\n" << 666 << true;
 *
 * 	MyClass my_object;
 *	serializer << my_object;
 *
 *	Vector<MyClass> objects;
 *	for(int i = 0; i < 10; i++)
 *		objects.add(my_object);
 *	serializer << objects;
 * }
 * @endcode
 *
 * In the example above, we show how serializable objects (scalar values, string,
 * MyClass and Vector) are easily serialized by a serializer providing a text
 * output. As a default, the output is performed to the standard output. Notice
 * that the unserialization of data from the bytes produced by this example must
 * be performed in the same order.
 *
 * @code
 * #include <elm/serial2/serial.h>
 * #include <elm/serial2/XOMUnserializer.h>
 * #include <elm/serial2/collections.h>
 *
 * using namespace elm;
 *
 * int main(void) {
 *  serial2::XOMUnserializer unser("unser.xml");
 *	string str;
 *	int x;
 *	bool boolean;
 *	serializer >> str >> x >> boolean;
 *
 * 	MyClass my_object;
 *	serializer >> my_object;
 *
 *	Vector<MyClass> objects;
 *	serializer >> objects;
 * }
 * }
 * @endcode
 *
 * The unserialization above looks much like the previous serialization
 * example. It works in the same way as the input stream of C++ standard library.
 * The read values must be put in variable. Notice that in the case of the Vector,
 * the unserializer automatically allocate enough space to store the serialized
 * collection of objects.
 * @p
 * This examples illustrates also the use of an XML unserializer taking its input
 * from the file "unser.xml". This file is usual XML text that may be modified
 * by hand and that contains very few serialization-systems items.
 *
 * ELM provides serializer / unserializer for usual types (like scalars, strings
 * or data collections). Yet, to serializer custom classes, the user has to
 * add some information about the fields to work with. In our example, we use
 * the following declaration for MyClass in MyClass.h:
 *
 * @code
 * #include <elm/serial2/macros.h>
 * #include <elm/serial2/collections.h>
 *
 * class MyClass {
 * 	SERIALIZABLE(MyClass, FIELD(name) & FIELD(value) & FIELD(attrs));
 *	string name;
 *	int value;
 *	Vector<string> attrs;
 * public:
 *	...
 * };
 * @endcode
 *
 * The macro SERIALIZABLE is used to add to the class some RTTI information
 * that is used by the serialization system. After the name of the class,
 * this macro takes the list of fields to serialize separated by the '&' operator
 * and embedded in the FIELD macro. The field macro is not mandatory, it allows
 * only to provide better reading on textual output by providing the name of
 * the field. This macro is used both by serialization and unserialization.
 *
 * @par Making a class serializable
 *
 * In ELM, making a class serializable is very easy. One has to add some
 * RTTI information in the class declaration and add a macro in the class
 * definition file.
 *
 * The declaration ".h" file must includes serialization headers:
 * @code
 * #include <elm/serial2/macros.h>
 * #include <elm/serial2/collections.h>
 * @endcode
 *
 * Then, we have to add the macro SERIALIZE in the class declaration including
 * the name of the class and the list of the field to serialize separated by
 * '&'.
 * @code
 * class ClassName {
 *	SERIALIZABLE(ClassName, field1 & field2 & field3 & ...);
 *	...
 * };
 * @endcode
 *
 * In the definition ".cpp" file, we have just to put the following macro that
 * provides the implementation of the RTTI information of the class:
 * @code
 *	SERIALIZE(ClassName)
 * @endcode
 * The passed class name must be the same between the SERIALIZABLE and the SERIALIZE
 * macro and fully qualified to avoid ambiguities.
 *
 * Declaring a field serializable is as easy as passing its name in the SERIALIZABLE
 * list. Actually, a reference to the field is taken and used to read or write
 * the serialized values. In human readable formats (like XML or text),
 * it may be useful to provide also the identifier of the field to the
 * serialization system. This is easily done using the FIELD macro in place
 * of the field name:
 * @code
 *	SERIALIZE(MyClass, FIELD(field1) & field2 & FIELD(field3) & ...);
 * @endcode
 *
 * Some serialization formats supports optional field definition. In this case,
 * a default value may be provided with the DFIELD macro:
 * @code
 *	SERIALIZE(MyClass, DFIELD(field1, default_value) & field2 & FIELD(field3) & ...);
 * @endcode
 *
 * Finally, if the serialized class inherit from a serializable class, the
 * base class must be added to the list of field with the BASE macro:
 * @code
 * class MyBaseClass {
 *	SERIALIZABLE(MyBaseClass, ...);
 *	...
 * };
 *
 * class MyClass: public MyBaseClass {
 *	SERIALIZABLE(MyClass, BASE(MyBaseClass) & field1 & ...);
 *	...
 * };
 * @endcode
 *
 * * @par Available serializers / unserializers
 *
 * ELM comes with some already implemented serializer / unserializers:
 * @li @ref TextSerializer (serializer only).
 * @li @ref XOMSerializer / @ref XOMUnserializer.
 *
 * More will be added as the library will evolve.
 *
 * @par Enumeration Serialization
 *
 * Depending on the serialization format (textual, XML, etc), it may be useful to
 * provide more readable information for the human user. This applies typically
 * to enumeration values. ELM provides already such a facility.
 *
 * In the code below, an enumeration type is declared (in header file) and *
 * serialization information is provided by "ENUM" macro:
 * @code
 * 	typedef enum color_t {
 *			RED,
 *			GREEN,
 *			BLUE
 *		} color_t;
 *		ENUM(color_t);
 * @endcode
 *
 * Then, list of enumeration value list must be provided in the matching source file:
 * @code
 * 	ENUM_BEGIN(color_t)
 *			VALUE(RED),
 *			VALUE(GREEN),
 *			VALUE(BLUE)
 *		ENUM_END
 * @endcode
 *
 * @par Collection serialization
 *
 * @par Customizing the serialization
 *
 * Basically, what does serialization is to pass forth and back field references to classes
 * @ref Serializer for serialization and @ref Unserializer for unserialization. Depending
 * on the operation, one of the following method is called:
 * @code
 * template <> void __serialize(Serializer& s, const T& v);
 * template <> void __unserialize(Unserializer& s, T& v);
 * @endcode
 *
 * With T being the type of the field to serialize. ELM provides serializer/unserializer for most
 * basic types and most of its collection types. Providing __serialize() and __unserialize() with
 * T being your own type is a first way to specialize the serialization process. In this configuration,
 * you have to use methods provided by classes @ref Serializer and @ref Unserializer to perform
 * the actual work.
 *
 * Then, one has to remark that the type T does not need to be the one of a field of serialized object.
 * This may be any type that will refer to any object. This is the case of @ref FIELD macro that build an object
 * of type @ref Field. The trick here is that the macro @ref SERIALIZABLE creates a specific method, named
 * "__visit" in where the field are copied. This means that, when the field are built, a specific instance
 * of the class is available and usable, for example, using the "this" self pointer. This means that at the
 * construction of the field, the whole object (and its methods) are available and may be used for customizing
 * the serialization process.
 *
 * Hence, to customize the way an object is serialized, one has to:
 * @li design a dedicated class C,
 * @li build an instance of class C passing possibly the "this" pointer or any reachable object resource,
 * @li provide __serialize() and __unserialize() methods on this object.
 *
 * This is illustrated in the example below where methods getID and setID are used instead of the direct access
 * to the field itself:
 * @code
 * class MyClass;
 * class GetSetID {
 * public:
 * 		inline GetSetField(MyClass *p): _p(p) { }
 *  	MyClass *_p;
 * };
 *
 * class MyClass {
 * public:
 * 	SERIALIZABLE("my_class", GetSetID(this));
 * };
 *
 * void __serialize(Serializer& s, const GetSet& i) {
 * 		s.beginField("id");
 *		s.onValue(i._p->getID());
 *		s.endField();
 *  }
 *
 * void __unserialize(Unserializer& s, const GetSet& i) {
 * 		s.beginField("id");
 * 		string s;
 *		s.onValue(s);
 *		i._p->setID(s);
 *		s.endField();
 *  }
 * @endcode
 *
 * @par Writing a serializer
 *
 * Writing a serializer or an unserializer is relatively easy in ELM: they have just
 * to implement either @ref Serializer interface or @ref Unserializer interfaces:
 *
 * Simple type (boolean, integer, float, string) are generated by a call to one of the
 * onValue() method.
 *
 * Collection or array types starts by a call to beginCompound() and endCompound().
 * For serializers, before each item, a call ton onItem() is performed. For unserializer,
 * first a call to countItems() may be done to get the number of items in the compound.
 * Then a call to nextItem() is performed before each value read (calls to onValue()).
 *
 * Object serialization / unserialization is surrounded by a beginObject() and endObject().
 * The first parameter of the beginObject() contains a reference to the actual class
 * of the object. Then each field is serialized / unserialized is surrounded by beginField()
 * and endField() and the field value passed using other serialization methods.
 *
 * Pointer are a particular value for serialization / deserialization. First, circularities
 * involved by the use of pointers must be supported. This means that the pointer to object
 * must be stored in a map and when an already encountered object is referenced or serialized,
 * a reference to it  must be serialized or the reference must be resolved to ensure
 * that the same structure is rebuilt in memory. Notice that each object passed to onValue()
 * takes a reference possibly dereferenced by "&" and that beginCompound() and beginObject()
 * get a pointer on their object. The current implementation serialization / deserialization
 * is not able to decide if an object is referenced or not by a pointer: either the serializer
 * implementation has to provide an identifier for each serialized object, or the implementation
 * has to provide a way to back patch with an identifier already serialized objects.
 *
 * Finally, one has to provide a special case for enumeration types with onEnum(). Notice that
 * enumeration may be simply serialized as integers or, for user readable types, to use
 * the provided enumeration type decription.
 */


/**
 * Storage of all available classes.
 */
static HashTable<CString, AbstractType *> types; 


/**
 * Initializer hook
 */
static Initializer<AbstractType> initializer;


/**
 * @class AbstractType
 * Objects representing a type in the serialization module.
 * @ingroup serial
 */


/**
 * Build an abstract type.
 * @param name	Type name (full name path with namespaces).
 * @param base	Base type for a class.
 */
AbstractType::AbstractType(CString name, AbstractType *base)
: elm::AbstractClass(name, base) {
	initializer.record(this);
}


/**
 * For internal use only.
 */
void AbstractType::initialize(void) {
	types.add(name(), this);
}


/**
 * Find a type by its name.
 * @param name	Type name.
 * @return		Found type or NULL.
 */
AbstractType *AbstractType::getType(CString name) {
	return types.get(name, 0);
}


/**
 */
class VoidType: public AbstractType {
public:
	VoidType(void): AbstractType("<void>") { }
	virtual void *instantiate(void) { return 0; } 
	virtual void unserialize(Unserializer& unserializer, void *object) { }
	virtual void serialize(Serializer& serializer, const void *object) { }
};
static VoidType void_type;


/**
 * Void type representation.
 */
AbstractType& AbstractType::T_VOID = void_type;


/**
 * @class ExternalSolver
 * The external solver is used by @ref Serializer and @ref Unserializer classes
 * to solve reference to object out of the scope the currently serialized / unserialized
 * object.
 *
 * For serializing, the solver provides references to object that does not need to be
 * serialized but a reference is obtained and stored instead.
 *
 * For unserializing, this solver provide pointer to object internal to the user application
 * and that are linked with unserialized objects. When a reference cannot be solved, it is
 * considered as being provided by the application and the solver is called for solving it.
 *
 * Its default implementation considers there is not external object.
 * @ingroup serial
 */


/**
 * Null external solver.
 */
ExternalSolver ExternalSolver::null;

/**
 */
ExternalSolver::~ExternalSolver(void) {
}

/**
 * Try to resolve an external reference.
 * @param ref	Reference to resolve.
 * @return		Reference to matching object or null.
 */
void *ExternalSolver::solve(string ref) {
	return 0;
}


/**
 * Test if the given object is an external object
 * and return its reference.
 * @param object	Object to test for.
 * @return			External reference of the object or an empty string.
 */
string ExternalSolver::ref(void *object) {
	return "";
}
	
} } // elm::serial2
