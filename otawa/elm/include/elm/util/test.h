/*
 * $Id$
 * Copyright (c) 2005, IRIT UPS.
 *
 * util/test.h -- facilities for performing unit test.
 */
#ifndef ELM_UTIL_TEST_H
#define ELM_UTIL_TEST_H

#include <elm/string.h>
#include <elm/io.h>
#include <elm/util/Initializer.h>
#include <elm/genstruct/SLList.h>

namespace elm {

// TestCase class
class TestCase {
	CString _name;
	int tests;
	int errors;
public:
	TestCase(CString name);
	void initialize(void);
	virtual ~TestCase(void);
	inline cstring name(void) { return _name; }
	void test(CString file, int line, CString text);
	void failed(void);
	void succeeded(void);
	void check(CString file, int line, CString text, bool result);
	bool require(CString file, int line, CString text, bool result);
	template <class T> inline void check_equal(CString file, int line, 
		CString text, const T& result, const T& reference);
	void prepare(void);
	void complete(void);
	void perform(void);
	inline bool hasFailed(void) const { return errors; }
protected:
	virtual void execute(void);
	TestCase& __case;
};

class TestSet: private Initializer<TestCase> {
public:
	static TestSet def;

	void perform(void);

	class Iterator: public genstruct::SLList<TestCase *>::Iterator {
	public:
		inline Iterator(const TestSet& set): genstruct::SLList<TestCase *>::Iterator(set.cases) { }
		inline Iterator(const Iterator& iter): genstruct::SLList<TestCase *>::Iterator(iter) { }
	};

private:
	friend class TestCase;
	void add(TestCase *tcase);
	genstruct::SLList<TestCase *> cases;
};


// Inlines
template <class T>
inline void TestCase::check_equal(CString file, int line, CString text,
const T& result, const T& reference) {
	check(file, line, text, result == reference);
	if(result != reference)
		cout << '\t' << result << " != " << reference << "\n";
}


// Macros
//#define ELM_CHECK_MAKE(name, actions) class name##Test: public { name##Test(void)
#define ELM_CHECK_BEGIN(name)	{ elm::TestCase __case(name); __case.prepare();
#define ELM_CHECK(tst)			__case.check(__FILE__, __LINE__, #tst, tst)
#define ELM_CHECK_MSG(msg, res)	__case.check(__FILE__, __LINE__, msg, res)
#define ELM_CHECK_END 			__case.complete(); }
#define ELM_REQUIRE(tst, action)	if(!__case.require(__FILE__, __LINE__, #tst, tst)) action
#define ELM_CHECK_EQUAL(res, ref)	__case.check_equal(__FILE__, __LINE__, #res " == " #ref, res, ref)
#define ELM_CHECK_EXCEPTION(exn, stat)	{ __case.test(__FILE__, __LINE__, #stat); \
	try { stat; __case.failed(); } catch(exn) { __case.succeeded(); } }
#define ELM_FAIL_ON_EXCEPTION(exn, stat) { __case.test(__FILE__, __LINE__, #stat); \
	try { stat; __case.succeeded(); } \
	catch(exn& e) { __case.failed(); cerr << "exception = " << e.message() << elm::io::endl; } }
#define ELM_TEST_BEGIN(name) \
	static class name##Test: public elm::TestCase { \
	public: \
		name##Test(void): elm::TestCase(#name) { } \
	protected: \
		virtual void execute(void) {
#define ELM_TEST_END \
		} \
	} __test;

// shortcuts
#ifndef ELM_NO_SHORTCUT
#	define CHECK_BEGIN(name) ELM_CHECK_BEGIN(name)
#	define CHECK(tst) ELM_CHECK(tst)
#	define CHECK_MSG(msg, res) ELM_CHECK_MSG(msg, res)
#	define REQUIRE(tst, action) ELM_REQUIRE(tst, action)
#	define CHECK_EQUAL(res, ref) ELM_CHECK_EQUAL(res, ref)
#	define CHECK_END ELM_CHECK_END
#	define CHECK_EXCEPTION(exn, stat) ELM_CHECK_EXCEPTION(exn, stat)
#	define FAIL_ON_EXCEPTION(exn, stat) ELM_FAIL_ON_EXCEPTION(exn, stat)
#	define TEST_BEGIN(name) ELM_TEST_BEGIN(name)
#	define TEST_END	 ELM_TEST_END
#endif

} // elm

#endif // ELM_UTIL_TEST_H
