/*
 *	$Id$
 *	Main documentation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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
#ifndef ELM_DOC_H_
#define ELM_DOC_H_

/**
 * @mainpage
 * 
 * ELM is a multi-purpose library used by the OTAWA project (http://www.otawa.fr).
 * It provides :
 * @li generic data structures,
 * @li generic interface with te operating system (files, command line, etc),
 * @li a checked plugin system,
 * @li efficient string class,
 * @li other utility classes.
 * 
 * Please take a look at the module part for detailed description.
 * 
 * @par Type Facilities
 * 
 * ELM provides a lot of OS-independent simple and reliable types in @ref int
 * together with efficient complex integer functions.
 * 
 * The @ref string module provides several class to manage strings:
 * C-string wrapper (@ref elm::CString), mostly 0 cost substring extraction,
 * fast and light string class  (@ref elm::String), string buffer (@ref elm::StringBuffer) to build strings
 * and automatic string building class (@ref elm::AutoString) with very handy syntax (@c _ << a1 << a2 << ...).
 * 
 * Facilities to handle efficiently are also provided in @ref array.
 *
 * The @ref types module provides RTTI support to handle types easily.
 * Used with the <elm/meta.h> primitives, it allows to build
 * powerful template classes and functions.
 * 
 * @par Data Structures
 * 
 * One goal of ELM is to provide very powerful and efficient data structures.
 * They includes:
 * @li array management (@ref elm::genstruct::Vector, @ref elm::genstruct::AllocatedTable, @ref elm::genstruct::FixArray, @ref elm::genstruct::FragTable, @ref elm::genstruct::Table),
 * @li list management (@ref elm::genstruct::SLList, @ref elm::genstruct::DLList),
 * @li tree management (@ref elm::avl::Tree, @ref elm::genstruct::SortedBinTree, @ref elm::genstruct::Tree),
 * @li queue management (@ref elm::genstruct::VectorQueue, @ref elm::genstruct::DLList),
 * @li stack management (@ref elm::genstruct::Vector),
 * @li set management (@ref elm::avl::Set),
 * @li map management (@ref elm::genstruct::HashTable, @ref elm::avl::Map, @ref elm::genstruct::AssocList, @ref elm::genstruct::SortedBinMap, @ref elm::stree::Tree).
 * 
 * All these classes share the same syntax for iterators:
 * @code
 * 	DataStructure<MyType> data_structure;
 * 	...
 * 	for(DayaStructure<MyType>::Iterator iter; iter; iter++)
 * 		process(*iter);
 * @endcode
 * 
 * A whole bunch of ordered sets comparators (@ref elm::Comparator) or hashing algorithms (@ref elm::HashKey)
 * are proposed by ELM but they can also be specialized for your own types.
 * 
 * To make easier the use of data structure and allows inter-operations, each one implements
 * a set of interfaces known in C++ as concepts (module @ref concepts).
 *
 * Finally, ELM provides convenient classes for data handling: @ref elm::Pair, @ref elm::Option.
 *
 * @par Input-Output System
 * 
 * The @ref ios module is split in two layers. The higher layer
 * (classes @ref elm::io::Output and @ref elm::io::Input) are responsible
 * for format-aware input / output (mostly equivalent to STL istream / ostream).
 * They provide formatter class (@ref elm::io::IntFormat and @ref elm::io::FloatFormat)
 * to fine-tune their work. The IO errors are processed by rising exceptions.
 * 
 * The lower layer handles input-output as a stream of bytes but the provided
 * classes allows to pipe efficiently these classes together, offering 
 * a significant potential of control on the streams. All stream classes
 * implement either the @ref elm::io::InStream or @ref elm::io::OutStream
 * and supports error handling through synchronized error return.
 * Both allows to get the OS-error message from getLastError() method.
 * 
 * Basically, input stream and output stream gives access to the OS streams
 * but other classes allows to buffer IO (@ref elm::io::BufferedInStream or @ref elm::io::BufferedOutStream),
 * to use block of data as streams (@ref elm::io::BlockInStream, @ref elm::io::BlockOutStream),
 * to compute checksums (@ref elm::checksum::Fletcher; @ref elm::checksum::MD5). In addition,
 * input and output stream can piped with the @ref elm::io::StreamPipe or diverted with @ref elm::io::TeeOutStream.
 * 
 *
 * @par Portability Features
 * 
 * ELM implements also several classes to abstract the host OS. The following
 * features have been tested on Linux, Windows and MacOSX:
 * @li access to generic features of the system (@ref elm::sys::System)
 * @li process management (@ref elm::sys::Process),
 * @li stop watch implementation (@ref elm::sys::StopWatch),
 * @li file items (@ref elm::sys::FileItem),
 * @li command line arguments processing (@ref options module),
 * @li portable plugin system (@ref plugins module).
 *
 * In the future, classes to handle threads and sockets will be added.
 *
 * @par Memory Management
 *
 * ELM supplies several classes to handle allocation and de-allocation:
 * @li default allocation scheme (@ref elm::DefaultAllocator),
 * @li allocate from a list of fixed size (@ref elm::BlockAllocator),
 * @li stack allocation with backtrack (@ref elm::StackAllocator),
 * @li semi-automatic specialized garbage collector (@ref elm::AbstractBlockAllocatorWithGC).
 *
 * And some cleanup classes or pointer management classes (@ref elm::AutoCleaner, @ref elm::AutoDestructor, @ref AutoPtr).
 *
 * @par Other Facilities
 * @li crash management (@ref elm::CrashHandler, only for Linux),
 * @li XML and XSLT using the XOM interface (@ref xom),
 * @li basic exceptions with message (@ref elm::Exception and @ref elm::MessageException),
 * @li very light test framework (@ref elm::TestCase and @ref elm::TestSet),
 * @li automatic serialization and unserialization (@ref serial),
 * @li work-around the C++ initialization fiasco (@ref elm::Initializer),
 * bit management (@ref elm::BitVector).
 */

/**
 * @defgroup utilities Development Utilities
 *
 * The entities defined in this section are targeted to help development and testing instead
 * of representing usual classes embedded in your application.
 *
 * @section deprecated Deprecation Management
 *
 * ELM provides a small macro found in <elm/deprecated.h>, called DEPRECATED.
 *
 * When put at the start of a function body, it displays a warning deprecation
 * message the first time it is called.
 *
 * @code
 * #include <elm/deprecated.h>
 *
 * class MyClass {
 * public:
 *
 * 	void myDeprecatedFunction(void) {
 * 		DEPRECATED
 * 		// body of the function
 * 	}
 * @endcode
 *
 * @section helper	Helper Methods
 *
 * ELM provides several helper method for using the different classes. They are prefixed by a double underscore and
 * all only compiled if a matching define is not provided.
 *
 * @li @c __size(): gives the full size of the data structure (removed if @c NSIZE is defined)
 * @li @c __print(): display an internal representation of the class (removed if @c NDEBUG is defined)
 *
 * @section test	Testing
 *
 * This module provides a poor, but existing, solution to unit testing in C++.
 * It provides macros and classes to perform statistics on unit testing
 * and makes automatic some test procedures.
 *
 * The example show how the module works:
 * @code
 * #include <elm/util/test.h>
 *
 * void my_test(void) {
 * 		CHECK_BEGIN("my test")
 *
 * 			// test 1
 * 			CHECK(result1 == expected1);
 *
 * 			// test 2
 * 			CHECK(result2 == expected2);
 *
 * 			...
 *
 * 		CHECK_END
 * }
 * @endcode
 *
 * The test must be enclosed in @c CHECK_BEGIN / @c CHECK_END pair and the test case name must be given.
 * Then the test are performed ending with a call to a macro allowing to test and record result of the test.
 *
 * The existing macro are:
 * @li @c CHECK(x) -- check if x is true
 * @li @c CHECK_EQUAL(result, expected) -- test equality and, if it fails, display the failed result value
 * @li @c CHECK_EXCEPTION(exception, action) -- perform the action and the test fails if the exception is not thrown
 * @li @c FAIL_ON_EXCEPTION(exception, action) -- perform the action and the test fails if the exception is thrown
 *
 * There are also some useful macros as:
 * @li @c REQUIRE(cnd, action) -- perform the action only if the given condition is true, else stop the test.
 *
 */

#endif /*ELM_DOC_H_*/
