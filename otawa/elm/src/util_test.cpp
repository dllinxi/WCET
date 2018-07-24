/*
 * $Id$
 * Copyright (c) 2005, IRIT UPS.
 *
 * util/test.cpp -- facilities for performing unit test.
 */

#include <elm/io.h>
#include <elm/util/test.h>

namespace elm {

/**
 * @def CHECK_BEGIN(name)
 * This is the first statement of a unit test case. Usually, a unit test case
 * is structured as below:
 * @code
 * 	CHECK_BEGIN(test case name);
 * 	CHECK(test1);
 * 	CHECK(test2);
 * 	...
 * 	CHECK_END;
 * @endcode
 * This statement display a state header, each CHECK displays a check status line
 * and TEST_END a summary of the full test case. A test case defines also
 * an environment like in C blocks { ... }.
 * @param name	Name of the test case (string).
 * @note	These macros definition may be discarded defining the ELM_NO_SHORTCUT
 * identifier, for avoiding name clashes for example.
 *
 * @ingroup utility
 * @seealso @ref CHECK_END, @ref CHECK, @ref elm::TestCase, @ref elm::TestSet
 */

/**
 * @def TEST_BEGIN(test)
 * This macro is used to create and to record an integrated test case.
 * Following the macro must be inserted the checked tests with macros based on CHECK
 * and the test must be ended by the macro @ref TEST_END.
 * @par
 * The test cases may be specified independently but may be also grouped inside a test set,
 * implemented by the class @ref elm::TestSet. This class is a singleton, @ref elm::TestSet::def,
 * containing the list of uses cases. The list of test cases can be explored using @ref elm::TestSet::Iterator.
 * @par
 * In this case, the macro @ref TEST_BEGIN and @ref TEST_END must be used as in the example below:
 * @code
 * 	TEST_BEGIN(my_test)
 * 		CHECK(test1);
 * 		CHECK(test2);
 * 		...
 * 	TEST_END
 * @endcode
 * @param test	Name of the test (must be unquoted !).
 * @ingroup utility
 * @seealso @ref TEST_END, @ref CHECK, @ref elm::TestCase, @ref elm::TestSet
 */


/**
 * @def TEST_END
 * End a test declared with @ref TEST_BEGIN.
 * @ingroup utility
 * @seealso @ref TEST_BEGIN, @ref CHECK, @ref elm::TestCase, @ref elm::TestSet
 */

/**
 * @def CHECK(test)
 * See @ref ELM_CHECK.
 * @param test	Test to check.
 * @ingroup utility
 * @seealso @ref ELM_CHECK, @ref REQUIRE, @ref CHECK_EQUAL, @ref CHECK_EXCEPTION, @ref FAIL_ON_EXCEPTION
 */


/**
 * @def CHECK_END
 * See @ref CHECK_END.
 * @ingroup utility
 * @seealso @ref CHECK_BEGIN, @ref CHECK, @ref elm::TestCase, @ref elm::TestSet
 */


/**
 * @def ELM_CHECK_BEGIN(name)
 * Same as @ref CHECK_BEGIN.
 * @ingroup utility
 */


/**
 * @def ELM_CHECK(test)
 * Same as @ref CHECK.
 * @ingroup utility
 */


/**
 * @def ELM_CHECK_END
 * Same as @ref CHECK_END
 * @ingroup utility
 */

/**
 * @def ELM_CHECK_EQUAL(res, ref)
 * Test if the result of a test is equal to a reference value.
 * @param res	Result of the test.
 * @param ref	Reference value.
 * @ingroup utility
 * @seealso @ref CHECK, @ref REQUIRE, @ref CHECK_EXCEPTION, @ref FAIL_ON_EXCEPTION
 */


/**
 * @def CHECK_EQUAL(res, ref)
 * See ELM_CHECK_EQUAL.
 * @ingroup utility
 */


/**
 * @class TestCase
 * This class is used for implementing macro for unit tests.
 * @ingroup utility
 * @seealso @ref TestSet, @ref CHECK, @ref TEST_BEGIN, @ref CHECK_BEGIN
 */


/**
 * Build a new tes case.
 * @param name	Name of the test case.
 */
TestCase::TestCase(CString name): _name(name), tests(0), errors(0), __case(*this) {
	TestSet::def.record(this);
}


/**
 * Perform the display before the test execution.
 */
void TestCase::prepare(void) {
#if defined(__unix) || defined(__APPLE__)
	cout << "\x1b[1;4mTEST CASE: " << name() << "\x1b[0m\n";
#elif defined(__WIN32) || defined(__WIN64)
	cout << "TEST CASE: " << name() << "\n";
#endif
}


/**
 * Prepare to do a test.
 * @param file	Source file.
 * @param line	Source line.
 * @param text	Code text to test.
 */
void TestCase::test(CString file, int line, CString text) {
	tests++;
	cout << file << ':' << line << ": " << text << '\t';
}


/**
 * Inform that the current test has failed.
 */
void TestCase::failed(void) {
#if defined(__unix) || defined(__APPLE__)
	cout << "\x1b[31m[FAILED]\x1b[0m\n";
#elif defined(__WIN32) || defined(__WIN64)
	cout << "FAILED\n";
#endif
	errors++;
}


/**
 * Inform that the current test has succeeded.
 */
void TestCase::succeeded(void) {
#if defined(__unix) || defined(__APPLE__)
	cout << "\x1b[32m[OK]\x1b[0m\n";
#elif defined(__WIN32) || defined(__WIN64)
	cout << "[OK] \n";
#endif
}


/**
 * Check if a test has returned true.
 * @param file		Source file containing the test.
 * @param line		Line in the source file of the test.
 * @param text		Explanation about the test.
 * @param result	Result of the test.
 */
void TestCase::check(CString file, int line, CString text, bool result) {
	test(file, line, text);
	if(result)
		succeeded();
	else
		failed();
}



/**
 * For internal work only. Don't call it.
 */
void TestCase::initialize(void) {
	TestSet::def.add(this);
}


/**
 * Final display of the test case.
 */
void TestCase::complete(void) {
	if(errors)
#if defined(__unix) || defined(__APPLE__)
		cout << "\x1b[1;31mFAILURE: \x1b[0m";
#elif defined(__WIN32)
		cout << "FAILURE :";
#endif
	else
#if defined(__unix) || defined(__APPLE__)
		cout << "\x1b[1;32mSUCCESS: \x1b[0m";
#elif defined(__WIN32) || defined(__WIN64)
		cout << "SUCCESS: ";
#endif
	cout << tests << " tests, " << errors  << " errors\n";
}


/**
 * Same as check but also returns value and specific message for ending the
 * test due to main failure.
 * @param file		File containing the test source.
 * @param line		Line where the test is implemented.
 * @param text		Text describing the test.
 * @param result	Result of the test.
 * @return			Result of the test.
 */
bool TestCase::require(CString file, int line, CString text, bool result) {
	check(file, line, text, result);
	if(!result)
#if defined(__unix) || defined(__APPLE__)
		cout << "\x1b[1;31mMain failure: test must be interrupted.\x1b[0m\n";
#elif defined(__WIN32) || defined(__WIN64)
		cout << "Main failure: test must be interrupted\n";
#endif
	return result;
}


/**
 * Actual actions of the test.
 */
void TestCase::execute(void) {
}


/**
 * Execute the test case.
 */
void TestCase::perform(void) {
	tests = 0;
	errors = 0;
	prepare();
	execute();
	complete();
}


/**
 */
TestCase::~TestCase(void) {
}


/**
 * @class TestSet
 * Singleton class containing all existing tests.
 * @ingroup utility
 * @seealso @ref elm::TestCase, @ref CHECK, @ref TEST_BEGIN
 */


/**
 * Perform the tests of the test set.
 */
void TestSet::perform(void) {
	for(Iterator test(*this); test; test++)
		test->perform();
}


/**
 * Singleton of TestSet.
 */
TestSet TestSet::def;


/**
 * Add the given test.
 * @param tcase		Test case to add.
 */
void TestSet::add(TestCase *tcase) {
	cases.add(tcase);
}

} //elm
