/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * Unserializer class.
 */
#ifndef ELM_SERIAL2_UNSERIALIZER_H
#define ELM_SERIAL2_UNSERIALIZER_H

namespace elm { namespace serial2 {

class AbstractType;

// Unserializer class
class Unserializer {
public:
	virtual ~Unserializer(void) { }
	virtual void flush(void) = 0;

	// Complex type serializers
	virtual void beginObject(AbstractType& clazz, void *object) = 0;
	virtual void endObject(AbstractType& clazz, void *object) = 0;
	virtual bool beginField(CString name) = 0;
	virtual void endField(void) = 0;
	virtual void onPointer(AbstractType& clazz, void **object) = 0;
	virtual bool beginCompound(void*) = 0;
	virtual bool nextItem(void) = 0;
	virtual int countItems(void) = 0;
	virtual void endCompound(void*) = 0;
	virtual int onEnum(AbstractEnum& clazz) = 0; 

	// Base value serializers
	virtual void onValue(bool& v) = 0;
	virtual void onValue(signed int& v) = 0;
	virtual void onValue(unsigned int& v) = 0;
	virtual void onValue(char& v) = 0;
	virtual void onValue(signed char& v) = 0;
	virtual void onValue(unsigned char& v) = 0;
	virtual void onValue(signed short& v) = 0;
	virtual void onValue(unsigned short& v) = 0;
	virtual void onValue(signed long& v) = 0;
	virtual void onValue(unsigned long& v) = 0;
	virtual void onValue(signed long long& v) = 0;
	virtual void onValue(unsigned long long& v) = 0;
	virtual void onValue(float& v) = 0;
	virtual void onValue(double& v) = 0;
	virtual void onValue(long double& v) = 0;
	virtual void onValue(CString& v) = 0;
	virtual void onValue(String& v) = 0;
};

} } // elm::serial2

#endif	// ELM_SERIAL2_UNSERIALIZER_H
