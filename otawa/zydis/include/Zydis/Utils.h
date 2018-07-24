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
 * @brief   Other utility functions.
 */

#ifndef ZYDIS_UTILS_H
#define ZYDIS_UTILS_H

#include <stdint.h>
#include <Zydis/Defines.h>
#include <Zydis/Status.h>
#include <Zydis/DecoderTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================================== */
/* Address calculation                                                                            */
/* ============================================================================================== */

/**
 * @brief   Calculates the absolute target-address of an relative instruction operand.
 *
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * @param   operand     A pointer to the @c ZydisDecodedOperand struct.
 * @param   address     A pointer to the memory that receives the absolute target-address.
 *
 * @return  A zydis status code
 */
ZYDIS_EXPORT ZydisStatus ZydisUtilsCalcAbsoluteTargetAddress(
    const ZydisDecodedInstruction* instruction, const ZydisDecodedOperand* operand, 
    uint64_t* address);

/* ============================================================================================== */
/* Flags                                                                                          */
/* ============================================================================================== */

/**
 * @brief   Returns a mask of CPU-flags matching the given `action`.
 *
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * @param   action      The CPU-flag action.
 * @param   flags       A pointer to the variable that receives the flag mask.
 *
 * @return  A zydis status code
 */
ZYDIS_EXPORT ZydisStatus ZydisGetCPUFlagsByAction(const ZydisDecodedInstruction* instruction, 
    ZydisCPUFlagAction action, ZydisCPUFlagMask* flags);

/* ============================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* ZYDIS_UTILS_H */
