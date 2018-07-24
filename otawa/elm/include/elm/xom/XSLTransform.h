/*
 *	$Id$
 *	XSLTransform class interface
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
#ifndef ELM_XOM_XSLTRANFORM_H_
#define ELM_XOM_XSLTRANFORM_H_

#include <elm/util/ErrorHandler.h>
#include <elm/xom/Nodes.h>
#include <elm/genstruct/Vector.h>
#include <elm/genstruct/AssocList.h>

namespace elm { namespace xom {

class Document;
class NodeFactory;

// XMLException class
class XSLException: public MessageException {
public:
	inline XSLException(const string& message): MessageException(message) { }
};


// XSLTransform class
class XSLTransform: public ErrorBase {
public:
	XSLTransform(Document *stylesheet) throw(XSLException);
	XSLTransform(Document *stylesheet, NodeFactory *factory) throw(XSLException);

	void setParameter(const string& name, const string& value);
	void setParameter(const string& name, const string& ns, const string& value);
	Document *toDocument(const Nodes& nodes) throw(XMLException);
	String toString(void);
	Nodes transform(Document *in) throw(XSLException);
	Nodes transform(Nodes in) throw(XSLException);
	Document *transformDocument(Document *in ) throw(XSLException);

private:
	static void handle_error(void *ctx, const char *msg, ...);
	Document *ss;
	NodeFactory *fact;
	genstruct::AssocList<string, string> params;
};

} }		// elm::xom

#endif /* ELM_XOM_XSLTRANFORM_H_ */
