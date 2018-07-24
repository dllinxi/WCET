/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         util_fft_parse
#define yylex           util_fft_lex
#define yyerror         util_fft_error
#define yydebug         util_fft_debug
#define yynerrs         util_fft_nerrs

#define yylval          util_fft_lval
#define yychar          util_fft_char
#define yylloc          util_fft_lloc

/* Copy the first part of user declarations.  */
#line 22 "util_fft_parser.yy" /* yacc.c:339  */

#include <stdio.h>
#include <otawa/util/FlowFactLoader.h>
#include <elm/io.h>
#include <elm/genstruct/Vector.h>
#include <otawa/prop/ContextualProperty.h>
#include <otawa/dfa/State.h>
using namespace elm::genstruct;
int util_fft_lex(void);
void util_fft_error(otawa::FlowFactLoader *loader, const char *msg);
static Vector<otawa::Address> addresses;
static otawa::ContextualPath path;

// Loop counting
static int loop_max = -1, loop_total = -1;
static void reset_counts(void) {
	loop_max = -1;
	loop_total = -1;
}

#line 96 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "util_fft_parser.h".  */
#ifndef YY_UTIL_FFT_CYGDRIVE_D_SEAFILE_WORKSPACE_MYPROJECT_WCET_WCET_OTAWA_OTAWA_SRC_PROG_UTIL_FFT_PARSER_H_INCLUDED
# define YY_UTIL_FFT_CYGDRIVE_D_SEAFILE_WORKSPACE_MYPROJECT_WCET_WCET_OTAWA_OTAWA_SRC_PROG_UTIL_FFT_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int util_fft_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTEGER = 258,
    STRING = 259,
    BAD_TOKEN = 260,
    CHECKSUM = 261,
    DOT_DOT = 262,
    KW_ACCESS = 263,
    KW_CALL = 264,
    KW_CONTROL = 265,
    KW_DOINLINE = 266,
    KW_ENTRY = 267,
    KW_IGNORE = 268,
    KW_IGNORECONTROL = 269,
    KW_IGNORESEQ = 270,
    KW_IN = 271,
    KW_INLINING_OFF = 272,
    KW_INLINING_ON = 273,
    KW_LIBRARY = 274,
    KW_MAX = 275,
    KW_MEMORY = 276,
    KW_MULTIBRANCH = 277,
    KW_MULTICALL = 278,
    KW_NO = 279,
    KW_NOCALL = 280,
    KW_NOINLINE = 281,
    KW_PRESERVE = 282,
    KW_REG = 283,
    KW_SEQ = 284,
    KW_TO = 285,
    KW_TOTAL = 286,
    LOOP = 287,
    NORETURN = 288,
    RETURN = 289,
    TYPE = 290
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 49 "util_fft_parser.yy" /* yacc.c:355  */

	int _int;
	elm::String *_str;
	otawa::Address *addr;
	const otawa::Type *type;
	otawa::dfa::Value *value;

#line 180 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE util_fft_lval;
extern YYLTYPE util_fft_lloc;
int util_fft_parse (otawa::FlowFactLoader *loader);

#endif /* !YY_UTIL_FFT_CYGDRIVE_D_SEAFILE_WORKSPACE_MYPROJECT_WCET_WCET_OTAWA_OTAWA_SRC_PROG_UTIL_FFT_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 211 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  50
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   157

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  17
/* YYNRULES -- Number of rules.  */
#define YYNRULES  58
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  150

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   290

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      47,    48,     2,    40,    39,    41,     2,    44,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    42,    36,
       2,    38,     2,    37,    43,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    45,     2,    46,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    96,    96,    98,   102,   103,   107,   109,   116,   118,
     124,   126,   128,   130,   132,   134,   136,   138,   140,   142,
     144,   146,   148,   150,   150,   152,   152,   154,   156,   158,
     166,   168,   170,   175,   176,   180,   187,   197,   199,   204,
     206,   211,   213,   218,   220,   226,   228,   230,   232,   234,
     240,   241,   245,   247,   252,   254,   259,   261,   263
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "STRING", "BAD_TOKEN",
  "CHECKSUM", "DOT_DOT", "KW_ACCESS", "KW_CALL", "KW_CONTROL",
  "KW_DOINLINE", "KW_ENTRY", "KW_IGNORE", "KW_IGNORECONTROL",
  "KW_IGNORESEQ", "KW_IN", "KW_INLINING_OFF", "KW_INLINING_ON",
  "KW_LIBRARY", "KW_MAX", "KW_MEMORY", "KW_MULTIBRANCH", "KW_MULTICALL",
  "KW_NO", "KW_NOCALL", "KW_NOINLINE", "KW_PRESERVE", "KW_REG", "KW_SEQ",
  "KW_TO", "KW_TOTAL", "LOOP", "NORETURN", "RETURN", "TYPE", "';'", "'?'",
  "'='", "','", "'+'", "'-'", "':'", "'@'", "'/'", "'['", "']'", "'('",
  "')'", "$accept", "file", "commands", "command", "$@1", "$@2", "counts",
  "count", "multibranch", "multicall", "address_list", "id_or_address",
  "full_address", "opt_in", "step", "path", "value", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,    59,    63,    61,    44,
      43,    45,    58,    64,    47,    91,    93,    40,    41
};
# endif

#define YYPACT_NINF -33

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-33)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     123,    28,    55,    18,    64,    64,    55,    55,   -14,     2,
     -33,   -33,     7,    55,    55,    64,    35,    64,    64,    64,
      43,   123,   -33,    52,   -33,   -33,    44,    64,    58,    64,
     -33,    24,    33,    34,    44,    44,   -33,    64,    64,    64,
      64,    55,    64,    37,    44,    38,    39,    26,    40,    42,
     -33,   -33,    45,     1,    47,    48,    49,    50,    68,    72,
      76,   -33,   -33,    51,    53,    64,    54,   -33,    60,   -33,
      61,    57,    59,   -33,    63,   -33,     5,    44,    77,    85,
      44,    44,    11,   -33,   -33,   -33,    64,   -33,    62,   -33,
     -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,    87,
       5,     3,    14,   -33,   -33,   -33,    64,    64,   -33,    65,
      69,   -33,   -33,    71,    73,   -33,   -33,     1,    64,    74,
      75,   -15,   -33,    79,    17,    97,    78,   -33,   -33,   -33,
     -33,   -33,    44,   -33,   -33,   -33,    64,   -33,   -33,    64,
     105,    80,   -33,    66,    81,   -33,   -33,   110,    70,   -33
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     3,     4,     0,    43,    44,    50,     0,     0,     0,
      45,    46,     0,     0,    50,    50,    32,     0,     0,     0,
       0,     0,     0,     0,    50,     0,     0,     0,     0,     0,
       1,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    19,    21,     0,     0,     0,     0,    24,     0,    26,
       0,     0,     0,    13,     0,    27,     0,    50,     0,     0,
      50,    50,    33,    11,    10,     9,     0,    52,    54,    51,
      15,    20,    28,    22,    47,    48,    49,    18,    17,     0,
       0,     0,     0,    14,    12,    16,     0,     0,    56,     0,
       0,    35,    36,     0,     0,    34,    53,     0,     0,     0,
       0,     0,    41,     0,     0,     0,     0,    30,     6,     8,
       7,    55,    50,    31,    38,    37,     0,    40,    39,     0,
       0,     0,    42,     0,     0,    29,    57,     0,     0,    58
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -33,   -33,   -33,    98,   -33,   -33,    41,   -33,   -33,   -33,
      19,    13,    -4,   -32,   -33,     8,    22
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    20,    21,    22,    39,    40,    81,    82,    67,    69,
     121,    26,    87,    54,    88,    89,   109
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      32,    33,    63,    64,    30,    31,    30,    31,    30,    31,
      37,    45,    74,    47,    48,    49,    41,    30,    31,    34,
      35,   135,    36,    55,   136,    57,    43,    44,    27,    77,
      28,    78,    23,    65,    66,    68,    70,    38,    72,    46,
     120,    42,    79,    50,    86,   110,    78,    29,   113,   114,
     106,   123,   107,   138,    71,    52,   136,    79,    24,    25,
      53,    99,    56,    80,    58,    59,    60,    30,    31,    61,
      62,    94,   108,    73,    75,    95,    83,    76,    84,    96,
     111,    85,   116,    90,    91,    92,    93,    97,   112,    98,
     101,   102,   100,   103,   118,   104,   108,   122,   122,   105,
     141,   127,   125,   126,   139,   128,   117,   129,   144,   130,
     133,   134,   146,   148,   132,   137,   145,   140,   149,    51,
     147,   124,   119,   115,     0,   131,     0,     0,     0,     1,
       0,     0,   142,     0,     2,   143,     3,     4,     5,     0,
       6,     7,     8,     0,     9,    10,    11,    12,    13,    14,
      15,    16,     0,     0,     0,    17,    18,    19
};

static const yytype_int16 yycheck[] =
{
       4,     5,    34,    35,     3,     4,     3,     4,     3,     4,
       8,    15,    44,    17,    18,    19,     9,     3,     4,     6,
       7,    36,    36,    27,    39,    29,    13,    14,    10,     3,
      12,    20,     4,    37,    38,    39,    40,    35,    42,     4,
      37,    34,    31,     0,    43,    77,    20,    29,    80,    81,
      45,    37,    47,    36,    41,     3,    39,    31,     3,     4,
      16,    65,     4,    37,    40,    41,    42,     3,     4,    36,
      36,     3,    76,    36,    36,     3,    36,    38,    36,     3,
       3,    36,    86,    36,    36,    36,    36,    36,     3,    36,
      30,    30,    38,    36,     7,    36,   100,   101,   102,    36,
     132,    36,   106,   107,     7,    36,    44,    36,     3,    36,
      36,    36,    46,     3,   118,    36,    36,    39,    48,    21,
      39,   102,   100,    82,    -1,   117,    -1,    -1,    -1,     6,
      -1,    -1,   136,    -1,    11,   139,    13,    14,    15,    -1,
      17,    18,    19,    -1,    21,    22,    23,    24,    25,    26,
      27,    28,    -1,    -1,    -1,    32,    33,    34
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     6,    11,    13,    14,    15,    17,    18,    19,    21,
      22,    23,    24,    25,    26,    27,    28,    32,    33,    34,
      50,    51,    52,     4,     3,     4,    60,    10,    12,    29,
       3,     4,    61,    61,    60,    60,    36,     8,    35,    53,
      54,     9,    34,    60,    60,    61,     4,    61,    61,    61,
       0,    52,     3,    16,    62,    61,     4,    61,    40,    41,
      42,    36,    36,    62,    62,    61,    61,    57,    61,    58,
      61,    60,    61,    36,    62,    36,    38,     3,    20,    31,
      37,    55,    56,    36,    36,    36,    43,    61,    63,    64,
      36,    36,    36,    36,     3,     3,     3,    36,    36,    61,
      38,    30,    30,    36,    36,    36,    45,    47,    61,    65,
      62,     3,     3,    62,    62,    55,    61,    44,     7,    65,
      37,    59,    61,    37,    59,    61,    61,    36,    36,    36,
      36,    64,    61,    36,    36,    36,    39,    36,    36,     7,
      39,    62,    61,    61,     3,    36,    46,    39,     3,    48
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    50,    50,    51,    51,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    53,    52,    54,    52,    52,    52,    52,
      52,    52,    52,    55,    55,    56,    56,    57,    57,    58,
      58,    59,    59,    60,    60,    61,    61,    61,    61,    61,
      62,    62,    63,    63,    64,    64,    65,    65,    65
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     5,     5,     5,     4,
       3,     3,     4,     3,     4,     4,     4,     4,     4,     3,
       4,     3,     4,     0,     3,     0,     3,     3,     4,     8,
       5,     6,     2,     1,     2,     2,     2,     4,     4,     4,
       4,     1,     3,     1,     1,     1,     1,     3,     3,     3,
       0,     2,     1,     2,     1,     3,     1,     5,     7
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (loader, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, loader); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, otawa::FlowFactLoader *loader)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (loader);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, otawa::FlowFactLoader *loader)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, loader);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, otawa::FlowFactLoader *loader)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , loader);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, loader); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, otawa::FlowFactLoader *loader)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (loader);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (otawa::FlowFactLoader *loader)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 108 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onLoop(*(yyvsp[-3].addr), (yyvsp[-2]._int), -1, path); delete (yyvsp[-3].addr); path.clear(); }
#line 1479 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 110 "util_fft_parser.yy" /* yacc.c:1646  */
    {
			loader->onLoop(*(yyvsp[-3].addr), loop_max, loop_total, path);
			delete (yyvsp[-3].addr);
			path.clear();
			reset_counts();
		}
#line 1490 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 117 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onUnknownLoop(*(yyvsp[-3].addr)); delete (yyvsp[-3].addr); path.clear(); }
#line 1496 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 119 "util_fft_parser.yy" /* yacc.c:1646  */
    {
			//cout << "checksum = " << io::hex($2) << io::endl;
			loader->onCheckSum(*(yyvsp[-2]._str), (yyvsp[-1]._int));
			delete (yyvsp[-2]._str);
		}
#line 1506 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 125 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onReturn(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1512 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 127 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onNoReturn(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1518 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 129 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onNoReturn(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1524 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 131 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onNoCall(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1530 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 133 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onNoCall(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1536 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 135 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onNoInline(*(yyvsp[-2].addr), false, path); delete (yyvsp[-2].addr); path.clear(); }
#line 1542 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 137 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onNoInline(*(yyvsp[-2].addr), true, path); delete (yyvsp[-2].addr); path.clear(); }
#line 1548 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 139 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onSetInlining(*(yyvsp[-2].addr), true, path); delete (yyvsp[-2].addr); path.clear(); }
#line 1554 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 141 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onSetInlining(*(yyvsp[-2].addr), false, path); delete (yyvsp[-2].addr); path.clear(); }
#line 1560 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 143 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onIgnoreControl(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1566 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 145 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onIgnoreControl(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1572 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 147 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onIgnoreSeq(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1578 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 149 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onIgnoreSeq(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1584 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 150 "util_fft_parser.yy" /* yacc.c:1646  */
    { addresses.setLength(0); }
#line 1590 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 151 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1596 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 152 "util_fft_parser.yy" /* yacc.c:1646  */
    { addresses.setLength(0); }
#line 1602 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 153 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1608 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 155 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onPreserve(*(yyvsp[-1].addr)); delete (yyvsp[-1].addr); }
#line 1614 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 157 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onIgnoreEntry(*(yyvsp[-1]._str)); delete (yyvsp[-1]._str); }
#line 1620 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 159 "util_fft_parser.yy" /* yacc.c:1646  */
    {
			loader->onMemoryAccess(*(yyvsp[-5].addr), *(yyvsp[-4].addr), *(yyvsp[-2].addr), path);
			path.clear();
			delete (yyvsp[-5].addr);
			delete (yyvsp[-4].addr);
			delete (yyvsp[-2].addr);
		}
#line 1632 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 167 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onRegSet(*(yyvsp[-3]._str), *(yyvsp[-1].value)); delete (yyvsp[-3]._str); delete (yyvsp[-1].value); }
#line 1638 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 169 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onMemSet(*(yyvsp[-3].addr), (yyvsp[-4].type), *(yyvsp[-1].value)); delete (yyvsp[-3].addr); delete (yyvsp[-1].value); }
#line 1644 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 171 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1650 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 175 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1656 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 176 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1662 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 181 "util_fft_parser.yy" /* yacc.c:1646  */
    {
			if(loop_max >= 0)
				loader->onError(elm::_ << "several 'max' keywords");
			else
				loop_max = (yyvsp[0]._int);
		}
#line 1673 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 188 "util_fft_parser.yy" /* yacc.c:1646  */
    {
			if(loop_total >= 0)
				loader->onError(elm::_ << "several 'total' keywords");
			else
				loop_total = (yyvsp[0]._int);
		}
#line 1684 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 198 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onMultiBranch(*(yyvsp[-3].addr), addresses); delete (yyvsp[-3].addr); }
#line 1690 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 200 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onUnknownMultiBranch(*(yyvsp[-3].addr)); delete (yyvsp[-3].addr); }
#line 1696 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 205 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onMultiCall(*(yyvsp[-3].addr), addresses); delete (yyvsp[-3].addr); }
#line 1702 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 207 "util_fft_parser.yy" /* yacc.c:1646  */
    { loader->onUnknownMultiCall(*(yyvsp[-3].addr)); delete (yyvsp[-3].addr); }
#line 1708 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 212 "util_fft_parser.yy" /* yacc.c:1646  */
    { addresses.add(*(yyvsp[0].addr)); delete (yyvsp[0].addr); }
#line 1714 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 214 "util_fft_parser.yy" /* yacc.c:1646  */
    { addresses.add(*(yyvsp[0].addr)); delete (yyvsp[0].addr); }
#line 1720 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 219 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.addr) = new otawa::Address((yyvsp[0]._int)); }
#line 1726 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 221 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.addr) = new otawa::Address(loader->addressOf(*(yyvsp[0]._str))); }
#line 1732 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 227 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.addr) = new otawa::Address((yyvsp[0]._int)); }
#line 1738 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 229 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.addr) = new otawa::Address(loader->addressOf(*(yyvsp[0]._str))); delete (yyvsp[0]._str); }
#line 1744 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 231 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.addr) = new otawa::Address(loader->addressOf(*(yyvsp[-2]._str)) + (yyvsp[0]._int)); delete (yyvsp[-2]._str); }
#line 1750 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 233 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.addr) = new otawa::Address(loader->addressOf(*(yyvsp[-2]._str)) - (yyvsp[0]._int)); delete (yyvsp[-2]._str); }
#line 1756 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 235 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.addr) = new otawa::Address(loader->addressOf(*(yyvsp[-2]._str), (yyvsp[0]._int))); delete (yyvsp[-2]._str); }
#line 1762 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 240 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1768 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 242 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1774 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 246 "util_fft_parser.yy" /* yacc.c:1646  */
    { path.push(otawa::ContextualStep::FUNCTION, *(yyvsp[0].addr)); delete (yyvsp[0].addr); }
#line 1780 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 248 "util_fft_parser.yy" /* yacc.c:1646  */
    { path.push(otawa::ContextualStep::CALL, *(yyvsp[0].addr)); delete (yyvsp[0].addr); }
#line 1786 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 253 "util_fft_parser.yy" /* yacc.c:1646  */
    {  }
#line 1792 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 255 "util_fft_parser.yy" /* yacc.c:1646  */
    { }
#line 1798 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 260 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.value) = new otawa::dfa::Value((yyvsp[0].addr)->offset()); delete (yyvsp[0].addr); }
#line 1804 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 262 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.value) = new otawa::dfa::Value((yyvsp[-3].addr)->offset(), (yyvsp[-1].addr)->offset()); delete (yyvsp[-3].addr); delete (yyvsp[-1].addr); }
#line 1810 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 264 "util_fft_parser.yy" /* yacc.c:1646  */
    { (yyval.value) = new otawa::dfa::Value((yyvsp[-5].addr)->offset(), (yyvsp[-3]._int), (yyvsp[-1]._int)); delete (yyvsp[-5].addr); }
#line 1816 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
    break;


#line 1820 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (loader, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (loader, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, loader);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, loader);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (loader, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, loader);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, loader);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 267 "util_fft_parser.yy" /* yacc.c:1906  */


// Error managed
void util_fft_error(otawa::FlowFactLoader *loader, const char *msg) {
	loader->onError(msg);
}
