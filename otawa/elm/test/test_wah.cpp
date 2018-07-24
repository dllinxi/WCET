#include <elm/util/WAHVector.h>
#include "elm/util/test.h"
#include <elm/util/BitVector.h>
#include <elm/sys/System.h>

#define	INTENSIVE

using namespace elm;

void print(io::Output& out, const BitVector& v) {
	t::uint32 w = 0;
	int c = 0;
	for(int i = 0; i < v.size(); i++) {
		if(v.bit(i))
			w |= 1 << c;
		c++;
		if(c == 31) {
			out << ' ' << io::hex(w).width(8).pad('0').right();
			w = 0;
			c = 0;
		}
	}
	if(c)
		out << ' ' << io::hex(w).width(8).pad('0').right();
}

TEST_BEGIN(wah)

	// empty vector test
	{
		WAHVector v(256);
		cout << "v = " << v << io::endl;
		CHECK(!v.bit(0));
		CHECK(!v.bit(15));
		CHECK(!v.bit(128));
		CHECK(v.isEmpty());
		CHECK(!v.isFull());
		CHECK_EQUAL(v.size(), 256);
		CHECK(v.countBits() == 0);
	}

	// full vector test
	{
		WAHVector v(256, true);
		cout << "v = " << v << io::endl;
		CHECK(v.bit(0));
		CHECK(v.bit(15));
		CHECK(v.bit(128));
		CHECK(!v.isEmpty());
		CHECK(v.isFull());
		CHECK_EQUAL(v.size(), 256);
		CHECK_EQUAL(v.countBits(), 256);
	}

	// set first word
	{
		WAHVector v(256);
		v.set(15);
		cout << "v = " << v << io::endl;
		CHECK(v.bit(15));
		CHECK_EQUAL(v.countBits(), 1);
	}

	// set middle word
	{
		WAHVector v(256);
		v.set(50);
		cout << "v = " << v << io::endl;
		CHECK(v.bit(50));
		CHECK_EQUAL(v.countBits(), 1);
	}

	// set last word
	{
		WAHVector v(256);
		v.set(255);
		cout << "v = " << v << io::endl;
		CHECK(v.bit(255));
		CHECK_EQUAL(v.countBits(), 1);
	}

	// clear first word
	{
		WAHVector v(256, true);
		v.clear(15);
		cout << "v = " << v << io::endl;
		CHECK(!v.bit(15));
		CHECK_EQUAL(v.countBits(), 255);
	}

	// clear middle word
	{
		WAHVector v(256, true);
		v.clear(50);
		cout << "v = " << v << io::endl;
		CHECK(!v.bit(50));
		CHECK_EQUAL(v.countBits(), 255);
	}

	// clear last word
	{
		WAHVector v(256, true);
		v.clear(255);
		cout << "v = " << v << io::endl;
		CHECK(!v.bit(255));
		CHECK_EQUAL(v.countBits(), 255);
	}

	// multi-set
	{
		WAHVector v(256, false);
		v.set(56);
		CHECK(v.bit(56));
		CHECK_EQUAL(v.size(), 256);
		v.set(101);
		CHECK(v.bit(101));
		CHECK_EQUAL(v.size(), 256);
	}

	// test equality
	{
		WAHVector v(256, false), w(256, false);
		CHECK(v == w);
		v.set(56);
		CHECK(v != w);
		w.set(56);
		CHECK(v == w);
		w.clear(56);
		CHECK(v != w);
		v.clear(56);
		CHECK(v == w);
	}

	// simple and test
	{
		WAHVector v(256, false), w(256, false);
		v.applyAnd(w);
		CHECK(v.isEmpty());
	}
	{
		WAHVector v(256, false), w(256, true);
		v.applyAnd(w);
		CHECK(v.isEmpty());
	}
	{
		WAHVector v(256, true), w(256, false);
		v.applyAnd(w);
		CHECK(v.isEmpty());
	}
	{
		WAHVector v(256, true), w(256, true);
		v.applyAnd(w);
		CHECK(v.isFull());
	}

	// more complex and test
	{
		WAHVector v(256, false), w(256, false);
		w.set(18);
		w.set(19);
		w.set(101);
		v.applyAnd(w);
		CHECK(v.isEmpty());
	}
	{
		WAHVector v(256, false), w(256, false);
		v.set(18);
		w.set(55);
		v.set(101);
		w.set(101);
		cerr << "v = " << v << io::endl;
		cerr << "w = " << w << io::endl;
		v.applyAnd(w);
		cerr << "r = " << v << io::endl;
		CHECK(!v.bit(18));
		CHECK(!v.bit(55));
		CHECK(v.bit(101));
	}

	// simple OR test
	{
		WAHVector v(256, false), w(256, false);
		v.applyOr(w);
		CHECK(v.isEmpty());
	}
	{
		WAHVector v(256, false), w(256, true);
		v.applyOr(w);
		CHECK(v.isFull());
	}
	{
		WAHVector v(256, true), w(256, false);
		v.applyOr(w);
		CHECK(v.isFull());
	}
	{
		WAHVector v(256, true), w(256, true);
		v.applyOr(w);
		CHECK(v.isFull());
	}

	// more complex OR test
	{
		WAHVector v(256, false), w(256, true);
		w.set(18);
		w.set(19);
		w.set(101);
		v.applyOr(w);
		CHECK(v.isFull());
	}
	{
		WAHVector v(256, false), w(256, false);
		v.set(18);
		w.set(55);
		v.set(101);
		w.set(101);
		v.applyOr(w);
		CHECK(v.bit(18));
		CHECK(v.bit(55));
		CHECK(v.bit(101));
	}

	// not testing
	{
		WAHVector v(256, false);
		v.applyNot();
		CHECK(v.isFull());
		WAHVector w(256, true);
		w.applyNot();
		CHECK(w.isEmpty());
	}
	{
		WAHVector v(256, false);
		v.set(18);
		v.set(19);
		v.set(56);
		v.set(211);
		cerr << "DEBUG: v = " << v << io::endl;
		v.applyNot();
		cerr << "DEBUG: ~v = " << v << io::endl;
		CHECK(!v.bit(18));
		CHECK(!v.bit(19));
		CHECK(!v.bit(56));
		CHECK(!v.bit(211));
		CHECK(v.bit(5));
		CHECK(v.bit(21));
		CHECK(v.bit(214));
	}

	// test array access
	{
		WAHVector v(256, false);
		v[18] = true;
		v[19] = true;
		v[51] = true;
		CHECK(v[18]);
		CHECK(v[19]);
		CHECK(v[51]);
		CHECK(!v[5]);
		CHECK(!v[121]);
	}

	// test inclusion
	{
		WAHVector v(256, false), w(256, false);
		CHECK(v.includes(w));
		CHECK(!v.includesStrictly(w));
	}
	{
		WAHVector v(256, false), w(256, true);
		cerr << "v = " << v << ", w = " << w << io::endl;
		CHECK(!v.includes(w));
		CHECK(!v.includesStrictly(w));
	}
	{
		WAHVector v(256, true), w(256, false);
		CHECK(v.includes(w));
		CHECK(v.includesStrictly(w));
	}
	{
		WAHVector v(256, true), w(256, true);
		CHECK(v.includes(w));
		CHECK(!v.includesStrictly(w));
	}
	{
		WAHVector v(256, false), w(256, false);
		v.set(100);
		w.set(100);
		v.set(102);
		CHECK(v.includes(w));
		CHECK(v.includesStrictly(w));
	}
	{
		WAHVector v(256, false), w(256, false);
		v.set(100);
		w.set(100);
		CHECK(v.includes(w));
		CHECK(!v.includesStrictly(w));
	}
	{
		WAHVector v(256, false), w(256, false);
		w.set(100);
		CHECK(!v.includes(w));
		CHECK(!v.includesStrictly(w));
	}
	{
		WAHVector v(4, true), w(4);
		w.set(0);
		w.set(1);
		w.set(2);
		w.set(3);
		CHECK(v == w);
	}
	{
		WAHVector v(4), w(4, true);
		w.clear(0);
		w.clear(1);
		w.clear(2);
		w.clear(3);
		CHECK(v == w);
	}
	{
		WAHVector v(35, true), w(35);
		for(int i = 0; i < 35; i++)
			w.set(i);
		CHECK(v == w);
	}
	{
		WAHVector v(35), w(35, true);
		for(int i = 0; i < 35; i++)
			w.clear(i);
		CHECK(v == w);
	}

#	ifdef INTENSIVE
	// massive testing
	{
		const int M = 100, T = 100000, S = 256;
		WAHVector *wvs[M];
		BitVector *bvs[M];
		int used = 0;
		typedef enum {
			NEW = 0,
			DELETE,
			SET,
			CLR,
			NOT,
			AND,
			OR,
			RESET,
			COUNT
		} action_t;
		bool failed = false;

		for(int i = 0; !failed && i < T; i++) {

			// determine action
			action_t action;
			int v = 0, w = 0;
			if(used < 2)
				action = NEW;
			else {
				action = action_t(sys::System::random(COUNT));
				v = sys::System::random(used);
				w = sys::System::random(used);
			}
			//cerr << "action = " << int(action) << ", v = " << v << ", w = " << w << ", used = " << used << io::endl;

			// perform the action
			switch(action) {

			case NEW:
				if(used < M) {
					v = used;
					wvs[v] = new WAHVector(S, false);
					bvs[v] = new BitVector(S, false);
					used++;
					break;
				}
				/* no break */

			case DELETE:
				delete wvs[v];
				wvs[v] = wvs[used - 1];
				delete bvs[v];
				bvs[v] = bvs[used - 1];
				used--;
				continue;

			case SET:
				wvs[v]->set(w % S);
				bvs[v]->set(w % S);
				break;

			case CLR:
				//cerr << "wvs = " << *wvs[v] << io::endl;
				//cerr << "bvs = "; print(cerr, *bvs[v]); cerr << io::endl;
				wvs[v]->clear(w % S);
				bvs[v]->clear(w % S);
				break;

			case NOT:
				wvs[v]->applyNot();
				bvs[v]->applyNot();
				break;

			case AND:
				wvs[v]->applyAnd(*wvs[w]);
				bvs[v]->applyAnd(*bvs[w]);
				break;

			case OR:
				wvs[v]->applyOr(*wvs[w]);
				bvs[v]->applyOr(*bvs[w]);
				break;

			case RESET:
				wvs[v]->applyReset(*wvs[w]);
				bvs[v]->applyReset(*bvs[w]);
				break;

			default:
				ASSERT(false);
				break;
			}

			// check
			ASSERT(bvs[v]->size() == S);
			if(wvs[v]->size() != S) {
				cerr << "bad size: " << wvs[v]->size() << " ! = " << S << io::endl;
				failed = true;
			}
			else
				for(int i = 0; i < S; i++)
					if(wvs[v]->bit(i) != bvs[v]->bit(i)) {
						failed = true;
						cerr << "ERROR: bit " << i << io::endl;
						break;
					}

			// display result
			if(failed) {
				cerr << "wv = " << *wvs[v] << io::endl;
				//cerr << "bv = " << *bvs[v] << io::endl;
				cerr << "bv: "; print(cerr, *bvs[v]); cerr << io::endl;
				cerr << io::endl;
			}
		}

		CHECK(!failed);
	}
#	endif	// INTENSIVE

TEST_END
