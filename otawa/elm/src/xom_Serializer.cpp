/*
 *	$Id$
 *	xom::Serializer class interface
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

#include <elm/xom/Serializer.h>
#include <elm/xom.h>

#define UNSUPPORTED	ASSERTP(false, "unsupported");

namespace elm { namespace xom {

/**
 * @class Serializer
 *
 * Outputs a Document object in a specific encoding using various options
 * for controlling white space, normalization, indenting, line breaking, and base URIs.
 * However, in general these options do affect the document's infoset. In particular,
 * if you set either the maximum line length or the indent size to a positive value,
 * then the serializer will not respect input white space. It may trim leading and
 * trailing space, condense runs of white space to a single space, convert carriage
 * returns and linefeeds to spaces, add extra space where none was present before,
 * and otherwise muck with the document's white space. The defaults, however, preserve
 * all significant white space including ignorable white space and boundary white space.
 *
 * @warning This is a very limited version of the serializer:
 * <ul>
 * @li supports only UTF-8 encoding,
 * @li no indentation, space, newline suppport.
 * </ul>
 *
 * @author	H. Cassé <casse@irit.fr>
 * @ingroup xom
 */

/**
 * Get the escape sequence matching the given character.
 * Only support character under code 127.
 * @param chr	Character to escape.
 * @return		Escaped string.
 */
static String escapeSimple(char chr) {
	switch(chr) {
	case '"': return "&quote;";
	case '&': return "&amp;";
	case '<': return "&lt;";
	case '>': return "&gt;";
	case '\'': return "&apos;";
	default: return "";
	}
}


/**
 * Test if the character must be escaped in an attribute value.
 * @param chr	Character to test.
 * @return		True if it is attribute escapable, false else.
 */
static bool isAttrEscape(char chr) {
	switch(chr) {
	case '"':
	case '&':
	case '<':
	case '>':	return true;
	default: return false;
	}
}


/**
 * Test if the character must be escaped in a text value.
 * @param chr	Character to test.
 * @return		True if it is text escapable, false else.
 */
static bool isTextEscape(char chr) {
	switch(chr) {
	case '&':
	case '<':
	case '>': return true;
	default: return false;
	}
}


/**
 * Create a new serializer that uses the UTF-8 encoding.
 * @param out_stream	the output stream to write the document on
 */
Serializer::Serializer(io::OutStream& out)
:	_out(out),
	_encoding("UTF-8"),
	_line_separator("\n"),
	_indent(2),
	_max_length(0),
	_ilevel(0),
	_preserve(false),
	_normalize(false)
{
}

/**
 * Create a new serializer that uses the specified encoding. The encoding must be
 * recognized by the libxml.
 * @param out		the output stream to write the document on
 * @param encoding	the character encoding for the serialization
 */
Serializer::Serializer(io::OutStream& out, string encoding)
:	_out(out),
	_encoding(encoding),
	_line_separator("\n"),
	_indent(0),
	_max_length(0),
	_preserve(false),
	_normalize(false)
{
}


/**
 * Flush the out stream.
 */
void Serializer::flush(void) {
	_out.flush();
}


/**
 * Returns the current column number of the output stream. This method useful for subclasses that implement
 * their own pretty printing strategies by inserting white space and line breaks at appropriate points.
 * Columns are counted based on Unicode characters, not UTF-8 chars. A surrogate pair counts as one character
 * in this context, not two. However, a character followed by a combining character (e.g. e followed by
 * combining accent acute) counts as two characters. This latter choice (treating combining characters
 * like regular characters) is under review, and may change in the future if it's not too big a performance hit.
 * @return	the current column number
 */
int Serializer::getColumnNumber(void) {
	return 0;
}

/**
 * Writes the current line break string onto the underlying output stream
 * and indents as specified by the current level and the indent property.
 */
void Serializer::breakLine(void) {
	_out << _line_separator;
}


/**
 * Writes a string onto the underlying output stream. without escaping any characters.
 * Non-ASCII characters that are not available in the current character set cause an IOException.
 * @param text		the String to serialize
 * @param length	length of the string (optional)
 */
void Serializer::writeRaw(String text, int length) {
	if(length < 0)
		length = text.length();
	_out.stream().write(&text, length);
}


/**
 * Writes a string onto the underlying output stream. Non-ASCII characters that are not available
 * in the current character set are escaped using hexadecimal numeric character references.
 * Carriage returns, line feeds, and tabs are also escaped using hexadecimal numeric character references
 * in order to ensure their preservation on a round trip. The four reserved characters <, >, &, and " are
 * escaped using the standard entity references &lt;, &gt;, &amp;, and &quot;. The single quote is not escaped.
 * @param value	the attribute value to serialize
 */
void Serializer::writeAttributeValue(String value) {
	int len = value.length();
	for(int i = 0; i < len; i++) {
		int j = i;
		while(i < len && !isAttrEscape(value[i]))
			i++;
		if(i > j)
			writeRaw(&value + j, j - i);
		if(i < len)
			writeRaw(escapeSimple(value[i++]));
	}
}


/**
 * Writes all the attributes of the specified element onto the output stream,
 * one at a time, separated by white space. If preserveBaseURI is true, and
 * it is necessary to add an xml:base attribute to the element in order to preserve
 * the base URI, then that attribute is also written here. Each individual attribute
 * is written by invoking write(Attribute).
 * @param element	the Element whose attributes are written
 */
void Serializer::writeAttributes(Element *element) {
	int cnt = element->getAttributeCount();
	for(int i = 0; i < cnt; i++)
		write(element->getAttribute(i));
}


/**
 * Writes an attribute in the form name="value". Characters in the attribute value are escaped as necessary.
 * @param attribute the Attribute to write
 */
void Serializer::write(Attribute *attribute) {
	writeRaw(" ", 1);
	writeRaw(attribute->getLocalName());
	writeRaw("=\"");
	writeAttributeValue(attribute->getValue());
	writeRaw("\"");
}


/**
 * Serializes a document onto the output stream using the current options.
 * @param doc	the Document to serialize
 */
void Serializer::write(Document *doc) {
	ASSERTP(doc, "null document");
	writeXMLDeclaration();
	Element *element = doc->getRootElement();
	ASSERTP(element, "null root element");
	write(doc->getRootElement());
}


/**
 * Writes a comment onto the output stream using the current options.
 * Since character and entity references are not resolved in comments,
 * comments can only be serialized when all characters they contain are available in the current encoding.
 * @param comment	the Comment to serialize
 */
void Serializer::write(Comment *comment) {
	writeRaw("<!--");
	writeRaw(comment->getText());
	writeRaw("-->");
}


/**
 * Writes a DocType object onto the output stream using the current options.
 * @param doctype	the document type declaration to serialize
 */
void Serializer::write(DocType *doctype) {
	UNSUPPORTED
}


/**
 * Serializes an element onto the output stream using the current options.
 * The result is guaranteed to be well-formed. If element does not have a parent element,
 * the output will also be namespace well-formed.
 *
 * If the element is empty, this method invokes writeEmptyElementTag. If the element is not empty, then:
 * <ol>
 * <li>It calls writeStartTag.</li>
 * <li>It passes each of the element's children to writeChild in order.</li>
 * <li>It calls writeEndTag.</li>
 * </ol>
 *
 * It may break lines or add white space if the serializer has been configured to indent or use a maximum line length.
 */
void Serializer::write(Element *element) {
	int cnt = element->getChildCount();
	if(cnt == 0)
		writeEmptyElementTag(element);
	else {
		writeStartTag(element);
		_ilevel ++;
		for(int i = 0; i < cnt; i++)
			this->writeChild(element->getChild(i));
		_ilevel --;
		writeEndTag(element);
	}
}


/**
 * Writes a processing instruction onto the output stream using the current options.
 * Since character and entity references are not resolved in processing instructions,
 * processing instructions can only be serialized when all characters they contain are
 * available in the current encoding.
 *
 * @param instruction	the ProcessingInstruction to serialize
 */
void Serializer::write(ProcessingInstruction *instruction) {
	UNSUPPORTED
}


/**
 * Writes a Text object onto the output stream using the current options.
 * Reserved characters such as <, > and " are escaped using the standard entity
 * references such as &lt;, &gt;, and &quot;.
 *
 * Characters which cannot be encoded in the current character set (for example, Ω in ISO-8859-1)
 * are encoded using character references.
 *
 * @param text	the Text to serialize
 */
void Serializer::write(Text *text) {
	ASSERTP(text, "null text");
	writeEscaped(text->getValue());
}


/**
 * Writes a child node onto the output stream using the current options.
 * It is invoked when walking the tree to serialize the entire document.
 * It is not called, and indeed should not be called, for either the Document node or for attributes.
 *
 * @param node	the Node to serialize
 */
void Serializer::writeChild(Node *node) {
	ASSERTP(node, "null node");
	switch(node->kind()) {
	case Node::ELEMENT: write(static_cast<Element *>(node)); break;
	case Node::DOCUMENT: write(static_cast<Document *>(node)); break;
	case Node::TEXT: write(static_cast<Text *>(node)); break;
	case Node::COMMENT: write(static_cast<Comment *>(node)); break;
	default: UNSUPPORTED;
	}
}

/**
 * Writes an empty-element tag for the element including all its namespace declarations and attributes.
 *
 * The writeAttributes method is called to write all the non-namespace-declaration attributes.
 * The writeNamespaceDeclarations method is called to write all the namespace declaration attributes.
 *
 *  If subclasses don't wish empty-element tags to be used, they can override this method to simply
 * invoke writeStartTag followed by writeEndTag.
 *
 * @param element	the element whose empty-element tag is written
 */
void Serializer::writeEmptyElementTag(Element *element) {
        for (int i = 0; i < (_ilevel*_indent); i++)
	  writeRaw(" ", 1);
	writeRaw("<");
	writeRaw(element->getLocalName());
	writeAttributes(element);
	writeRaw("/>");
	breakLine();
}


/**
 * Writes the end-tag for an element in the form </name>.
 * @param	element	the element whose end-tag is written
 */
void Serializer::writeEndTag(Element *element) {
        if (element->getChild(element->getChildCount() - 1)->kind() == Node::ELEMENT) {
          for (int i = 0; i < (_ilevel*_indent); i++)
	    writeRaw(" ", 1);
	}

	writeRaw("</");
	String qname = element->getQualifiedName();
	writeRaw(qname);
	qname.free();
	writeRaw(">");
	breakLine();
}


/**
 * Writes a namespace declaration in the form xmlns:prefix="uri" or xmlns="uri".
 * It does not write the spaces on either side of the namespace declaration.
 * These are written by writeNamespaceDeclarations.
 * @param prefix	the namespace prefix; the empty string for the default namespace
 * @param uri		the namespace URI
 */
void Serializer::writeNamespaceDeclaration(const string& prefix, const string& uri) {
	UNSUPPORTED
}


/**
 * Writes all the namespace declaration attributes of the specified element
 * onto the output stream, one at a time, separated by white space. Each individual
 * declaration is written by invoking writeNamespaceDeclaration.
 *
 * @param element	the Element whose namespace declarations are written
 */
void Serializer::writeNamespaceDeclarations(Element *element) {
	UNSUPPORTED
}


/**
 * Writes the start-tag for the element including all its namespace declarations and attributes.
 *
 * The writeAttributes method is called to write all the non-namespace-declaration attributes.
 * The writeNamespaceDeclarations method is called to write all the namespace declaration attributes.
 *
 * @param element	the element whose start-tag is written
 */
void Serializer::writeStartTag(Element *element) {

	// indentation
	for (int i = 0; i < (_ilevel*_indent); i++)
		writeRaw(" ", 1);

	// tag start
	writeRaw("<");
	String qname = element->getQualifiedName();
	writeRaw(qname);
	qname.free();

	// namespace declaration
	for(int i = 0; i < element->getNamespaceDeclarationCount(); i++) {
		String prefix = element->getNamespacePrefix(i);
		if(prefix) {
			writeRaw(" xmlns:");
			writeRaw(prefix);
			writeRaw("=\"");
			writeRaw(element->getNamespaceURI(prefix));
		}
		else {
			writeRaw(" xmlns=\"");
			writeRaw(element->getNamespaceURI());
		}
		writeRaw("\"");
	}

	// display attributes
	writeAttributes(element);
	writeRaw(">");

	// write a newline, except if there is a single, non-element, child.
	if ((element->getChildCount() == 1) && (element->getChild(0)->kind() != Node::ELEMENT))
	  return;
	breakLine();

}


/**
 * Writes the XML declaration onto the output stream, followed by a line break.
 */
void Serializer::writeXMLDeclaration(void) {
	writeRaw("<?xml version=\"1.0\" encoding=\"");
	writeRaw(_encoding.toCString());
	writeRaw("\"?>");
	breakLine();
}


/**
 * Writes a string onto the underlying output stream.
 * Non-ASCII characters that are not available in the current character set
 * are encoded with numeric character references. The three reserved characters <, >, and &
 * are escaped using the standard entity references &lt;, &gt;, and &amp;.
 * Double and single quotes are not escaped.
 * @param text	the parsed character data to serialize
 */
void Serializer::writeEscaped(String text) {
	int len = text.length();
	for(int i = 0; i < len; i++) {
		int j = i;
		while(i < len && !isTextEscape(text[i]))
			i++;
		if(i > j)
			writeRaw(&text + j, i - j);
		if(i < len)
			writeRaw(escapeSimple(text[i]));
	}
}

} } // elm::xom
