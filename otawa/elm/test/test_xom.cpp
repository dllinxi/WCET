/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_xom.cpp -- unit tests for elm::xom classes.
 */

#include <elm/util/test.h>
#include <elm/xom.h>
#include <elm/xom/XIncluder.h>

using namespace elm;
using namespace xom;

void display_unsupported(Node *node, int level) {
	for(int i = 0; i < level; i++)
		cout << "  ";
	cout << "unsupported\n";
}

void display_text(Text *text, int level) {
	for(int i = 0; i < level; i++)
		cout << "  ";
	cout << "text(" << text->getText() << ")\n";
}

void display_element(Element *elem, int level) {
	for(int i = 0; i < level; i++)
		cout << "  ";
	cout << "ELEMENT {"
		 << elem->getNamespacePrefix()
		 << ':'
		 << elem->getNamespaceURI()
		 << '}'
		 << elem->getLocalName()
		 << '\n';
	for(int i = 0; i < elem->getChildCount(); i++) {
		Node *node = elem->getChild(i);
		switch(node->kind()) {
		case Node::ELEMENT:
			display_element((Element *)node, level + 1);
			break;
		case Node::TEXT:
			display_text((Text *)node, level + 1);
			break;
		default:
			display_unsupported(node, level + 1);
		}
	}
}

// test_xom()
void test_xom(void) {
	CHECK_BEGIN("XOM");
	Builder builder;
	Document *doc = builder.build("file.xml");
	CHECK(doc);
	Element *root_element = doc->getRootElement();
	CHECK(root_element);
	display_element(root_element, 0);
	
	// Look check Elements
	{
		Element *celem = root_element->getFirstChildElement("c");
		CHECK(celem);
		Elements *elems = celem->getChildElements();
		CHECK_EQUAL(elems->size(), 4);
		CHECK_EQUAL(elems->get(0)->getLocalName(), xom::String("d"));
		CHECK_EQUAL(elems->get(1)->getLocalName(), xom::String("d"));
		CHECK_EQUAL(elems->get(2)->getLocalName(), xom::String("c"));
		CHECK_EQUAL(elems->get(3)->getLocalName(), xom::String("d"));
		delete elems;
		elems = celem->getChildElements("d");
		CHECK_EQUAL(elems->size(), 3);
		CHECK_EQUAL(elems->get(0)->getLocalName(), xom::String("d"));
		CHECK_EQUAL(elems->get(1)->getLocalName(), xom::String("d"));
		CHECK_EQUAL(elems->get(2)->getLocalName(), xom::String("d"));
		delete elems;
	}
	
	// Check xinclude
	{
		Document *doc = builder.build("including.xml");
		CHECK(doc);
		XIncluder::resolveInPlace(doc);
		Element *root_element = doc->getRootElement();
		CHECK(root_element);
		display_element(root_element, 0);
	}
	
	CHECK_END;
}

