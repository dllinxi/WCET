/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test_plugin.cpp -- test the plugin feature.
 */

#include <elm/util/test.h>
#include <elm/sys/Thread.h>

using namespace elm;
using namespace elm::sys;

Mutex *m1, *m2;

class Ping: public Runnable {
public:

	Ping(void) { m1->lock(); }

	virtual void run(void) {
		for(int i = 0; i < 10; i++) {
			m2->lock();
			cout << "Ping!\n";
			m1->unlock();
		}
	}
};

class Pong: public Runnable {
public:

	Pong(void) { m2->lock(); }

	virtual void run(void) {
		for(int i = 0; i < 10; i++) {
			m2->unlock();
			m1->lock();
			cout << "Pong!\n";
		}
	}
};

// test routine
TEST_BEGIN(mutex)

	// initialization
	m1 = Mutex::make();
	m2 = Mutex::make();
	Ping ping;
	Pong pong;
	Thread *pingt = Thread::make(ping);
	Thread *pongt = Thread::make(pong);

	// start up
	pingt->start();
	pongt->start();

	// end
	pingt->join();
	pongt->join();

TEST_END
