/*
 * test_ini.cpp
 *
 *  Created on: 10 août 2013
 *      Author: casse
 */

#include <elm/ini.h>
#include <elm/util/test.h>

#if 0
using namespace elm;
using namespace elm::ini;

TEST_BEGIN(ini)

	// load the file
	File *file;
	FAIL_ON_EXCEPTION(ini::Exception, file = File::load("test.ini"));

	// dump the content
	for(File::Iterator s(file); s; s++) {
		cerr << "[" << s->name() << "]\n";
		for(Section::Iterator v(s); v; v++)
			cerr << v.key() << "=" << v.value() << io::endl;
	}

	// look for sections
	Section *s1 = file->get("s1");
	CHECK(s1);
	Section *plug = file->get("elm-plugin");
	CHECK(plug);

	// look for content
	CHECK(file->defaultSection()->get("val3") == "");
	CHECK_EQUAL(file->defaultSection()->get("val1"), string("ok"));
	CHECK(file->defaultSection()->get("val2") == "666");
	CHECK(s1->get("v1") == "bof");
	CHECK(s1->get("v2") == "1;2;3;4");
	CHECK(plug->get("rpath") == "$ORIGIN/../lib;$ORIGIN:/home/ici/lib");
	CHECK(plug->get("deps") == "plugin1;plugin2;$ORIGIN/proc/plug3");

	// look for values
	CHECK_EQUAL(file->defaultSection()->getInt("val1", 111), 111);
	CHECK_EQUAL(file->defaultSection()->getInt("val2", 0), 666);
	CHECK_EQUAL(file->defaultSection()->getInt("val3", 333), 333);
	genstruct::Vector<string> list;
	s1->getList("v2", list);
	CHECK(list.length() == 4);
	CHECK(list[0] == "1");
	CHECK(list[1] == "2");
	CHECK(list[2] == "3");
	CHECK(list[3] == "4");

	// multi-line
	CHECK_EQUAL(plug->get("description"), string("multi-line test"));

	delete file;

TEST_END

#endif


