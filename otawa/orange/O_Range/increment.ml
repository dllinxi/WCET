(* increment -- use Frontc CAML. Computes informations needed for loop normalisation (except covariance). Example: variation of induction variable, multiple increment...
**
** Project:	O_Range
** File:	increment.ml
** Version:	1.1
** Date:	11.7.2008
** Author:	Marianne de Michiel
*)
open Cabs
open Frontc

let version = "increment.ml Marianne de Michiel"

open Cprint
open Cexptostr
open Cvarabs
open Cvariables
open Constante
open Interval
open Util
open Rename
open ExtractinfoPtr


type typeEQ =	INCVIDE (*0 pour + ou -, 1 pour * ou div*)|   NEG|   POS|   NDEF|NOVALID
type typeVAL =	INTEGERV |   FLOATV|   INDEFINEDV

let opEstPlus = ref true
let estPosInc = ref   INCVIDE (*1 positif 0 nul -1 négatif ou diff si incrément *//*)
let expressionIncFor = ref NOTHING

let isMultiInc = ref false
	let listAssosExactLoopInit = ref [(-1, 0)]
	let existAssosExactLoopInit  name  = (List.mem_assoc name !listAssosExactLoopInit)
	let setAssosExactLoopInit  name t =  
		if existAssosExactLoopInit  name = false then
		begin
			let val1 = calculer (EXP t) !infoaffichNull  []  (-1) in 
			if estNoComp val1  =false then 
 				if estDefExp val1 then
					match val1 with 
					ConstInt(j) -> 		 listAssosExactLoopInit :=  List.append [(name, int_of_string  j)] !listAssosExactLoopInit 	
					|_->()
		end
						
let getAssosExactLoopInit name  = (List.assoc name !listAssosExactLoopInit)

type increaseType = POSITIV|NEGATIV|MULTI|DIVI|NOTYPE|NOTYPEMUL (* +,-,*or/, NODEF*)
type increaseValue = NOINC|NODEFINC| INC of increaseType * expression

let print_intType t =
match t with
	POSITIV->Printf.printf"POSITIV\n" |NEGATIV->Printf.printf"NEGATIV\n" |MULTI->Printf.printf"MULTI\n" 
	|DIVI->Printf.printf"DIVI\n"|NOTYPE ->Printf.printf"NOTYPE\n"|NOTYPEMUL->Printf.printf"NOTYPEMUL\n" (* +,-,*or/, NODEF*)

let getIncValue inc =
match inc with INC(_,v)->v	|_->NOTHING

let getIncType inc =
match inc with INC(t,_)->t	|_->NOTYPE

let getIstMulInc inc =
match inc with
	INC(MULTI,_)|INC(DIVI,_)|INC(NOTYPEMUL,_)->true |_->false

let getIsAddInc inc =
match inc with
	INC(MULTI,_)|INC(DIVI,_)|INC(NOTYPEMUL,_)->false |_->true


let haveTheSameType inc1 inc2 =
(getIncType inc1) = (getIncType inc2) 

let haveTheInvesType inc1 inc2 =
match (getIncType inc1) with
 POSITIV->(getIncType inc2)=NEGATIV
|NEGATIV->(getIncType inc2)=POSITIV
|MULTI->(getIncType inc2)=DIVI
|DIVI->(getIncType inc2)=MULTI
|NOTYPE |NOTYPEMUL->false

let isCroissant inc1   =
match (getIncType inc1) with
 POSITIV-> 1
|NEGATIV->  -1
|MULTI->1
|DIVI-> -1
|NOTYPE|NOTYPEMUL ->0


let rec containtMINIMUMCALL exp =
match exp with
		NOTHING -> false
		| UNARY (_, e1) -> containtMINIMUMCALL e1
		| BINARY (_, e1, e2) ->containtMINIMUMCALL e1 || containtMINIMUMCALL e2
		| CALL (exp, args) ->
			(match exp with VARIABLE("MINIMUM") -> true|_-> false)
		| _  ->false


let getIsMultipleIncrement incValue =
match incValue with
NOTHING -> false
| exp-> containtMINIMUMCALL exp


let quitBecauseOfBoolean  beforeInst nextInst cond hasAndCond firstcond=
(* Printf.printf "TRUE ARITHGEO dans 1  quitBecauseOfBoolean \n" ;*)
	if hasAndCond = false then false
	else
	begin

(* Printf.printf "TRUE ARITHGEO dans 2  quitBecauseOfBoolean \n" ;
print_expression cond 0; space() ;flush() ;new_line(); flush();new_line(); 
print_expression firstcond 0; space() ;flush() ;new_line(); flush();new_line(); *)
		let las = evalStore (new_instBEGIN  (List.append beforeInst nextInst)) [] [] !myCurrentPtrContext in
		let assign =expVaToExp( applyStoreVA (EXP cond) las) in	
		let assign2 =expVaToExp( applyStoreVA (EXP firstcond) las) in	
		let ncond =  calculer (EXP  assign)  !infoaffichNull [] (-1) in	
		let ncond1 =  calculer (EXP   assign2)  !infoaffichNull [] (-1) in	
		if   estNoComp ncond  && estNoComp ncond1   then false
		else 
		begin 
			let isExecutedV = if   estNoComp ncond = false then (match ncond with Boolean(b)	->  if b = false then false  else true 
																|_-> if estNul ncond then false else true ) else true in	

			let isExecutedV2 = if   estNoComp ncond1 = false then (match ncond1 with Boolean(b)	->  if b = false then false  else true 
																|_-> if estNul ncond1 then false else true )  else true in	
			if isExecutedV = false || isExecutedV2 = false then  (  true) else   ( false) 
			 
		end	
end



let rec rechercheInc var exp before=
	(*Printf.printf"rechercheInc\n"; 
	print_expression exp 0;new_line(); flush();new_line(); flush();new_line(); flush();new_line(); flush();space();new_line(); flush();space();*)
	let val1 = calculer (EXP exp) !infoaffichNull [] (-1) in 
	if val1 = NOCOMP  then 
	begin (*Printf.printf"NOCOMP\n"; *)
		NOTHING
	end
	else
	begin
		if (estVarDsExpEval var val1 = false)  then (*exp*) NOTHING
		else
		begin
			if (estAffine var val1)   then 
			begin
				let (a,b) = calculaetbAffineForne var val1 in	
				(*	Printf.printf"rechercheInc affine\n";*)
				let (expa, expb) = if estDefExp a = false  || estDefExp b = false  then
									begin
										(*getOnlyBoolAssignment := false;*)
										(* set to change abstract interpretation of if to find Bool condition loop increment*)
										let las = evalStore (new_instBEGIN(before)) [] [] !myCurrentPtrContext in
											(*afficherListeAS las;*)
										(*getOnlyBoolAssignment := true;*)
										applyStoreVA (EXP(expressionEvalueeToExpression a)) las ,
										applyStoreVA (EXP(expressionEvalueeToExpression b)) las	 
									end 
									else ((EXP(expressionEvalueeToExpression a)),(EXP(expressionEvalueeToExpression b)))
									in 
				(* because inc has to be menimized => (-1))*)
				hasSETCALL := false;
			
				let var1 = calculer (expa) !infoaffichNull [] (-1) in 
				let isMulti1 = !hasSETCALL in
				hasSETCALL := false;
				let var2 = calculer (expb) !infoaffichNull [] (-1) in
				let isMulti2 = !hasSETCALL in

				let varMoinsUn = (evalexpression (Diff( var1,  ConstInt("1")))) in

				let isaffineButDef = estPositif varMoinsUn in
				if isMulti1 || isMulti2 || (estStricPositif varMoinsUn && estNul var2= false && estPositif var2) then isMultiInc := true;
				(*		Printf.printf"rechercheInc affine\n"; 
					print_expression exp 0;new_line(); flush();new_line(); flush();new_line(); flush();new_line(); flush();space();new_line(); flush();
					print_expTerm val1  ;new_line(); flush();new_line(); flush();new_line(); flush();new_line(); flush();space();new_line(); flush();
					print_expTerm var1  ;new_line(); flush();new_line(); flush();new_line(); flush();new_line(); flush();space();new_line(); flush();
					print_expTerm var2  ;new_line(); flush();new_line(); flush();new_line(); flush();new_line(); flush();space();new_line(); flush();*)
			
				if   isMulti1 =false &&((estNul var1 = true) || var1 = ConstInt("1") || var1 =  ConstFloat("1.0")||var1 =  RConstFloat(1.0))  || (isaffineButDef && estNul var2= false && estPositif var2) then
				begin
					opEstPlus := true ; 

					if estNul var2 then	estPosInc := INCVIDE 
					else 	if estPositif var2 then estPosInc := POS 
							else if estDefExp  var2 then estPosInc := NEG else estPosInc := NDEF;
						(*print_expTerm var2  ;new_line(); flush();new_line(); flush();new_line(); flush();new_line(); flush();space();new_line(); flush()*)
					(*if !estPosInc = NDEF then Printf.printf "var 2 nodef";*)
					if estDefExp var2 then expressionEvalueeToExpression var2  else expVaToExp expb
				end	
				else 
					if (estNul var2) then 
					begin   
						let val1 = if estDefExp var1 then expressionEvalueeToExpression var1 else expVaToExp expa in
						(*print_expression val1  0;new_line();flush();new_line();flush();new_line(); flush();new_line();flush();space();new_line(); *)
						opEstPlus := false ; (*Printf.printf"rechercheInc affine var1 struct pos et val2  nul\n"; *)
						
						if estStricPositif var1 then
						begin (*Printf.printf"rechercheInc affine test de var1 - 1 \n"; *)
							if estNul varMoinsUn then	begin estPosInc := INCVIDE; val1 end
							else  
							begin
								estPosInc := POS ;	(*Printf.printf"div or mult inc\n"; *)
								if estStricPositif varMoinsUn then val1
								else begin estPosInc := NEG ;  (* BINARY (DIV, CONSTANT  (CONST_INT "1"),*) val1(* ) *) end
							end
						end
						else 
						begin
							estPosInc := NDEF;(* NOTYPEMUL Printf.printf"rechercheInc affine var1 struct pos et val2 non nul\n"; *)
						    val1
						end
					end
					else (	(*Printf.printf"rechercheInc NOT affine\n"; *) NOTHING		)
			end
			else NOTHING
		end
	end 


let beforeAffin = ref (VAR ("", MULTIPLE,[],[]))

let getCondition e =
let var =( match e with VARIABLE(v)->v |_->"") in
(match !beforeAffin with
VAR (x, exp,_,_)-> if x = var then expVaToExp exp else ((*Printf.printf "%s not found\n" v;*) NOTHING)
|_->  NOTHING)

let rec analyseIncFor var exp inst  asAs completList beforei=
(*Printf.printf "analyse inc de %s \n" var ;
print_expression exp 0; new_line(); flush();*)
(	match exp with	
	BINARY (op, exp1, exp2) ->
		(	match exp1 with
			VARIABLE (v) -> 
				if v = var then
				begin
				(	match op with
					ASSIGN ->  (*	print_expression exp2 0; new_line(); flush();*)
						if List.mem var (listeDesVarsDeExpSeules  exp2) =false  then
						begin
					(* si j=i+N avec i mofifiée par la boucle alors on peut remplacer la condition sur j dans le test de boucle par une condition sur i+N sans décalage pour un do while mais avec décalage pour les autres boucles*)							
							let (inc, before, isAddinc, isComp, varDep) = getIndirectIncrease var exp2 inst  asAs completList in
							if isComp then
							begin
							(*	Printf.printf "%s depend indirectement de %s \n" var varDep;
								if inc = NOTHING then Printf.printf "no  inc\n" else print_expression inc 0; new_line();flush();
								if before then Printf.printf "before\n" else Printf.printf "next\n";
								if isAddinc then Printf.printf "adding inc\n" else Printf.printf "prodinc\n";*)
								(inc, before, isAddinc,true,varDep)(*(NOTHING, true, true,false,var)*)
							end
							else begin (* Printf.printf "nocomp indirect\n" ; *)
												(NOTHING, before, isAddinc,false,var) end
						end
						else 
						begin 
									(*Printf.printf "recherche incremet %s\n" var;*)
							expressionIncFor:= rechercheInc var exp2	beforei; (*print_expression !expressionIncFor 0; new_line();flush();*)
							(!expressionIncFor, true, !opEstPlus,(!expressionIncFor != NOTHING),var)			
						end			

					| ADD_ASSIGN ->  (exp2, true, true, true, var)
					| SUB_ASSIGN ->  (UNARY (MINUS, exp2),true, true, true, var)
					| MUL_ASSIGN ->  (exp2,true,  false,true, var)
					| DIV_ASSIGN ->  (BINARY (DIV, CONSTANT  (CONST_INT "1"), exp2),true,  false,true, var)

							(*| MOD_ASSIGN ->    expressionIncFor := BINARY (MOD, exp1, exp2)*)
					| SHL_ASSIGN ->  (BINARY (MUL, CONSTANT  (CONST_INT "2"), exp2),true,  false,true, var)
					| SHR_ASSIGN ->  (BINARY(DIV,CONSTANT(CONST_INT "1"),BINARY(MUL,CONSTANT(CONST_INT "2"),exp2)),true,false,true,var)
					|_ ->  (NOTHING, true, true,false,var)
				)
				end
				else  (NOTHING, true, true,false,var)
			|_->if List.mem var (listeDesVarsDeExpSeules  exp1) =false   then	analyseIncFor var exp2 inst  asAs completList beforei
				else if List.mem var (listeDesVarsDeExpSeules  exp2) =false  then	analyseIncFor var exp1 inst  asAs completList beforei
					 else (NOTHING, true, true,false,var)
		)
		| UNARY (op, exp1) ->
			(match exp1 with
				VARIABLE (v) -> 
					if v = var then
					begin
						(match op with
						  PREINCR | POSINCR -> (CONSTANT (CONST_INT "1"), true, true, true, var)
						| PREDECR | POSDECR -> (CONSTANT(CONST_INT "-1"), true, true, true, var)
						|_ -> 		(NOTHING, true, true,false,var)
						)
					end
					else   (NOTHING, true, true,false,var)
				|_->  (NOTHING, true, true,false,var)
			)
		| COMMA exps ->	
			if List.mem var (listeDesVarsDeExpSeules exp) =false then	(NOTHING, true, true,false,var)
			else applyComma  exps var exps inst  asAs completList	beforei		
								   			
		|_-> (NOTHING, true, true,false,var)
)
and applyComma  exps var ex inst  asAs completList  beforei=
if exps = [] then (NOTHING, true, true,false,var) 
else
begin
	let (head, others) = (List.hd exps, List.tl exps) in
	if List.mem var  (listeDesVarsDeExpSeules head) = true then  analyseIncFor var head inst  asAs completList beforei
	else applyComma  others var ex inst  asAs completList (*List.append beforei [head]*)  beforei
end

and rechercheAffectVDsListeASAndWhere v var l isbefore=
if l = [] then (NOTHING, true)
else 
let a = List.hd l in
let suite = List.tl l in
	match a with
		ASSIGN_SIMPLE (s, e) 	 -> 	if s = v then (expVaToExp(e), isbefore) 
										else 
											if s = var then rechercheAffectVDsListeASAndWhere v var suite true
											else rechercheAffectVDsListeASAndWhere v var suite isbefore
	|	ASSIGN_DOUBLE (s,e1, e2) -> 	
			if s = v (*and il faut évaluer les 2 expression index = e1*) then 
			begin
				if !vDEBUG then begin	Printf.eprintf "tableau avec index à terminer\n";(* afficherAS a *) end; 
				(expVaToExp(e2), isbefore) 
			end
			else  if s = var then rechercheAffectVDsListeASAndWhere v var suite true
				  else rechercheAffectVDsListeASAndWhere v var suite isbefore
	|	ASSIGN_MEM (s,e1, e2) -> 	
			if s = v (*and il faut évaluer les 2 expression index = e1*) then 
			begin
				if !vDEBUG then begin	Printf.eprintf "tableau avec index à terminer\n";(* afficherAS a *) end; 
				(expVaToExp(e2), isbefore) 
			end
			else  if s = var then rechercheAffectVDsListeASAndWhere v var suite true
				  else rechercheAffectVDsListeASAndWhere v var suite isbefore



and getIndirectIncrease var exp inst  asAs completList =
(*afficherLesAffectations completList;*)
	if listeDesVarsDeExpSeules  exp  = []  then 
	begin
		(* BOOL en fait de la forme var = cte si cette affectation est gardée par une condition de if alors peut être éventuellement traité *)
	(*	print_expression exp 0; new_line(); flush();
		Printf.printf "Dans increment.ml getIndirectIncrease : it may be a boolean condition non encore traité\n";*)
		(NOTHING, true,true,false,"other")
	end
	else
	 match exp with
		VARIABLE (v) -> 
			let pred = !getOnlyBoolAssignment in
			getOnlyBoolAssignment := true;
			let cas =evalStore (new_instBEGIN(completList)) [] [] !myCurrentPtrContext in
			getOnlyBoolAssignment := pred;
			let assignj = expVaToExp(rechercheAffectVDsListeAS var cas) in
						
			(match assignj with
				VARIABLE (x)-> 
					if x = v then
					begin
						opEstPlus:= true;
						let (assign,after)= rechercheAffectVDsListeASAndWhere v var cas false in	
						let (inc, before, isplus,iscomp,nvar)=
								analyseIncFor v (BINARY(ASSIGN,VARIABLE(v),assign)) inst  true completList [] in
						if nvar = v then ( inc,after=false, isplus,  (inc != NOTHING), v) else (NOTHING, true,true,false,"other")
					end
					else (NOTHING, true,true,false,"other")
				| NOTHING -> (NOTHING, true,true,false,"other")
				|_-> 		 		
					opEstPlus:= true;
					let (assign,after)= rechercheAffectVDsListeASAndWhere v var cas false in
					if  assign =  assignj then
					begin	
						let (inc, before, isplus,iscomp,nvar)=
							analyseIncFor v (BINARY(ASSIGN,VARIABLE(v),assign)) inst  true completList [] in
						if nvar = v then ( inc,after=false, isplus,  (inc != NOTHING), v) else (NOTHING, true,true,false,"other")
					end
					else (NOTHING, true,true,false,"other") )
					(* more than one dependancy on can be before the other after... to see*)
				|_->
					if !vDEBUG then 
						Printf.eprintf "depend d'une expression contenant une autre variable est pas de cette variable seule non encore traité\n";
					(NOTHING, true,true,false,"other")
	 	
and getInc var assign inst  asAs completList beforei=
		opEstPlus:= true;	
		let (inc, before, isplus,iscomp,nvar)= analyseIncFor var (BINARY(ASSIGN,VARIABLE(var),assign)) inst  asAs completList beforei in
		let valinc = calculer (EXP(inc))  !infoaffichNull  [](*appel*) (-1) in	
		(*let resinc = if estDefExp valinc then expressionEvalueeToExpression valinc  else expVaToExp inc in*)
		(*Printf.printf"isIndirect MUORDI\n"; print_expression inc 0;*)
		let isIndirect = nvar != var in
		if estNoComp valinc then (* si aucun increment alors peut être condition var bool*)
			if isNoDef inc then (false,NODEFINC,"others", before) 
			else if isplus then ( (*Printf.printf "inc var dependent\n" ; print_expression inc 0; new_line();flush();*)
								(isIndirect,INC(NOTYPE,  inc),nvar, before) )
				 else ((*Printf.printf"isIndirect DIVI or MUL\n"; *)opEstPlus:= false;	(isIndirect,INC(NOTYPEMUL,  inc),nvar, before))
		else
		begin 
			if isplus (*op +or -*) then 
				if estNul valinc then (false,NOINC,"others", before)  
				else if estStricPositif valinc then (isIndirect,INC(POSITIV, expressionEvalueeToExpression valinc),nvar, before)  
					else  (isIndirect,INC(NEGATIV, expressionEvalueeToExpression valinc),nvar, before)
			else (* or / if estStricPositif valinc then *)
			begin
				(*Printf.printf"isIndirect MUORDI 2\n";*)
				let varMoinsUn = (evalexpression (Diff( valinc,  ConstInt("1")))) in
				if estStricPositif valinc then
				begin
					if estNul varMoinsUn then (false,NOINC,"others", before)  
					else   	if estStricPositif varMoinsUn then 
							begin
									(*Printf.printf"isIndirect MULTI\n";*)(isIndirect, INC(MULTI, inc),nvar, before) 
							end
							else 
							begin
								(*Printf.printf"isIndirect DIVI\n";*)(isIndirect, INC(DIVI, inc),nvar, before) 
							end
				end
				else (false,NODEFINC,"others", before) 
				 (*else NODEFINC*)
			end
		end
			

and joinSequence x inc1 inc2 =
(*print_interval inter ;*)
match inc1 with 
	INC(POSITIV,v1)->
   			(match inc2 with INC(POSITIV,v2) -> INC(POSITIV,(BINARY(ADD,v1,v2)))
							| INC(NEGATIV,v2) -> 
								let v1Moinsv2 = calculer  (EXP(BINARY(ADD, v1,  v2))) !infoaffichNull [] (-1) in
								if estStricPositif v1Moinsv2 then INC(POSITIV,(BINARY(ADD,v1,v2)))
								else
									if estNul v1Moinsv2 then	NOINC
									else  INC(NEGATIV,(BINARY(ADD,v1,v2)))
							| INC(NOTYPE,v2) ->   INC(NOTYPE,(BINARY(ADD,v1,v2)))								
							| NOINC -> inc1
							|_->NODEFINC)
	| INC(NEGATIV,v1) ->
		 (	match inc2 with INC(NEGATIV,v2) -> INC(NEGATIV,(BINARY(ADD,v1,v2)))
							| INC(POSITIV,v2) -> 
								let v1Moinsv2 = calculer  (EXP(BINARY(ADD, v1,  v2))) !infoaffichNull [] (-1) in
								if estStricPositif v1Moinsv2 then INC(POSITIV,(BINARY(ADD,v2,v1)))
								else
									if estNul v1Moinsv2 then	NOINC
									else  INC(NEGATIV,(BINARY(ADD,v2,v1)))
							| INC(NOTYPE,v2) ->   INC(NOTYPE,(BINARY(ADD,v2,v1)))		
							| NOINC -> inc1
							|_->NODEFINC)
	
	| NOINC -> inc2
	| NODEFINC -> NODEFINC
	| INC(MULTI,v1)->
   			(match inc2 with INC(MULTI,v2) -> INC(MULTI,(BINARY(MUL,v1,v2)))
							| INC(DIVI,v2) -> 
								let v1Divv2 = calculer  (EXP(BINARY(MUL, v1,  v2))) !infoaffichNull [] (-1) in
								let varMoinsUn = (evalexpression (Diff( v1Divv2,  ConstInt("1")))) in
								if estStricPositif v1Divv2 then
								begin
									if estNul varMoinsUn then  NOINC 
									else   if estStricPositif varMoinsUn then   INC(MULTI, (BINARY(MUL, v1,  v2))) 
											else   INC(DIVI, (BINARY(MUL, v1,  v2))) 
								end
								else  NODEFINC
							| INC(NOTYPEMUL,v2)->INC(NOTYPEMUL,(BINARY(MUL,v1,v2)))
							| NOINC -> inc1
							|_->NODEFINC)
	| INC(DIVI,v1)->
   			(match inc2 with INC(DIVI,v2) -> INC(DIVI,(BINARY(MUL,v1,v2)))
							| INC(MULTI,v2) -> 
								let v1Divv2 = calculer  (EXP(BINARY(MUL, v2,  v1))) !infoaffichNull [] (-1) in
								let varMoinsUn = (evalexpression (Diff( v1Divv2,  ConstInt("1")))) in
								if estStricPositif v1Divv2 then
								begin
									if estNul varMoinsUn then  NOINC 
									else   if estStricPositif varMoinsUn then   INC(MULTI, (BINARY(MUL, v2,  v1))) 
											else   INC(DIVI, (BINARY(MUL, v2,  v1))) 
								end
								else  NODEFINC
							| INC(NOTYPEMUL,v2)->INC(NOTYPEMUL,(BINARY(MUL,v1,v2)))
							| NOINC -> inc1
							|_->NODEFINC)
	| INC(NOTYPEMUL,v1)->	(match inc2 with INC(DIVI,v2) | INC(MULTI,v2) | INC(NOTYPEMUL,v2)-> INC(NOTYPEMUL,(BINARY(MUL,v1,v2)))
							| NOINC -> inc1
							|_->NODEFINC)
	| INC(NOTYPE,v1) ->  (	match inc2 with  NOINC -> inc1 | INC(POSITIV,v2) | INC(NEGATIV,v2) | INC(NOTYPE,v2)->   INC(NOTYPE,BINARY(ADD,v2,v1))		
							|_->NODEFINC)



and sameType inc1 inc2 =
	let value1  =  (calculer (EXP (getIncValue inc1 )) !infoaffichNull  []  (-1)  ) in
						 

	let value2  =  (calculer (EXP (getIncValue inc2)) !infoaffichNull  []  (-1)  ) in
	if estNoComp value1  =false && estDefExp value1  &&estNoComp value2  =false && estDefExp value2 then
	(match inc1 with 
		INC(POSITIV,_)->
	   			(match inc2 with INC(POSITIV,_)  -> true |_->false)
		| INC(NEGATIV,_) ->
			 (	match inc2 with INC(NEGATIV,_)  -> true |_->false)
	
		| INC(MULTI,v1)->
	   			 (	match inc2 with INC(MULTI,_)  -> true |_->false)
		| INC(DIVI,v1)->
	   			 (	match inc2 with INC(DIVI,_) -> true |_->false)
		| _ ->false

	)
	else false



and joinAlternate  x inc1 inc2 inter1 inter2=
match inc1 with 
	INC(POSITIV,v1)->
   			(match inc2 with INC(POSITIV,v2) -> isMultiInc := true;INC(POSITIV,CALL (VARIABLE("MINIMUM") , (List.append [v1] [v2] )))
							| INC(MULTI,v2)-> 
							let minValue = getLower inter2 in
							if estDefExp minValue = false then NODEFINC
							else
							begin
								let minFValue = getDefValue minValue in
								let value  = getDefValue(calculer (EXP (getIncValue inc2)) !infoaffichNull  []  (-1)  ) in
								let valueInc  = getDefValue(calculer (EXP (getIncValue inc1)) !infoaffichNull  []  (-1)  ) in
								if value > 0.0 && minFValue > 0.0 then
								begin isMultiInc := true;
									let bound  = value *. minFValue -. minFValue in
(*Printf.printf "joinAlternate1 for %s bound = %f inc = %f \n"x bound (getDefValue(calculer (EXP (getIncValue inc2)) !infoaffichNull  []  1  ) );*)
									if bound >= (*value*)valueInc then inc1 else NODEFINC
								end 
								else NODEFINC
							end
							|_->NODEFINC)
	| INC(NEGATIV,v1) ->
(*Printf.printf "joinAlternate MAXI 1 \n";*)
		 	(match inc2 with INC(NEGATIV,v2) ->isMultiInc := true; INC(NEGATIV,CALL (VARIABLE("MAXIMUM") , (List.append [v1] [v2] ))) (*MAXIMUM ou MINIMUM?*)
							|_->NODEFINC)
	| NOINC ->(*Printf.printf "joinAlternate for %s\n"x;*) ( match inc2 with NOINC -> NOINC |_->NODEFINC)
	| NODEFINC -> NODEFINC
	| INC(MULTI,v1)->
   			(match inc2 with INC(MULTI,v2) ->isMultiInc := true; INC(MULTI,CALL (VARIABLE("MINIMUM") , (List.append [v1] [v2] )))	
							|INC(POSITIV,v2)->
							let minValue = getLower inter1 in
							if estDefExp minValue = false then NODEFINC
							else
							begin isMultiInc := true;
								let minFValue = getDefValue minValue in
								let value  = getDefValue(calculer (EXP (getIncValue inc1)) !infoaffichNull  []  (-1)  ) in
								let valueInc  = getDefValue(calculer (EXP (getIncValue inc2)) !infoaffichNull  []  (-1)  ) in
								if value > 0.0 && minFValue > 0.0 then
								begin
									let bound  = value *. minFValue -. minFValue in
(*Printf.printf "joinAlternate for %s bound = %f inc = %f \n"x bound (getDefValue(calculer (EXP (getIncValue inc2)) !infoaffichNull  []  1  ) );
print_intType (getIncType inc1);
print_interval inter1 ;
print_intType (getIncType inc2);
print_interval inter2 ;*)

									if bound >= valueInc then inc2 else NODEFINC
								end 
								else NODEFINC
							end
							|_->NODEFINC)
	| INC(DIVI,v1)->(*Printf.printf "joinAlternate MAXI 2 \n";*)
   			(match inc2 with INC(DIVI,v2) -> isMultiInc := true; INC(DIVI,CALL (VARIABLE("MAXIMUM") , (List.append [v1] [v2] )))	(*MAXIMUM ou MINIMUM?*)
							|_-> NODEFINC)
	|_->NODEFINC



and getIncOfInstList x iList completList interval previous firstcond hasAndCond=
(*Printf.printf "getIncOfInstList variable %s\n" x;
afficherLesAffectations iList;*)
	if iList = [] then (false,NOINC,x,false)
	else
	begin 
		let (firstInst, nextInst) =  (List.hd iList, List.tl iList) in
		match firstInst with
			VAR (id, exp,_,_) -> beforeAffin := firstInst;
				 let (isindirect,inc1,v, before) = 
					 if id = x then
					 begin

						 getInc x (expVaToExp exp) 	
								iList 
								
								false 
								completList previous
					end
					else (false,NOINC,x, false)  in
				if inc1 = NODEFINC then (false,inc1, x, before)
				else
					if isindirect = false then
					begin
						let (indirect2, inc, var, before2) = getIncOfInstList x nextInst completList interval (List.append previous [firstInst] ) firstcond hasAndCond in
						if indirect2 = false then (false, joinSequence x inc1  inc, x, false) else (true, inc, var, before2) 
					end
					else (true, inc1, v, before)

			| TAB (id, _, _,_,_) -> 
				if id = x then (false,NODEFINC,x,false) 
				else  (*NOINC *)getIncOfInstList x nextInst completList   interval (List.append previous [firstInst] ) firstcond hasAndCond
			| MEMASSIGN (id, _, _,_,_) -> 
				if id = x then (false,NODEFINC,x,false) 
				else  (*NOINC *)getIncOfInstList x nextInst completList  interval (List.append previous [firstInst] ) firstcond hasAndCond

			| BEGIN liste ->
				let (indirect1, inc1, var1, before1) = (getIncOfInstList x  liste completList interval (List.append previous [firstInst] ) firstcond hasAndCond) in
				if inc1 = NODEFINC then (indirect1,inc1, var1, before1)
				else
					if indirect1 = false then 
					begin 
						let (indirect2, inc2, var2, before2) = (getIncOfInstList x nextInst completList interval (List.append previous [firstInst] ) firstcond hasAndCond) in
						if indirect2 = false then (false,joinSequence x inc1 inc2,x, false) else (true, inc2, var2, before2) 
					end
					else (true, inc1, var1,before1) 

			| IFVF (exp, i1, i2) ->

				let trueinterval = restictIntervalFromCond (getCondition(expVaToExp  exp)) x  interval in
				let falseinterval = restictIntervalFromCond ( UNARY (NOT,(getCondition(expVaToExp  exp)))) x  interval in
				let (indirect1, inc1, var1, before1) = (getIncOfInstList x   [i1] completList trueinterval (List.append previous [firstInst] )firstcond hasAndCond) in

				if inc1 = NODEFINC then 
				begin
					if hasAndCond  &&  indirect1 = false then
					(		let (res, a,b,c,d) = getIncOfANDOne x  i1 i2 completList falseinterval  nextInst firstInst interval previous   firstcond hasAndCond   in
							if res then ( a,b,c,d) else (indirect1, inc1, var1, before1))
					else  (indirect1, inc1, var1, before1)

				end
				else
					if indirect1 = false then 
					begin 
						let (indirect2, inc2, var2, before2) = (getIncOfInstList x [i2] completList falseinterval (List.append previous [firstInst] )firstcond hasAndCond) in
						if inc2 = NODEFINC then 


							if hasAndCond && indirect2 = false  then
								begin
									let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i2]) nextInst firstcond hasAndCond firstcond in
									if isOkExe1 then
									begin
				 						let (indirect3, inc3, var3, before3) = (getIncOfInstList x  nextInst completList interval(List.append previous [firstInst] ) firstcond hasAndCond) in
										if indirect3 = false then  
											(false,joinSequence x  inc1   inc3  ,x, false) 
										else (true, inc3, var3, before3) 


									end
									else  (indirect2, inc2, var2, before2) 
								end 

							else	(indirect2, inc2, var2, before2) 
						else
							if indirect2 = false then 
							begin
								let (indirect3, inc3, var3, before3) = (getIncOfInstList x  nextInst completList interval(List.append previous [firstInst] ) firstcond hasAndCond) in

(*if inc2 = NOINC || inc1 =NOINC then Printf.printf"joinSequence %s\n" x;*)
								if indirect3 = false then  
										(false,joinSequence x ( joinAlternate x inc1 inc2 trueinterval falseinterval)  inc3  ,x, false) 
								else (true, inc3, var3, before3) 
							end
							else (true, inc2, var2, before2) 
						end
						else (true, inc1, var1, before1) 
				

			| IFV ( exp, i1) ->
				let trueinterval = restictIntervalFromCond (getCondition(expVaToExp  exp)) x  interval in

				let (indirect1, inc1, var1, before1) = (getIncOfInstList x   [i1] completList trueinterval (List.append previous [firstInst] )firstcond hasAndCond) in
				if inc1 = NODEFINC then
					if hasAndCond &&  indirect1 = false  then
						begin
							let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst firstcond hasAndCond firstcond in
							if isOkExe1 then
		 						(getIncOfInstList x nextInst completList interval (List.append previous [firstInst] )firstcond hasAndCond)  (*REVOIR*)
							else  (indirect1,inc1, var1, before1)
						end 

					else	 (indirect1,inc1, var1, before1)
				else
					if indirect1 = false then 
					begin 
						let (indirect2, inc2, var2, before2) = (getIncOfInstList x nextInst completList interval (List.append previous [firstInst] )firstcond hasAndCond)  in

						if hasAndCond &&  inc1 =NOINC   then
						begin

							let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst firstcond hasAndCond firstcond in
							if isOkExe1 then
		 						(getIncOfInstList x nextInst completList interval (List.append previous [firstInst] )firstcond hasAndCond)  (*REVOIR*)
							else   if indirect2 = false then   (false,joinSequence x ( joinAlternate x inc1 NOINC trueinterval trueinterval)  inc2  ,x, false)  else (true, inc2, var2, before2) 

							 
						end
						else if indirect2 = false then   (false,joinSequence x ( joinAlternate x inc1 NOINC trueinterval trueinterval)  inc2  ,x, false)  else (true, inc2, var2, before2) 
					end
					else (true, inc1, var1, before1) 

				

			| FORV (num,id, _, _, _, _, body,_)->
				let nbItMin = if  existAssosExactLoopInit  num then getAssosExactLoopInit num  else 0 in
				let (indirect1, inc1, var1, before1,incifexe) = (extractIncOfLoop x [body] id nbItMin (*nbItMin_{numLoop}*)completList) (List.append previous [firstInst] ) firstcond hasAndCond in
				(*if nbItMin_{numLoop}=nbItMax_{numLoop} then joinSequence x (extractIncOfLoop x [body] id nbItMin_{numLoop} )(getIncOfInstList x [nextInst])
				else*)
				if incifexe = NODEFINC then (indirect1,inc1, var1, before1)
				else
					if indirect1 = false then 
					begin 
						let (indirect2, inc2, var2, before2) = (getIncOfInstList x nextInst completList interval (List.append previous [firstInst] )firstcond hasAndCond)  in
						if indirect2 = false then 
						begin  
							if incifexe = inc1 then
								(false,(*joinAlternate*) joinSequence x inc1 inc2 ,x, false)  
							else  if sameType incifexe inc2 then ((false,(*joinAlternate*)  inc2 ,x, false) )  else ((indirect1,NODEFINC, var1, before1))							
						end
						else (true, inc2, var2, before2) 
					end
					else (true, inc1, var1, before1) 


			| APPEL (_,_,name,s,c,_,_,_)->
				let (indirect1, inc1, var1, before1) = (getIncOfCall x name c completList s (List.append previous [firstInst] )  ) in
				if inc1 = NODEFINC then (indirect1,inc1, var1, before1)
				else
					if indirect1 = false then 
					begin 
						let (indirect2, inc2, var2, before2) = (getIncOfInstList x nextInst completList interval (List.append previous [firstInst] )firstcond hasAndCond)  in
						if indirect2 = false then   (false,joinSequence x inc1 inc2,x, false)  else (true, inc2, var2, before2) 
					end
					else (true, inc1, var1, before1) 
	end

and  getIncOfANDOne  x i1 i2 completList falseinterval  nextInst firstInst interval previous     firstcond hasAndCond  =
		let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst firstcond hasAndCond firstcond in
		if isOkExe1 then
		begin
			let (correct2, inc,_, _) = 
						getIncOfInstList x [i2] completList falseinterval   previous   firstcond hasAndCond in

			let isOkExe2 = quitBecauseOfBoolean  (List.append previous [i2]) nextInst firstcond hasAndCond firstcond in

			if correct2 = true || isOkExe2 = false then (false, false,  inc ,x, false)
			else
			begin
					let (a,b,c,d) =	 getIncOfInstList x    nextInst completList interval (List.append previous [firstInst] ) firstcond hasAndCond in
					(true, a,b,c,d)


			end
				 
		end
		else (false, false,  NODEFINC ,x, false)


and extractIncOfLoop x inst varL nbItL completList beforei firstcond hasAndCond=
	(*if nbItL = 0 then*)
	begin
		
		(*let las = evalStore (new_instBEGIN(inst)) [] [] !myCurrentPtrContext in*)

 
		let (isindirect,inc1,v, before,ifexe) = 
			if List.mem x (assignVar inst) then
			begin
				let (isindirect,inc,var, before) = getIncOfInstList x inst inst (INTERVALLE(INFINI,INFINI)) [] firstcond hasAndCond in
				(*let varBPUN = BINARY(SUB, VARIABLE varL, CONSTANT(CONST_INT("1"))) in
			    isMultiInc := true;
				let extinc = expVaToExp(applyStoreVA (rechercheAffectVDsListeAS x las)  	[ASSIGN_SIMPLE (varL, EXP(varBPUN))] )   in
				getInc x extinc inst !listeASCourant true completList beforei*)
				if inc = NODEFINC || inc = NOINC then ( (isindirect,inc,var, before, inc))
				else
					begin
						let value  =  (calculer (EXP (getIncValue inc)) !infoaffichNull  []  (-1)  ) in
						if estNoComp value  =false && estDefExp value then ( (isindirect,NODEFINC,var, before, inc) )else  (isindirect,NODEFINC,var, before, NODEFINC)
					end
			end
			else (false,NOINC,x, false,NOINC) in
		
		 (isindirect,inc1,x, false, ifexe) (*(isindirect,NOINC,x, before,NOINC)*)

	end
	(*else 
	begin
		listeASCourant := [];
		let las =evalStore (new_instBEGIN(inst)) [] [] !myCurrentPtrContext in
		let (isindirect,inc1,v, before) = 
			if existAffectVDsListeAS x las then
			begin
				let varBPUN = BINARY(SUB, VARIABLE varL, CONSTANT(CONST_INT("1"))) in
				isMultiInc := true;
				let extinc = expVaToExp(applyStoreVA (rechercheAffectVDsListeAS x las)  	[ASSIGN_SIMPLE (varL, EXP(varBPUN))] )   in
				getInc x extinc inst !listeASCourant true completList beforei
			end
			else (false,NOINC,x, false) in
		 (isindirect,inc1,v, before, inc1) 
	end*)



and getIncOfCall x name body  completList s beforei =
 
(*let predval = !withoutTakingCallIntoAccount in*)
let inc =
		if changeGlobal x name body then  (false,NODEFINC,x, false) else
		begin
			let sorties = (match s with BEGIN(sss)-> sss |_->[]) in
			let ass = 
							List.map 
											( fun sortie -> 
												(match sortie with 
													VAR (id, _,_,_) 
													| TAB (id, _, _,_,_) 
													|MEMASSIGN (id, _,_,_,_)->
														let fid = 	if  String.length id > 1 then 
														if (String.sub id  0 1)="*" then  String.sub id 1 ((String.length id)-1) else id
														else id  in 

															"*"^fid 
													|_->"")
											)sorties	in
			(*let affectSortie = evalStore s [] [] [] in	*)
			if  List.mem x (ass)  then (false,NODEFINC,x, false) 
			else (false,NOINC,x, false)
		end   in
(*withoutTakingCallIntoAccount := predval;*)
inc


and changeGlobal x name body =

let  used = 
 	if AFContext.mem  !myAF name = false then
 	begin
		match body with CORPS(_) ->    
		(*	!alreadyAffectedGlobales see also util.ml    function function*)[]
			| ABSSTORE(l) ->  realgetAllVARAssign l 
 	end
 	else
	begin
		
		match body with CORPS(_) ->  
			 	if List.mem_assoc  name !alreadyEvalFunctionAS then realgetAllVARAssign ( List.assoc name !alreadyEvalFunctionAS) 
				else assignVar (get_fct_body   name) 			 
			| ABSSTORE(l) ->  realgetAllVARAssign l
	end
		in
		let global  = List.filter(fun x->  let fid = 	if  String.length x > 1 then 
										if (String.sub x  0 1)="*" then  String.sub x 1 ((String.length x)-1) else x
									else x  in List.mem fid !alreadyAffectedGlobales )used in
		let globalPtr = List.filter(fun x-> List.mem_assoc x !listeAssosPtrNameType  )global in
		 if globalPtr != [] then true  else List.mem x global  

 


and getLoopVarInc v inst firstcond hasAndCond=
		let pred = !getOnlyBoolAssignment in
 		getOnlyBoolAssignment := false;
		setOnlyIncrement true;
		isMultiInc := false;(*Printf.printf "getincrement %s \n "v;afficherLesAffectations inst;*)
		let (isindirect,inc,var, before) = getIncOfInstList v inst inst (INTERVALLE(INFINI,INFINI)) [] firstcond hasAndCond in
		getOnlyBoolAssignment := pred;

if List.mem v !myChangeVar || List.mem var !myChangeVar then
( (*Printf.printf "pb increment %s %s\n" v var;*)
(*Printf.printf "\n\nAssigned  \n" ;			List.iter(fun x -> Printf.printf "%s" x)!myChangeVar ;
Printf.printf "\nptr  \n" ;	LocalAPContext.print !myCurrentPtrContext;*)
expressionIncFor :=NOTHING;


opEstPlus :=false;
(isindirect,NODEFINC,var, before, !isMultiInc)

)
else
(

	(*	
let myCurrentPtrContext = ref []
let intoLoopCurrent = ref false
let myChangeVar = ref []*)

			opEstPlus := getIsAddInc inc;
			expressionIncFor :=  getIncValue inc ;
		 
(*
Printf.printf "getincrement %s FIN\n "v;print_intType (getIncType inc);
print_expression !expressionIncFor 0; flush();new_line();flush();flush();new_line();flush();flush();new_line();flush();
Printf.printf "getincrement %s FIN res\n "v;*)
		setOnlyIncrement false;

		(isindirect,inc,var, before, !isMultiInc)

)

