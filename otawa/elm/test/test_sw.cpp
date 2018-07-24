/**
 * StopWatch class test
 */

#include <elm/io.h>
#include <elm/sys/StopWatch.h>

using namespace elm;
using namespace elm::sys;

int main(void) {
	StopWatch sw;
	sw.start();
	int n = 1;
	for(int i = 1; i < 10000000; i++)
		n *= i;
	sw.stop();
	cerr << "TIME = " << sw.delay() << "us\n";
	return 0;
}
