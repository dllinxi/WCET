/*
 *	$Id$
 *	xom::Node class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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
#include "xom_macros.h"
#include <libxml/xinclude.h>
#include <elm/xom/XIncluder.h>
#include <elm/xom/Exception.h>

namespace elm { namespace xom {

/**
 * @class XIncluder
 * Implements XInclude resolution as specified in XML Inclusions (XInclude)
 * Version 1.0. Fallbacks are supported. The XPointer element() scheme and
 * shorthand XPointers are also supported. The XPointer xpointer() scheme is 
 * not supported. The accept and accept-language attributes are supported.
 * @ingroup xom
 */ 


/**
 * The namespace name of all XInclude elements.
 */
const String XIncluder::NS = XINCLUDE_NS;


/**
 * Returns a copy of the document in which all xinclude:include elements have
 * been replaced by their referenced content. The original Document object is
 * not modified. Resolution is recursive; that is, include elements in the
 * included documents are themselves resolved. The Document returned contains
 * no include elements.
 * 
 * @param in 	The document in which include elements should be resolved.
 * @return		Copy of the document in which all xinclude:include elements have 
 * been replaced by their referenced content.
 * @warning Not implemented !
 */
Document *XIncluder::resolve(Document *in) {
	// !!TODO!!
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * Returns a copy of the document in which all xinclude:include elements have
 * been replaced by their referenced content as loaded by the builder.
 * The original Document object is not modified. Resolution is recursive;
 * that is, include elements in the included documents are themselves resolved.
 * The document returned contains no include elements.
 *
 * @param in 		The document in which include elements should be resolved.
 * @param builder	The builder used to build the nodes included from other
 * 					documents.
 * @return 			Copy of the document in which all xinclude:include elements
 * 					have been replaced by their referenced content.
 * @warning Not implemented !
 */
Document *XIncluder::resolve(Document *in, Builder& builder) {
	// !!TODO!!
	ASSERTP(0, "unsupported");
	return 0;
}


/**
 * Modifies a document by replacing all xinclude:include elements by their
 * referenced content. Resolution is recursive; that is, include elements in
 * the included documents are themselves resolved. The resolved document
 * contains no xinclude:include elements.
 * 
 * If the inclusion fails for any reason—XInclude syntax error, missing
 * resource with no fallback, etc.—the document may be left in a partially
 * resolved state.
 *
 * @param in	The document in which include elements should be resolved.
 */
void XIncluder::resolveInPlace(Document *in) {
	if(xmlXIncludeProcessFlags(
		DOC(in->node),
		XML_PARSE_NOCDATA | XML_PARSE_NOXINCNODE)
	< 0)
		throw Exception(in, "xinclude error");
}


/**
 * Modifies a document by replacing all xinclude:include elements with their
 * referenced content as loaded by the builder. Resolution is recursive;
 * that is, include elements in the included documents are themselves resolved.
 * The resolved document contains no xinclude:include elements.
 * 
 * If the inclusion fails for any reason — XInclude syntax error, missing
 * resource with no fallback, etc. — the document may be left in a partially
 * resolved state.
 * 
 * @param in		The document in which include elements should be resolved.
 * @param builder	The builder used to build the nodes included from other
 * 					documents
 * @warning Not implemented !
 */
void XIncluder::resolveInPlace(Document *in, Builder& builder) {
	// !!TODO!!
	ASSERTP(0, "unsupported");
}

} } // elm::xom

