/*
 *	serial module interface
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
#ifndef ELM_SERIAL2_TYPE_H
#define ELM_SERIAL2_TYPE_H

#include <elm/rtti.h>
#include <elm/meta.h>
#include <elm/type_info.h>
#include <elm/serial2/Serializer.h>
#include <elm/serial2/Unserializer.h>


namespace elm { namespace serial2 {

// Class information
template <class T> struct from_class {
	static inline AbstractType& type(void) { return T::__class; }
	static inline AbstractType& type(const T& v) { return v.__getSerialClass(); }
	static inline void serialize(Serializer& s, const T& v) {
		s.beginObject(T::__class, &v);
		((T&)v).__visit(s);
		s.endObject(T::__class, &v);
	}
	static inline void unserialize(Unserializer& s, T& v) {
		s.beginObject(T::__class, &v);
		v.__visit(s);
		s.endObject(T::__class, &v);
	}
};




// Type information
template <class T> struct from_type {
	inline static AbstractType& type(void);
	inline static AbstractType& type(const T& v);
	static inline void serialize(Serializer& s, const T& v) { s.onValue(v); }
	static inline void unserialize(Unserializer& s, T& v) { s.onValue(v); }
};


// Enum information
template <class T> struct from_enum {
	inline static AbstractType& type(void);
	inline static AbstractType& type(const T& v);
	static inline void serialize(Serializer& s, const T& v);
	static inline void unserialize(Unserializer& s, T& v);
};

// Implementation


template <class T> inline AbstractType& type(void) {
	return _if<type_info<T>::is_class, from_class<T>,
				typename _if<type_info<T>::is_enum, from_enum<T>, from_type<T> >::_
			>::_::type();
}

template <class T> inline AbstractType& type(const T& v) {
	return _if<type_info<T>::is_class, from_class<T>,
				typename _if<type_info<T>::is_enum, from_enum<T>, from_type<T> >::_
			>::_::type();
}


// Serialization

template <class T>
inline void __serialize(Serializer& s, T *v) {
	s.onPointer(type<T>(), v);
}
template <class T>
inline void __serialize(Serializer& s, const T *v) {
	s.onPointer(type<T>(), v);
}
template <class T>
inline void __serialize(Serializer& s, const T& v) {
	_if<type_info<T>::is_class, from_class<T>,
			typename _if<type_info<T>::is_enum, from_enum<T>, from_type<T> >::_
		>::_::serialize(s, v);
}
template <class T>
inline Serializer& operator&(Serializer& serializer, const T& data) {
	__serialize(serializer, data);
	return serializer;
}
template <class T>
inline Serializer& operator<<(Serializer& serializer, const T& data) {
	__serialize(serializer, data);
	return serializer;
}


// Unserialization
template <class T>
inline void __unserialize(Unserializer& s, T *&v) {
	s.onPointer(type<T>(), (void **)&v);
}
template <class T>
inline void __unserialize(Unserializer& s, const T *&v) {
	s.onPointer(type<T>(), (void **)&v);
}
template <class T>
inline void __unserialize(Unserializer& s, T& v) {
	_if<type_info<T>::is_class, from_class<T>,
			typename _if<type_info<T>::is_enum, from_enum<T>, from_type<T> >::_
		>::_::unserialize(s, v);
}
template <class T>
inline Unserializer& operator&(Unserializer& serializer, T& data) {
	__unserialize(serializer, data);
	return serializer;
}
template <class T>
inline Unserializer& operator&(Unserializer& s, const Field<T>& field) {
	__unserialize(s, field);
	return s;
}
template <class T>
inline Unserializer& operator&(Unserializer& s, const DefaultField<T>& field) {
	__unserialize(s, field);
	return s;
}
template <class T>
inline Unserializer& operator>>(Unserializer& serializer, T& data) {
	__unserialize(serializer, data);
	return serializer;
}
template <class T>
inline Unserializer& operator>>(Unserializer &s, const Field<T>& field) {
	__unserialize(s, field);
	return s;
}
template <class T>
inline Unserializer& operator>>(Unserializer &s, const DefaultField<T>& field) {
	__unserialize(s, field);
	return s;
}


// AbstractType class
class AbstractType: public elm::AbstractClass {
public:
	static AbstractType& T_VOID;
	AbstractType(CString name, AbstractType *base = &T_VOID);
	void initialize(void);
	static AbstractType *getType(CString name);

	virtual void unserialize(Unserializer& unserializer, void *object) = 0;
	virtual void serialize(Serializer& serializer, const void *object) = 0;
};


// Class<T> class
template <class T>
class Class: public AbstractType {
public:
	inline Class(CString name): AbstractType(name) { };
	
	virtual void *instantiate(void) { return new T; }; 
	
	virtual void unserialize(Unserializer& unserializer, void *object) {
		__unserialize(unserializer, *static_cast<T *>(object));
	}
	
	virtual void serialize(Serializer& serializer, const void *object) {
		__serialize(serializer, *static_cast<const T *>(object));
	}
};


// Type<T> class
template <class T>
class Type: public AbstractType {
public:
	inline Type(CString name = ""): AbstractType(type_info<T>::name()) { };
	
	virtual void *instantiate(void) { return new T; }; 
	
	virtual void unserialize(Unserializer& unserializer, void *object) {
		unserializer.onValue(*static_cast<T *>(object));
	}
	
	virtual void serialize(Serializer& serializer, const void *object) {
		serializer.onValue(*static_cast<const T *>(object));
	}

	static Type<T> type;
};
template <class T> Type<T> Type<T>::type;
template <class T> AbstractType& from_type<T>::type(void) {
	return Type<T>::type;
}
template <class T> AbstractType& from_type<T>::type(const T& v) {
	return Type<T>::type;
}


// ExternalSolver class
class ExternalSolver {
public:
	static ExternalSolver null;
	virtual ~ExternalSolver(void);
	void *solve(string ref);
	string ref(void *object);
};


// AbstractEnum class
class AbstractEnum: public AbstractType {
public:
	inline AbstractEnum(CString name, value_t *values)
		: AbstractType(name), _values(values) { } 
	inline CString nameOf(int v) {
		for(value_t *vals = _values; vals->name(); vals++)
			if(vals->value() == v)
				return vals->name();
		return "";
	}
	inline int valueOf(CString name) {
		for(value_t *vals = _values; vals->name(); vals++)
			if(vals->name() == name)
				return vals->value();
		return -1;
	}
private:
	value_t *_values;
};


// Enum<T> class
template <class T>
class Enum: public AbstractEnum {
public:
	inline Enum(void)
		: AbstractEnum(type_info<T>::name(), type_info<T>::values()) { }
	
	virtual void *instantiate(void) { return new T; }; 
	
	virtual void unserialize(Unserializer& unserializer, void *object) {
		//unserializer.onValue(object, *static_cast<T *>(object), name(), );
	}
	
	virtual void serialize(Serializer& serializer, const void *object) {
		//serializer.onValue(*static_cast<const T *>(object));
	}

	static Enum<T> type;
};
template <class T> Enum<T> Enum<T>::type;


// Enum inlines
template <class T>
inline AbstractType& from_enum<T>::type(void) {
	return Enum<T>::type;
}

template <class T>
inline AbstractType& from_enum<T>::type(const T& v) {
	return Enum<T>::type;
}

template <class T>
inline void from_enum<T>::serialize(Serializer& s, const T& v) {
	s.onEnum(&v, v, Enum<T>::type);
}

template <class T>
inline void from_enum<T>::unserialize(Unserializer& s, T& v) {
	v = (T)s.onEnum(Enum<T>::type);
}


// Field inlines
template <class T>
inline void __serialize(Serializer& s, const Field<const T>& field) {
	s.beginField(field.name());
	__serialize(s, field.value());
	s.endField();
}
template <class T>
inline void __serialize(Serializer& s, const DefaultField<const T>& field) {
	__serialize(s, (const Field<const T>&)field);
}

template <class T>
inline void __serialize(Serializer& s, const Field<T>& field) {
	s.beginField(field.name());
	__serialize(s, field.value());
	s.endField();
}
template <class T>
inline void __serialize(Serializer& s, const DefaultField<T>& field) {
	__serialize(s, (const Field<T>&)field);
}

template <class T>
inline void __unserialize(Unserializer& s, const Field<T>& field) {
	if(s.beginField(field.name())) {
		__unserialize(s, field.value());
		s.endField();
	}
}

template <class T>
inline void __unserialize(Unserializer& s, const DefaultField<T>& field) {
	if(s.beginField(field.name())) {
		__unserialize(s, field.value());
		s.endField();
	}
	else
		field.value() = field.defaultValue();
}




} } // elm::serial2
	
#endif // ELM_SERIAL2_TYPE_H
