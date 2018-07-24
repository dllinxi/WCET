/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_lock.cpp -- unit tests for elm::Lock class.
 */

#include <elm/util/test.h>
#include <elm/sys/ProcessBuilder.h>
#include <elm/sys/System.h>
#include <elm/io.h>
#include <elm/io/OutFileStream.h>

#ifdef __unix
#elif defined(__WIN32) || defined(__WIN64)
#	include <windows.h>
#else
#	error "Unsupported OS"
#endif

using namespace elm;
using namespace elm::sys;

TEST_BEGIN(process)

	ProcessBuilder builder("ps");
	Pair<SystemInStream *, SystemOutStream *> pipes = System::pipe();
	builder.setOutput(pipes.snd);
	io::OutFileStream err_file("error.txt");
	builder.setError(&err_file);
	//builder.setError(pipes.snd);
	Process *process = builder.run();
	delete pipes.snd;
	CHECK(process);
	io::Input input(*pipes.fst);
	String line;
	cerr << "Waiting input ...\n";
	input >> line;
	while(line) {
		cout << "> " << line;
		input >> line;
	}
	cerr << "Waiting process end ...\n";
	process->wait();
	cerr << "Process ended!\n";
	CHECK_EQUAL(process->returnCode(), 0);
	delete process;
	delete pipes.fst;
	
TEST_END

