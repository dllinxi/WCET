/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_lock.cpp -- unit tests for elm::Lock class.
 */

#include <elm/util/test.h>
#include <elm/utility.h>
#include <elm/util/LockPtr.h>

using namespace elm;

// Utility class
class LockTest: public Lock {
	bool *alloc;
public:
	LockTest(bool *_alloc): alloc(_alloc) { *alloc = true; };
	~LockTest(void) { *alloc = false; };
	inline bool getAlloc(void) const { return *alloc; };
};


// test_lock()
TEST_BEGIN(lock)
	
	// Simple test
	{
		bool alloc = false;
		{
			LockPtr<LockTest> p = new LockTest(&alloc);
			CHECK(alloc);
		}
		CHECK(!alloc);
	}
	
	// Null assignment test
	{
		bool alloc = false;
		{
			LockPtr<LockTest> p;
			CHECK(!alloc);
			p = new LockTest(&alloc);
			CHECK(alloc);
		}
		CHECK(!alloc);
	}
	
	// Passing test
	{
		bool alloc = false;
		{
			LockPtr<LockTest> p1;
			{
				LockPtr<LockTest> p2 = new LockTest(&alloc);
				CHECK(alloc);
				p1 = p2;
				CHECK(alloc);
			}
			CHECK(alloc);
		}
		CHECK(!alloc);
	}
	
	// Passing null
	{
		bool alloc = false;
		LockPtr<LockTest> p = new LockTest(&alloc);
		CHECK(alloc);
		p = 0;
		CHECK(!alloc);
	}
	
	// Transitivity
	{
		bool alloc = false;
		LockPtr<LockTest> p1 = new LockTest(&alloc);
		CHECK(alloc);
		LockPtr<LockTest> p2 = p1;
		CHECK(alloc);
		LockPtr<LockTest> p3 = p2;
		CHECK(alloc);
		p1 = 0;
		CHECK(alloc);
		p3 = 0;
		CHECK(alloc);
		p2 = 0;
		CHECK(!alloc);
	}
	
	// Usage test
	{
		bool alloc = false;
		LockPtr<LockTest> p = new LockTest(&alloc);
		CHECK(p->getAlloc() == alloc);
	}
	
TEST_END



