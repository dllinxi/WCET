/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 49 "util_fft_parser.yy" /* yacc.c:1909  */

	int _int;
	elm::String *_str;
	otawa::Address *addr;
	const otawa::Type *type;
	otawa::dfa::Value *value;

#line 98 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/util_fft_parser.h" /* yacc.c:1909  */
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
