/*
 *	$Id$
 *	XSLTransform class implementation
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

#include <elm/xom/XSLTransform.h>
#include <elm/xom/NodeFactory.h>
#include <elm/xom.h>
#include "xom_macros.h"
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

namespace elm { namespace xom {

/**
 * @class XSLException
 * Thrown when an XSL stylesheet fails to compile or an XSL transform fails.
 * @author	H. Cassé <casse@irit.fr>
 */


/**
 * @class XSLTransform
 *  Serves as an interface to a XSLT processor.
 *  The following example shows how to apply an XSL Transformation
 *  to a XOM document and get the transformation result in the form of a XOM Nodes:
 *
 * @code
 *   public static Nodes transform(Document in)
 *      throws XSLException, ParsingException, IOException {
 * #include <elm/xom/Builder.h>
 * #include <elm/xom/XSLTransform.h>
 *
 * Builder builder = new Builder();
 * Document *stylesheet = builder.build("mystylesheet.xsl");
 * XSLTransform transform(stylesheet);
 * Nodes nodes = transform.transform(doc);
 * @endcode
 *
 * @author	H. Cassé <casse@irit.fr>
 */


/**
 * Creates a new XSLTransform by reading the stylesheet from the supplied document.
 * @param stylesheet 	document containing the stylesheet
 * @throw XSLException	when the supplied document is not syntactically correct XSLT
 */
XSLTransform::XSLTransform(Document *stylesheet) throw(XSLException)
: ss(stylesheet), fact(&NodeFactory::default_factory) {
}


/**
 * Creates a new XSLTransform by reading the stylesheet from the supplied document.
 * The supplied factory will be used to create all nodes in the result tree, so
 * that a transform can create instances of subclasses of the standard XOM classes.
 * Because an XSL transformation generates a list of nodes rather than a document,
 * the factory's startMakingDocument and finishMakingDocument methods are not called.
 *
 * @param stylesheet	document containing the stylesheet
 * @param factory		the factory used to build nodes in the result tree
 * @throw XSLException	when the supplied document is not syntactically correct XSLT
 */
XSLTransform::XSLTransform(Document *stylesheet, NodeFactory *factory) throw(XSLException)
: ss(stylesheet), fact(factory) {

}


/**
 * Supply a parameter to transformations performed by this object.
 *
 * @param name	the name of the parameter
 * @param value	the value of the parameter
 * @note	The parameter value must be a valid XPath expression, meaning that,
 * @li		strings must be quoted with "'",
 * @li		boolean value must converted to integer (zero for false, non-zero for true) or using functions true() or false().
 */
void XSLTransform::setParameter(const string& name, const string& value) {
	params.put(name, value);
}


/**
 *  Supply a parameter to transformations performed by this object.
 *  @param name			the name of the parameter
 *  @param namespace	the namespace URI of the parameter
 *  @param value		the value of the parameter
 */
void XSLTransform::setParameter(const string& name, const string& ns, const string& value) {
	ASSERTP(false, "unsupported method");
}


/**
 * Creates a new Nodes object from the input Nodes object by applying this object's stylesheet.
 * The original Nodes object is not changed.
 *
 * @param in		document to transform
 * @return 				a Nodes containing the result of the transformation
 * @throw XSLException	if the transformation fails, normally due to an XSLT error
 * 						all nodes passed in in parameter are cleaned up
 */
Document *XSLTransform::toDocument(const Nodes& nodes) throw(XMLException) {
	Document *doc = fact->startMakingDocument();

	// find the root
	bool root = false;
	for(int i = 0; i < nodes.size(); i++) {
		if(nodes.get(i)->kind() == Node::ELEMENT) {
			if(root) {
				delete doc;
				while (i < nodes.size())
					delete nodes.get(i);
				throw XMLException("several root element in this node list.");
			}
			else
				doc->setRootElement((Element *)nodes.get(i));
		}
		else
			doc->appendChild(nodes.get(i));
	}

	fact->finishMakingDocument(doc);
	return doc;
}


/**
 * Returns a string form of this XSLTransform, suitable for debugging.
 * @return debugging string
 */
String XSLTransform::toString(void) {
	return ss->toString();
}


// Global error handler
void XSLTransform::handle_error(void *ctx, const char *msg, ...) {
	static const int buf_size = 1024;
	XSLTransform *xslt = (XSLTransform *)ctx;
	char buf[buf_size];
	va_list args;
	va_start(args, msg);
	vsnprintf(buf, buf_size, msg, args);
	va_end(args);
	xslt->onError(level_error, buf);
}


/**
 * Creates a new Nodes from the input Document by applying this object's stylesheet.
 * The original Document is not changed.
 *
 * @param in			document to transform
 * @return 				a Nodes containing the result of the transformation
 * @throw XSLException	if the transformation fails, normally due to an XSLT error
 */
Nodes XSLTransform::transform(Document *in) throw(XSLException) {
	ASSERTP(false, "unsupported");
	return Nodes();
}


/**
 * Transform a document and return it.
 * @param in	document to transform.
 * @return		transformed document.
 */
Document *XSLTransform::transformDocument(Document *in) throw(XSLException) {
	xsltInit();

	// prepare parameters
	const char **aparams = new const char *[params.count() * 2 + 1];
	ASSERT(aparams);
	int i = 0;
	for(genstruct::AssocList<string, string>::PairIterator iter(params); iter; iter++) {
		aparams[i] = new char[(*iter).fst.length() + 1];
		strcpy((char *)aparams[i], &(*iter).fst);
		i++;
		aparams[i] = new char[(*iter).snd.length() + 1];
		strcpy((char *)aparams[i], &(*iter).snd);
		i++;
	}
	aparams[params.count() * 2] = 0;

	// perform the transformation
	xsltSetGenericErrorFunc(this, handle_error);
	xmlDoc *new_doc = xmlCopyDoc(DOC(ss->getNode()), 1);
	xsltStylesheetPtr stylesheet = xsltParseStylesheetDoc(new_doc);
	if(!stylesheet)
		throw XSLException("can not compile stylesheet");
	xmlDoc *res = xsltApplyStylesheet(stylesheet, DOC(in->getNode()), aparams);

	// clean up
	for(int i = 0; aparams[i]; i++)
		delete [] aparams[i];
	delete [] aparams;
	xmlSetStructuredErrorFunc(0, 0);
	xsltFreeStylesheet(stylesheet);
	if(!res)
		throw XSLException("error during XSLT transformation");

	// return result
	return fact->makeDocument(res);

}



/**
 * Not currently supported.
 */
Nodes XSLTransform::transform(Nodes in) throw(XSLException) {
	ASSERTP(false, "unsupported");
	return Nodes();
}

} } // elm::xom

