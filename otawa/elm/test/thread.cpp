/*
 * test_thread.cpp
 *
 *  Created on: 24 janv. 2011
 *      Author: casse
 */

#include <elm/io.h>
#include <elm/sys/Thread.h>
#include <elm/system/StopWatch.h>
#include <math.h>

using namespace elm;
using namespace elm::system;

double my_cos(double x) {
    double t , s ;
    static const double prec = 1e-10;
    int p;
    p = 0;
    s = 1.0;
    t = 1.0;
    while(fabs(t/s) > prec)
    {
        p++;
        t = (-t * x * x) / ((2 * p - 1) * (2 * p));
        s += t;
    }
    return s;
}

int int_comp(int x) {
	int s = 0;
	for(int i = 0; i < x; i++)
		for(int j = i; j < x; j++)
			s += i * j;
	return s;
}

void work(long long m, long long n, int step) {
	float x = 0;
	cerr << "WORK from " << m << " to " << n << io::endl;
	for(long long i = m; i < n; i += step)
		//x += my_cos(i);
		x += int_comp(i);
}

class MyRunnable: public Runnable {
public:
	long long n, m;
	int step;

	MyRunnable(long long _n, long long _m, int s):
		n(_n), m(_m), step(s) { }

	virtual void run(void) {
		work(n, m, step);
	}
};

void test_thread(void) {
	static const long long N = 3000;

	// alone
	StopWatch sw;
	sw.start();
	work(0, N, 1);
	sw.stop();
	cerr << "alone: " << sw.delay() << io::endl;

	// at two
	StopWatch sw2;
	sw2.start();
	MyRunnable run(0, N, 2);
	Thread *thread = Thread::make(run);
	thread->start();
	work(1, N, 2);
	thread->join();
	sw2.stop();
	cerr << "two: " << sw2.delay() << io::endl;
}

int main(void) {
	test_thread();
	return 0;
}
