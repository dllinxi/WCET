(* cexptostr -- use Frontc CAML change Cabs expression to string
**
** Project:	O_Range
** File:	cexptostr.ml
** Version:	1.1
** Date:	11.7.2008
** Author:	Marianne de Michiel
*)


open Cabs
open Frontc

let version = "cexptostr Marianne de Michiel"

let get_sign si =
	match si with
	NO_SIGN -> ""
	| SIGNED -> "signed "
	| UNSIGNED -> "unsigned "

let get_size siz =
	match siz with
	NO_SIZE -> ""
	| SHORT -> "short "
	| LONG -> "long "
	| LONG_LONG -> "long long "


let rec get_operator exp =
	match exp with
	NOTHING -> ("", 16)
	| UNARY (op, _) ->
		(match op with
		MINUS -> ("-", 13)
		| PLUS -> ("+", 13)
		| NOT -> ("!", 13)
		| BNOT -> ("~", 13)
		| MEMOF -> ("*", 13)
		| ADDROF -> ("&", 13)
		| PREINCR -> ("++", 13)
		| PREDECR -> ("--", 13)
		| POSINCR -> ("++", 14)
		| POSDECR -> ("--", 14))
	| BINARY (op, _, _) ->
		(match op with
		MUL -> ("*", 12)
		| DIV -> ("/", 12)
		| MOD -> ("%", 12)
		| ADD -> ("+", 11)
		| SUB -> ("-", 11)
		| SHL -> ("<<", 10)
		| SHR -> (">>", 10)
		| LT -> ("<", 9)
		| LE -> ("<=", 9)
		| GT -> (">", 9)
		| GE -> (">=", 9)
		| EQ -> ("==", 8)
		| NE -> ("!=", 8)
		| BAND -> ("&", 7)
		| XOR -> ("^", 6)
		| BOR -> ("|", 5)
		| AND -> ("&&", 4)
		| OR -> ("||", 3)
		| ASSIGN -> ("=", 1)
		| ADD_ASSIGN -> ("+=", 1)
		| SUB_ASSIGN -> ("-=", 1)
		| MUL_ASSIGN -> ("*=", 1)
		| DIV_ASSIGN -> ("/=", 1)
		| MOD_ASSIGN -> ("%=", 1)
		| BAND_ASSIGN -> ("&=", 1)
		| BOR_ASSIGN -> ("|=", 1)
		| XOR_ASSIGN -> ("^=", 1)
		| SHL_ASSIGN -> ("<<=", 1)
		| SHR_ASSIGN -> (">>=", 1))
	| QUESTION _ -> ("", 2)
	| CAST _ -> ("", 13)
	| CALL _ -> ("", 15)
	| COMMA _ -> ("", 0)
	| CONSTANT _ -> ("", 16)
	| VARIABLE _ -> ("", 16)
	| EXPR_SIZEOF _ -> ("", 16)
	| TYPE_SIZEOF _ -> ("", 16)
	| INDEX _ -> ("", 15)
	| MEMBEROF _ -> ("", 15)
	| MEMBEROFPTR _ -> ("", 15)
	| GNU_BODY _ -> ("", 17)
	| EXPR_LINE (expr, _, _) -> get_operator expr


let escape_string str =
	let lng = String.length str in
	let conv value = String.make 1 (Char.chr (value + 
			(if value < 10 then (Char.code '0') else (Char.code 'a' - 10)))) in
	let rec build idx =
		if idx >= lng then ""
		else
			let sub = String.sub str idx 1 in
			let res = match sub with
				"\n" -> "\\n"
				| "\"" -> "\\\""
				| "'" -> "\\'"
				| "\r" -> "\\r"
				| "\t" -> "\\t"
				| "\b" -> "\\b"
				| "\000" -> "\\0"
				| _ -> if sub = (Char.escaped (String.get sub 0))
					then sub
					else let code = Char.code (String.get sub 0) in
						"\\"
						^ (conv (code / 64))
						^ (conv ((code mod 64) / 8))
						^ (conv (code mod 8)) in
			res ^ (build (idx + 1)) in
	build 0	

let new_line _ =
		output_char   stdout '\n'



(* convertir une expression en chaine *)	
	(*let rec pointerToString typ =
		match typ with
		PTR typ -> pointerToString typ^"*"
		| RESTRICT_PTR typ -> 
			pointerToString typ^"* __restrict ";
		| CONST typ -> pointerToString typ^" const "
		| VOLATILE typ -> pointerToString typ^" volatile "
		| ARRAY (typ, _) -> pointerToString typ
		| _ -> ""*)
	
	let rec base_typeToString expP typ =
		match typ with
		
		| VOID -> 						expP^ "void"
		| CHAR sign -> 					expP^ ((get_sign sign) ^ "char")
		| INT (size, sign) -> 			expP^ ((get_sign sign) ^ (get_size size) ^ "int")
		| BITFIELD (t, _) -> 			(*base_typeToString expP t*)base_typeToString expP typ
		| FLOAT size -> 				expP^ ((if size then "long " else "") ^ "float")
		| DOUBLE size -> 				expP^ ((if size then "long " else "") ^ "double")
		| NAMED_TYPE id -> 				expP^ id
		| ENUM ((*id*)_, (*items*)_) -> expP^"enum a finir"(*print_enum id items*)
		| STRUCT ((*id*)_, (*flds*)_) ->expP^"print_fields a finir"(*print_fields ("struct " ^ id) flds*)
		| UNION ((*id*)_, (*flds*)_) -> expP^"print_fields a finir"(*print_fields ("union " ^ id) flds*)
		| PROTO (typ, _, _) -> 			base_typeToString expP typ
		| OLD_PROTO (typ, _, _) ->		base_typeToString expP typ
		| PTR typ -> 					base_typeToString expP typ
		| RESTRICT_PTR typ -> 			base_typeToString expP typ
		| ARRAY (typ, _) -> 			base_typeToString expP typ(*; print_init_tab exp*)
		| CONST typ -> 					base_typeToString expP typ
		| VOLATILE typ -> 				base_typeToString expP typ
		| GNU_TYPE ((*attrs*)_, typ) ->  expP ^(*print_attributes attrs;*)"attribute a finir"^ base_typeToString "" typ
		| _(*NO_TYPE...*) -> 						expP			

		and onlytypeToString typ =
			base_typeToString "" typ;
			(*typeToString (fun _ -> ()) "" typ	*)
				
		(*let expressionToString ;*)
		and constantToString cst =
			match cst with
			CONST_INT i ->	  	 		i
			| CONST_FLOAT r ->		 	r
			| CONST_CHAR c ->		 		"'" ^ (escape_string c) ^ "'"
			| CONST_STRING s ->			"\"" ^ (escape_string s) ^ "\""
			| CONST_COMPOUND exps ->	"{ "^comma_expsToString exps ^"}"			(*print_comma_exps exps;*)
			| RCONST_INT i  -> Printf.sprintf "%d" i
			| RCONST_FLOAT r  -> Printf.sprintf "%g"  r
		
			
		and comma_expsToString (*exps*)_ =
			Printf.printf "definir la fonction  comma_expsToString\n";
			"commasToString"
			
	and expressionToString (expPr : string) (exp : expression) (lvl : int) =
		let (txt, lvl1) = get_operator exp in
		let expP =	if lvl > lvl1 then    "("^expPr else expPr in
		let fin =	if lvl > lvl1 then    ")" else "" in
				(match exp with
					
					  UNARY (op, exp1) ->		
							let (chaine) = expressionToString "" exp1 lvl1 in
								(match op with
									POSINCR | POSDECR ->		(expP^chaine^txt^fin);						
									| _ ->		(expP^txt^chaine ^fin);
							)
					| BINARY (_, exp1, exp2) ->
							(expP^(expressionToString "" exp1 lvl1)^" "^txt^" "^(expressionToString "" exp2 (lvl1+1) )^fin) ;				
					| QUESTION (exp1, exp2, exp3) ->	 
							(expP^(expressionToString "" exp1 2)^" ? "^	(expressionToString "" exp2 2) ^" : "^(expressionToString "" exp3 3 )^fin);
					| CAST (typ, exp) -> 	(expP^"("^onlytypeToString typ^	 ")"^(expressionToString "" exp 15)^fin);
					| CALL (exp, args) ->  	(expP^(expressionToString "" exp 16)^"( "^(comma_expsToString args)^")");
					| COMMA exps -> 		(expP^" "^(comma_expsToString exps)^fin);
					| CONSTANT cst ->		(expP^constantToString  cst^fin)
					| VARIABLE name ->		(expP^name^fin)			;
					| EXPR_SIZEOF exp ->  	(expP^"sizeof(" ^(expressionToString "" exp 0 ) ^ ")"^fin);
					| TYPE_SIZEOF typ -> 	(expP^"sizeof("^onlytypeToString typ^")"^fin);						
					| INDEX (exp, idx) ->	(expP^(expressionToString "" exp 16) ^ "["^expressionToString "" idx 0^ "]"^fin);
					| MEMBEROF (exp, fld) ->(expP^(expressionToString "" exp 16)^ "." ^ fld^fin);
					| MEMBEROFPTR (exp, fld) -> (expP^(expressionToString "" exp 16)^"->" ^ fld^fin);
					| GNU_BODY ((*decs*)_, (*stat*)_) ->  (expP^"( statement )"^fin);	
					| _ -> 			(expP^fin) ; 				
				)
			
		
