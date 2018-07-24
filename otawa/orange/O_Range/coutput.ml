(* coutput -- (non-pretty) expression printer
**
** Project:	FrontC
** File:	coutput.ml
** Version:	0.00001
** Date:	XX.XX.08
** Author:	moi
**
*)

open Cabs
open Cprint
let version = "Coutput"





(** Module for converting strings to Xml PCDATA. *)

(** Converts a character to the corresponding entity (if any).
    The initial '&' and final ';' are not part of the result.
    Eg. '<' -> Some "lt". *)
let pcdata_from_char = function
  | '&' -> Some "amp"
  | '<' -> Some "lt"
  | '>' -> Some "gt"
  | '\'' -> Some "apos"
  | '"' -> Some "quot"
  | _ -> None

(** Convert a string to Xml pcdata.
    This means that the Xml reserved characters have been changed to the corresponding Xml entity.
    Eg. "<a>" -> "&lt;a&gt;". *)
let pcdata_from_string s =
  let s_length = String.length s in
  let buf = Buffer.create 1 in
  let upto = ref 0 in
  (* Invariant: all characters of indices up to !upto excluded
     have been treated and transfered to the buffer buf. *)
  let copy_upto new_upto =
    Buffer.add_substring buf s !upto (new_upto - !upto);
    upto := new_upto in
  let copy_entity ent =
    Buffer.add_char buf '&';
    Buffer.add_string buf ent;
    Buffer.add_char buf ';';
    incr upto in
  (* Traversal of the string. *)
  for i = 0 to s_length - 1
  do
    match pcdata_from_char (String.get s i) with
    | Some entity ->
      begin
	copy_upto i;
	copy_entity entity
      end
    | None -> ()
  done;
  if !upto = 0 then s (* Optimization: no entities were met. *)
  else
    begin
      copy_upto s_length;
      Buffer.contents buf
    end




let string_from_expr = 
     let rec aux currentPrio e =
	  let (str, prio) = get_operator e in
	  let result = match e with

		  NOTHING -> ""

		 | UNARY(op, arg) -> 	
			str^(aux prio arg) 			

		 | BINARY(op, arg1, arg2)  -> 	
			(aux prio arg1)^str^(aux (prio + 1) arg2)

		 | QUESTION (arg1, arg2, arg3)  -> 
			(aux 2 arg1)^"?"^(aux 2 arg2)^(aux 2 arg3)

		 | CAST (typ, arg) -> 
			"(CAST)"^(aux 15 arg) 

		 | CALL (fexpr, argl)-> 
			(aux 16 fexpr)^"("^(aux 0 (COMMA argl))^")"

		 | (COMMA l)  -> 
	 		List.fold_left (fun s arg -> s^(if (s = "") then s else ",")^(aux 1 arg)) "" l

		 | CONSTANT cst -> 
		      (match cst with
	 		 CONST_INT i -> i   
			|CONST_FLOAT f -> f
			|RCONST_INT i -> Printf.sprintf "%d" i   
			|RCONST_FLOAT f -> Printf.sprintf "%g" f
			|CONST_CHAR c -> c
			|CONST_STRING s -> "\""^s^"\""
			|CONST_COMPOUND cmp -> "[COMPOUND]"
		      )

		 | VARIABLE name -> name

		 | (EXPR_SIZEOF arg) -> 
	 		"sizeof("^(aux 0 arg)^")" 

		 | (TYPE_SIZEOF arg)-> "sizeof(TYPE)"

		 | INDEX (arg, idx) -> (aux 16 arg)^"["^(aux 0 idx)^"]"


		 | MEMBEROF (arg, field) -> (aux 16 arg)^"."^field


		 | MEMBEROFPTR (arg, field) -> (aux 16 arg)^"->"^field


		 | GNU_BODY _ -> "[GNU_BODY]"

		 | EXPR_LINE (expr, _,_ ) -> aux currentPrio expr
	 
	 
	 in if (prio < currentPrio) then "("^result^")" else result
		
      in (aux 0 )




let pcdata_from_expr e =
	pcdata_from_string (string_from_expr e)




