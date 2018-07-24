/***************************************************************************************************

  Zyan Disassembler Engine (Zydis)

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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <Zydis/Zydis.h>

#if defined(ZYDIS_WINDOWS)
#   include <Windows.h>
#elif defined(ZYDIS_APPLE)
#   include <mach/mach_time.h>
#elif defined(ZYDIS_LINUX)
#   include <sys/time.h>
#else
#   error "Unsupported platform detected"
#endif

/* ============================================================================================== */
/* Helper functions                                                                               */
/* ============================================================================================== */

#if defined(ZYDIS_WINDOWS)
double   CounterFreq  = 0.0;
uint64_t CounterStart = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
    {
        fputs("QueryPerformanceFrequency failed!\n", stderr);
    }
    CounterFreq = (double)li.QuadPart / 1000.0;
    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double)(li.QuadPart - CounterStart) / CounterFreq;
}
#elif defined(ZYDIS_APPLE)
uint64_t counterStart = 0;
mach_timebase_info_data_t timebaseInfo;

void StartCounter()
{
    counterStart = mach_absolute_time();
}

double GetCounter()
{
    uint64_t elapsed = mach_absolute_time() - counterStart;

    if (timebaseInfo.denom == 0)
    {
        mach_timebase_info(&timebaseInfo);
    }

    return (double)elapsed * timebaseInfo.numer / timebaseInfo.denom / 1000000;
}
#elif defined(ZYDIS_LINUX)
// TODO:
#endif

/* ============================================================================================== */
/* Internal functions                                                                             */
/* ============================================================================================== */

uint64_t processBuffer(const char* buffer, size_t length, ZydisDecodeGranularity granularity, 
    ZydisBool format)
{
    ZydisDecoder decoder;
    if (!ZYDIS_SUCCESS(ZydisDecoderInitEx(&decoder, 
        ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64, granularity)))
    {
        fputs("Failed to initialize decoder\n", stderr);
        exit(EXIT_FAILURE);
    }

    ZydisFormatter formatter;
    if (format)
    {
        if (!ZYDIS_SUCCESS(ZydisFormatterInitEx(&formatter, ZYDIS_FORMATTER_STYLE_INTEL, 
            ZYDIS_FMTFLAG_FORCE_SEGMENTS | ZYDIS_FMTFLAG_FORCE_OPERANDSIZE,
            ZYDIS_FORMATTER_ADDR_ABSOLUTE, ZYDIS_FORMATTER_DISP_DEFAULT, 
            ZYDIS_FORMATTER_IMM_DEFAULT)))
        {
            fputs("Failed to initialized instruction-formatter\n", stderr);
            exit(EXIT_FAILURE);
        }
    }

    uint64_t count = 0;
    size_t offset = 0;
    ZydisStatus status;
    ZydisDecodedInstruction instruction;
    char formatBuffer[256];
    while ((status = ZydisDecoderDecodeBuffer(&decoder, buffer + offset, length - offset, offset, 
        &instruction)) != ZYDIS_STATUS_NO_MORE_DATA)
    {
        ZYDIS_ASSERT(ZYDIS_SUCCESS(status));
        if (!ZYDIS_SUCCESS(status))
        {
            puts("Unexpected decoding error");
            exit(EXIT_FAILURE);
        }
        ++count;
        if (format)
        {
            ZydisFormatterFormatInstruction(
                &formatter, &instruction, formatBuffer, sizeof(formatBuffer));
        }            
        offset += instruction.length;
    } 
    
    return count;
}

void testPerformance(const char* buffer, size_t length, ZydisDecodeGranularity granularity, 
    ZydisBool format)
{
    uint64_t count = 0;
    StartCounter();
    for (uint8_t j = 0; j < 100; ++j)
    {
        count += processBuffer(buffer, length, granularity, format);
    }
    printf("Granularity %d, Formatting %d, Instructions: ~%6.2fM, Time: %8.2f msec\n", 
        granularity, format, (double)count / 1000000, GetCounter());  
}

void generateTestData(FILE* file, uint8_t encoding)
{
    ZydisDecoder decoder;
    if (!ZYDIS_SUCCESS(
        ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64)))
    {
        fputs("Failed to initialize decoder\n", stderr);
        exit(EXIT_FAILURE);
    }
    
    uint8_t last = 0;
    double size = 0;
    ZydisDecodedInstruction instruction;
    while (size < 1024 * 1024)
    {
        uint8_t data[ZYDIS_MAX_INSTRUCTION_LENGTH];
        for (int i = 0; i < ZYDIS_MAX_INSTRUCTION_LENGTH; ++i)
        {
            data[i] = rand() % 256;
        }
        uint8_t offset = rand() % (ZYDIS_MAX_INSTRUCTION_LENGTH - 2);
        switch (encoding)
        {
        case 0:
            break;
        case 1:
            data[offset    ] = 0x0F;
            data[offset + 1] = 0x0F;
            break;
        case 2:
            data[offset    ] = 0x8F;
            break;
        case 3:
            data[offset    ] = 0xC4;
            break;
        case 4:
            data[offset    ] = 0xC5;
            break;
        case 5:
        case 6:
            data[offset    ] = 0x62;
            break;
        default:
            ZYDIS_UNREACHABLE;
        }
        if (ZYDIS_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data, sizeof(data), 0, &instruction)))
        {
            ZydisBool b = ZYDIS_FALSE;
            switch (encoding)
            {
            case 0:
                b = (instruction.encoding == ZYDIS_INSTRUCTION_ENCODING_DEFAULT);
                break;
            case 1:
                b = (instruction.encoding == ZYDIS_INSTRUCTION_ENCODING_3DNOW);
                break;
            case 2:
                b = (instruction.encoding == ZYDIS_INSTRUCTION_ENCODING_XOP);
                break;
            case 3:
            case 4:
                b = (instruction.encoding == ZYDIS_INSTRUCTION_ENCODING_VEX);
                break;
            case 5:
                b = (instruction.encoding == ZYDIS_INSTRUCTION_ENCODING_EVEX);
                break;
            case 6:
                b = (instruction.encoding == ZYDIS_INSTRUCTION_ENCODING_MVEX);
                break;
            default:
                ZYDIS_UNREACHABLE;
            }
            if (b)
            {
                fwrite(&instruction.data[0], 1, instruction.length, file);
                size += instruction.length;

                double p = (size / (1024 * 1024) * 100);
                if (last < (uint8_t)p)
                {
                    last = (uint8_t)p;
                    printf("%3.0f%%\n", p);
                }
                
            }
        }
    }
}

/* ============================================================================================== */
/* Entry point                                                                                    */
/* ============================================================================================== */

int main(int argc, char** argv)
{
    if (argc < 3 || (strcmp(argv[1], "-test") && strcmp(argv[1], "-generate")))
    {
        fputs("Usage: PerfTest -[test|generate] [directory]\n", stderr);
        return EXIT_FAILURE;
    }

    ZydisBool generate = ZYDIS_FALSE;
    if (!strcmp(argv[1], "-generate"))
    {
        generate = ZYDIS_TRUE;
    }
    const char* directory = argv[2]; 

    static const struct
    {
        const char* encoding;
        const char* filename;
    } tests[7] =
    {
        { "DEFAULT", "enc_default.dat" },
        { "3DNOW"  , "enc_3dnow.dat"   },
        { "XOP"    , "enc_xop.dat"     },
        { "VEX_C4" , "enc_vex_c4.dat"  },
        { "VEX_C5" , "enc_vex_c5.dat"  },
        { "EVEX"   , "enc_evex.dat"    },
        { "MVEX"   , "enc_mvex.dat"    }
    };
    
    if (generate)
    {
        time_t t;
        srand((unsigned) time(&t));
    }

    for (uint8_t i = 0; i < ZYDIS_ARRAY_SIZE(tests); ++i)
    {
        FILE* file;

        char buf[256];
        strcpy(&buf[0], directory);
        if (generate)
        {
            file = fopen(strcat(buf, tests[i].filename), "wb");   
        } else
        {
            file = fopen(strcat(buf, tests[i].filename), "rb");
        }
        if (!file)
        {
            fprintf(stderr, "Can not open file \"%s\": %s\n", &buf[0], strerror(errno));
            continue;
        }

        if (generate)
        {
            printf("Generating %s ...\n", tests[i].encoding);
            generateTestData(file, i);
        } else
        {
            fseek(file, 0L, SEEK_END);
            long length = ftell(file);
            void* buffer = malloc(length);
            rewind(file);
            fread(buffer, 1, length, file);

            printf("Testing %s ...\n", tests[i].encoding);
            testPerformance(buffer, length, ZYDIS_DECODE_GRANULARITY_MINIMAL, ZYDIS_FALSE);
            testPerformance(buffer, length, ZYDIS_DECODE_GRANULARITY_FULL   , ZYDIS_FALSE);
            // testPerformance(buffer, length, ZYDIS_DECODE_GRANULARITY_FULL   , ZYDIS_TRUE );

            puts("");
            free(buffer);
            fclose(file);
        }
    }

    return 0;
}

/* ============================================================================================== */
