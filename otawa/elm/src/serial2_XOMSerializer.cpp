/*
 *	XOMSerializer class implementation
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
#include <elm/assert.h>
#include <elm/io.h>
#include <elm/io/OutFileStream.h>
#include <elm/xom.h>
#include <elm/xom/Attribute.h>
#include <elm/xom/Serializer.h>
#include <elm/serial2/collections.h>
#include <elm/serial2/serial.h>
#include <elm/serial2/XOMSerializer.h> 

using namespace elm::genstruct;

namespace elm { namespace serial2 {

/**
 * @class XOMElementSerializer
 * An XOM serializer provides serialization facility to XML using XOM library.
 *
 * The XML serialization follows the rules below:
 *
 * @li An object is serialized by an XML element. XML element name
 * is not important as the deserialization is led by the C++ types.
 *
 * @li An element representing an object supports the "id" attribute
 * to implemenet pointer or references to it.
 *
 * @li unserializing of an object is led by the reference or
 * the pointer of access but the actual type, if different,
 * is given by a "class" providing the fully qualified C++ path
 * of the class.
 *
 * @li A pointer or a reference field are implemented either by
 * included the pointed or referenced object inside the element,
 * or by providing a "ref" attribute with the identifier of the object.
 *
 * @li Null pointers have a special "ref" attribute with value "NULL".
 *
 * @li Field are implemented as sub-element of the object element
 * whose name is the field name.
 *
 * @li Compound object is implemented with a sub-element for each
 * item of the compound.
 *
 * @li Enumeration are serialized using their non-qualified identifier
 * string.
 *
 * @li Other values must be serialized as human readable text nodes.
 *
 * @ingroup serial
 */


/**
 * Build a serializer to the given XOM document.
 * @param element	Element to populate with serialization.
 */
XOMElementSerializer::XOMElementSerializer(xom::Element *element)
: ref_current(0), _solver(&ExternalSolver::null), top(element) {
	ctx.elem = element;
}


/**
 */
XOMElementSerializer::~XOMElementSerializer(void) {
	flush();
}

/**
 * Serialize pending objects.
 */
void XOMElementSerializer::flush(void) {
	while(!toprocess.isEmpty()) {
		Pair<AbstractType *, const void*> obj = toprocess.get();
		if (!done.hasKey(obj.snd)) {
			ctx.elem = top;
			xom::Element *current = new xom::Element("dyn-obj");
			ctx.elem->appendChild(current);
			ctx.elem = current; 
			obj.fst->serialize(*this, obj.snd);
			done.put(obj.snd, ctx.elem);
		}
	}
}

/**
 */
void XOMElementSerializer::beginObject(AbstractType& clazz, const void *object) {
	
	// object already processed?
	if(done.hasKey(object)) {
	    if(ctx.elem->getAttributeValue("class"))
	    	return;
		xom::Element *src = done.get(object, 0);
		ASSERT(src != 0);
		for (int i = 0; i < src->getChildCount(); i++) {
			xom::Node *tmp = src->getChild(0);
			src->removeChild(0);
			ctx.elem->appendChild(tmp);
		}
		top->removeChild(src);
		/* delete src; XXX TODO */
		
	}

	//  not already processed
	else {
		
		String id = _ << refGet(object);
		xom::String *xom_id = new xom::String(id.toCString());
		if(!ctx.elem->getAttributeValue("class")) {
			cout << "CLASSE DE BASE: " <<  (clazz.name()) << "\n";
			
			
		} else {	
			CString name = clazz.name();	
			cout << "LA CLASSE C EST: " << name << "\n";
			xom::Attribute *attr  = new xom::Attribute("class", name);	
			ctx.elem->addAttribute(attr);
		}	
	
		xom::Attribute *attr2 = new xom::Attribute("id", *xom_id);
		ctx.elem->addAttribute(attr2);
	}

}


/**
 */
void XOMElementSerializer::endObject(AbstractType& type, const void *object) {
	done.put(object, ctx.elem);
}


/**
 */
void XOMElementSerializer::beginField(CString name) {
	xom::Element *current = new xom::Element(name);
	ctx.elem->appendChild(current);
	stack.push(ctx); 
	ctx.elem = current; 
}


/**
 */
void XOMElementSerializer::endField(void) {
	ctx = stack.pop();
}


/**
 */
void XOMElementSerializer::onPointer(AbstractType& clazz, const void *object) {
	String id = _ << refGet(object); 
	xom::String *xom_id = new xom::String(id.toCString());
	
	// if a pointed object has never been seen, add it to the "seen" and "toprocess" lists
	if (object && !seen.hasKey(object)) {
		seen.put(object, true);
		toprocess.put(pair(&clazz,object));		
	}

	// in any case, put a ref to the object in the parent field
	xom::Attribute *attr = new xom::Attribute("ref", object ? *xom_id : "NULL");
	ctx.elem->addAttribute(attr);
}


/**
 */
void XOMElementSerializer::beginCompound(const void *object) {
	String id = _ << refGet(object);
	xom::String *xom_id = new xom::String(id.toCString());
                                  	
	xom::Attribute *attr = new xom::Attribute("id", *xom_id);
	ctx.elem->addAttribute(attr);
	xom::Element *elem = new xom::Element("item");
	ctx.elem->appendChild(elem);
	stack.push(ctx);
	ctx.elem = elem;
	ctx.firstItem = true;
}


/**
 */
void XOMElementSerializer::endCompound(const void *object) {
	ctx = stack.pop();
}


/**
 */
void XOMElementSerializer::onEnum(const void *address, int value, AbstractEnum& clazz) {
	ctx.elem->appendChild(clazz.nameOf(value));	
}


/**
 */
void XOMElementSerializer::onValue(const bool& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);
}


/**
 */
void XOMElementSerializer::onValue(const signed int& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onItem(void) {
	if (ctx.firstItem)
	  ctx.firstItem = false;
	else {
	  ctx = stack.pop();
	  xom::Element *elem = new xom::Element("item");
	  ctx.elem->appendChild(elem);
	  stack.push(ctx);
	  ctx.elem = elem;
	}
}


/**
 */
void XOMElementSerializer::onValue(const unsigned int& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const char& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const signed char& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const unsigned char& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const signed short& v) {
	
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const unsigned short& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
	
}


/**
 */
void XOMElementSerializer::onValue(const signed long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const unsigned long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const signed long long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const unsigned long long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const float& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const double& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const long double& v) {
	/* !! TODO !! */
	String str1 = _ << ((double)v);
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);
	 
}


/**
 */
void XOMElementSerializer::onValue(const CString& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 */
void XOMElementSerializer::onValue(const String& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


/**
 * @class XOMSerializer
 * A specialization of class XOMElementSerializer, the obtained XML element
 * zerialized is saved to a file.
 */

/**
 * Build a serializer to the given XOM document.
 * @param path	Path document to serialize to.
 */
XOMSerializer::XOMSerializer(const sys::Path& path)
: XOMElementSerializer(prepare()), _path(_path) {
}


/**
 * Build the XML document to be serialized.
 * @return	Root element.
 */
xom::Element *XOMSerializer::prepare(void) {
	xom::Element *elem = new xom::Element("root");
	doc = new xom::Document(elem);
	return doc->getRootElement();
}


/**
 */
XOMSerializer::~XOMSerializer(void) {
	flush();
	if(doc) {
		io::OutFileStream outfile(_path);
		xom::Serializer ser(outfile);
		ser.write(doc);
		ser.flush();
		delete doc;
	}
}

} } // elm::serial
