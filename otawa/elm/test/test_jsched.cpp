/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test_plugin.cpp -- test the plugin feature.
 */

#include <elm/util/test.h>
#include <elm/sys/JobScheduler.h>

using namespace elm;
using namespace elm::sys;

static int fibo(int n) {
	if(n < 2)
		return 1;
	else
		return fibo(n - 1) + fibo(n - 2);
}

class FiboJob: public Job {
public:
	FiboJob(int _n): n(_n), r(0) { }
	virtual void run(void) {
		cout << "starting fibo(" << n << ") ...\n";
		r = fibo(n);
	}
	int n;
	int r;
};

class FiboProducer: public JobProducer {
public:
	FiboProducer(int _l, int _u): l(_l), u(_u), i(l) { }

	virtual Job *next(void) {
		if(i > u)
			return 0;
		else {
			i++;
			return new FiboJob(i);
		}
	}

	virtual void harvest(Job *job) {
		FiboJob *j = static_cast<FiboJob *>(job);
		cout << "fibo(" << j->n << ") = " << j->r << io::endl;
		delete job;
	}

private:
	int l, u, i;
};

// test routine
TEST_BEGIN(jsched)
	FiboProducer prod(10, 40);
	JobScheduler sched(prod);
	sched.start();
TEST_END

