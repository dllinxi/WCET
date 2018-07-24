/*
 *	$Id$
 *	xom::Nodes class interface
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
#ifndef ELM_XOM_NODES_H_
#define ELM_XOM_NODES_H_

#include <elm/util/MessageException.h>
#include <elm/genstruct/Vector.h>
#include <elm/xom/Node.h>

namespace elm { namespace xom {

// Nodes class
class Nodes {
public:
	inline Nodes(void) { }
	inline Nodes(Node *node) { nodes.add(node); }
	inline Nodes(const Nodes& n): nodes(n.nodes) {  }
	inline Nodes& operator=(const Nodes& n)
		{ nodes.clear(); nodes.addAll(n.nodes); return *this; }

	inline void append(Node *node) { nodes.add(node); }
	inline bool contains(Node *node) const { return nodes.contains(node); }
	inline Node *get(int index) const { return nodes[index]; }
	inline void insert(Node *node, int index) { nodes.insert(index, node); }
	inline Node *remove(int index)
		{ Node *node = nodes[index]; nodes.remove(node); return node; }
	inline int size(void) const { return nodes.count(); }
private:
	genstruct::Vector<Node *> nodes;
};

// XMLException class
class XMLException: public MessageException {
public:
	inline XMLException(const string& message): MessageException(message) { }
};

} }	// elm::xom

#endif /* ELM_XOM_NODES_H_ */
