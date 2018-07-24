/*
 *	XOMUnserializer class implementation
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
#include <elm/xom.h>
#include <elm/io/BlockInStream.h>
#include <elm/serial2/serial.h>
#include <elm/serial2/XOMUnserializer.h>

namespace elm { namespace serial2 {

/**
 * @class XOMUnserializer
 * This class an unserializer for XML files using XOM module.
 *
 * The XML file / node must meet the following rules:
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
 *
 * @ingroup serial
 */


/**
 */
void XOMUnserializer::ref_t::put(void **_ptr) {
	ASSERT(_ptr);
	if(ptr)
		*_ptr = ptr;
	else {
		patch_t *patch = new patch_t(_ptr);
		patch->next = patches;
		patches = patch;
	}
}


/**
 */
void XOMUnserializer::ref_t::record(void *_ptr) {
	ASSERT(_ptr);
	ptr = _ptr;
	for(patch_t *patch = patches, *next; patch; patch = next) {
		next = patch->next;
		*patch->ptr = ptr;
		delete patch;
	}
	patches = 0;
}


/**
 * Build the unserializer for using the given element.
 * @param element	XOM element to use.
 */
XOMUnserializer::XOMUnserializer(xom::Element *element)
: doc(0), _solver(&ExternalSolver::null) {
	ctx.elem = element;
}


/**
 * Build an unserializer from the given XOM document.
 * @param path	Path document to unserialize from.
 */
XOMUnserializer::XOMUnserializer(const char *path)
: doc(0), _solver(&ExternalSolver::null) {
	init(path);
}


/**
 * Build an unserializer from the given XOM document.
 * @param path	Path document to unserialize from.
 */
XOMUnserializer::XOMUnserializer(cstring path)
: doc(0), _solver(&ExternalSolver::null) {
	init(path);
}


/**
 * Build an unserializer from the given XOM document.
 * @param path	Path document to unserialize from.
 */
XOMUnserializer::XOMUnserializer(sys::Path path)
: doc(0), _solver(&ExternalSolver::null) {
	init(path.toString().toCString());
}


/**
 * Initialization from a named path.
 * @param path	Path name.
 */
void XOMUnserializer::init(cstring path) {
	ctx.elem = 0;
	xom::Builder builder;
	doc = builder.build(path);
	if(!doc)
		throw io::IOException(_ << "cannot open \"" << path << "\"");
	ctx.elem = doc->getRootElement();
	ASSERT(ctx.elem);
}


/**
 */
XOMUnserializer::~XOMUnserializer(void) {
	flush();
	if(doc)
		delete doc;
}


/**
 * @fn ExternalSolver& XOMUnserializer::solver(void) const;
 * Get the current solver of external entities. External entities are
 * objects provided by the application with which serialized objects ma be linked.
 * @return	Current external solver.
 */


/**
 * @fn void XOMUnserializer::setSolver(ExternalSolver& solver);
 * Set the current external solver.
 * @param solver	New solver to use.
 */


/**
 */
void XOMUnserializer::flush(void) {
	if(pending) {

		// build ID dictionary
		genstruct::HashTable<CString, xom::Element *> elems;
		genstruct::Vector<xom::Element *> todo;
		todo.push(doc->getRootElement());
		while(todo) {
			xom::Element *elem = todo.pop();
			Option<xom::String> id = elem->getAttributeValue("id");
			if(id)
				elems.put(*id, elem);
			for(int i = 0; i < elem->getChildCount(); i++) {
				xom::Node *node = elem->getChild(i);
				if(node->kind() == xom::Node::ELEMENT)
					todo.push((xom::Element *)node);
			}
		}

		// look in references
		while(pending) {
			Pair<cstring, ref_t *> pair = pending.pop();
			if(!pair.snd->ptr) {

				// look for the element
				xom::Element *elem = elems.get(pair.fst, 0);
				if(!elem) {
					void *obj = _solver->solve(pair.fst);
					if(!obj)
						throw io::IOException(_ << "unresolved reference \"" << pair.fst << "\"");
					else
						// TODO	Add type checking.
						pair.snd->record(obj);
				}
				ctx.elem = elem;

				// unserialize the object
				void *obj;
				embed(pair.snd->t, &obj);
				pair.snd->record(obj);
			}
		}
	}
}


/**
 * Process the description of an object embedded in a pointer.
 * @param clazz		Type of embedded object.
 * @param ptr		Pointer to the clazz pointer.
 */
void XOMUnserializer::embed(AbstractType& clazz, void **ptr) {
	AbstractType *uclass = &clazz;

	// Find the class
	string clazz_name = clazz.name();
	Option<xom::String> name = ctx.elem->getAttributeValue("class");
	if(name) {
		clazz_name = name;
		uclass = AbstractType::getType(&clazz_name);
		if(!uclass)
			throw io::IOException(_ << "no class " << clazz_name);
	}

	// Build the object
	*ptr = uclass->instantiate();
	beginObject(*uclass, *ptr);
	uclass->unserialize(*this, *ptr);
	endObject(clazz, ptr);
}


/**
 * Produce a line information to output error.
 * @param element	Element to get line information.
 * @return			File and line information of the element.
 */
string XOMUnserializer::xline(xom::Element *element) {
	return _ << doc->getBaseURI() << ":" << element->line();
}


/**
 */
void XOMUnserializer::onPointer(AbstractType& clazz, void **ptr) {

	// Is there a reference ?
	Option<xom::String> id = ctx.elem->getAttributeValue("ref");

	if(id) {
		if (id == "NULL")
			*ptr = NULL;
		else {
			ref_t *ref = refs.get(id, 0);
			if(!ref) {
				ref = new ref_t(clazz);
				pending.push(pair(cstring(*id), ref));
				refs.put(id, ref);
			}
			ref->put(ptr);
		}
	}
	// Else it should be embedded
	else
		embed(clazz, ptr);
}


/**
 */
void XOMUnserializer::beginObject(AbstractType& type, void *ptr) {
	Option<xom::String> id = ctx.elem->getAttributeValue("id");
	if(id) {
		ref_t *ref = refs.get(id, 0);
		if(ref)
			ref->record(ptr);
		else {
			if(refs.get(id, 0))
				throw io::IOException(_ << "XML identifier \"" << *id << "\" used multiple times");
			refs.put(id, new ref_t(type, ptr));
		}
	}
}


/**
 */
void XOMUnserializer::endObject(AbstractType& type, void *object) {
}


/**
 */
bool XOMUnserializer::beginField(CString name) {
	xom::Element *felem = ctx.elem->getFirstChildElement(xom::String(name));
	if(!felem)
		return false;
	stack.push(ctx);
	ctx.elem = felem;
	return true;
}


/**
 */
void XOMUnserializer::endField(void) {
	ctx = stack.pop();
}


/**
 */
bool XOMUnserializer::beginCompound(void *object) {
	int cnt = ctx.elem->getChildCount();
	for(ctx.i = 0; ctx.i < cnt; ctx.i++) {
		xom::Node *node = ctx.elem->getChild(ctx.i);
		if(node->kind() == xom::Node::ELEMENT) {
			stack.push(ctx);
			ctx.elem = (xom::Element *)node;
			return true;
		}
	}
	return false;
}


/**
 */
void XOMUnserializer::endCompound(void *object) {
}


/**
 */
bool XOMUnserializer::nextItem(void) {
	ctx = stack.pop();
	int cnt = ctx.elem->getChildCount();
	for(ctx.i++; ctx.i < cnt; ctx.i++) {
		xom::Node *node = ctx.elem->getChild(ctx.i);
		if(node->kind() == xom::Node::ELEMENT) {
			stack.push(ctx);
			ctx.elem = (xom::Element *)node;
			return true;
		}
	}
	return false;
}


/**
 */
int XOMUnserializer::countItems(void) {
	int cnt = 0;
	int n = ctx.elem->getChildCount();
	for(int i = 0; i < n; i++) {
		xom::Node *node = ctx.elem->getChild(i);
		if(node->kind() == xom::Node::ELEMENT)
			cnt++;
	}
	return cnt;
}


/**
 */
int XOMUnserializer::onEnum(AbstractEnum& clazz) {
	xom::String text = ctx.elem->getValue();
	int result = clazz.valueOf(text);
	if(result < 0) {
		String name = text;
		text.free();
		throw io::IOException(_ << "bad enumerated value \"" << name << "\"");
	}
	else {
		text.free();
		return result;
	}
}

/**
 */
void XOMUnserializer::onValue(bool& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(char& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	char chr;
	in >> chr;
	val = chr;
	text.free();
}


/**
 */
void XOMUnserializer::onValue(signed char& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	char chr;
	in >> chr;
	val = chr;
	text.free();
}


/**
 */
void XOMUnserializer::onValue(unsigned char& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::onValue(signed short& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(unsigned short& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(signed long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(unsigned long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(signed int& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(unsigned int& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(signed long long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(unsigned long long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed integer");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(float& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed float value");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(double& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	try {
		in >> val;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed float value");
	}
	text.free();
}


/**
 */
void XOMUnserializer::onValue(long double& v) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	double aux;
	try {
		in >> aux;
	}
	catch(io::IOException& e) {
		throw io::IOException(_ << xline(ctx.elem) << ": malformed float value");
	}
	v = aux;
	text.free();
}


/**
 */
void XOMUnserializer::onValue(CString& val) {
	val = ctx.elem->getValue();
}


/**
 */
void XOMUnserializer::onValue(String& val) {
	xom::String text = ctx.elem->getValue();
	val = String(text);
	text.free();
}

} } // elm::serial
