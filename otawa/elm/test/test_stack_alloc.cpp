#include <elm/util/test.h>
#include <elm/alloc/StackAllocator.h>
#include <elm/sys/System.h>

using namespace elm;

const int ALLOC_COUNT = 10000;
const int MAX_SIZE = 1024;

TEST_BEGIN(stack_alloc)

	// brutal force allocation
	StackAllocator alloc;
	char *bases[ALLOC_COUNT];
	int sizes[ALLOC_COUNT];
	bool success = true;

	for(int i = 0; success && i < ALLOC_COUNT; i++) {

		// perform allocation
		sizes[i] = sys::System::random(MAX_SIZE - 1) + 1;
		bases[i] = static_cast<char *>(alloc.allocate(sizes[i]));
		//cerr << "alloc " << static_cast<void *>(bases[i]) << "-" << static_cast<void *>(bases[i] + sizes[i] - 1) << io::endl;

		// check no overlapping
		for(int j = 0; j < i; j++)
			if((bases[j] <= bases[i] && bases[i] < bases[j] + sizes[j])
			|| (bases[j] <= bases[i] + sizes[i] - 1 && bases[i]  + sizes[i] - 1 < bases[j] + sizes[j])) {
				success = false;
				break;
			}
	}
	CHECK_MSG("long run", success);

TEST_END
