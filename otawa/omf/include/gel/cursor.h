/*
 * $Id$
 * Copyright (c) 2005-11, IRIT- UPS
 *
 * GEL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GEL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GEL; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GEL_CURSOR_H_
#define GEL_CURSOR_H_

#include <assert.h>
#include <string.h>
#include <gel/common.h>
#include <gel/sect.h>

__BEGIN_DECLS

#define GEL_ALIGN(v, n)		(((v) + (n) - 1) & ~((n) - 1))
#define GEL_ALIGNP(t, p, n)	((t *)GEL_ALIGN((intptr_t)(p), (n)))

typedef struct gel_cursor_s {
  u8_t *base_addr;
  u8_t *cur_addr;
  u32_t	size;
  int endianness;
  vaddr_t vaddr;
  gel_platform_t *plat;
} gel_cursor_t;


/* Macros */
#define gel_cursor_at_end(c)	((c).cur_addr >= (c).base_addr + (c).size)
#define gel_cursor_avail(c)		((c).base_addr + (c).size - (c).cur_addr)
#define gel_cursor_align(c, n)	(c).cur_addr = GEL_ALIGNP(u8_t, (c).cur_addr, (n))
#define gel_cursor_reset(c)		{ (c).cur_addr = (c).base_addr; }
#define gel_cursor_vaddr(c)		((c).vaddr + (c).cur_addr - (c).base_addr)

#define gel_read_u8(c) ((c).cur_addr += 1 , *((u8_t*)((c).cur_addr - 1)))
#define gel_read_s8(c) ((c).cur_addr += 1 , *((s8_t*)((c).cur_addr - 1)))
#define gel_read_u16(c) ((c).cur_addr += 2, ENDIAN2((c).endianness, (*((u16_t*)((c).cur_addr - 2)))))
#define gel_read_s16(c) ((c).cur_addr += 2, ENDIAN2((c).endianness, (*((s16_t*)((c).cur_addr - 2)))))
#define gel_read_u32(c) ((c).cur_addr += 4, ENDIAN4((c).endianness, (*((u32_t*)((c).cur_addr - 4)))))
#define gel_read_s32(c) ((c).cur_addr += 4, ENDIAN4((c).endianness, (*((s32_t*)((c).cur_addr - 4)))))
#define gel_read_vaddr(c) ((c).cur_addr += 4, ENDIAN4((c).endianness, (*((vaddr_t*)((c).cur_addr - 4)))))
#define gel_read_raddr(c) ((c).cur_addr += 4, ENDIAN4((c).endianness, (*((raddr_t*)((c).cur_addr - 4)))))
#define gel_read_data(c, b, s) { memcpy((b), (c).cur_addr, (s)); (c).cur_addr += (s); }

#define gel_write_u8(c,v) {*((u8_t*)((c).cur_addr)) = (v) ; (c).cur_addr += 1; } while (0)
#define gel_write_s8(c,v) {*((s8_t*)((c).cur_addr)) = (v) ; (c).cur_addr += 1; } while (0)
#define gel_write_u16(c,v) {*((u16_t*)((c).cur_addr)) = ENDIAN2((c).endianness,(v)) ; (c).cur_addr += 2; } while (0)
#define gel_write_s16(c,v) {*((s16_t*)((c).cur_addr)) = ENDIAN2((c).endianness,(v)) ; (c).cur_addr += 2; } while (0)
#define gel_write_u32(c,v) {*((u32_t*)((c).cur_addr)) = ENDIAN4((c).endianness,(v)) ; (c).cur_addr += 4; } while (0)
#define gel_write_s32(c,v) {*((s32_t*)((c).cur_addr)) = ENDIAN4((c).endianness,(v)) ; (c).cur_addr += 4; } while (0)
#define gel_write_vaddr(c,v) {*((vaddr_t*)((c).cur_addr)) = ENDIAN4((c).endianness,(v)) ; (c).cur_addr += 4; } while (0)
#define gel_write_raddr(c,v) {*((raddr_t*)((c).cur_addr)) = ENDIAN4((c).endianness,(v)) ; (c).cur_addr += 4; } while (0)
#define gel_write_data(c, b, s) \
	{ assert((c).cur_addr + (s) < (c).base_addr + (c).size); \
	memcpy((c).cur_addr, (b), (s)); (c).cur_addr += (s); }


/* Functions */
int gel_block2cursor(gel_block_t *f, gel_cursor_t *c);
int gel_sect2cursor(gel_sect_t *p, gel_cursor_t *c);
int gel_subcursor(gel_cursor_t *base, size_t size, gel_cursor_t *sub);
int gel_move(gel_cursor_t *p, u32_t off);
int gel_move_abs(gel_cursor_t *p, raddr_t addr);
int gel_move_rel(gel_cursor_t *p, s32_t off);
raddr_t gel_cursor_addr(gel_cursor_t *c);
int gel_cursor_bounds(gel_cursor_t *p);
int gel_write_block(gel_cursor_t *bc, char *blk, int blksize, raddr_t dest);
int gel_read_block(gel_cursor_t *bc, char *blk, int blksize, raddr_t src);
void gel_align_hi(gel_cursor_t *p);
void gel_align_lo(gel_cursor_t *p);

__END_DECLS

#endif /* GEL_CURSOR_H_ */
