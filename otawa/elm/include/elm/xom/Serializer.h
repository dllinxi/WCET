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
#ifndef ELM_XOM_SERIALIZER_H_
#define ELM_XOM_SERIALIZER_H_

#include <elm/string.h>
#include <elm/xom/String.h>
#include <elm/io/Output.h>

namespace elm {

namespace io { class OutStream; }

namespace xom {

// Predeclaration
class Attribute;
class Comment;
class DocType;
class Document;
class Element;
class Node;
class ProcessingInstruction;
class Text;

// Serialier class
class Serializer {
public:
	Serializer(io::OutStream& out_stream);
	Serializer(io::OutStream& out, string encoding);

	inline const string& getEncoding(void) const { return _encoding; }
	inline int getIndent(void) const { return _indent; }
	inline const string& getLineSeparator(void) const { return _line_separator; }
	inline int getMaxLength(void) const { return _max_length; }
	inline bool getPreserveBaseURI(void) const { return _preserve; }
	inline bool getUnicodeNormalizationFormC() const { return _normalize; }

	inline void setIndent(int indent) { _indent = indent; }
	inline void setLineSeparator(string line_separator) { _line_separator = line_separator; }
	inline void setMaxLength(int max_length) { _max_length = max_length; }
	inline void setOutputStream(io::OutStream& out) { _out.setStream(out); }
	inline void setPreserveBaseURI(bool preserve) { _preserve = preserve; }
	inline void setUnicodeNormalizationFormC(bool normalize) { _normalize = normalize; }
	virtual void write(Document *doc);
	
	void flush(void);

protected:
	int getColumnNumber(void);
	virtual void breakLine(void);
	virtual void write(Attribute *attribute);
	virtual void write(Comment *comment);
	virtual void write(DocType *doctype);
	virtual void write(Element *element);
	virtual void write(ProcessingInstruction *instruction);
	virtual void write(Text *text);
	virtual void writeAttributes(Element *element);
	virtual void writeAttributeValue(String value);
	virtual void writeChild(Node *node);
	virtual void writeEmptyElementTag(Element *element);
	virtual void writeEndTag(Element *element);
	virtual void writeEscaped(String text);
	virtual void writeNamespaceDeclaration(const string& prefix, const string& uri);
	virtual void writeNamespaceDeclarations(Element *element);
	virtual void writeRaw(String text, int length = -1);
	virtual void writeStartTag(Element *element);
	virtual void writeXMLDeclaration(void);

private:
	io::Output _out;
	string _encoding;
	string _line_separator;
	int _indent, _max_length;
	int _ilevel;
	bool _preserve, _normalize;
};

} } // elm::xom

#endif /* ELM_XOM_SERIALIZER_H_ */

