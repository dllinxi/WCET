(* cvariable -- use Frontc CAML  set of variable of cabs expression union , inter, inclus of variable set
**
** Project:	O_Range
** File:	cvariable.ml
** Version:	1.1
** Date:	11.7.2008
** Author:	Marianne de Michiel
*)

open Cabs
open Cxml
open Cprint

let version = "cvariable Marianne de Michiel"
(* les variables *)	
let rechercheVar liste var1 =		List.mem var1 liste 
	
	
let rec union l1 l2 =
if l1 = [] then l2
else 	
begin
	if l2 = [] then l1
	else 
		begin
			if List.mem (List.hd l1) l2 then union (List.tl l1) l2
			else (List.hd l1) ::(union (List.tl l1)   l2)
		end
end



let rec inclus l1 l2 =
if l1 = [] then true
else 	
begin
	if l2 = [] then false
	else 
		begin
			if List.mem (List.hd l1) l2 then inclus (List.tl l1) l2
			else false
		end
end
(*let rec eqexp e1 e2 =
	match e1 with
		UNARY (op1, en1) -> 				(match e2 with UNARY (op2, en2) -> 	if op1 = op2 then	eqexp en1 en2 else false |_-> false)
	| BINARY (op1, en11, en12) -> 			(match e2 with  BINARY (op2, en21, en22) -> 	if op1 = op2 then	eqexp en11 en21 &&   eqexp en12 en22 else false |_-> false)
	| CONSTANT n -> 						(match e2 with CONSTANT b -> 	if b = n then	true else false |_-> false)
	| VARIABLE name -> 						(match e2 with VARIABLE name2 -> 	if  name = name2 then	true else false |_-> false)
	| EXPR_SIZEOF e ->						(match e2 with EXPR_SIZEOF e3 ->		eqexp e e3  |_-> false)
	| TYPE_SIZEOF t ->						(match e2 with TYPE_SIZEOF t2 -> 	if t = t2 then true else false |_-> false)
	| _ -> 						 		false*)

	
let rec listeDesVarsDeExpSeules exp =
	match exp with
		UNARY (_, e) -> 				listeDesVarsDeExpSeules e 
	| BINARY (_, exp1, exp2) -> 		union( listeDesVarsDeExpSeules  exp1)	(listeDesVarsDeExpSeules exp2)
	| QUESTION (exp1, exp2, exp3) ->	union (listeDesVarsDeExpSeules exp1) (union (listeDesVarsDeExpSeules exp2)	(listeDesVarsDeExpSeules  exp3))
	| CAST (_, e) ->					listeDesVarsDeExpSeules  e
	| CALL (e, args) ->					(*union	(listeDesVarsDeExpSeules e)*) ( traiterCommaExp args)
	| COMMA e -> 						traiterCommaExp e
	| CONSTANT _ -> 					[]
	| VARIABLE name -> 					[name ] 
	| EXPR_SIZEOF e ->					listeDesVarsDeExpSeules e
	| TYPE_SIZEOF _ ->					[]
	| INDEX (e, idx) ->					union (listeDesVarsDeExpSeules  e) (listeDesVarsDeExpSeules idx)
	| MEMBEROF (e, _) ->				listeDesVarsDeExpSeules e
	| MEMBEROFPTR (e, _) ->				listeDesVarsDeExpSeules  e
	| GNU_BODY (_,_)(*decs, stat*) ->	[]
	| _ -> 						 		[]
	 
	
and traiterCommaExp liste =
if liste = [] then [] else union (listeDesVarsDeExpSeules (List.hd liste)) (traiterCommaExp (List.tl liste)) 

let rec listeDesVarsDeExpSeulesTab exp =
	match exp with
		UNARY (_, e) -> 				listeDesVarsDeExpSeulesTab e 
	| BINARY (_, exp1, exp2) -> 		union( listeDesVarsDeExpSeulesTab  exp1)	(listeDesVarsDeExpSeulesTab exp2)
	| QUESTION (exp1, exp2, exp3) ->	union (listeDesVarsDeExpSeulesTab exp1) (union (listeDesVarsDeExpSeulesTab exp2)	(listeDesVarsDeExpSeulesTab  exp3))
	| CAST (_, e) ->					listeDesVarsDeExpSeulesTab  e
	| CALL (e, args) ->					[]
	| COMMA e -> 						[]
	| CONSTANT _ -> 					[]
	| VARIABLE name -> 					[name ] 
	| EXPR_SIZEOF e ->					[]
	| TYPE_SIZEOF _ ->					[]
	| INDEX (e, idx) ->					 (listeDesVarsDeExpSeulesTab  e) 
	| MEMBEROF (e, _) ->				listeDesVarsDeExpSeulesTab e
	| MEMBEROFPTR (e, _) ->				listeDesVarsDeExpSeulesTab  e
	| GNU_BODY (_,_)(*decs, stat*) ->	[]
	| _ -> 						 		[]
let rec listeDesVarsDeExpSeulesWCAndIF exp =
	match exp with
		UNARY (_, e) -> 				listeDesVarsDeExpSeules e 
	| BINARY (_, exp1, exp2) -> 		union( listeDesVarsDeExpSeules  exp1)	(listeDesVarsDeExpSeules exp2)
	| QUESTION (exp1, exp2, exp3) ->	union (listeDesVarsDeExpSeules exp1) (union (listeDesVarsDeExpSeules exp2)	(listeDesVarsDeExpSeules  exp3))
	| CAST (_, e) ->					listeDesVarsDeExpSeules  e
	| CALL (e, args) ->					(*union	(listeDesVarsDeExpSeules e)*) ( traiterCommaExp args)
	| COMMA e -> 						traiterCommaExp e
	| CONSTANT _ -> 					[]
	| VARIABLE name -> 					[name ] 
	| EXPR_SIZEOF e ->					listeDesVarsDeExpSeules e
	| TYPE_SIZEOF _ ->					[]
	| INDEX (e, idx) ->					union (listeDesVarsDeExpSeules  e) (listeDesVarsDeExpSeules idx)
	| MEMBEROF (e, _) ->				listeDesVarsDeExpSeules e
	| MEMBEROFPTR (e, _) ->				listeDesVarsDeExpSeules  e
	| GNU_BODY (_,_)(*decs, stat*) ->	[]
	| _ -> 						 		[]









let rec getNbIt v exp =
	match exp with
		UNARY (_, e) -> 				getNbIt v e 
	| BINARY (_, exp1, exp2) -> 		 ( getNbIt v  exp1)	+ (getNbIt v exp2)
	| QUESTION (exp1, exp2, exp3) ->	 (getNbIt v exp1) + (getNbIt v exp2)	+ (getNbIt v  exp3)
	| CAST (_, e) ->					getNbIt v  e
	| CALL (e, args) ->					(getNbIt v e)+  ( traiterCommaExpNb v args)
	| COMMA e -> 						traiterCommaExpNb v  e
	| CONSTANT _ -> 					0
	| VARIABLE name -> 					if name = v then 1 else 0
	| EXPR_SIZEOF e ->					getNbIt v  e
	| TYPE_SIZEOF _ ->					0
	| INDEX (e, idx) ->					(getNbIt v  e) + (getNbIt v  idx)
	| MEMBEROF (e, _) ->				getNbIt v  e
	| MEMBEROFPTR (e, _) ->				getNbIt v  e
	| GNU_BODY (_,_)(*decs, stat*) ->	0
	| _ -> 						 		0
and traiterCommaExpNb v liste =
if liste = [] then 0 else   (getNbIt v  (List.hd liste)) + (traiterCommaExpNb v (List.tl liste)) 

(*



*)


let afficherListeVarExp l =
List.iter (fun e-> Printf.printf "%s " e)l;
Printf.printf "\n"

let rec estDansListeVar id liste =
List.mem id liste 

let rec intersection l1 l2  =
if l1 = [] || l2 = [] then begin  []end
else	
begin
	if List.mem (List.hd l2) l1 then List.append [List.hd l2] (	intersection l1 (List.tl l2) )
	else  intersection l1 (List.tl l2) 
end
