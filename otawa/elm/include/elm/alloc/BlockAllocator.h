/*
 *	$Id$
 *	BlockAllocator class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 20010, IRIT UPS.
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
#ifndef ELM_ALLOC_BLOCKALLOCATOR_H_
#define ELM_ALLOC_BLOCKALLOCATOR_H_

#include <elm/alloc/StackAllocator.h>

namespace elm {

// BlockAllocator class
class BlockAllocator {
public:
	BlockAllocator(int block_size, int block_per_chunk = 16);
	void *allocate(void) throw(BadAlloc);
	void free(void *block);

private:
	typedef struct block_t {
		struct block_t *next;
	} block_t;

	int size;
	StackAllocator alloc;
	block_t *list;
};

} // elm

#endif /* ELM_ALLOC_BLOCKALLOCATOR_H_ */
