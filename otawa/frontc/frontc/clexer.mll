(*
 *	FrontC Lexer
 *	Copyright (C) 1999  Université de Toulouse <casse@irit.fr>
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation; either
 *	version 2.1 of the License, or (at your option) any later version.
 *	
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Lesser General Public License for more details.
 *	
 *	You should have received a copy of the GNU Lesser General Public
 *	License along with this library; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *)
{
open Cparser
exception Eof
exception InternalError of string
let version = "Clexer V1.0f 10.8.99 Hugues Cassé"


(*
 * input handle
 *)
type handle = {
	h_interactive: bool;
	h_in_channel: in_channel;
	mutable h_line: string;
	mutable h_buffer: string;
	mutable h_pos: int;
	mutable h_lineno: int;
	h_out_channel: out_channel;
	mutable h_file_name: string;
	h_gcc: bool;
	h_linerec: bool;
	h_strict: bool;
	mutable h_pragma: (string * int * string) list
}
let current_handle = ref {
		h_interactive = false;
		h_in_channel = stdin;
		h_line = "";
		h_buffer = "";
		h_pos = 0;
		h_lineno = 0;
		h_out_channel = stdout;
		h_file_name = "";
		h_gcc = true;
		h_linerec = false;
		h_strict = false;
		h_pragma = []
	}

let interactive (h : handle) = h.h_interactive
let in_channel (h : handle) = h.h_in_channel
let line (h : handle) = h.h_line
let buffer (h : handle) = h.h_buffer
let pos (h : handle) = h.h_pos
let real_pos (i : int) (h : handle) = i - h.h_pos
let lineno (h : handle) = h.h_lineno
let out_channel (h : handle) = h.h_out_channel
let file_name (h : handle) = h.h_file_name
let linerec (h: handle) = h.h_linerec
let curfile _ = (!current_handle).h_file_name
let curline _ = (!current_handle).h_lineno
let has_gcc _ = (!current_handle).h_gcc
let is_strict _ = (!current_handle).h_strict
let pragma _ = (!current_handle).h_pragma

let add_pragma p = 
	(!current_handle).h_pragma <- (curfile (), curline (), p) :: (pragma ())

(*
 * Error handling
 *)
let underline_error (buffer : string) (start : int) (stop : int) =
	let len = String.length buffer in
	let start' = max 0 start in
	let stop' = max 1 stop in
	(
		(if start' > 0 then (String.sub buffer 0 start') else "")
		^ "\027[4m"
		^ (if (stop' - start') <> 0
			then (String.sub buffer start' (stop' - start' ) )
			else ""
		)
		^ "\027[0m"
		^ (if stop' < len then (String.sub buffer stop' (len - stop') ) else "")
	)

let display_error msg token_start token_end =
	output_string (out_channel !current_handle) (
		(if (interactive !current_handle)
			then ""
			else
				(file_name !current_handle) ^ "["
				^ (string_of_int (lineno !current_handle)) ^ "] "
		)
		^ msg ^ ": "
		^ (underline_error
				(line !current_handle)
				(real_pos token_start !current_handle)
				(real_pos token_end !current_handle)
		)
	);
	flush (out_channel !current_handle)

let display_semantic_error msg =
	display_error msg (pos !current_handle) (pos !current_handle)


let error msg =
	display_error msg (Parsing.symbol_start ()) (Parsing.symbol_end ());
	raise Parsing.Parse_error

let test_gcc _ = if not (!current_handle).h_gcc then  error "forbidden GCC syntax"


(*
** Keyword hashtable
*)
let id token _ = token

module HashString =
struct
	type t = string
	let equal (s1 : t) (s2 : t) = s1 = s2
	let hash (s : t) = Hashtbl.hash s
end
module StringHashtbl = Hashtbl.Make(HashString)
let lexicon = StringHashtbl.create 211
let keywords =
	[
		("__const", id CONST);
		("__restrict", id RESTRICT);
		("_Bool", id (NAMED_TYPE "_Bool"));
		("_Complex", id (NAMED_TYPE "_Complex"));
		("_Imaginery", id (NAMED_TYPE "_Imaginery"));
		("asm", id ASM);
		("auto", id AUTO);
		("break", fun _ -> BREAK (curfile(), curline()));
		("char", id CHAR);
		("case", fun _ -> CASE (curfile(), curline()));
		("const", id CONST);
		("continue", fun _ -> CONTINUE (curfile(), curline()));
		("default", fun _ -> DEFAULT (curfile(), curline()));
		("do", fun _ -> DO (curfile(), curline()));
		("double", id DOUBLE);
		("else", fun _ -> ELSE (curfile(), curline()));
		("enum", id ENUM);
		("extern", id EXTERN);
		("float", id FLOAT);
		("for", fun _ -> FOR (curfile(), curline()));
		("goto", fun _ -> GOTO (curfile(), curline()));
		("if", fun _ -> IF (curfile(), curline()));
		("inline", id INLINE);	(* C99 *)
		("int", id INT);
		("long", id LONG);
		("register", id REGISTER);
		("restrict", id RESTRICT);	(** Non-supported by GCC ??? *)
		("return", fun _ -> RETURN (curfile(), curline()));
		("short", id SHORT);
		("signed", id SIGNED);
		("static", id STATIC);
		("struct", id STRUCT);
		("switch", fun _ -> SWITCH (curfile(), curline()));
		("typedef", id TYPEDEF);
		("union", id UNION);
		("unsigned", id UNSIGNED);
		("void", id VOID);
		("volatile", id VOLATILE);		
		("while", fun _ -> WHILE (curfile(), curline()));
	]

(*** Specific GNU ***)
let gnu_keywords : (string * (unit -> Cparser.token)) list = [
		("__attribute__", id ATTRIBUTE);
		("__extension__", id EXTENSION);
		("__inline", id INLINE);	(* strange: really ? *)
		("__inline__", id INLINE);
		("__asm__", id ASM);
		("__asm", id ASM);
		("__volatile__", id VOLATILE)
	]

let init_lexicon _ =
	let add (key, token) = StringHashtbl.add lexicon key token in
	StringHashtbl.clear lexicon;
	List.iter add keywords;
	if has_gcc ()  then
		List.iter add gnu_keywords

let add_type name =
	StringHashtbl.add lexicon name (id (NAMED_TYPE name))

let context : string list list ref = ref []

let push_context _ = context := []::!context

let pop_context _ =
	match !context with
	[] -> raise (InternalError "Empty context stack")
	| con::sub ->
		(context := sub;
		List.iter (fun name -> StringHashtbl.remove lexicon name) con)

let add_identifier name =
	match !context with
	[] -> raise (InternalError "Empty context stack")
	| con::sub ->
		(context := (name::con)::sub;
		StringHashtbl.add lexicon name (id (IDENT name)))


(*
** Useful primitives
*)
let rem_quotes str = String.sub str 1 ((String.length str) - 2)
let scan_ident id =
	try (StringHashtbl.find lexicon id) ()
	with Not_found ->
		IDENT id
(*
** Buffer processor
*)

let set_line num =
	(!current_handle).h_lineno <- num - 1

let set_name name =
	(!current_handle).h_file_name <- name


(*** escape character management ***)
let scan_escape str =
	match str with
	"n" -> "\n"
	| "r" -> "\r"
	| "t" -> "\t"
	| "b" -> "\b"
	| _ -> str
let get_value chr =
	match chr with
	'0'..'9' -> (Char.code chr) - (Char.code '0')
	| 'a'..'z' -> (Char.code chr) - (Char.code 'a') + 10
	| 'A'..'Z' -> (Char.code chr) - (Char.code 'A') + 10
	| _ -> 0
let scan_hex_escape str =
	String.make 1 (Char.chr (
		(get_value (String.get str 0)) * 16
		+ (get_value (String.get str 1))
	))
let scan_oct_escape str =
	String.make 1 (Char.chr (
		(get_value (String.get str 0)) * 64
		+ (get_value (String.get str 1)) * 8
		+ (get_value (String.get str 2))
	))
}

let bindigit = ['0' '1']
let	decdigit = ['0'-'9']
let octdigit = ['0'-'7']
let hexdigit = ['0'-'9' 'a'-'f' 'A'-'F']
let letter = ['a'- 'z' 'A'-'Z']

let usuffix = ('u'|'U')
let lsuffix = ('l'|'L')
let llsuffix = ("ll"|"LL")
let intsuffix = (llsuffix|lsuffix|usuffix|(usuffix lsuffix)|(lsuffix usuffix)|(llsuffix usuffix)|(usuffix llsuffix))?
let floatsuffix = ['f' 'F' 'l' 'L']

let intnum = decdigit+ intsuffix?
let octnum = '0' octdigit+ intsuffix?
let hexnum = '0' ['x' 'X'] hexdigit+ intsuffix?
let binnum = '0' ['b' 'B'] bindigit+ intsuffix?

let exponent = ['e' 'E']['+' '-']? decdigit+
let fraction  = '.' decdigit+
let floatraw = (intnum? fraction)
			|(intnum exponent)
			|(intnum? fraction exponent)
			|(intnum '.')
let hfloatraw =	('0' ['x' 'X'] hexdigit* '.' hexdigit+)
			  |	('0' ['x' 'X'] hexdigit+ ['p' 'P'] ['+' '-']? decdigit+)
			  |	('0' ['x' 'X'] hexdigit* '.' hexdigit+ ['p' 'P'] ['+' '-']? decdigit+)
let floatnum = floatraw floatsuffix?
let hfloatnum = hfloatraw floatsuffix?

let ident = (letter|'_')(letter|decdigit|'_')*
let blank = [' ' '\t' '\n' '\r']
let space = [' ' '\t']
let escape = '\\' _
let hex_escape = '\\' ['x' 'X'] hexdigit hexdigit
let oct_escape = '\\' octdigit  octdigit octdigit

rule initial =
	parse 	"/*"				{let _ = comment lexbuf in initial lexbuf}
	|		"//"				{test_gcc (); let _ = line_comment lexbuf in initial lexbuf }
	|		blank				{initial lexbuf}
	|		"#" space* "pragma"	{ add_pragma (pragma lexbuf); initial lexbuf }
	|		'#' space* "line"	{ line lexbuf }
	|		'#'					{ line lexbuf}

	|		'\''			{CST_CHAR (chr lexbuf)}
	|		'"'				{CST_STRING (str lexbuf)}
	|		floatnum		{CST_FLOAT (Lexing.lexeme lexbuf)}
	|		hfloatnum		{CST_FLOAT (Lexing.lexeme lexbuf)}
	|		hexnum			{CST_INT (Lexing.lexeme lexbuf)}
	|		octnum			{CST_INT (Lexing.lexeme lexbuf)}
	|		intnum			{CST_INT (Lexing.lexeme lexbuf)}
	|		binnum			{CST_INT (Lexing.lexeme lexbuf)}

	|		"!quit!"		{EOF}
	|		"..."			{ELLIPSIS(curfile(), curline())}
	|		"+="			{PLUS_EQ(curfile(), curline())}
	|		"-="			{MINUS_EQ(curfile(), curline())}
	|		"*="			{STAR_EQ(curfile(), curline())}
	|		"/="			{SLASH_EQ(curfile(), curline())}
	|		"%="			{PERCENT_EQ(curfile(), curline())}
	|		"|="			{PIPE_EQ(curfile(), curline())}
	|		"&="			{AND_EQ(curfile(), curline())}
	|		"^="			{CIRC_EQ(curfile(), curline())}
	|		"<<="			{INF_INF_EQ(curfile(), curline())}
	|		">>="			{SUP_SUP_EQ(curfile(), curline())}
	|		"<<"			{INF_INF}
	|		">>"			{SUP_SUP}
	|		"=="			{EQ_EQ}
	|		"!="			{EXCLAM_EQ}
	|		"<="			{INF_EQ}
	|		">="			{SUP_EQ}
	|		"="				{EQ(curfile(), curline())}
	|		"<"				{INF}
	|		">"				{SUP}
	|		"++"			{PLUS_PLUS}
	|		"--"			{MINUS_MINUS}
	|		"->"			{ARROW}
	|		'+'				{PLUS}
	|		'-'				{MINUS}
	|		'*'				{STAR}
	|		'/'				{SLASH}
	|		'%'				{PERCENT}
	|		'!'				{EXCLAM}
	|		"&&"			{AND_AND}
	|		"||"			{PIPE_PIPE}
	|		'&'				{AND}
	|		'|'				{PIPE}
	|		'^'				{CIRC}
	|		'?'				{QUEST(curfile(), curline())}
	|		':'				{COLON(curfile(), curline())}
	|		'~'				{TILDE}

	|		'{'				{LBRACE(curfile(), curline())}
	|		'}'				{RBRACE(curfile(), curline())}
	|		'['				{LBRACKET(curfile(), curline())}
	|		']'				{RBRACKET(curfile(), curline())}
	|		'('				{LPAREN(curfile(), curline())}
	|		')'				{RPAREN(curfile(), curline())}
	|		';'				{SEMICOLON(curfile(), curline())}
	|		','				{COMMA(curfile(), curline())}
	|		'.'				{DOT}
	|		"sizeof"		{SIZEOF}
	|		ident			{ scan_ident (Lexing.lexeme lexbuf)}

	|		eof				{EOF}
	|		_				{display_error
								"Invalid symbol"
								(Lexing.lexeme_start lexbuf)
								(Lexing.lexeme_end lexbuf);
							initial lexbuf}
and comment =
	parse 	"*/"			{()}
	| 		_ 				{comment lexbuf}

and line_comment =
	parse 	"\n"			{()}
	| 		_ 				{line_comment lexbuf}

(* # <line number> <file name> ... *)
and line =
	parse	'\n'			{initial lexbuf}
	|	blank				{line lexbuf}
	|	intnum				{set_line (int_of_string (Lexing.lexeme lexbuf));
							file lexbuf}
	|	_					{endline lexbuf}
and file =
	parse '\n'				{initial lexbuf}
	|	blank				{file lexbuf}
	|	'"' [^ '"']* '"' 	{set_name (rem_quotes (Lexing.lexeme lexbuf));
							endline lexbuf}
	|	_					{endline lexbuf}
and endline =
	parse '\n' 				{initial lexbuf}
	|	_					{endline lexbuf}

and str =
	parse	'"'				{""}
	|		hex_escape		{let cur = scan_hex_escape (String.sub
							(Lexing.lexeme lexbuf) 2 2) in cur ^ (str lexbuf)}
	|		oct_escape		{let cur = scan_oct_escape (String.sub
							(Lexing.lexeme lexbuf) 1 3) in cur ^ (str lexbuf)}
	|		"\\0"			{(String.make 1 (Char.chr 0)) ^ (str lexbuf)}
	|		escape			{let cur = scan_escape (String.sub
							(Lexing.lexeme lexbuf) 1 1) in cur ^ (str lexbuf)}
	|		_				{ (Lexing.lexeme lexbuf) ^  (str lexbuf)}

and chr =
	parse	'\''			{""}
	|		hex_escape		{let cur = scan_hex_escape (String.sub
							(Lexing.lexeme lexbuf) 2 2) in cur ^ (chr lexbuf)}
	|		oct_escape		{let cur = scan_oct_escape (String.sub
							(Lexing.lexeme lexbuf) 1 3) in cur ^ (chr lexbuf)}
	|		"\\0"			{(String.make 1 (Char.chr 0)) ^ (chr lexbuf)}
	|		escape			{let cur = scan_escape (String.sub
							(Lexing.lexeme lexbuf) 1 1) in cur ^ (chr lexbuf)}
	|		_				{let cur = Lexing.lexeme lexbuf in cur ^ (chr lexbuf)}

and pragma =
	parse	'\n'			{ "" }
	|		_				{ (Lexing.lexeme lexbuf) ^ (pragma lexbuf) }

{

(*** get_buffer ***)
let get_buffer (hr : handle ref) (dst : string) (len : int) : int =
	(*let (inter, chan, line, buffer, pos, lineno, out, name) = !hr in*)
	let h = !hr in
	try
		let (bufferp, linep, posp, linenop) =
			if h.h_buffer <> ""
			then (h.h_buffer, h.h_line , h.h_pos, h.h_lineno)
			else
				let buffer = (input_line h.h_in_channel) ^ "\n" in
				(
					buffer,
					(if h.h_interactive then h.h_line ^ buffer else buffer),
					(if h.h_interactive then h.h_pos else h.h_pos + (String.length h.h_line)),
					h.h_lineno + 1
				) in
		(*let _ = print_endline ("-->" ^ linep) in*)
		let bufl = String.length bufferp in
		let lenp = min len bufl in
		let buffers = if bufl = lenp
			then ""
			else String.sub bufferp lenp (bufl - lenp) in
		begin
			String.blit bufferp 0 dst 0 lenp;
			h.h_line <- linep;
			h.h_buffer <- buffers;
			h.h_pos <- posp;
			h.h_lineno <- linenop;
			lenp
		end
	with End_of_file -> 0


(* init: handle -> ()
**	Initialize lexer.
*)
let init hdl =
	init_lexicon ();
	current_handle := hdl
}
