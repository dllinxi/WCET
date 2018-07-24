/*
 * test for ListQueue class
 *
 */

#include <elm/util/test.h>
#include <elm/data/ListQueue.h>
#include <elm/sys/System.h>

using namespace elm;

TEST_BEGIN(listqueue)

	// base test
	{
		ListQueue<int> q;
		for(int i = 0; i < 10; i++)
			q << i;
		for(int i = 0; i < 10; i++) {
			int x;
			q >> x;
			CHECK_EQUAL(x, i);
		}
		CHECK(q.isEmpty());
	}

TEST_END

