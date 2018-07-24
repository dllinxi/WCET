/*
 *	stree module test
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2013, IRIT UPS.
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
#define ELM_STREE_DEBUG
#include <elm/stree/MarkerBuilder.h>
#include <elm/stree/SegmentBuilder.h>

using namespace elm;
using namespace elm::stree;

typedef enum {
	NONE = 0,
	ARM = 1,
	THUMB = 2,
	DATA = 3
} area_t;

io::Output& operator<<(io::Output& out, area_t area) {
	switch(area) {
	case NONE:	out << "none"; break;
	case ARM:	out << "arm"; break;
	case THUMB:	out << "thumb"; break;
	case DATA:	out << "data"; break;
	}
	return out;
}

typedef t::uint32 addr_t;
io::Output& operator<<(io::Output& out, addr_t addr) {
	out << io::hex(addr);
	return out;
}

TEST_BEGIN(stree)

	MarkerBuilder<addr_t, area_t> builder;
	Tree<addr_t, area_t> tree;

	// store the values
	Pair<t::uint32, area_t> marks[] = {
		pair(0U, ARM),
		pair(0x000080d8U, ARM),
		pair(0x0000849cU, ARM),
		pair(0x000084a0U, ARM),
		pair(0x000104b0U, DATA),
		pair(0x00008258U, DATA),
		pair(0x00008484U, DATA),
		pair(0x000105c0U, DATA),
		pair(0x000105b0U, DATA),
		pair(0x000084a8U, DATA),
		pair(0x0000825cU, THUMB),
		pair(0x00008498U, THUMB),

		pair(0x00008000U, ARM),
		pair(0x0000a910U, ARM),
		pair(0x00008020U, ARM),
		pair(0x000080a8U, ARM),
		pair(0x0000a91cU, ARM),
		pair(0x00008104U, ARM),
		pair(0x0000800cU, ARM),
		pair(0x0000a8c0U, ARM),
		pair(0x0000a8f4U, ARM),
		pair(0x00008010U, ARM),
		pair(0x00008014U, ARM),
		pair(0x0000a920U, ARM),
		pair(0x0000810cU, ARM),
		pair(0x00008218U, ARM),
		pair(0x000085c8U, ARM),
		pair(0x000085dcU, ARM),
		pair(0x00008610U, ARM),
		pair(0x00008650U, ARM),
		pair(0x000086ccU, ARM),
		pair(0x000087bcU, ARM),
		pair(0x00008864U, ARM),
		pair(0x00008910U, ARM),
		pair(0x000089d0U, ARM),
		pair(0x00008accU, ARM),
		pair(0x00008ba8U, ARM),
		pair(0x00008c78U, ARM),
		pair(0x00008d24U, ARM),
		pair(0x00008d70U, ARM),
		pair(0x00008d94U, ARM),
		pair(0x00008df0U, ARM),
		pair(0x00009094U, ARM),
		pair(0x00009180U, ARM),
		pair(0x000092f0U, ARM),
		pair(0x00009300U, ARM),
		pair(0x0000931cU, ARM),
		pair(0x00009394U, ARM),
		pair(0x000093e8U, ARM),
		pair(0x00009458U, ARM),
		pair(0x00009570U, ARM),
		pair(0x00009598U, ARM),
		pair(0x000095b0U, ARM),
		pair(0x000096b4U, ARM),
		pair(0x00009dd0U, ARM),
		pair(0x00009dd8U, ARM),
		pair(0x00009e10U, ARM),
		pair(0x00009ed4U, ARM),
		pair(0x00009f34U, ARM),
		pair(0x00009f74U, ARM),
		pair(0x00009facU, ARM),
		pair(0x0000a0b4U, ARM),
		pair(0x0000a350U, ARM),
		pair(0x0000a448U, ARM),
		pair(0x0000a6a8U, ARM),
		pair(0x0000a6e8U, ARM),
		pair(0x0000a728U, ARM),
		pair(0x0000a84cU, ARM),
		pair(0x0000a8fcU, ARM),
		pair(0x0000a90cU, ARM),
		pair(0x0000000cU, DATA),
		pair(0x00012950U, DATA),
		pair(0x0001293cU, DATA),
		pair(0x00012944U, DATA),
		pair(0x00008090U, DATA),
		pair(0x000080f0U, DATA),
		pair(0x000133b4U, DATA),
		pair(0x000133b0U, DATA),
		pair(0x00012940U, DATA),
		pair(0x00012948U, DATA),
		pair(0x0000a938U, DATA),
		pair(0x0001294cU, DATA),
		pair(0x0000a8f0U, DATA),
		pair(0x00000354U, DATA),
		pair(0x00008200U, DATA),
		pair(0x00012968U, DATA),
		pair(0x00000034U, DATA),
		pair(0x00012a68U, DATA),
		pair(0x0000838cU, DATA),
		pair(0x000085b4U, DATA),
		pair(0x000133d4U, DATA),
		pair(0x00012b68U, DATA),
		pair(0x00000010U, DATA),
		pair(0x0000860cU, DATA),
		pair(0x00000030U, DATA),
		pair(0x00008648U, DATA),
		pair(0x00000058U, DATA),
		pair(0x0000a92cU, DATA),
		pair(0x00012b78U, DATA),
		pair(0x0000a930U, DATA),
		pair(0x000086bcU, DATA),
		pair(0x00000084U, DATA),
		pair(0x000000b0U, DATA),
		pair(0x0000885cU, DATA),
		pair(0x00008908U, DATA),
		pair(0x000089c8U, DATA),
		pair(0x00008ac4U, DATA),
		pair(0x00008ba0U, DATA),
		pair(0x00008c70U, DATA),
		pair(0x00008d1cU, DATA),
		pair(0x00008d68U, DATA),
		pair(0x00008d90U, DATA),
		pair(0x00008de8U, DATA),
		pair(0x0000a934U, DATA),
		pair(0x000136d8U, DATA),
		pair(0x000000d8U, DATA),
		pair(0x00009178U, DATA),
		pair(0x000003ccU, DATA),
		pair(0x000092e8U, DATA),
		pair(0x00000400U, DATA),
		pair(0x000092fcU, DATA),
		pair(0x00000438U, DATA),
		pair(0x00009318U, DATA),
		pair(0x00009384U, DATA),
		pair(0x000093e4U, DATA),
		pair(0x00009454U, DATA),
		pair(0x0000956cU, DATA),
		pair(0x00009590U, DATA),
		pair(0x000095a8U, DATA),
		pair(0x00000458U, DATA),
		pair(0x00000574U, DATA),
		pair(0x00009dc4U, DATA),
		pair(0x00012fa0U, DATA),
		pair(0x00013788U, DATA),
		pair(0x000005c8U, DATA),
		pair(0x00000600U, DATA),
		pair(0x00009e0cU, DATA),
		pair(0x00000630U, DATA),
		pair(0x0000065cU, DATA),
		pair(0x000006dcU, DATA),
		pair(0x00009f70U, DATA),
		pair(0x000006fcU, DATA),
		pair(0x00009fa8U, DATA),
		pair(0x00000728U, DATA),
		pair(0x0000a0b0U, DATA),
		pair(0x00000754U, DATA),
		pair(0x0000a344U, DATA),
		pair(0x00000790U, DATA),
		pair(0x0000a43cU, DATA),
		pair(0x0000a694U, DATA),
		pair(0x000007d0U, DATA),
		pair(0x0000a6e4U, DATA),
		pair(0x00000820U, DATA),
		pair(0x0000a724U, DATA),
		pair(0x0000084cU, DATA),
		pair(0x0000a848U, DATA),
		pair(0x0000a8bcU, DATA),
		pair(0x00000878U, DATA),
		pair(0x0000a904U, DATA),
		pair(0x00008390U, THUMB),
		pair(0x0000a908U, THUMB),
		pair(0xffffffff, ARM)
	};
	int marks_count = sizeof(marks) / sizeof(Pair<t::uint32, area_t>);
	for(int i = 0; i < marks_count; i++)
		builder.add(marks[i].fst, marks[i].snd);
	builder.make(tree);
	//tree.dump();

	// test start of content
	for(int i = 0; i < marks_count - 1; i++)
		CHECK_EQUAL(tree.get(marks[i].fst, NONE), marks[i].snd);

	// test middle of the content
	for(int i = 0; i < marks_count - 1; i++)
		CHECK_EQUAL(tree.get(marks[i].fst + 1, NONE), marks[i].snd);

	// test the segment builder
	{
		SegmentBuilder<int, int> sbuilder(0);
		sbuilder.add(1000, 2000, 1);
		sbuilder.add(3000, 4000, 2);
		sbuilder.add(4000, 5000, 3);
		sbuilder.add(10000, 11000, 4);
		sbuilder.add(11000, 13000, 5);
		sbuilder.add(13500, 14000, 6);
		Tree<int, int> tree;
		sbuilder.make(tree);
		CHECK_EQUAL(tree.get(500, 0), 0);
		CHECK_EQUAL(tree.get(1000, 0), 1);
		CHECK_EQUAL(tree.get(1500, 0), 1);
		CHECK_EQUAL(tree.get(2500, 0), 0);
	}

TEST_END
