/*
 *	XOMSerializer class interface
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
#ifndef ELM_SERIAL2_XOM_SERIALIZER_H
#define ELM_SERIAL2_XOM_SERIALIZER_H

#include <elm/genstruct/HashTable.h>
#include <elm/genstruct/Vector.h>
#include <elm/serial2/Serializer.h>
#include <elm/genstruct/HashTable.h>
#include <elm/genstruct/VectorQueue.h>
#include <elm/util/Pair.h>

namespace elm {

// Extern classes

namespace xom {
	class Document;
	class Attribute;
	class Element;
}

namespace serial2 {


class XOMElementSerializer: public Serializer {
public:
	XOMElementSerializer(xom::Element *top);
	virtual ~XOMElementSerializer(void);
	inline ExternalSolver& solver(void) const { return *_solver; }
	inline void setSolver(ExternalSolver& solver) { _solver = &solver; }

	// XOMUnserializer overload
	virtual void flush(void);
	virtual void beginObject(AbstractType& clazz, const void *object);
	virtual void endObject(AbstractType& clazz, const void *object);
	virtual void beginField(CString name);
	virtual void endField(void);
	virtual void onPointer(AbstractType& clazz, const void *object);
	virtual void beginCompound(const void*);
	virtual void endCompound(const void*);
	virtual void onEnum(const void *address, int value, AbstractEnum& clazz); 
	virtual void onValue(const bool& v);
	virtual void onValue(const signed int& v);
	virtual void onValue(const unsigned int& v);
	virtual void onValue(const char& v);
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
	virtual void onItem (void);

private:
	genstruct::HashTable<const void*, bool> seen;
	genstruct::HashTable<const void*, xom::Element*> done;
	genstruct::HashTable<const void*, int> idmap;	
	elm::genstruct::VectorQueue<Pair<AbstractType* ,const void*> > toprocess;	
	
	typedef struct context_t {
		xom::Element *elem;
		bool firstItem;
	} context_t;
	context_t ctx;
	elm::genstruct::Vector<context_t> stack;	
	int ref_current;
	
	int refGet(const void* object) {		
		if (!idmap.hasKey(object)) {	
			idmap.put(object, ref_current);
			ref_current++;
		}
		return idmap.get(object, -1);
	}

	ExternalSolver *_solver;
	xom::Element *top;
}; 

class XOMSerializer: public XOMElementSerializer {
public:
	XOMSerializer(const sys::Path& path);
	~XOMSerializer(void);

private:
	xom::Element *prepare(void);
	xom::Document *doc;
	const sys::Path& _path;
};

} } // elm::serial2

#endif // ELM_SERIAL2_XOM_SERIALIZER_H

