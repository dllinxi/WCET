/*
 *	$Id: grt.h,v 1.3 2009/07/08 12:09:19 barre Exp $
 *	gliss declarations
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
 *
 *	GLISS2 is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	GLISS2 is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with GLISS2; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PPC_GRT_H
#define PPC_GRT_H

#include <stdint.h>

/* module */
#define PPC_GRT_STATE
#define PPC_GRT_INIT(s)
#define PPC_GRT_DESTROY(s)

/* compatibility */
#ifndef INLINE
#	if __GNUC__
#		if !__GNUC_STDC_INLINE__
#			define INLINE extern inline
#		else
#			define INLINE inline
#		endif
#	else
#		ifdef _MSC_VER
#			define INLINE __forceinline
#		else
#			define INLINE	inline
#		endif
#	endif
#endif

/* useful */
uint32_t ppc_f2bits(float f);
uint64_t ppc_d2bits(double f);

/* extensions */
#define ppc_ext(cs, vs, v)	(((v) << (cs - vs)) >> (cs  - vs))

/*	Notations :
 * - v  : value
 * - u  : uppermost (meaning the most significant bit)
 * - l  : lowermost (meaning the least significant bit)
 * - s  : set value (the field defined by [u l] in v is set to s)
 * - n  :
 * - r  : rotate    (number of times we rotate v)
 * - bo : bit order ()
 * */

/*	Fast modulo : FMOD(A, PW2) <=> A % PW2
	PW2 must be a power two */
#define FMOD(A, PW2) ((A) & ((PW2) - 1u))

/* rotations */
#define ppc_mask32(n)	((n) == 32 ? (-1Lu) : (1Lu << (n)) - 1)
#define ppc_mask64(n)	((n) == 64 ? (-1LLu) : (1LLu << (n)) - 1)

/* returns the l lowest bits of v rotated by r positions,
 * result is l bit long (with leading 0s if needed) */
uint32_t ppc_rotate_left32(int l, uint32_t v, int r);
uint32_t ppc_rotate_left8(int l, uint32_t v, int r);
uint32_t ppc_rotate_left16(int l, uint32_t v, int r);
uint64_t ppc_rotate_left64(int l, uint64_t v, int r);

uint32_t ppc_rotate_right32(int l, uint32_t v, int r);
uint32_t ppc_rotate_right8(int l, uint32_t v, int r);
uint32_t ppc_rotate_right16(int l, uint32_t v, int r);
uint64_t ppc_rotate_right64(int l, uint64_t v, int r);

/* concatenation */
#define ppc_concat32(v1, v2, n1, n2)  ((((uint32_t)(v1) << n2) | ((v2) & ppc_mask32(n2))))
#define ppc_concat8(v1, v2, n1, n2) 	((uint8_t) ppc_concat32(v1, v2, n1, n2))
#define ppc_concat16(v1, v2, n1, n2) 	((uint16_t)ppc_concat32(v1, v2, n1, n2))
#define ppc_concat64(v1, v2, n1, n2)  ((((uint64_t)(v1) << n2) | ((v2) & ppc_mask64(n2))))

/* exponent */
#define ppc_exp8(v1, v2) 		((int8_t)  ppc_exp32(v1, v2))
#define ppc_exp8u(v1, v2) 	((uint8_t) ppc_exp32u(v1, v2))
#define ppc_exp16(v1, v2) 	((int16_t) ppc_exp32(v1, v2))
#define ppc_exp16u(v1, v2) 	((uint16_t)ppc_exp32u(v1, v2))
int32_t ppc_exp32(int32_t v1, int32_t v2);
uint32_t ppc_exp32u(uint32_t v1, uint32_t v2);
int64_t ppc_exp64(int64_t v1, int64_t v2);
uint64_t ppc_exp64u(uint64_t v1, uint64_t v2);


/* for the field modification or extraction functions, there are 3 family:
 *  - function name not suffixed, used when field's bounds are known (constant)
 * and in the same order as specified by the nml var bit_order (no bit inversion needed)
 *  - <name>_inverted, bounds are known and order is different from bit_order (inversion needed)
 *  - <name>_generic, bounds known or not, will call a function from 1 of the 2 previous family
 * depending on the order of the bounds and bit_order.
 * Convention for bit_order:
 *  - 0 => lowermost
 *  - !=0 => uppermost
 */

/* simple set bit field (value to modify, set value, upper bits, lower bit) */
#ifndef COMPAT
INLINE uint32_t ppc_set_field32u(uint32_t v, uint32_t s, int32_t u, int32_t l) {
    uint32_t mask = ppc_mask32(u - l + 1) << l;
    return (v & ~mask) | ((s << l) & mask);
}
#endif

#define ppc_set_field8(  v, s, u, l) 	((int8_t)  ppc_set_field32u((v), (s), (u), (l)))
#define ppc_set_field8u( v, s, u, l) 	((uint8_t) ppc_set_field32u((v), (s), (l), (u)))
#define ppc_set_field16( v, s, u, l) 	((int16_t) ppc_set_field32u((v), (s), (u), (l)))
#define ppc_set_field16u(v, s, u, l) 	((uint16_t)ppc_set_field32u((v), (s), (l), (u)))
#define ppc_set_field32( v, s, u, l) 	((int32_t) ppc_set_field32u((v), (s), (u), (l)))
float ppc_set_fieldf(float v, uint32_t s, int32_t u, int32_t l);
#define ppc_set_field64(v, s, u, l)	((int64_t) ppc_set_field64u(v, s, u, l))
uint64_t ppc_set_field64u(uint64_t v, uint64_t s, int32_t u, int32_t l);
double ppc_set_fieldd(double v, uint64_t s, int32_t u, int32_t l);

/* inverted set bit field (value to modify, set value, upper bits, lower bit) */
#define ppc_set_field8_inverted(v, s, u, l) 		((int8_t)ppc_set_field32u_inverted(v, s, u, l))
#define ppc_set_field8u_inverted(v, s, u, l) 		((uint8_t)ppc_set_field32u_inverted(v, s, l, u))
#define ppc_set_field16_inverted(v, s, u, l) 		((int16_t)ppc_set_field32u_inverted(v, s, u, l))
#define ppc_set_field16u_inverted(v, s, u, l) 	((uint16_t)ppc_set_field32u_inverted(v, s, l, u))
#define ppc_set_field32_inverted(v, s, u, l) 		((int32_t)ppc_set_field32u_inverted(v, s, u, l))
uint32_t ppc_set_field32u_inverted(uint32_t v, uint32_t s, int32_t u, int32_t l);
#define ppc_set_field64_inverted(v, s, u, l) 		((int64_t)ppc_set_field64u_inverted(v, s, u, l))
uint64_t ppc_set_field64u_inverted(uint64_t v, uint64_t s, int32_t u, int32_t l);
float ppc_set_fieldf_inverted(float v, uint32_t s, int32_t u, int32_t l);
double ppc_set_fieldd_inverted(double v, uint64_t s, int32_t u, int32_t l);

/* generic set bit field (value to modify, set value, upper bits, lower bit) */
#define ppc_set_field8_generic(v, s, a, b, bo) 	((int8_t)  ppc_set_field32u_generic(v, s, a, b, bo))
#define ppc_set_field8u_generic(v, s, a, b, bo) 	((uint8_t) ppc_set_field32u_generic(v, s, a, b, bo))
#define ppc_set_field16_generic(v, s, a, b, bo) 	((int16_t) ppc_set_field32u_generic(v, s, a, b, bo))
#define ppc_set_field16u_generic(v, s, a, b, bo)	((uint16_t)ppc_set_field32u_generic(v, s, a, b, bo)
#define ppc_set_field32_generic(v, s, a, b, bo) 	((int32_t) ppc_set_field32u_generic(v, s, a, b, bo))
uint32_t ppc_set_field32u_generic(uint32_t v, uint32_t s, int32_t a, int32_t b, int bit_order);
#define ppc_set_field64_generic(v, s, a, b, bo) 	((int64_t) ppc_set_field64u_generic(v, s, a, b, bo))
uint64_t ppc_set_field64u_generic(uint64_t v, uint64_t s, int32_t a, int32_t b, int bit_order);
float ppc_set_fieldf_generic(float v, uint32_t s, int32_t a, int32_t b, int bit_order);
double ppc_set_fieldd_generic(double v, uint64_t s, int32_t a, int32_t b, int bit_order);

/* single bit field (value, but) */
#define ppc_bit(v, b)		(((v) >> (b)) & 0x1)
#define ppc_bit8(v, b)	ppc_bit(v, b)
#define ppc_bit8u(v, b)	ppc_bit(v, b)
#define ppc_bit16(v, b)	ppc_bit(v, b)
#define ppc_bit16u(v, b)	ppc_bit(v, b)
#define ppc_bit32(v, b)	ppc_bit(v, b)
#define ppc_bit32u(v, b)	ppc_bit(v, b)
#define ppc_bit32f(v, b)	ppc_bit(ppc_f2bits((v)), b)
#define ppc_bit64(v, b)	ppc_bit(v, b)
#define ppc_bit64u(v, b)	ppc_bit(v, b)
#define ppc_bitf(v, b)	ppc_bit32f(v, b)
#define ppc_bitd(v, b)	ppc_bit(ppc_d2bits((v)), b)

/* simple field access (value, upper bit, lower bit) */
#define ppc_field32u(v, u, l)	((uint32_t)(v & (ppc_mask32(u - l + 1) << l)) >> l)
#define ppc_field64u(v, u, l)	((uint64_t)(v & (ppc_mask64(u - l + 1) << l)) >> l)
#define ppc_field8(v, u, l)	ppc_field32u((v), (u), (l))
#define ppc_field8u(v, u, l)	ppc_field32u((v), (u), (l))
#define ppc_field16(v, u, l)	ppc_field32u((v), (u), (l))
#define ppc_field16u(v, u, l)	ppc_field32u((v), (u), (l))
#define ppc_field32(v, u, l)	ppc_field32u((v), (u), (l))
#define ppc_fieldf(v, u, l)	ppc_field32u(ppc_f2bits((v)), (u), (l))
#define ppc_field64(v, u, l)	ppc_field64u((v), (u), (l))
#define ppc_fieldd(v, u, l)	ppc_field64u(ppc_d2bits((v)), (u), (l))

/* field access with inversion (value, upper bit, lower bit) */
#define ppc_field8_inverted(  v, u, l)	((int8_t)  ppc_field32u_inverted(v, u, l))
#define ppc_field8u_inverted( v, u, l)	((uint8_t) ppc_field32u_inverted(v, u, l))
#define ppc_field16_inverted( v, u, l)	((int16_t) ppc_field32u_inverted(v, u, l))
#define ppc_field16u_inverted(v, u, l)	((uint16_t)ppc_field32u_inverted(v, u, l))
#define ppc_field32_inverted( v, u, l)	((int32_t) ppc_field32u_inverted(v, u, l))
uint32_t ppc_field32u_inverted(uint32_t v, uint32_t u, uint32_t l);
#define ppc_field64_inverted(v, u, l)		((int64_t) ppc_field64u_inverted(v, u, l))
uint64_t ppc_field64u_inverted(uint64_t v, uint32_t u, uint32_t l);
uint32_t ppc_fieldf_inverted(float v, uint32_t u, uint32_t l);
uint64_t ppc_fieldd_inverted(double v, uint32_t u, uint32_t l);

/* generic field access */
#define ppc_field8_generic(v, a, b, bo) 	((int8_t)  ppc_field32u_generic(v, a, b, bo))
#define ppc_field8u_generic(v, a, b, bo) 	((uint8_t) ppc_field32u_generic(v, a, b, bo))
#define ppc_field16_generic(v, a, b, bo) 	((int16_t) ppc_field32u_generic(v, a, b, bo))
#define ppc_field16u_generic(v, a, b, bo)	((uint16_t)ppc_field32u_generic(v, a, b, bo))
#define ppc_field32_generic(v, a, b, bo) 	((int32_t) ppc_field32u_generic(v, a, b, bo))
uint32_t ppc_field32u_generic(uint32_t v, uint32_t a, uint32_t b, int bit_order);
#define ppc_field64_generic(v, a, b, bo) 	((int64_t) ppc_field64u_generic(v, a, b, bo))
uint64_t ppc_field64u_generic(uint64_t v, uint32_t a, uint32_t b, int bit_order);
uint32_t ppc_fieldf_generic(float v, uint32_t a, uint32_t b, int bit_order);
uint64_t ppc_fieldd_generic(double v, uint32_t a, uint32_t b, int bit_order);

/* enumeration */
int ppc_enumerate(int val, int max);

/* coercition */
#define ppc_coerce_ftoi(f) ((int32_t)(f))
#define ppc_coerce_ftou(f) ((uint32_t)(f))
#define ppc_coerce_dtoi(d) ((int64_t)(d))
#define ppc_coerce_dtou(d) ((uint64_t)(d))
#define ppc_coerce_itof(i) ((float)(i))
#define ppc_coerce_utof(u) ((float)(u))
#define ppc_coerce_itod(i) ((double)(i))
#define ppc_coerce_utod(u) ((double)(u))

/* cast */
#define ppc_cast_fto8(f)	((uint8_t)ppc_cast_fto32(f))
#define ppc_cast_fto16(f) ((uint16_t)ppc_cast_fto32(f))
uint32_t ppc_cast_fto32(float f);
#define ppc_cast_fto64(f) ((uint64_t)ppc_cast_fto32(f))

#define ppc_cast_dto8(f)	((uint8_t)ppc_cast_dto64(f))
#define ppc_cast_dto16(f) ((uint16_t)ppc_cast_dto64(f))
#define ppc_cast_dto32(f) ((uint32_t)ppc_cast_dto64(f))
uint64_t ppc_cast_dto64(double f);

#define ppc_cast_8tof(i)	((uint8_t)ppc_cast_32tof(i))
#define ppc_cast_16tof(i)	((uint8_t)ppc_cast_32tof(i))
float ppc_cast_32tof(uint32_t i);
#define ppc_cast_64tof(i)	((uint64_t)ppc_cast_32tof(i))

#define ppc_cast_8tod(i)	ppc_cast_64tod(i)
#define ppc_cast_16tod(i) ppc_cast_64tod(i)
#define ppc_cast_32tod(i) ppc_cast_64tod(i)
double ppc_cast_64tod(uint64_t d);

/* signed/unsigned support */
#define __PPC_MASK32(n, e)	((n)==32 ? (e) : ((e) & ((1 << n) - 1)))
#define __PPC_MASK64(n, e)	((n)==64 ? (e) : ((e) & ((1LL << n) - 1)))
#define __PPC_EXTS(n, e)		(((e) << n) >> n)

#endif /* PPC_GRT_H */
