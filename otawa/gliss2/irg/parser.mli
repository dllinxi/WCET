type token =
  | ID of (string)
  | CARD_CONST of (Int32.t)
  | CARD_CONST_64 of (Int64.t)
  | FIXED_CONST of (float)
  | STRING_CONST of (string)
  | BIN_CONST of (Int32.t * int)
  | BIN_CONST_64 of (Int64.t * int)
  | EOF
  | DOLLAR
  | MEM of (int)
  | VOLATILE
  | ALIAS
  | PORTS
  | COERCE
  | ERROR
  | TYPE of (int)
  | LET of (int)
  | MACRO
  | IF
  | THEN
  | ELSE
  | ENDIF
  | SWITCH
  | CASE
  | DEFAULT
  | BOOL
  | INT
  | CARD
  | FIX
  | FLOAT
  | ENUM
  | MODE of (int)
  | REG of (int)
  | VAR of (int)
  | OP of (int)
  | NOT
  | FORMAT
  | LIST
  | NOP
  | USES
  | SYNTAX
  | IMAGE
  | ACTION
  | INITIALA
  | RESOURCE of (int)
  | EXCEPTION of (int)
  | BINARY_CONST
  | HEX_CONST
  | OR
  | AND
  | LEQ
  | GEQ
  | EQU
  | NEQ
  | LEFT_SHIFT
  | RIGHT_SHIFT
  | DOUBLE_STAR
  | ROTATE_RIGHT
  | ROTATE_LEFT
  | DOUBLE_COLON
  | DOUBLE_DOT
  | BIT_LEFT
  | BIT_RIGHT
  | EQ
  | EXCLAM
  | PIPE
  | CIRC
  | AMPERS
  | GT
  | LT
  | SHARP
  | PLUS
  | MINUS
  | STAR
  | SLASH
  | PERCENT
  | TILD
  | COLON
  | COMMA
  | LBRACE
  | RBRACE
  | LBRACK
  | RBRACK
  | LPAREN
  | RPAREN
  | SEMI
  | DOT
  | ATTR
  | EXTEND
  | AROBAS
  | CANON

val top :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> unit
