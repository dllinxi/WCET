/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * TextSerializer class interface.
 */
#ifndef ELM_SERIAL2_TEXT_SERIALIZER_H
#define ELM_SERIAL2_TEXT_SERIALIZER_H

#include <elm/io.h>
#include <elm/serial2/Serializer.h>
#include <elm/genstruct/HashTable.h>
#include <elm/genstruct/VectorQueue.h>
#include <elm/util/Pair.h>

namespace elm { namespace serial2 {

using namespace genstruct;

// TextSerializer class
class TextSerializer: public Serializer {
public:
	inline TextSerializer(io::Output &out = cout): level(0), _out(out) { }
	virtual void flush(void);

	// Complex type serializers
	virtual void beginObject(AbstractType& clazz, const void *object);
	virtual void endObject(AbstractType& clazz, const void *object);
	virtual void beginField(CString name);
	virtual void endField(void);
	virtual void onPointer(AbstractType& clazz, const void *object);
	virtual void beginCompound(const void*);
	virtual void onItem(void);
	virtual void endCompound(const void*);
	virtual void onEnum(const void *address, int value, AbstractEnum& clazz); 

	// Base value serializers
	virtual void onValue(const bool& v);
	virtual void onValue(const signed int& v);
	virtual void onValue(const unsigned int& v);
	virtual void onValue(const signed char& v);
	virtual void onValue(const unsigned char& v);
	virtual void onValue(const signed short& v);
	virtual void onValue(const unsigned short& v);
	virtual void onValue(const signed long& v);
	virtual void onValue(const unsigned long& v);
	virtual void onValue(const signed long long& v);
	virtual void onValue(const unsigned long long& v);
	virtual void onValue(const float& v);
	virtual void onValue(const double& v);
	virtual void onValue(const long double& v);
	virtual void onValue(const CString& v);
	virtual void onValue(const String& v);

private:
	typedef Pair<const void *, AbstractType *> delay_t;
	HashTable<const void *, bool> objects;
	VectorQueue<delay_t> to_process;
	int level;
	io::Output& _out;
	void prefix(AbstractType& type, const void *object);
	void suffix(void);
};

} } // elm::serial2

#endif // ELM_SERIAL2_TEXT_SERIALIZER_H
