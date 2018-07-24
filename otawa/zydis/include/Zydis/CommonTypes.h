/***************************************************************************************************

  Zyan Disassembler Library (Zydis)

  Original Author : Florian Bernd

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

***************************************************************************************************/

/**
 * @file
 * @brief Includes and defines some default datatypes.
 */

#ifndef ZYDIS_COMMONTYPES_H
#define ZYDIS_COMMONTYPES_H

/* ============================================================================================== */
/* Integral types                                                                                 */
/* ============================================================================================== */

/**
 * uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t
 */
#include <stdint.h>

/**
 * size_t, ptrdiff_t
 */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================================== */
/* Boolean                                                                                        */
/* ============================================================================================== */

#define ZYDIS_FALSE 0
#define ZYDIS_TRUE  1

/**
 * @briefs  Defines the @c ZydisBool datatype.
 */
typedef uint8_t ZydisBool;

/* ============================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* ZYDIS_COMMONTYPES_H */
