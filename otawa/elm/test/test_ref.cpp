/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS.
 *
 * Reference class test
 */

#include <elm/util/test.h>
#include <elm/util/Ref.h>

using namespace elm;

typedef struct {
	int i;
	double f;
	char t[2];
} s_t;

// Entry point
TEST_BEGIN(ref)
		int x = 666, y = 111;
		Ref<int> px = &x;
		CHECK_EQUAL(&px, &x);
		CHECK(px == x);
		px = &y;
		CHECK(px == y);
		CHECK_EQUAL(&px, &y);
		px = 12;
		CHECK(px == 12);
		cout << px << io::endl;
		
		int t[10];
		for(int i = 0; i < 10; i++)
			t[i] = i;
		for(int i = 0; i < 10; i++)
			CHECK(t[i] == i);
		
		s_t s = { 666, 111.666, { 'a', 'b' } };
		Ref<s_t> pt(&s);
		CHECK_EQUAL(pt->i, 666);
		CHECK_EQUAL(pt->f, 111.666);
		CHECK_EQUAL(pt->t[0], 'a');

TEST_END
