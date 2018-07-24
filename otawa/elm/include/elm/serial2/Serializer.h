/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * Serializer2 class.
 */
#ifndef ELM_SERIAL2_SERIALIZER_H
#define ELM_SERIAL2_SERIALIZER_H

#include <elm/rtti.h>

namespace elm { namespace serial2 {

class AbstractType;
class AbstractEnum;

// Serializer class
class Serializer {
public:
	virtual ~Serializer(void) { }
	virtual void flush(void) = 0;

	// Complex type serializers
	virtual void beginObject(AbstractType& clazz, const void *object) = 0;
	virtual void endObject(AbstractType& clazz, const void *object) = 0;
	virtual void beginField(CString name) = 0;
	virtual void endField(void) = 0;
	virtual void onPointer(AbstractType& clazz, const void *object) = 0;
	virtual void beginCompound(const void *object) = 0;
	virtual void onItem(void) = 0;
	virtual void endCompound(const void*) = 0;
	virtual void onEnum(const void *address, int value, AbstractEnum& clazz) = 0; 

	// Base value serializers
	virtual void onValue(const bool& v) = 0;
	virtual void onValue(const signed int& v) = 0;
	virtual void onValue(const unsigned int& v) = 0;
	virtual void onValue(const signed char& v) = 0;
	virtual void onValue(const unsigned char& v) = 0;
	virtual void onValue(const signed short& v) = 0;
	virtual void onValue(const unsigned short& v) = 0;
	virtual void onValue(const signed long& v) = 0;
	virtual void onValue(const unsigned long& v) = 0;
	virtual void onValue(const signed long long& v) = 0;
	virtual void onValue(const unsigned long long& v) = 0;
	virtual void onValue(const float& v) = 0;
	virtual void onValue(const double& v) = 0;
	virtual void onValue(const long double& v) = 0;
	virtual void onValue(const CString& v) = 0;
	virtual void onValue(const String& v) = 0;
};

} } // elm::serial2

#endif	// ELM_SERIAL2_SERIALIZER_H
