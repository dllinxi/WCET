/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_file.cpp -- system/file classes test.
 */

#include <elm/util/test.h>
#include <elm/sys/Directory.h>
#include <elm/io.h>

using namespace elm;
using namespace elm::sys;

cstring files[] = {
	"a.txt",
	"b.txt",
	"c.txt"
};
int found = 0;

static bool lookup(FileItem *item) {
	string name = item->path().namePart();
	for(t::uint i = 0; i < sizeof(files) / sizeof(cstring); i++)
		if(name == files[i]) {
			if(found & (1 << i)) {
				cerr << name << " found several times\n";
				return false;
			}
			found |= 1 << i;
			return true;
		}
	cerr << name << " not in the list\n";
	return false;
}

static bool look_all(void) {
	for(t::uint i = 0; i < sizeof(files) / sizeof(cstring); i++)
	if(!(found & (1 << i))) {
		cerr << files[i] << " has not been found !\n";
		return false;
	}
	return true;
}

// Entry point
TEST_BEGIN(file)

	// Read directory
	FileItem *file = FileItem::get(sys::Path::current() / "test-file");
	CHECK(file->path().namePart() == "test-file");
	CHECK(file);
	Directory *dir = file->toDirectory();
	CHECK(dir);
	CHECK(dir->path().namePart() == "test-file");
	for(Directory::Iterator item(dir); item; item++) {
		cerr << item->path() << io::endl;
		CHECK(lookup(item));
	}
	CHECK(look_all());	
TEST_END
