/*
 *	$Id$
 *	IO module test program
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007-08, IRIT UPS.
 *
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <elm/util/test.h>
#include <elm/io/RandomAccessStream.h>
#include <elm/checksum/Fletcher.h>
#include <elm/io/BufferedInStream.h>
#include <elm/io/BufferedOutStream.h>
#include <elm/io/InFileStream.h>
#include <elm/system/System.h>
#include <elm/io/BlockInStream.h>

using namespace elm;
using namespace elm::io;
using namespace elm::checksum;

// test_RandomAccessStream()
void test_RandomAccessStream(void) {
	CHECK_BEGIN("io::RandomAccessStream");
	int data[] = { 111, 666, 0, 1, 2, 3 };

	// Write to the file
	{
		OutStream *stream = RandomAccessStream::createFile("random_access.txt");
		CHECK(stream);
		CHECK(stream->write((char *)data, sizeof(data)) == sizeof(data));
		delete stream;
	}

	// Read from the file
	{
		InStream *stream = RandomAccessStream::openFile("random_access.txt");
		CHECK(stream);
		int read_data[sizeof(data) / sizeof(int)];
		CHECK(stream->read((char *)read_data, sizeof(read_data)) == sizeof(read_data));
		delete stream;
		for(int i = 0; i < (int)(sizeof(data) / sizeof(int)); i++)
			CHECK_EQUAL(read_data[i], data[i]);
	}

	// Random read the file
	{
		RandomAccessStream *stream = RandomAccessStream::openFile("random_access.txt");
		CHECK(stream);
		for(int i = sizeof(data) / sizeof(int) - 1; i >= 0; i--) {
			CHECK(stream->moveTo(i * sizeof(int)));
			int x;
			CHECK(stream->read((char *)&x, sizeof(int)) == sizeof(int));
			CHECK_EQUAL(x, data[i]);
		}
		delete stream;
	}
	CHECK_END
}

// test_BufferedInStream()
void test_BufferedInStream(void) {
	CHECK_BEGIN("io::BufferedInStream");

	{
		InFileStream in1("test_io");
		InFileStream _in("test_io");
		BufferedInStream in2(_in, 128);
		char buf1[2], buf2[2];
		bool done = true;
		while(1) {
			int size1 = in1.read(buf1, 2);
			int size2 = in2.read(buf2, 2);
			done = size1 == size2;
			if(!done)
				break;
			if(size1 <= 0)
				break;
			done = buf1[0] == buf2[0] && buf1[1] == buf2[1];
			if(!done)
				break;
		}
		CHECK(done);
	}

	unsigned long sum1;
	{
		Fletcher sum;
		InFileStream in("test_io");
		CHECK(in.isReady());
		sum.put(in);
		sum1 = sum.sum();
	}
	unsigned long sum2;
	{
		Fletcher sum;
		InFileStream in("test_io");
		CHECK(in.isReady());
		BufferedInStream buf(in, 129);
		sum.put(buf);
		sum2 = sum.sum();
	}
	CHECK(sum1 == sum2);
	cerr << "sum1 = " << io::hex(sum1) << ", sum2 = " << io::hex(sum2) << io::endl;
	CHECK_END
}


// BufferedOutStream class
void test_BufferedOutStream(void) {
	CHECK_BEGIN("io::BufferedOutStream");

	// copy the file
	{
#		define BUF_SIZE 64
		char buf[BUF_SIZE];
		InStream *in = system::System::readFile("file.xml");
		OutStream *str = system::System::createFile("out.xml");
		BufferedOutStream out(*str, 32);
		int len;
		while((len = in->read(buf, system::System::random(BUF_SIZE))) > 0)
			out.write(buf, len);
	}

	// compare the file
	{
		bool ok = true;
		int res1, res2;
		char buf1[BUF_SIZE], buf2[BUF_SIZE];
		InStream *in1 = system::System::readFile("file.xml");
		InStream *in2 = system::System::readFile("out.xml");
		while(1) {
			res1 = in1->read(buf1, BUF_SIZE);
			res2 = in2->read(buf2, BUF_SIZE);
			if(res1 != res2)
				break;
			if(res1 == 0)
				break;
			if(memcmp(buf1, buf2, BUF_SIZE) != 0) {
				ok = false;
				break;
			}
		}
		CHECK(res1 == res2);
		CHECK(ok);
	}

	CHECK_END;
}


// BufferedOutStream class
void test_Input(void) {
	CHECK_BEGIN("io::BufferedOutStream");

#	define TINPUT(t, c) \
		{ \
			io::BlockInStream stream(#c); \
			io::Input in(stream); \
			t v; \
			in >> v; \
			CHECK(v == c); \
		}

	try {
		TINPUT(long, 666);
		TINPUT(long, -666);
		TINPUT(long, +666);
		TINPUT(unsigned long, 666);

		TINPUT(long, 0xff);
		TINPUT(long, -0xff);
		TINPUT(long, +0xff);
		TINPUT(unsigned long, 0xff);

		TINPUT(long long, 666);
		TINPUT(long long, -666);
		TINPUT(long long, +666);
		TINPUT(unsigned long long, 666);

		TINPUT(long long, 0xff);
		TINPUT(long long, -0xff);
		TINPUT(long long, +0xff);
		TINPUT(unsigned long long, 0xff);
	}
	catch(elm::Exception& e) {
		cerr << "ERROR: exception raised: " << e.message() << io::endl;
	}

	CHECK_END;
}


// StringInput class
void test_StringInput(void) {
	CHECK_BEGIN("io::StringInput");

	{
		StringInput in("128");
		t::int32 v;
		in >> v;
		CHECK_EQUAL(v, 128);
	}

	{
		string in = "128";
		t::int32 v;
		in >> v;
		CHECK_EQUAL(v, 128);
	}

	{
		string in = "0x80";
		t::int32 v;
		in >> v;
		CHECK_EQUAL(v, 0x80);
	}

	{
		string in = "aa";
		t::int32 v;
		CHECK_EXCEPTION(elm::io::IOException, in >> v);
	}

	{
		string in = "0xffffffffff";
		t::int32 v;
		CHECK_EXCEPTION(elm::io::IOException, in >> v);
	}

	CHECK_END;
}


int main(void) {
	test_RandomAccessStream();
	test_BufferedInStream();
	test_BufferedOutStream();
	test_Input();
	test_StringInput();
	return 0;
}

