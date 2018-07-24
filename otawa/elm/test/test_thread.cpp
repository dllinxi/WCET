/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test_plugin.cpp -- test the plugin feature.
 */

#include <elm/sys/System.h>
#include <elm/sys/Thread.h>
#include <elm/util/test.h>

using namespace elm;
using namespace elm::sys;

const t::int64 N = 1000000;
t::int64 t[N];
t::int64 s1, s2;

class Sum: public Runnable {
public:
	Sum(t::int64& _r, t::int64 _s, t::int64 _n): ok(true), r(_r), s(_s), n(_n) { }

	virtual void run(void) {
		t::int64 sum = 0;
		for(t::int64 i = 0; i < n; i++)
			sum += ::t[i + s];
		r = sum;
		ok = ok && (&Runnable::current() == this);
	}

	bool ok;

private:
	t::int64& r;
	t::int64 s, n;
};


// test routine
TEST_BEGIN(thread)

	{
		// fill the array
		for(t::int64 i = 0; i < N; i++)
			::t[i] = i;

		// launch both threads
		Sum sum1(s1, 0, N / 2);
		Sum sum2(s2, N / 2, N / 2);
		Thread *t1 = Thread::make(sum1);
		CHECK(t1);
		Thread *t2 = Thread::make(sum2);
		CHECK(t2);
		t1->start();
		t2->start();
		t1->join();
		t2->join();
		CHECK_EQUAL(s1 + s2, (N * (N - 1) / 2));
		CHECK(sum1.ok);
		CHECK(sum2.ok);
		CHECK_EQUAL(&Runnable::current(), &Runnable::null);
	}

TEST_END

