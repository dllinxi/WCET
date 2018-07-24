/*
 * test_array.h
 *
 *  Created on: 5 janv. 2011
 *      Author: casse
 */

#include <elm/util/test.h>
#include <elm/util/array.h>
#include <elm/system/StopWatch.h>

using namespace elm;
using namespace elm::system;

static bool flag = false;

class Deep {
public:
	inline Deep& operator=(const Deep& d) { flag = true; return *this; }
};

class Integer {
public:
	inline Integer(void) { i = 0; }
	Integer& operator=(const Integer& d) { i = d.i; return *this; }
private:
	int i;
};

void test_array(void) {

	CHECK_BEGIN("array")

	{
		int t1[20], t2[20];
		array::copy(t1, t2, 20);
	}

	{
		Deep t1[20], t2[20];
		array::copy(t1, t2, 20);
		CHECK(flag);
	}

	{
		static const int size = 100000, count = 1000;
		int t1[size], t2[size];
		Integer u1[size], u2[size];
		StopWatch sw;

		// shallow copy
		sw.start();
		for(int i = 0; i < count; i++)
			array::copy(t1, t2, size);
		sw.stop();
		time_t t_shallow = sw.stopTime() - sw.startTime();

		// deep copy
		sw.start();
		for(int i = 0; i < count; i++)
			array::copy(u1, u2, size);
		sw.stop();
		time_t t_deep = sw.stopTime() - sw.startTime();

		// compare
		cerr << "shallow time = " << t_shallow << io::endl;
		cerr << "deep time = " << t_deep << io::endl;
		cerr << "factor = " << (t_deep / t_shallow) << io::endl;
		CHECK(t_shallow < t_deep);
	}

	CHECK_END
}
