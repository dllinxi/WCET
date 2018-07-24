#ifndef GEL_PUBLIC_H_
#define GEL_PUBLIC_H_

/*==============================================================================
 * gel_public.h
 *==============================================================================
 * \file This header contains the public definitions.
 * @author Sylvain Lemouzy (syl.lemouzy@free.fr)
 * @date 2006/08/02
 *==============================================================================
 */

#include <gel/common.h>
#include <gel/types.h>
#include "gel_base_public.h"
#include "gel_load_public.h"
#include "cursor.h"
#include <gel/file.h>
#include "util.h"

__BEGIN_DECLS

/*==============================================================================
 * Macro definitions for address conversion
 *==============================================================================
 */

/** Convert a virtual address in a relocated virtual address. */
#define GEL_VADDR2VRELOC(f,a) ((a) + (f).base_vaddr)

/** Convert a relocated virtual address in a simple virtual address. */
#define GEL_VRELOC2VADDR(f,a) ((a) - (f).base_vaddr)

/** Convert a virtual address in an actual address. */
#define GEL_VADDR2RADDR(f,a) ((a) + (u8_t *)(f).base_raddr)

/** Convert an actual address in a virtual address. */
#define GEL_RADDR2VADDR(f,a) ((u32_t)(a) - (f).base_raddr)

/** Convert an actual address in a relocated virtual address. */
#define GEL_VRELOC2RADDR(f,a) (VADDR2RADDR((f),VRELOC2VADDR((f),(a))))

/** Convert a relocated virtual address in an actual address. */
#define GEL_RADDR2VRELOC(f,a) (VADDR2VRELOC((f),RADDR2VADDR((f),(a))))


/* endianess management */
#define GEL_BIG_ENDIAN 2
#define GEL_LITTLE_ENDIAN 1


#define SWAP2(x) ((((x) & 0xFF) << 8) | (((x) & 0xFF00 ) >> 8))
#define SWAP4(x) ((((SWAP2((x) & 0xFFFF))) << 16) | SWAP2(((x) & 0xFFFF0000) >> 16))

#ifdef WORDS_BIGENDIAN
#define ENDIAN2(e,v) (((e) == GEL_BIG_ENDIAN) ? (v) : SWAP2(v))
#define ENDIAN4(e,v) (((e) == GEL_BIG_ENDIAN) ? (v) : SWAP4(v))
#else
#define ENDIAN2(e,v) (((e) == GEL_LITTLE_ENDIAN) ? (v) : SWAP2(v))
#define ENDIAN4(e,v) (((e) == GEL_LITTLE_ENDIAN) ? (v) : SWAP4(v))
#endif

__END_DECLS

#endif /*GEL_PUBLIC_H_*/
