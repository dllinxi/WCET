/*
 *	$Id$
 *	test for alloc module
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
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

#include <elm/alloc/StackAllocator.h>
#include <elm/system/System.h>
#include <elm/io.h>

using namespace elm;

int main(int argc, char **argv) {
	StackAllocator stack;
	for(int i = 0; i < 1000; i++) {
		elm::t::size size = system::System::random(4096);
		cout << "allocate(" << size << ") = " << stack.allocate(size) << io::endl;
	}
}
