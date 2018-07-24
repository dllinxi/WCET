#ifndef ELM_ARRAYFIELD_H
#define ELM_ARRAYFIELD_H

#include <elm/string.h>
#include <elm/serial2/serial.h>




#define ARRAYFIELD(name, size) elm::serial2::getArrayField(#name, name, size)

namespace elm { namespace serial2 {

template <class T>
class ArrayField {
	// private fields
	const elm::CString name;
	T *tab;	
	const int &size;

public:

	// Constructor
	inline ArrayField(elm::CString _name, T *_tab, int &_size) : name(_name), tab(_tab) , size(_size){
	}
	
	// Accessors
	inline const elm::CString& getName() const {
		return name;
	}
	inline T *value() const {
		return tab;
	}
	inline const int& getSize() const {
		return size;
	}
};


template <class T>
ArrayField<T> getArrayField(elm::CString name, T *tab, int &size) {
	return ArrayField<T>(name, tab, size);
}





	template <class T>
	inline void __serialize(Serializer& s,  const ArrayField<T> &field) {
        s.beginField(field.getName());
        s.beginCompound(field.value());
        for (int i = 0; i < field.getSize(); i++) {
        	s.onItem();
        	s.onValue((field.value())[i]);
        }
        s.endCompound(field.value());
        s.endField();
	}

	template <class T>
	inline void __unserialize(Unserializer& s,  const ArrayField<T> &field) {
        s.beginField(field.getName());
        s.beginCompound(field.value());
        for (int i = 0; i < field.getSize(); i++) {
        	s.onValue((field.value())[i]);
        }        
        s.endCompound(field.value());
        s.endField();
	}


	template <class T>
	inline Serializer& operator&(Serializer& s, const ArrayField<T> &field) {
		__serialize(s, field);
		return s;
	}
	
	template <class T>
	inline Unserializer& operator&(Unserializer& s, const ArrayField<T> &field) {
		__unserialize(s, field);
		return s;
	}
} }

#endif
