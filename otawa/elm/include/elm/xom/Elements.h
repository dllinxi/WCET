/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/Elements.h -- XOM Elements class interface.
 */
#ifndef ELM_XOM_ELEMENTS_H
#define ELM_XOM_ELEMENTS_H

#include <elm/genstruct/Vector.h>
#include <elm/xom/String.h>

namespace elm { namespace xom {

// Predeclaration
class Element;

// Elements class
class Elements {
	friend class Element;
	genstruct::Vector<Element *> elems;
public:
	inline Element *get(int index) const;
	inline int size(void) const;
	inline Element *operator[](int index) const;
};

// Elements Inlines
inline Element *Elements::get(int index) const {
	return elems[index];
}

inline int Elements::size(void) const {
	return elems.count();
}

inline Element *Elements::operator[](int index) const {
	return get(index);
}

} } // elm::xom

#endif	// ELM_XOM_ELEMENTS_H
