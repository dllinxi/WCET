/*
 * $Id$
 * Copyright (c) 2005-07, IRIT- UPS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <gel/prog.h>
#include <gel/error.h>
#include "gel_private.h"

/**
 * @defgroup cursor		Block Cursor
 * 
 * @code
 * #include <gel/cursor.h>
 * @endcode
 * 
 * This group contains definitions of items used to handle cursor to read
 * or write in section and segments. The cursor perform two tasks:
 * @li accessing the buffer,
 * @li fixing values according file and current endianess.
 * 
 * Cursor may be obtained from:
 * @li image block - gel_block2cursor()
 * @li program header - gel_prog2cursor()
 * @li sections - gel_sect2cursor()
 * 
 * 
 * @warning For efficiency purpose, the cursor does not check if the accessed
 * data are in the buffer bounds.
 * @date	2012/01/07
 * @author	S. Lemouzy, H. Cassé
 */

/**
 * Create a cursor on the buffer represented by a program header (loading
 * the program header if needed).
 * @param p Program header.
 * @param c Cursor instance.
 * @return Error code (0 for success, -1 for failure).
 * @ingroup		phdr
 */
int gel_prog2cursor(gel_prog_t *p, gel_cursor_t *c) {
	gel_prog_s *pp = (gel_prog_s *)p;

	if (pp->data == NULL)
	{
		if (gel_prog_load(p, NULL) == -1)
		return -1;
	}
 	c->base_addr = pp->data;
	c->cur_addr = pp->data;
	c->plat = ((gel_file_s *)p->container)->plat;
	c->endianness = c->plat->endianness;
	c->size = p->memsz;
	c->vaddr = p->vaddr;
	return 0;
}


/**
 * Create a cursor on a memory block.
 * @param f Block descriptor.
 * @param c Cursor instance.
 * @return Error code (0 for success, -1 for failure)
 * @ingroup image
 */
int gel_block2cursor(gel_block_t *f, gel_cursor_t *c)
{
	assert(f->data != NULL);

	c->base_addr = f->data;
	c->cur_addr = f->data;
	c->plat = f->plat;
	c->endianness = f->plat->endianness;
	c->size = f->exec_size;
	c->vaddr = f->exec_begin /*->base_vaddr*/;
	return 0;
}


/**
 * Create a cursor on a section (loaded if needed).
 * @param s Section descriptor.
 * @param c Cursor instance.
 * @return	Error code (0 for success, -1 for failure)
 * @ingroup sect
 */
int gel_sect2cursor(gel_sect_t *s, gel_cursor_t *c)
{
	if (s->data == NULL)
	{
    	if (gel_sect_load(s, NULL) == -1)
    	{
      		return -1;
    	}
  	}

	c->base_addr = (u8_t*)s->data;
	c->cur_addr = (u8_t*)s->data;
	c->plat = ((gel_file_s *)s->container)->plat;
	c->endianness = c->plat->endianness;
	c->size = s->size;
	c->vaddr = s->vaddr;
	return 0;
}


/**
 * Move the cursor the position given by the offset from the start of the buffer.
 * @param p 	Cursor to set.
 * @param off 	Offset to move to.
 * @return 		Error code (0 for success, -1 for failure)
 * @ingroup cursor
 */
int gel_move(gel_cursor_t *p, u32_t off)
{
	p->cur_addr = p->base_addr + off;
	return 0;
}


/**
 * Move the cursor to an actual addresse (that must be in the cursor buffer).
 * @param p		Cursor to set.
 * @param addr	Actual address to set.
 * @return 		Error code (0 for success, -1 for failure)
 * @ingroup cursor
 */
int gel_move_abs(gel_cursor_t *p, raddr_t addr)
{
	p->cur_addr = addr;
	return 0;
}


/**
 * Move the cursor relatively to the current position.
 * @param p 	Cursot to set.
 * @param off	Offset to add to the current position (may be negative).
 * @return 		Error code (0 for success, -1 for failure)
 * @ingroup cursor
 */
int gel_move_rel(gel_cursor_t *p, s32_t off)
{
  u32_t abs_off = off + ((p->cur_addr) - (p->base_addr));
  p->cur_addr = p->base_addr + abs_off;
  return 0;
}


/**
 * Get the current actual address of the cursor.
 * @param c 	Cursor to get address of.
 * @return 		Current actual address.
 * @ingroup cursor
 */
raddr_t gel_cursor_addr(gel_cursor_t *c)
{
	return (c->cur_addr);
}


/**
 * Check if the current cursor is in the buffer bounds.
 * @param p	Current cursor.
 * @return	0 if cursor is in bounds, -1 else.
 * @ingroup cursor
 */
int gel_cursor_bounds(gel_cursor_t *p)
{
  if ( (p->cur_addr < p->base_addr) ||
       (p->cur_addr >= (p->base_addr + p->size-1)) )
  {
  	return -1;
  }
  else
  {
  	return 0;
  }
}


/**
 * Write a block in the cursor buffer with bound checking.
 * @param bc		Current cursor.
 * @param blk 		Base address of the block to copy.
 * @param blksize	Block size.
 * @param dest		Actual destination address.
 * @return 			Error code (0 for success, -1 for failure)
 * @ingroup cursor
 */
int gel_write_block(gel_cursor_t *bc, char *blk, int blksize, raddr_t dest)
{
  int i;

  gel_move_abs(bc, (u8_t*)dest + blksize - 1);
  if (gel_cursor_bounds(bc) == -1)
  {
    gel_errno = GEL_EFORMAT;
    return -1;
  }

  gel_move_abs(bc, dest);
  if (gel_cursor_bounds(bc) == -1)
  {
    gel_errno = GEL_EFORMAT;
    return -1;
  }

  for (i = 0; i < blksize; i++)
  {
    gel_write_u8(*bc, blk[i]);
  }

  return 0;
}


/**
 * Read a block from a cursor buffer at the given actual address with bound
 * checking.
 * @param bc 		Current cursor.
 * @param blk 		Base block to copy to.
 * @param blksize	Block size.
 * @param src 		Actual address in the buffer of the block to copy.
 * @return			Error code (0 for success, -1 for failure)
 * @ingroup cursor
 */
int gel_read_block(gel_cursor_t *bc, char *blk, int blksize, raddr_t src)
{
  int i;

  gel_move_abs(bc, (u8_t *) src + blksize - 1);
  if (gel_cursor_bounds(bc) == -1)
  {
    gel_errno = GEL_EFORMAT;
    return -1;
  }

  gel_move_abs(bc, src);
  if (gel_cursor_bounds(bc) == -1)
  {
    gel_errno = GEL_EFORMAT;
    return -1;
  }

  for (i = 0; i < blksize; i++)
  {
    blk[i] = gel_read_u8(*bc);
  }

  return 0;
}


/**
 * Align the cursor on the higher bound.
 * @param p 	Cursor to move.
 * @ingroup cursor
 */
void gel_align_hi(gel_cursor_t *p)
{
  intptr_t tmp = (intptr_t) p->cur_addr;

  if (p->plat->arch == NULL)
  {
    return;
  }

  if ((tmp % p->plat->arch->align) == tmp)
  {
    return;
  }

  tmp += p->plat->arch->align - (tmp % p->plat->arch->align);
  p->cur_addr = (void*) tmp;
}


/**
 * Align the cursor on the lower bound.
 * @param p 	Cursor to move.
 * @ingroup cursor
 */
void gel_align_lo(gel_cursor_t *p)
{
  intptr_t tmp = (intptr_t) p->cur_addr;
  if (p->plat->arch == NULL)
  {
    return ;
  }

  if ((tmp % p->plat->arch->align) == tmp)
  {
    return;
  }

  tmp -= (tmp % p->plat->arch->align);
  p->cur_addr = (void*) tmp;
}


/**
 * @def gel_cursor_at_end(c)
 * Test if the end of the buffer is reached.
 * @param c	Current cursor.
 * @return	1 if the end is reached, 0 else.
 * @ingroup cursor
 */


/**
 * @def gel_cursor_avail(c)
 * Compute the number of available bytes in the current block.
 * @param c	Used cursor.
 * @return	Available bytes count.
 * @ingroup cursor
 */


/**
 * @def gel_read_data(c, b, s)
 * Read a block of data.
 * @param c	Cursor to read from.
 * @param b	Byte buffer to write to.
 * @param s	Number of bytes to read.
 * @ingroup cursor
 */


/**
 * @def gel_read_u8(c)
 * Read an unsigned byte and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_read_s8(c)
 * Read a signed byte and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_read_u16(c)
 * Read an unsigned half-word and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_read_s16(c)
 * Read a signed half-word and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_read_u32(c)
 * Read an unsigned word and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_read_s32(c)
 * Read a signed word and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_read_vaddr(c)
 * Read a virtual address and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_read_raddr(c)
 * Read an actual address and increment the cursor.
 * @param c 	Used cursor.
 * @return		Read value.
 * @ingroup cursor
 */


/**
 * @def gel_write_u8(c,v)
 * Write an unsigned byte and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * @def gel_write_s8(c,v)
 * Write a signed byte and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * @def gel_write_u16(c,v)
 * Write an unsigned half-word and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * @def gel_write_s16(c,v)
 * Write a signed half-word and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * @def gel_write_u32(c,v)
 * Write an unsigned word and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * @def gel_write_s32(c,v)
 * Write a signed byte word and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * @def gel_write_vaddr(c,v)
 * Write a virtual address and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * @def gel_write_raddr(c,v)
 * Write an actual address and increment the cursor.
 * @param c Current cursor.
 * @param v Value to write.
 * @ingroup cursor
 */


/**
 * Build a cursor as sub-cursor on the buffer the base cursor.
 * @param base	Base cursor.
 * @param size	Size of the sub-buffer.
 * @param sub	New sub-cursor.
 * @return		0 for success, -1 if the size creates a sub-buffer out of bounds
 * 				of the enclosing buffer.
 * @ingroup cursor
 */
int gel_subcursor(gel_cursor_t *base, size_t size, gel_cursor_t *sub) {
	sub->base_addr = base->cur_addr;
	sub->cur_addr = base->cur_addr;
	sub->endianness = base->endianness;
	sub->plat = base->plat;
	sub->size = size;
	if(sub->cur_addr + size <= base->cur_addr + base->size)
		return 0;
	else
		return -1;
}

/**
 * @def gel_cursor_reset(c)
 * Reset the position of the cursor to the start.
 * @param c	Cursor to reset.
 * @ingroup cursor
 */


/**
 * @def GEL_ALIGN(v, n)
 * Align an integer value to an upper bound multiple of v.
 * @param v		Integer value to align.
 * @param n		Multiple to align to (must be a power of 2).
 * @return		v aligned to n.
 * @ingroup cursor
 */


/**
 * @def GEL_ALIGNP(t, p, n)
 * Align a real pointer value to an upper bound multiple of v.
 * @param t		Pointed type.
 * @param p		Real pointer value to align.
 * @param n		Multiple to align to (must be a power of 2).
 * @return		p aligned to n.
 * @ingroup cursor
 */


/**
 * @typedef gel_cursor_t
 * Datastructure to represent a cursor on  memory block,
 * allowing to read or write in it (independently of the endianness
 * of executable or host machine).
 * @ingroup cursor
 */

/**
 * @var gel_cursor_t::base_addr
 * Base address of the block buffer.
 */

/**
 * @var gel_cursor_t::cur_addr
 * Current address.
 */

/**
 * @var gel_cursor_t::size
 * Size of the block.
 */

/**
 * @var gel_cursor_t::endianness
 * Endianness of the block.
 */

/**
 * @var gel_cursor_t::vaddr;
 * Virtual address of the block.
 */

/**
 * @var gel_cursor_t::plat
 * Platform of the block.
 */
