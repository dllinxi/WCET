#include <elm/util/test.h>
#include <elm/alloc/SimpleGC.h>
#include <elm/sys/System.h>

using namespace elm;

#define SAY(x)	//x

const int ACTION_COUNT = 10000;
const int MAX_SIZE = 1024;
const int MAX_BLOCKS = 512;
const int MAX_REDUNDANTS = 256;

typedef struct block_t {
	inline block_t(void): base(0), size(0) { }
	inline block_t(char *b, int s): base(b), size(s) { }
	char *base;
	int size;
} block_t;
genstruct::Vector<block_t> blocks;
static int allocated_size = 0;

class MyGC: public SimpleGC {
public:
	MyGC(void): SimpleGC(2048) { }
protected:
	virtual void collect(void) {
		SAY(cerr << "DEBUG: collecting!\n");

		// basic collection
		for(int i = 0; i < blocks.count(); i++)
			mark(blocks[i].base, blocks[i].size);

		// redundant
		if(blocks.count())
			for(int i = 0; i < MAX_REDUNDANTS; i++) {
				int j = sys::System::random(blocks.count());
				mark(blocks[j].base, blocks[j].size);
			}
	}

};

TEST_BEGIN(simplegc)
	MyGC gc;
	bool success = true;
	for(int i = 0; success && i < ACTION_COUNT; i++) {
		int a = sys::System::random(100);

		// allocation
		if((!blocks.count() || a < 50) && blocks.count() < MAX_BLOCKS) {

			// perform allocation
			int size = sys::System::random(MAX_SIZE - 1) + 1;
			allocated_size += size;
			char *base = static_cast<char *>(gc.allocate(size));
			blocks.push(block_t(base, size));
			SAY(cerr << "DEBUG: allocate(0x" << io::hex(size) << ") = "
				 << (void *)base << "-" << static_cast<void *>(base + size - 1)
				 << " [" << io::hex(allocated_size) << "]\n");

			// check overlapping
			for(int i = 0; i < blocks.count() - 1; i++)
				if((blocks[i].base <= base && base < blocks[i].base + blocks[i].size)
				|| (blocks[i].base <= base + size - 1 && base  + size < blocks[i].base + blocks[i].size)) {
					SAY(cerr << "DEBUG: conflict with " << static_cast<void *>(blocks[i].base) << "-" << static_cast<void *>(blocks[i].base + blocks[i].size - 1) << io::endl);
					success = false;
					break;
				}
		}

		// free
		else {
			int p = sys::System::random(blocks.count());
			allocated_size -= blocks[p].size;
			SAY(cerr << "DEBUG: free(0x" << static_cast<void *>(blocks[p].base) << ":" << io::hex(blocks[p].size)
				 << " [" << io::hex(allocated_size) << "]\n");
			blocks.removeAt(p);
		}

	}

	CHECK_MSG("long run", success);
TEST_END
