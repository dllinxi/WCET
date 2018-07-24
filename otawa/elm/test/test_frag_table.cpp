/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * test/test_frag_table.cpp -- FragTable class unit test.
 */

#include <elm/util/test.h>
#include <elm/genstruct/FragTable.h>

using namespace elm;
using namespace elm::genstruct;

TEST_BEGIN(frag_table)
	
	FragTable<int> tab(2);
	
	// Fill the table
	CHECK(tab.isEmpty());
	bool test_length = true;
	for(int i = 0; i < 123; i++) {
		if(tab.length() != i) {
			test_length = false;
			break;
		}
		tab.add(i);
	}
	CHECK(test_length);
	CHECK_EQUAL(tab.length(), 123);
	
	// Test the content
	bool test_content = true;
	for(int i = 0; i < 123; i++)
		if(tab[i] != i) {
			test_content = false;
			break;
		}
	CHECK(test_content);
	
	// Test iterator
	bool test_iter = true;
	int i = 0;
	for(FragTable<int>::Iterator iter(tab); iter; iter++, i++)
		if(iter != i) {
			test_iter = false;
			break;
		}
	CHECK(test_iter);
	CHECK_EQUAL(i, 123);
	
	// Test assignment
	for(int i = 0, j = 122; i < j; i++, j--) {
		int tmp = tab[i];
		tab[i] = tab[j];
		tab[j] = tmp;
	}
	bool test_set = true;
	for(int i = 0; i < 123; i++)
		if(tab[i] != 122 - i) {
			test_set = false;
			cerr << "tab[" << i << "] = " << tab[i] << io::endl;
			break;
		}	
	CHECK(test_set);
	
	// Test mutable table
	/*i = 0;
	for(FragTable<int>::MutableIterator iter(tab); iter; iter++, i++)
		iter = i;
	bool test_mut_iter = true;
	for(int i = 0; i < 123; i++)
		if(tab[i] != i) {
			test_mut_iter = false;
			break;
		}
	CHECK(test_mut_iter);*/
	
TEST_END
