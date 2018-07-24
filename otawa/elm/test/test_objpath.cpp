/*
 * test_objpath.cpp
 *
 *  Created on: 26 juin 2009
 *      Author: casse
 */

#include <elm/system/System.h>
#include <elm/io.h>
#include <elm/util/test.h>

using namespace elm;
using namespace elm::system;

int main(void) {
	CHECK_BEGIN("Lock");

	system::Path path = System::getUnitPath(&cout);
	cout << "ELM path = " << path << io::endl;
	CHECK(path != "");

	path = System::getUnitPath((void *)System::getUnitPath);
	cout << "ELM path = " << path << io::endl;
	CHECK(path != "");

	CHECK_END;
	return 0;
}
