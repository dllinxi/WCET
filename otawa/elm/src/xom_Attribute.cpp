/*
 *	$Id$
 *	xom::Attribute class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
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

#include <elm/xom/Attribute.h>

#include <elm/assert.h>
#include "xom_macros.h"

#define UNSUPPORTED ASSERTP(0, "unsupported");

namespace elm { namespace xom {

/**
 * @class Attribute
 * 
 * This class represents an attribute such as type="empty" or xlink:href="http://www.example.com".
 *
 * Attributes that declare namespaces such as xmlns="http://www.w3.org/TR/1999/xhtml" or
 * xmlns:xlink="http://www.w3.org/TR/1999/xlink" are stored separately on the elements where they appear.
 * They are never represented as Attribute objects. 
 * 
 * @author H. Cassé <casse@irit.fr>
 * @ingroup xom
 */


/**
 * Unsupported.
 */
Attribute::Attribute(Attribute *attribute): Node(0) {
	UNSUPPORTED
}


/**
 * Creates a new attribute in no namespace with the specified name and value and undeclared type.
 * @param localName	the unprefixed attribute name
 * @param value		the attribute value
 */
Attribute::Attribute(String localName, String value): Node(0) {
	name = localName;
	val = value;
}


/**
 * Creates a new attribute in the specified namespace with the specified name and value and undeclared type.
 * @param localName	the prefixed attribute name
 * @param URI		the namespace URI
 * @param value		the attribute value
 */
Attribute::Attribute(String localName, String URI, String value): Node(0) {
	name = localName;
	val = value;
	ns = URI;
}


/**
 * Returns the local name of this attribute, not including the prefix.
 * @return	the attribute's local name
 * Unsupported.
 */
String Attribute::getLocalName(void) const {
	if(!name)
		name = String(NODE(node)->name); 
	return name;
}


/**
 * Unsupported.
 */
String Attribute::getNamespacePrefix(void) const {
	UNSUPPORTED
	return "";
}


/**
 * Unsupported.
 */
String Attribute::getNamespaceURI(void) const {
	if(!ns)
		ns = String(NODE(node)->ns->href); 
	return ns;
}


/**
 * Unspported.
 */
String Attribute::getQualifiedName(void) const {
	UNSUPPORTED
	return "";
}


/**
 * Unsupported.
 */
void Attribute::setLocalName(String localName) {
	UNSUPPORTED
}


/**
 * Unsupported.
 */
void Attribute::setNamespace(String prefix, String URI) {
	UNSUPPORTED
}


/**
 * Unsupported.
 */
void Attribute::setValue(String value) {
	UNSUPPORTED
}


/**
 * Unsupported.
 */
Node *Attribute::copy(void) {
	UNSUPPORTED
	return 0;
}


/**
 */
String Attribute::getValue(void) {
	if(!val)
		val = internGetValue(); 
	return &val;
}


/**
 * Unsupported.
 */
String Attribute::toXML(void) {
	UNSUPPORTED
	return "";
}


/**
 * Unsupported.
 */
Attribute::Attribute(void *node): Node(node) {
	ASSERT(node);
}


/**
 */
String Attribute::getBaseURI (void) {
	UNSUPPORTED;
	return "";
}


/**
 */
Node *Attribute::getChild (int index) {
	ASSERTP(false, "no child in attribute");
	return 0;
}


/**
 */
int Attribute::getChildCount (void) {
	return 0;
}

} }		// elm::xom
