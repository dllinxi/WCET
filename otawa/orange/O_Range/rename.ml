(* a variable name must be used only once -> rename
printf call must be replace...*)
(** rename -- uses Frontc CAML. A variable name must be used only once -> rename
**
** Project:	O_Range
** File:	rename.ml
** Version:	1.1
** Date:	11.7.2008
** Author:	Marianne de Michiel
*)


open Cabs
let version = "rename Marianne de Michiel"

let rec isProto typ =
	match typ with
	 	PROTO (_, _, _) -> 		true
	| OLD_PROTO (_, _, _) ->	true
	| GNU_TYPE (_, typ) ->isProto typ
	| _ -> false

module type FunctiunContext = sig
  type funcon 
  val empty :funcon
  val setInput : string list->funcon->  funcon
  val setOutput  : string list->funcon->  funcon
  val setGF  : string list->funcon->  funcon
  val setGA  : string list->funcon->  funcon
  val setLocal : string list->funcon->  funcon
 val setOU : string list->funcon->  funcon
  val getInput : funcon-> string list
  val getOutput  : funcon-> string list
  val getGF  : funcon-> string list
  val getGA  : funcon-> string list
  val getLocal : funcon-> string list
   val getOtherUsed : funcon-> string list
  val setMulti :string list-> string list-> string list-> string list-> string list-> string list->  funcon
  val print: funcon -> unit
end ;;


module MyFunCont:FunctiunContext = struct
  type funcon = I of string list * string list * string list * string list * string list* string list
  (*  let empty = I([],[],[],[],[])
  let setInput x (I(i,o,gf,ga,l))=   I(x::i,o,gf,ga,l)
  let setOutput x (I(i,o,gf,ga,l))=   I(i,x::o,gf,ga,l)
  let setGF x   (I(i,o,gf,ga,l))=   I(i,o,x::gf,ga,l)
  let setGA x (I(i,o,gf,ga,l))=   I(i,o,gf,x::ga,l)
  let setLocal x (I(i,o,gf,ga,l))=   I(i,o,gf,ga,x::l)*)
  let empty = I([],[],[],[],[],[])
  let setInput x (I(i,o,gf,ga,l,u))=   I(x,o,gf,ga,l,u)
  let setOutput x (I(i,o,gf,ga,l,u))=   I(i,x,gf,ga,l,u)
  let setGF x   (I(i,o,gf,ga,l,u))=   I(i,o,x,ga,l,u)
  let setGA x (I(i,o,gf,ga,l,u))=   I(i,o,gf,x,l,u)
  let setLocal x (I(i,o,gf,ga,l,u))=   I(i,o,gf,ga,x,u)
  let setOU x (I(i,o,gf,ga,l,u))=   I(i,o,gf,ga,l,x)
  let getInput (I(i,o,gf,ga,l,u)) =  i
  let getOutput(I(i,o,gf,ga,l,u))  =   o
  let getGF (I(i,o,gf,ga,l,u)) = gf
  let getGA  (I(i,o,gf,ga,l,u))=  ga
  let getLocal (I(i,o,gf,ga,l,u))=   l
  let getOtherUsed (I(i,o,gf,ga,l,u))=   u
 
  let print (I(i,o,gf,ga,l,u)) = 
	Printf.printf "\n\t inputs\t:\t" ;List.iter (fun x -> Printf.printf "%s " x)i; 
	Printf.printf "\n\t outputs\t:\t" ;List.iter (fun x -> Printf.printf "%s " x)o;
	Printf.printf "\n\t globales first used\t:\t" ;List.iter (fun x -> Printf.printf "%s " x)gf;
	Printf.printf "\n\t globales assigned\t:\t" ;List.iter (fun x -> Printf.printf "%s " x)ga;
	Printf.printf "\n\t local\t:\t" ;List.iter (fun x -> Printf.printf "%s " x)l;  
	Printf.printf "\n\t other used\t:\t" ;List.iter (fun x -> Printf.printf "%s " x)u; Printf.printf "\n" 

  let setMulti i o gf ga l  u= I(i,o,gf,ga,l,u)
end 

module AllContext(D: FunctiunContext) = struct
	type t = (string * D.funcon) list
 	let empty : t = []
	let get s n = List.assoc n s
	let mem s n = List.mem_assoc n s
	let rec set s n v =
		match s with
		| [] ->[(n, v)]
		| (np, vp)::sp ->
			if n = np then (np, v)::sp else
			if n < np then (n, v)::s  else
			(np, vp)::(set sp n v)	
	let rec print s =
	match s with
	|[]->Printf.printf "\n\n"
	|   (np, vp)::sp ->Printf.printf "%s\n" np; MyFunCont.print vp; print sp
end 
module AFContext  = AllContext(MyFunCont)
let myAF =  ref []







(* type newIdent  (string, string) List *)
let monContexte = ref [] (* newIdent list *) 
let monContexteNotRenammed = ref [] (* newIdent list *) 
let used = ref [] (* newIdent list *) 



let listeDesVariablesFic = ref [] (* ident list *)
let existsVar var =  List.mem  var !listeDesVariablesFic
let addVar var = 
		listeDesVariablesFic := List.append [var] !listeDesVariablesFic

let existeAssosVarNewVal var contexte = List.mem_assoc var  contexte
let getAssosVarNewVal var contexte = List.assoc var contexte

let rec getNewIdent var instance= 
	let newStr = (Printf.sprintf "%s_%d" var instance) in
	if (existsVar newStr) then	getNewIdent var  (instance + 1) 
	else newStr

let renameIfNecessary var =
	let newVar = (	if existsVar var then 
					begin 
						let newStr = getNewIdent var 0 in
						
						monContexte := List.append !monContexte [(var, newStr)];
						newStr
					end 
					else ( var )) in

monContexteNotRenammed := List.append  [newVar] !monContexteNotRenammed ;
addVar newVar;
newVar


let rec  concatContexte contCour contEng =
if contEng = [] then contCour
else
begin
	let (var , _) = List.hd contEng in
	if existeAssosVarNewVal var contCour then concatContexte contCour (List.tl contEng)
	else List.append [List.hd contEng] (concatContexte contCour (List.tl contEng))
	
end




let convertListeParamES (pars : single_name list) =
	List.map
	(fun courant ->	
			let (typ, a, name) = courant in
			let (nom, x, y, z) = name in
			let name = (renameIfNecessary nom, x, y,z) in
			(typ, a, name)			
	) pars


let convert_type (typ : base_type ) =
		match typ with
		PROTO (typ1, pars, ell) ->  PROTO(typ1,convertListeParamES pars, ell) 
		| _ -> typ

let onlyNewVar context = List.map (fun (o,n)-> n)context
let mfilterLocal  mContexte myParam contexteEnglobant =
		List.filter (fun c -> if List.mem c myParam || List.mem c contexteEnglobant then false else true)mContexte

let rec convert_param ((id, typ, attr, exp) : name) =
		(id, convert_type typ, attr, convert_expression exp)



and convert_comma_exps exps =	List.map  convert_expression  exps


and convert_expression (exp : expression) =
		 match exp with
			  UNARY (op, exp1) -> UNARY (op, (convert_expression exp1))
			| BINARY (op, exp1, exp2) -> 
					BINARY (op, (convert_expression exp1), (convert_expression exp2))
			| QUESTION (exp1, exp2, exp3) ->
					QUESTION ((convert_expression exp1), (convert_expression exp2), (convert_expression exp3))
			| CAST (typ, exp) ->		CAST (typ, (convert_expression exp))
			| CALL (exp, args) ->		
						(*if exp = VARIABLE("printf") || exp= VARIABLE("sprintf") || exp = VARIABLE("fprintf") then
						begin
							(* to be completed || exp ="scanf" *)
							(*lineariser_comma_exp(exps)*) NOTHING
						end
						else*)	CALL(exp,convert_comma_exps args)
			| COMMA exps ->				COMMA(convert_comma_exps exps)
			| CONSTANT cst -> 			CONSTANT cst	
			| VARIABLE name ->			
					let newvar = if existeAssosVarNewVal name !monContexte then getAssosVarNewVal name !monContexte
								else   name in
					if List.mem newvar !used = false then used := newvar::!used;
						VARIABLE(newvar)
					
			| EXPR_SIZEOF exp ->		EXPR_SIZEOF (convert_expression exp)
			| TYPE_SIZEOF typ ->		TYPE_SIZEOF typ
			| INDEX (exp, idx) ->		INDEX ((convert_expression exp), idx)
			| MEMBEROF (exp, fld) ->	MEMBEROF ((convert_expression exp), fld)
			| MEMBEROFPTR (exp, fld) ->	MEMBEROFPTR ((convert_expression exp), fld)
			| EXPR_LINE (exp, file, line)->EXPR_LINE ((convert_expression exp), file, line)
			| GNU_BODY (decs, stat) ->  let decs = List.map convert_def decs in
										GNU_BODY (decs,convert_statement stat) 
			| _ -> exp


and conc l1 l2 =
if l1 = [] then l2
else if l2 = [] then l1
	 else if List.mem (List.hd l1) l2 then conc (List.tl l1) l2 else conc 	(List.tl l1) ( (List.hd l1) ::l2	)	

and convert_statement stat = 
		match stat with
		  COMPUTATION exp -> COMPUTATION (convert_expression exp)
		| BLOCK (defs, stat) -> 
			let contexteEnglobant = !monContexte in
			let monContexteNotRenammedEnglobant = !monContexteNotRenammed in
			monContexteNotRenammed := [];
			monContexte := [];
			let defs = List.map convert_def defs in
			monContexte := concatContexte !monContexte contexteEnglobant;
			monContexteNotRenammed := conc !monContexteNotRenammed  monContexteNotRenammedEnglobant ;
			let res = (if stat <> NOP then   BLOCK (defs, (convert_statement stat))  else  BLOCK (defs, NOP) ) in
			 monContexte := contexteEnglobant;
			(*monContexteNotRenammed := monContexteNotRenammedEnglobant;*)(* local to the function but to the block *)
			 
			res
		| SEQUENCE (s1, s2) -> 
				SEQUENCE (convert_statement s1, convert_statement s2)
		| IF (exp, s1, s2) ->  IF (convert_expression exp, convert_statement s1, convert_statement s2)
		| WHILE (exp, stat) -> WHILE (convert_expression  exp, convert_statement stat)
		| DOWHILE (exp, stat)->DOWHILE (convert_expression  exp, convert_statement stat) 
		| FOR (exp1, exp2, exp3, stat) ->
			FOR (convert_expression  exp1, convert_expression  exp2,convert_expression  exp3, convert_statement stat)
		| BREAK ->				BREAK
		| CONTINUE ->			CONTINUE
		| RETURN exp -> 		RETURN (convert_expression exp)
		| SWITCH (exp, stat) ->	SWITCH (convert_expression exp, convert_statement stat)
		| CASE (exp, stat) ->	CASE (convert_expression exp, convert_statement stat)
		| DEFAULT stat -> 		DEFAULT (convert_statement stat)
		| LABEL (name, stat) ->	LABEL (name, convert_statement stat) 
		| GOTO name ->			GOTO name 
		| ASM desc ->			ASM desc
		| GNU_ASM (desc, output, input, mods) ->GNU_ASM (desc, output, input, mods) (*REVOIR*)		
		| STAT_LINE (stat, file, line) -> STAT_LINE (convert_statement stat, file, line) 
		| _ ->	stat


	
	and convert_name ((id, typ, attr, exp) : name) =
		(*let nouType = convert_type typ in*)
		let nouId = renameIfNecessary id  in (* on ne renomme pas les fonctions pas les mêmes noms*)
		let nouExp = (if exp <> NOTHING then  convert_expression exp else exp) in
		(nouId, typ, attr, nouExp)

	and convert_name_group 	(typ, sto, names) =
		(typ, sto, List.map convert_name names)


and convert_def def =
		match def with
			FUNDEF (proto, body) ->
				used := [];
				let contexteEnglobant = !monContexte in
				let monContexteNotRenammedEnglobant = !monContexteNotRenammed in 
				monContexteNotRenammed := [];
				monContexte := [];
				let (typeP,storageP,nameP)=proto in
				let proto = (typeP, storageP,convert_param nameP) in
				let contexteDecFunc = !monContexte in
				monContexte := contexteDecFunc;
				let myParam = !monContexteNotRenammed in
				monContexteNotRenammed:=[];
				 let (_ , _ , fct )=proto in
		        let (nom,_,_,_) =fct in 
			    let (decs, stat) = body in 
				
				let decs = List.map	(fun dec -> convert_def dec)		decs in
				 
				
				monContexte := concatContexte   !monContexte contexteEnglobant;
				
				let res = FUNDEF (  proto,  (decs , convert_statement stat)) in
				let myLocal = !monContexteNotRenammed in	


				let nc = 	MyFunCont.setMulti 
								myParam []	[] [] 
								myLocal !used in

 

				if AFContext.mem !myAF nom   = false then myAF :=  AFContext.set !myAF nom  nc  ;	
				monContexte := contexteEnglobant;
				used := [];
				monContexteNotRenammed :=   monContexteNotRenammedEnglobant ; 
				(*CF.set nom (CF.D.setMulti myParam [] [] [] myLocal) *)
			res
			| OLDFUNDEF (proto, decs, body) -> (* no because convert by frontc into FUNDEF*)
				def
				(*
					let contexteEnglobant = monContexte in
					monContexte := [];
						List.map	(fun dec -> convert_name_group dec)		decs;(* !!!!VOIR***)
						let ndecs = List.map	(fun dec -> convert_def dec)		decs in
						let (decs, stat) = body in OLDFUNDEF (proto, ndecs, convert_statement (BLOCK (decs, stat)))
					monContexte := contexteEnglobant;
				*)
			| DECDEF names -> 
				let (baseType, _, namelist) = names in
				if isProto baseType = false then 
					DECDEF (convert_name_group names)
				else def
			| _ -> def 


let convert_comma_exps exps =	List.map  convert_expression  exps
(*let lineariser_comma_exp exps =	List.map  linea_expression  exps*)

let go  (defs : file) =
(*CF.empty;*) 
(*myAF:=AFContext.empty ;*)
listeDesVariablesFic :=[];
  let decs = List.map	(fun dec -> convert_def dec)		defs in
(*AFContext.print !myAF;*)


decs
  
