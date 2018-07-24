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
 * @brief   Functions for formatting human-readable instructions.
 */

#ifndef ZYDIS_FORMATTER_H
#define ZYDIS_FORMATTER_H

#include <Zydis/DecoderTypes.h>
#include <Zydis/Defines.h>
#include <Zydis/Status.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================================== */
/* Enums and types                                                                                */
/* ============================================================================================== */

/**
 * @brief   Defines the @c ZydisFormatterStyle datatype.
 */
typedef uint8_t ZydisFormatterStyle;

/**
 * @brief   Values that represent formatter-styles.
 */
enum ZydisFormatterStyles
{
    /**
     * @brief   Generates intel-style disassembly.
     */
    ZYDIS_FORMATTER_STYLE_INTEL
};

/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the @c ZydisFormatFlags datatype.
 */
typedef uint32_t ZydisFormatterFlags;

/**
 * @brief   Formats the instruction in uppercase instead of lowercase.
 */
#define ZYDIS_FMTFLAG_UPPERCASE         0x00000001
/**
 * @brief   Forces the formatter to always print the segment register of memory-operands, instead
 *          of ommiting implicit DS/SS segments.
 */
#define ZYDIS_FMTFLAG_FORCE_SEGMENTS    0x00000002
/**
 * @brief   Forces the formatter to always print the size of memory-operands. 
 */
#define ZYDIS_FMTFLAG_FORCE_OPERANDSIZE 0x00000004

/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the @c ZydisFormatterAddressFormat datatype.
 */
typedef uint8_t ZydisFormatterAddressFormat;

/**
 * @brief   Values that represent address-formats.
 */
enum ZydisFormatterAddressFormats
{   
    /**
     * @brief   Currently defaults to @c ZYDIS_FORMATTER_ADDR_ABSOLUTE.
     */
    ZYDIS_FORMATTER_ADDR_DEFAULT,
    /**
     * @brief   Displays absolute addresses instead of relative ones.
     */
    ZYDIS_FORMATTER_ADDR_ABSOLUTE,
    /**
     * @brief   Uses signed hexadecimal values to display relative addresses.
     *          
     * Examples:
     * "JMP  0x20"
     * "JMP -0x20"
     */
    ZYDIS_FORMATTER_ADDR_RELATIVE_SIGNED,
    /**
     * @brief   Uses unsigned hexadecimal values to display relative addresses.
     *          
     * Examples:
     * "JMP 0x20"
     * "JMP 0xE0"
     */
    ZYDIS_FORMATTER_ADDR_RELATIVE_UNSIGNED,
};

/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the @c ZydisFormatterDisplacementFormat datatype.
 */
typedef uint8_t ZydisFormatterDisplacementFormat;

/**
 * @brief   Values that represent displacement-formats.
 */
enum ZydisFormatterDisplacementFormats
{
    /**
     * @brief   Currently defaults to @c ZYDIS_FORMATTER_DISP_HEX_SIGNED.
     */
    ZYDIS_FORMATTER_DISP_DEFAULT,
    /**
     * @brief   Formats displacements as signed hexadecimal values.
     *          
     * Examples: 
     * "MOV EAX, DWORD PTR SS:[ESP+0x400]"
     * "MOV EAX, DWORD PTR SS:[ESP-0x400]"
     */
    ZYDIS_FORMATTER_DISP_HEX_SIGNED,
    /**
     * @brief   Formats displacements as unsigned hexadecimal values.
     *          
     * Examples: 
     * "MOV EAX, DWORD PTR SS:[ESP+0x400]"
     * "MOV EAX, DWORD PTR SS:[ESP+0xFFFFFC00]"
     */
    ZYDIS_FORMATTER_DISP_HEX_UNSIGNED
};

/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the @c ZydisFormatterImmediateFormat datatype.
 */
typedef uint8_t ZydisFormatterImmediateFormat;

/**
 * @brief   Values that represent formatter immediate-formats.
 */
enum ZydisFormatterImmediateFormats
{
    /**
     * @brief   Currently defaults to @c ZYDIS_FORMATTER_IMM_HEX_UNSIGNED.
     */
    ZYDIS_FORMATTER_IMM_DEFAULT,
    /**
     * @brief   Automatically chooses the most suitable formatting-mode based on the operands
     *          @c ZydisOperandInfo.imm.isSigned attribute.
     */
    ZYDIS_FORMATTER_IMM_HEX_AUTO,
    /**
     * @brief   Formats immediates as signed hexadecimal values.
     *          
     * Examples: 
     * "MOV EAX, 0x400"
     * "MOV EAX, -0x400"
     */
    ZYDIS_FORMATTER_IMM_HEX_SIGNED,
    /**
     * @brief   Formats immediates as unsigned hexadecimal values.
     *          
     * Examples: 
     * "MOV EAX, 0x400"
     * "MOV EAX, 0xFFFFFC00"
     */
    ZYDIS_FORMATTER_IMM_HEX_UNSIGNED
};

/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the @c ZydisFormatterHookType datatype.
 */
typedef uint8_t ZydisFormatterHookType;

/**
 * @brief   Values that represent formatter hook-types.
 */
enum ZydisFormatterHookTypes
{
    /**
     * @brief   This function is called before the formatter starts formatting an instruction.
     */
    ZYDIS_FORMATTER_HOOK_PRE,
    /**
     * @brief   This function is called before the formatter finished formatting an instruction.
     */
    ZYDIS_FORMATTER_HOOK_POST,
    /**
     * @brief   This function refers to the main formatting function, that internally calls all  
     *          other function except the ones that are hooked by @c ZYDIS_FORMATTER_HOOK_PRE and 
     *          @c ZYDIS_FORMATTER_HOOK_POST.
     *          
     * Replacing this function allows for complete custom formatting, but indirectly disables all 
     * other hooks except for @c ZYDIS_FORMATTER_HOOK_PRE and @c ZYDIS_FORMATTER_HOOK_POST. 
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_INSTRUCTION,
    /**
     * @brief   This function is called to print the instruction prefixes.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_PREFIXES,
    /**
     * @brief   This function is called to print the instruction mnemonic.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_MNEMONIC,
    /**
     * @brief   This function is called to format an register operand.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_REG,
    /**
     * @brief   This function is called to format an memory operand.
     *
     * Replacing this function might indirectly disable some specific calls to the 
     * @c ZYDIS_FORMATTER_PRINT_ADDRESS and @c ZYDIS_FORMATTER_HOOK_PRINT_DISPLACEMENT functions.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_MEM,
    /**
     * @brief   This function is called to format an pointer operand.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_PTR,
    /**
     * @brief   This function is called to format an immediate operand.
     *
     * Replacing this function might indirectly disable some specific calls to the 
     * @c ZYDIS_FORMATTER_PRINT_ADDRESS and @c ZYDIS_FORMATTER_HOOK_PRINT_IMMEDIATE functions.
     */
    ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_IMM,  
    /**
     * @brief   This function is called right before formatting an memory operand to print the 
     *          optional size-specifier.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_OPERANDSIZE,
    /**
     * @brief   This function is called right before formatting an memory operand to print the 
     *          optional segment-register.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_SEGMENT,
    /**
     * @brief   This function is called right after formatting an operand to print the optional 
     *          EVEX/MVEX operand-decorator.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_DECORATOR,
    /**
     * @brief   This function is called to print an absolute address.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_ADDRESS,
    /**
     * @brief   This function is called to print a memory displacement value.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_DISPLACEMENT,
    /**
     * @brief   This function is called to print an immediate value.
     */
    ZYDIS_FORMATTER_HOOK_PRINT_IMMEDIATE
};

/* ---------------------------------------------------------------------------------------------- */

/**
 * @brief   Defines the @c ZydisDecoratorType datatype.
 */
typedef uint8_t ZydisDecoratorType;

/**
 * @brief   Values that represent decorator-types.
 */
enum ZydisDecoratorTypes
{
    ZYDIS_DECORATOR_TYPE_INVALID,
    ZYDIS_DECORATOR_TYPE_MASK,
    ZYDIS_DECORATOR_TYPE_BROADCAST,
    ZYDIS_DECORATOR_TYPE_ROUNDING_CONTROL,
    ZYDIS_DECORATOR_TYPE_SAE,
    ZYDIS_DECORATOR_TYPE_SWIZZLE,
    ZYDIS_DECORATOR_TYPE_CONVERSION,
    ZYDIS_DECORATOR_TYPE_EVICTION_HINT
};

typedef struct ZydisFormatter_  ZydisFormatter;

/**
 * @brief   Defines the @c ZydisFormatterNotifyFunc function pointer.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * 
 * @return  Returning a status code other than @c ZYDIS_STATUS_SUCCESS will immediately cause the 
 *          formatting process to fail.
 * 
 * This function type is used for the @c ZYDIS_FORMATTER_HOOK_PRE and
 * @c ZYDIS_FORMATTER_HOOK_POST hook-types.
 */
typedef ZydisStatus (*ZydisFormatterNotifyFunc)(const ZydisFormatter* formatter, 
    ZydisDecodedInstruction* instruction);

/**
 * @brief   Defines the @c ZydisFormatterFormatFunc function pointer.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   buffer      A pointer to the string-buffer.
 * @param   bufferLen   The length of the string-buffer.
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * 
 * @return  Returning a status code other than @c ZYDIS_STATUS_SUCCESS will immediately cause the 
 *          formatting process to fail.
 * 
 * After appending text to the @c buffer you MUST increase the buffer-pointer by the size of the
 * number of chars written. Not increasing the buffer-pointer will cause unexpected behavior.
 *
 * This function type is used for the @c ZYDIS_FORMATTER_HOOK_FORMAT_INSTRUCTION, 
 * @c ZYDIS_FORMATTER_HOOK_PRINT_PREFIXES and @c ZYDIS_FORMATTER_HOOK_PRINT_MNEMONIC hook-types.
 */
typedef ZydisStatus (*ZydisFormatterFormatFunc)(const ZydisFormatter* formatter, 
    char** buffer, size_t bufferLen, ZydisDecodedInstruction* instruction);

/**
 * @brief   Defines the @c ZydisFormatterFormatOperandFunc function pointer.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   buffer      A pointer to the string-buffer.
 * @param   bufferLen   The length of the string-buffer.
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * @param   operand     A pointer to the @c ZydisDecodedOperand struct.
 * 
 * @return  Returning a status code other than @c ZYDIS_STATUS_SUCCESS will immediately cause the 
 *          formatting process to fail.
 * 
 * After appending text to the @c buffer you MUST increase the buffer-pointer by the size of the
 * number of chars written.
 * 
 * Returning @c ZYDIS_STATUS_SUCCESS in one of the @c ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_X hooks 
 * without increasing the buffer-pointer is valid and will cause the formatter to omit the current 
 * operand.
 * 
 * Returning @c ZYDIS_STATUS_SUCCESS in @c ZYDIS_FORMATTER_HOOK_PRINT_OPERANDSIZE, 
 * @c ZYDIS_FORMATTER_HOOK_PRINT_SEGMENT or @c ZYDIS_FORMATTER_HOOK_PRINT_DECORATOR without 
 * increasing the buffer-pointer is valid and signals that the corresponding element should not be 
 * printed for the current operand.
 * 
 * Not increasing the buffer-pointer for any other hook-type will cause unexpected behavior.
 *
 * This function type is used for the @c ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_REG,
 * @c ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_MEM, @c ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_PTR, 
 * @c ZYDIS_FORMATTER_HOOK_FORMAT_OPERAND_IMM, @c ZYDIS_FORMATTER_HOOK_PRINT_OPERANDSIZE,
 * @c ZYDIS_FORMATTER_HOOK_PRINT_SEGMENT, @c ZYDIS_FORMATTER_HOOK_PRINT_DECORATOR,
 * @c ZYDIS_FORMATTER_HOOK_PRINT_DISPLACEMENT and @c ZYDIS_FORMATTER_HOOK_PRINT_IMMEDIATE 
 * hook-types.
 */
typedef ZydisStatus (*ZydisFormatterFormatOperandFunc)(const ZydisFormatter* formatter, 
    char** buffer, size_t bufferLen, ZydisDecodedInstruction* instruction, 
    ZydisDecodedOperand* operand);

 /**
 * @brief   Defines the @c ZydisFormatterFormatAddressFunc function pointer.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   buffer      A pointer to the string-buffer.
 * @param   bufferLen   The length of the string-buffer.
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * @param   operand     A pointer to the @c ZydisDecodedOperand struct.
 * 
 * @return  Returning a status code other than @c ZYDIS_STATUS_SUCCESS will immediately cause the 
 *          formatting process to fail.
 * 
 * After appending text to the @c buffer you MUST increase the buffer-pointer by the size of the
 * number of chars written.
 * Not increasing the buffer-pointer will cause unexpected behavior.
 *
 * This function type is used for the @c ZYDIS_FORMATTER_HOOK_PRINT_ADDRESS hook-type.
 */
typedef ZydisStatus (*ZydisFormatterFormatAddressFunc)(const ZydisFormatter* formatter, 
    char** buffer, size_t bufferLen, ZydisDecodedInstruction* instruction, 
    ZydisDecodedOperand* operand, uint64_t address);

/**
 * @brief   Defines the @c ZydisFormatterFormatDecoratorFunc function pointer.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   buffer      A pointer to the string-buffer.
 * @param   bufferLen   The length of the string-buffer.
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * @param   operand     A pointer to the @c ZydisDecodedOperand struct.
 * @param   type        The decorator type.
 * 
 * @return  Returning a status code other than @c ZYDIS_STATUS_SUCCESS will immediately cause the 
 *          formatting process to fail.
 * 
 * After appending text to the @c buffer you MUST increase the buffer-pointer by the size of the
 * number of chars written.
 * 
 * Returning @c ZYDIS_STATUS_SUCCESS without increasing the buffer-pointer is valid and will cause 
 * the formatter to omit the current decorator.
 *
 * This function type is used for the @c ZYDIS_FORMATTER_HOOK_PRINT_DECORATOR hook-type.
 */
typedef ZydisStatus (*ZydisFormatterFormatDecoratorFunc)(const ZydisFormatter* formatter, 
    char** buffer, size_t bufferLen, ZydisDecodedInstruction* instruction, 
    ZydisDecodedOperand* operand, ZydisDecoratorType type);

/**
 * @brief   Defines the @c ZydisFormatter struct.
 */
struct ZydisFormatter_
{
    ZydisFormatterFlags flags;
    ZydisFormatterAddressFormat addressFormat;
    ZydisFormatterDisplacementFormat displacementFormat;
    ZydisFormatterImmediateFormat immediateFormat;
    ZydisFormatterNotifyFunc funcPre;
    ZydisFormatterNotifyFunc funcPost;
    ZydisFormatterFormatFunc funcFormatInstruction;
    ZydisFormatterFormatFunc funcPrintPrefixes;
    ZydisFormatterFormatFunc funcPrintMnemonic;
    ZydisFormatterFormatOperandFunc funcFormatOperandReg;
    ZydisFormatterFormatOperandFunc funcFormatOperandMem;
    ZydisFormatterFormatOperandFunc funcFormatOperandPtr;
    ZydisFormatterFormatOperandFunc funcFormatOperandImm;
    ZydisFormatterFormatOperandFunc funcPrintOperandSize;
    ZydisFormatterFormatOperandFunc funcPrintSegment;
    ZydisFormatterFormatDecoratorFunc funcPrintDecorator;
    ZydisFormatterFormatAddressFunc funcPrintAddress;
    ZydisFormatterFormatOperandFunc funcPrintDisplacement;
    ZydisFormatterFormatOperandFunc funcPrintImmediate; 
};

/* ============================================================================================== */
/* Exported functions                                                                             */
/* ============================================================================================== */

/**
 * @brief   Initializes the given @c ZydisFormatter instance.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   style       The formatter style.
 *
 * @return  A zydis status code.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterInit(ZydisFormatter* formatter, ZydisFormatterStyle style);

/**
 * @brief   Initializes the given @c ZydisFormatter instance.
 *
 * @param   formatter           A pointer to the @c ZydisFormatter instance.
 * @param   style               The formatter style.
 * @param   addressFormat       The address format.
 * @param   displacementFormat  The displacement format.
 * @param   immmediateFormat    The immediate format.
 *
 * @return  A zydis status code.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterInitEx(ZydisFormatter* formatter, ZydisFormatterStyle style, 
    ZydisFormatterFlags flags, ZydisFormatterAddressFormat addressFormat, 
    ZydisFormatterDisplacementFormat displacementFormat,
    ZydisFormatterImmediateFormat immmediateFormat);

/**
 * @brief   Replaces a formatter function with a custom callback and/or retrieves the currently
 *          used function.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   hook        The formatter hook-type.
 * @param   callback    A pointer to a variable that contains the pointer of the callback function
 *                      and receives the pointer of the currently used function.
 *
 * @return  A zydis status code.
 * 
 * Call this function with `callback` pointing to a `NULL` value to retrieve the currently used
 * function without replacing it.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterSetHook(ZydisFormatter* formatter, 
    ZydisFormatterHookType hook, const void** callback);

/**
 * @brief   Formats the given instruction and writes it into the output buffer.
 *
 * @param   formatter   A pointer to the @c ZydisFormatter instance.
 * @param   instruction A pointer to the @c ZydisDecodedInstruction struct.
 * @param   buffer      A pointer to the output buffer.
 * @param   bufferLen   The length of the output buffer.
 *
 * @return  A zydis status code.
 */
ZYDIS_EXPORT ZydisStatus ZydisFormatterFormatInstruction(const ZydisFormatter* formatter, 
    ZydisDecodedInstruction* instruction, char* buffer, size_t bufferLen);

/* ============================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* ZYDIS_FORMATTER_H */
