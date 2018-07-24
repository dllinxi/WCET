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

#ifndef YY_AST_CYGDRIVE_D_SEAFILE_WORKSPACE_MYPROJECT_WCET_WCET_OTAWA_OTAWA_SRC_AST_AST_PARSER_H_INCLUDED
# define YY_AST_CYGDRIVE_D_SEAFILE_WORKSPACE_MYPROJECT_WCET_WCET_OTAWA_OTAWA_SRC_AST_AST_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ast_debug;
#endif
/* "%code requires" blocks.  */
#line 21 "ast_parser.yy" /* yacc.c:1909  */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <otawa/ast.h>
#include <elm/genstruct/Vector.h>
#include <otawa/ast/ASTLoader.h>

using namespace elm;
using namespace otawa;
using namespace otawa::ast;

// Proto
#define size_t unsigned long
int ast_lex(void);
void ast_error(otawa::ast::ASTLoader *loader, const char *msg);


#line 63 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ast/ast_parser.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ERROR = 258,
    _LABEL = 259,
    NAME = 260,
    SEQ = 261,
    IF = 262,
    WHILE = 263,
    DOWHILE = 264,
    FOR = 265,
    CODE = 266,
    APPEL = 267,
    VIDE = 268,
    NUMBER = 269,
    UN_OP = 270,
    BIN_OP = 271,
    BI_OP = 272
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 46 "ast_parser.yy" /* yacc.c:1909  */

	char *str;
	otawa::ast::AST *ast;

#line 98 "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ast/ast_parser.h" /* yacc.c:1909  */
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


extern YYSTYPE ast_lval;
extern YYLTYPE ast_lloc;
int ast_parse (otawa::ast::ASTLoader *loader);

#endif /* !YY_AST_CYGDRIVE_D_SEAFILE_WORKSPACE_MYPROJECT_WCET_WCET_OTAWA_OTAWA_SRC_AST_AST_PARSER_H_INCLUDED  */
