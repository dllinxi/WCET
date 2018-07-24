/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/Builder.h -- XOM Builder class interface.
 */
#ifndef ELM_XOM_BUILDER_H
#define ELM_XOM_BUILDER_H

#include <elm/string.h>
#include <elm/io.h>

namespace elm { namespace xom {

// External class
class NodeFactory;
class Document;

// Builder class
class Builder {
	bool validate;
	NodeFactory *fact;
	static NodeFactory default_factory;
public:
	Builder(void);
	Builder(bool validate);
	Builder(bool validate, NodeFactory *factory);
	Builder(NodeFactory *factory);
	
	inline NodeFactory *getNodeFactory(void) const;
	Document *build(CString system_id);
	Document *build(CString document, CString base_uri);
	Document *build(elm::io::InStream *stream);
	Document *build(elm::io::InStream *stream, CString base_uri);
};

// Builder inlines
inline NodeFactory *Builder::getNodeFactory(void) const {
	return fact;
};

} } // elm::xom

#endif // ELM_XOM_BUILDER_H
