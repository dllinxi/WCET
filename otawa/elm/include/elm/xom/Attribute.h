/*
 *	$Id$
 *	xom::Attribute class interface
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
#ifndef ELM_XOM_ATTRIBUTE_H_
#define ELM_XOM_ATTRIBUTE_H_

#include <elm/xom/Node.h>

namespace elm { namespace xom {

// Attribute class
class Attribute: public Node {
	friend class NodeFactory;

public:
	Attribute(Attribute *attribute);
	Attribute(String localName, String value);
	//Attribute(String localName, String value, Attribute.Type type)
	Attribute(String name, String URI, String value);
	//Attribute(String name, String URI, String value, Attribute.Type type)

	String getLocalName(void) const;
	String getNamespacePrefix(void) const;
	String getNamespaceURI(void) const;
	String getQualifiedName(void) const;
	//Attribute.Type getType()

	void setLocalName(String localName);
	void setNamespace(String prefix, String URI);
	//void setType(Attribute.Type type);
	void setValue(String value);

	// Node overload
	virtual Node *copy(void);
	virtual String getValue(void);
	virtual String toXML(void);
	virtual String 	getBaseURI (void);
	virtual Node *getChild (int index);
	virtual int getChildCount (void);

	
private:
	friend class Element;
	friend class Node;
	Attribute(void *node);
	mutable String ns, name;
	elm::string val;
};

} } // elm::xom

#endif /* ELM_XOM_ATTRIBUTE_H_ */

