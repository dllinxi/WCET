#include <elm/alloc/BlockAllocatorWithGC.h>
#include <elm/sys/System.h>
#include <elm/io.h>

using namespace elm;



class Cons {
public:
	int hd;
	Cons *tl;

	Cons(int h, Cons *t = 0): hd(h), tl(t) { }
	void *operator new(size_t size, BlockAllocatorWithGC<Cons>& pool) { return pool.allocate(); }
};

class MyAllocator: public BlockAllocatorWithGC<Cons> {
	static const int
		heads_cnt = 256,
		new_prob = 16,
		delete_prob = 12,
		relink_prob = 4,
		cut_prob = 4,
		test_cnt = 20000;
	static const int
		total_prob = new_prob + delete_prob + relink_prob + cut_prob,
		new_thresh = 0,
		delete_thresh = new_prob,
		relink_thresh = new_prob + delete_prob,
		cut_thresh = new_prob + delete_prob + relink_prob;

public:
	MyAllocator(void): BlockAllocatorWithGC<Cons>(1 << 10) {
	}

	void run(void) {

		// init heads
		for(int i = 0; i < heads_cnt; i++)
			hds[i] = 0;

		// perform the work
		for(int i = 0; i < test_cnt; i++) {
			int action = sys::System::random(total_prob);
			int hd = sys::System::random(heads_cnt);
			if(new_thresh <= action && action < new_thresh + new_prob) {
				hds[hd] = new(*this) Cons(0, hds[hd]);
				cout << "creating " << (void *)hds[hd] << " in head " << hd << io::endl;
			}
			else if(delete_thresh <= action && action < delete_thresh + delete_prob) {
				if(hds[hd]) {
					cout << "deleting " << (void *)hds[hd] << " from head " << hd << io::endl;
					hds[hd] = hds[hd]->tl;
				}
				else
					i--;
			}
			else if(relink_thresh <= action && action < relink_thresh + relink_prob) {
				int rhd = sys::System::random(heads_cnt);
				cerr << "relinking head " << hd << " to head " << rhd << io::endl;
				hds[rhd] = hds[hd];
			}
			else {
				cerr << "cutting head " << hd << io::endl;
				hds[hd] = 0;
			}
		}

		// succeeded !
		cerr << "INFO: success !\n";
	}

protected:
	virtual void collect(void) {
		for(int i = 0; i < heads_cnt; i++)
			for(Cons *cur = hds[i]; cur && mark(cur); cur = cur->tl);
	}

	virtual void beginGC(void) {
		cerr << "GC: starting\n";
	}

	virtual void endGC(void) {

		// display any lifing is not free !
		for(int i = 0; i < heads_cnt; i++)
			for(Cons *cur = hds[i]; cur; cur = cur->tl)
				for(free_t *f = free_list; f; f = f->next)
					ASSERTP(f != (free_t *)cur, "block " << (void *)cur << " is in the free list !\n");

		// display end
		int total = totalCount(), free = freeCount(), used = usedCount();
		cerr << "INFO: used = " << used << " (" << (used * 100 / total) << "%), "
			 << " free = " << free << " (" << (free * 100 / total) << "%)\n";
		cerr << "GC: ending\n";
	}

private:
	Cons *hds[heads_cnt];
};

int main(void) {
	MyAllocator alloc;
	alloc.run();
}
