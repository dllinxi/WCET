/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/xom/XIncluder.h -- XOM XIncluder class interface.
 */
#ifndef ELM_XOM_XINCLUDER_H
#define ELM_XOM_XINCLUDER_H

#include <elm/xom/Document.h>

namespace elm { namespace xom {
	
// XIncluder class
class XIncluder {
public:
	static const String NS;
	static Document *resolve(Document *in);
	static Document *resolve(Document *in, Builder& builder);
	static void	resolveInPlace(Document *in);
	static void	resolveInPlace(Document *in, Builder& builder);
};

} } // elm::xom

#endif // elm::xom
