(* a variable name must be used only once -> rename
printf call must be replace...*)
(* rename -- use Frontc CAML  a variable name must be used only once -> rename. Considers statics. 
**
** Project:	O_Range
** File:	rename.ml
** Version:	1.1
** Date:	11.7.2008
** Author:	Marianne de Michiel
*)


open Cabs
open Rename
let version = "renamestatic Marianne de Michiel"


(* type newIdent  (string, string) List *)
let monContexteStatic = ref [] (* newIdent list *) 


let existeAssosStaticVarNewVal var contexte = List.mem_assoc var  contexte
let getAssosStaticVarNewVal var contexte = List.assoc var contexte



let rec newStaticName var fname=
						let newStr =  (Printf.sprintf "%s_%s" var fname) in
						monContexteStatic := List.append !monContexteStatic [(var, newStr)];
						newStr

and convert_comma_expsStatic exps name =	  List.map (fun dec -> convert_expressionStatic  dec name)		exps 


and convert_expressionStatic (exp : expression)( name:string) =
		 match exp with
			  UNARY (op, exp1) -> UNARY (op, (convert_expressionStatic exp1 name))
			| BINARY (op, exp1, exp2) -> 
					BINARY (op, (convert_expressionStatic exp1 name), (convert_expressionStatic exp2 name))
			| QUESTION (exp1, exp2, exp3) ->
					QUESTION ((convert_expressionStatic exp1 name), (convert_expressionStatic exp2 name), (convert_expressionStatic exp3 name))
			| CAST (typ, exp) ->		CAST (typ, (convert_expressionStatic exp name))
			| CALL (exp, args) ->		
						if exp = VARIABLE("printf") || exp= VARIABLE("sprintf") || exp = VARIABLE("fprintf") then
						begin
							(* to be completed || exp ="scanf" *)
							(*lineariser_comma_exp(exps)*) NOTHING
						end
						else	CALL(exp,convert_comma_expsStatic args name)
			| COMMA exps ->				COMMA(convert_comma_expsStatic exps name)
			| CONSTANT cst -> 			CONSTANT cst	
			| VARIABLE n ->			
					if existeAssosStaticVarNewVal n !monContexteStatic then VARIABLE (getAssosStaticVarNewVal n !monContexteStatic)
					else VARIABLE n
			| EXPR_SIZEOF exp ->		EXPR_SIZEOF (convert_expressionStatic exp name)
			| TYPE_SIZEOF typ ->		TYPE_SIZEOF typ
			| INDEX (exp, idx) ->		INDEX ((convert_expressionStatic exp name), idx)
			| MEMBEROF (exp, fld) ->	MEMBEROF ((convert_expressionStatic exp name), fld)
			| MEMBEROFPTR (exp, fld) ->	MEMBEROFPTR ((convert_expressionStatic exp name), fld)
			| EXPR_LINE (exp, file, line)->EXPR_LINE ((convert_expressionStatic exp name), file, line)
			| GNU_BODY (decs, stat) ->  let decs = List.map (fun def-> convert_defStatic def name ) decs in
										GNU_BODY (decs,convert_statementStatic  stat name) 
			| _ -> exp


			

and convert_statementStatic  stat name = 
		match stat with
		  COMPUTATION exp -> COMPUTATION (convert_expressionStatic exp name)
		| BLOCK (defs, stat) -> 
			
			let defs =  List.map (fun def-> convert_defStatic def name ) defs  in
			let res = (if stat <> NOP then   BLOCK (defs, (convert_statementStatic stat name))  else  BLOCK (defs, NOP) ) in
			
			res
		| SEQUENCE (s1, s2) -> 
				SEQUENCE (convert_statementStatic s1 name, convert_statementStatic s2 name)
		| IF (exp, s1, s2) ->  IF (convert_expressionStatic exp name, convert_statementStatic s1 name, convert_statementStatic s2 name)
		| WHILE (exp, stat) -> WHILE (convert_expressionStatic  exp name, convert_statementStatic  stat name)
		| DOWHILE (exp, stat)->DOWHILE (convert_expressionStatic  exp name, convert_statementStatic  stat name) 
		| FOR (exp1, exp2, exp3, stat) ->
			FOR (convert_expressionStatic  exp1 name, convert_expressionStatic  exp2 name,convert_expressionStatic  exp3 name, convert_statementStatic  stat name)
		| BREAK ->				BREAK
		| CONTINUE ->			CONTINUE
		| RETURN exp -> 		RETURN (convert_expressionStatic exp name)
		| SWITCH (exp, stat) ->	SWITCH (convert_expressionStatic exp name, convert_statementStatic stat name)
		| CASE (exp, stat) ->	CASE (convert_expressionStatic exp name, convert_statementStatic stat name)
		| DEFAULT stat -> 		DEFAULT (convert_statementStatic stat name)
		| LABEL (name, stat) ->	LABEL (name, convert_statementStatic stat name) 
		| GOTO name ->			GOTO name 
		| ASM desc ->			ASM desc
		| GNU_ASM (desc, output, input, mods) ->GNU_ASM (desc, output, input, mods) (*REVOIR*)		
		| STAT_LINE (stat, file, line) -> STAT_LINE (convert_statementStatic  stat  name, file, line) 
		| _ ->	stat


	
	and convert_nameStatic ((id, typ, attr, exp) : name) fname=
		let nouId = newStaticName id fname in (* on ne renomme pas les fonctions pas les mêmes noms*)
		let nouExp = (if exp <> NOTHING then  convert_expressionStatic exp fname else exp) in
		(nouId, typ, attr, nouExp)

	and convert_nameStatic_groupStatic 	(typ, sto, names) fname =
		(typ, sto, List.map (fun def-> convert_nameStatic def fname)names)


and convert_defStatic def name=
		match def with
			FUNDEF (proto, body) ->
				let (typeP,storageP,nameP)=proto in
				let proto = (typeP, storageP,convert_param nameP) in
			    let (decs, stat) = body in 
				let ndecs = List.map	(fun dec -> convert_def dec)		decs in
				let res = FUNDEF (  proto,  (ndecs, convert_statementStatic stat name)) in
			res
			| OLDFUNDEF (proto, decs, body) -> (* no because convert by frontc into FUNDEF*)
				def
			| DECDEF names -> 
				let (baseType, sto, namelist) = names in
 				let isStatic = (	match sto with 	 STATIC-> true |_-> false) in
				if isProto baseType = false && isStatic then 
					DECDEF (convert_nameStatic_groupStatic names name)
				else def
			| _ -> def 

 

let goStatic  (defs : file) (name:string) =
monContexteStatic :=[];
  let decs = List.map	(fun dec -> convert_defStatic dec name)		defs in
decs
  
