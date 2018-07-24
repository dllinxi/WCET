/*
 *	$Id$
 *	xom::Builder class interface
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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <elm/xom/Builder.h>
#include <elm/xom/NodeFactory.h>
#include <elm/xom/Document.h>

namespace elm { namespace xom {

/**
 * @class Builder
 * A builder may be used for building or loading an XML document.
 * @ingroup xom
 */


/**
 * Default factory.
 */
NodeFactory Builder::default_factory;


/**
 * Create a builder with the default node factory and no validation.
 */
Builder::Builder(void): validate(false), fact(&default_factory) {
}


/**
 * Build a document from the given system identifier.
 * @param system_id	System identifier to get the document from.
 * @return			Read document or NULL if there is an error.
 */
Document *Builder::build(CString system_id) {
	
	// Configuration
	xmlLineNumbersDefault(1);
	
	// Read the file
	xmlDoc *xml_doc = xmlReadFile(system_id, NULL,
		(validate ? XML_PARSE_DTDVALID : 0)
		| XML_PARSE_NOENT
		| XML_PARSE_NOBLANKS
		| XML_PARSE_NOCDATA);
	if(!xml_doc)
		return 0;
		
	// Build the document
	return fact->makeDocument(xml_doc);
}

} }	// elm::xom

