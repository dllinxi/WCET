(**
	Transfor simple recusive functions into loop (if to for).
*)

open Frontc
open Cabs
open Cprint

(** compare 2 String
	@param n	one string.
	@param m	other string 
	@param cpt	number of first compared char
	@return		boolean
 *)
let rec compareString n m cpt = 
let taille =(String.length m) in
	if ((String.length n) != taille)
	then false
		
	else 
		if (cpt==(taille)) then true
		else 
			if (String.get n cpt == String.get m cpt) 
			then compareString n m (cpt+1)
			else false
;;


(** compare 2 String
	@param n	name of function
	@param m	name of call funcion
	@return		boolean
 *)
let call_me = fun n m -> 
	compareString n m 0 	
;;


(** search if function is called
	@param n	name of function.
	@param exp	expression
	@return		boolean
 *)
let rec e_call n exp =  
match exp with
	CALL (VARIABLE(m),_)-> (*Printf.printf "e_call CALL\n" ;*)call_me n m
	|UNARY (_,e) -> (*Printf.printf "e_call UNARY\n" ;*)e_call n e
	|BINARY (_,eg,ed)-> (*Printf.printf "e_call BINARY\n" ;*)(e_call n eg) || (e_call n ed)
	|QUESTION (e,f,g) -> (*Printf.printf "e_call QUESTION\n" ;*)(e_call n e) || (e_call n f) || (e_call n g)
	|CAST (_,e) -> (*Printf.printf "e_call CAST\n" ;*)(e_call n e)
	|COMMA (e::[]) -> (*Printf.printf "e_call COMMA\n" ;*)(e_call n e)
	|COMMA (e::l) -> (*Printf.printf "e_call COMMA\n" ;*)let exp=COMMA(l) in (e_call n e) || (e_call n exp)
	|EXPR_SIZEOF (e) -> (*Printf.printf "e_call EXPR_SIZEOF\n" ;*)(e_call n e)
	|INDEX (e,f) -> (*Printf.printf "e_call INDEX\n" ;*)(e_call n e) || (e_call n f)
	|MEMBEROF (e,_) -> (*Printf.printf "e_call MEMBEROF\n" ;*)(e_call n e)
	|MEMBEROFPTR (e,_) -> (*Printf.printf "e_call MEMBEROFPTR\n" ;*)(e_call n e)
	|EXPR_LINE(e,_,_) -> (*Printf.printf "e_call EXPR_LINE\n" ;*)(e_call n e)
	|GNU_BODY(b) -> (*Printf.printf "e_call GNU\n" ;*)search_call n (snd b)
	| _ -> (*Printf.printf "e_call OTHER\n" ;*)false


(** search in statement if function is called
	@param n	name of function.
	@param exp	statement
	@return		boolean
 *)
and search_call =  fun n c ->
match c with  
	 COMPUTATION (e)-> (*Printf.printf "search_call COMPUTATION\n" ;*)e_call n e
	|BLOCK (_,c) -> (*Printf.printf "search_call BLOCK\n" ;*)(search_call n c)
	|SEQUENCE (a,b) -> (*Printf.printf "search_call SEQUENCE\n" ;*)(search_call n a) || (search_call n b)
	|IF (c,t,e) -> (*Printf.printf "search_call IF\n" ;*)(e_call n c) || (search_call n t) || (search_call n e)
	|WHILE (e,s) -> (*Printf.printf "search_call WHILE\n" ;*)(e_call n e) || (search_call n s)
	|DOWHILE (e,s) -> (*Printf.printf "search_call DOWHILE\n" ;*)(e_call n e) || (search_call n s)
	|FOR(a,b,c,s) -> (*Printf.printf "search_call FOR\n" ;*)(e_call n a) || (e_call n b) || (e_call n c) || (search_call n s)
	|RETURN (e) -> (*Printf.printf "search_call RETURN\n" ;*)(e_call n e)
	|SWITCH (e,s) -> (*Printf.printf "search_call SWITCH\n" ;*)(e_call n e) || (search_call n s)
	|CASE (e,s) -> (*Printf.printf "search_call CASE\n" ;*)(e_call n e) || (search_call n s)
	|DEFAULT (s) -> (*Printf.printf "search_call DEFAULT\n" ;*)(search_call n s)
	|LABEL (_,s) -> (*Printf.printf "search_call LABEL\n" ;*)(search_call n s)
	|STAT_LINE (s,_,_) -> (*Printf.printf "search_call STAT_LINE\n" ;*)(search_call n s)
	|(_) ->  (*Printf.printf "search_call OTHER\n" ;*)false



(** search in statements if function is called in IF
	@param n	name of function.
	@param exp	statement
	@return		boolean
 *)
and search_rec_ok =  fun n c ->
match c with
	BLOCK (_,c) -> (search_rec_ok n c)
	|SEQUENCE (a,b) -> (search_rec_ok n a) || (search_rec_ok n b)
	|IF (c,t,e) -> (e_call n c) || (search_call n t) || (search_call n e)
	|WHILE (_,s) ->  (search_rec_ok n s)
	|DOWHILE (_,s) ->  (search_rec_ok n s)
	|FOR(_,_,_,s) -> (search_rec_ok n s)
	|SWITCH (_,s) -> (search_rec_ok n s)
	|CASE (_,s) -> (search_rec_ok n s)
	|DEFAULT (s) -> (search_rec_ok n s)
	|LABEL (_,s) -> (search_rec_ok n s)
	|STAT_LINE (s,_,_) -> (search_rec_ok n s)
	|(_) -> (*Printf.printf "search_rec_ok OTHER\n" ;*)false



(** search an element of list at numpara position
	@param li		name of function.
	@param numpara		expression
	@param cpt		meter
	@return			element of list
 *)
and search_arg li numpara cpt=
	match li with
	e::l ->	if (cpt == numpara) then e else search_arg l numpara (cpt+1)
	|[] -> failwith ("frontc/iftofor.ml : search_arg")


(** search parameter at npara position in expression
	@param exp		expression
	@param name		name of function
	@param npara		number of concerned parameter
	@return			argument then recursive call
 *)
and location_arg_in_expr exp name npara =
match exp with 
	UNARY(_,e) -> location_arg_in_expr e name npara
	| BINARY (_,e1,e2) -> if (e_call name e1) then location_arg_in_expr e1 name npara else location_arg_in_expr e2 name npara
	| QUESTION (e1,e2,e3) -> if (e_call name e1) then location_arg_in_expr e1 name npara
				else (
					if (e_call name e2) 
					then location_arg_in_expr e2 name npara
					else location_arg_in_expr e3 name npara
				)
	| CAST (_,e) -> location_arg_in_expr e name npara
	| CALL (_,l) -> search_arg l npara 1
	| EXPR_SIZEOF (e) -> location_arg_in_expr e name npara
	| INDEX (e1,e2) ->  if (e_call name e1) then location_arg_in_expr e1 name npara else location_arg_in_expr e2 name npara
	| MEMBEROF (e,_) -> location_arg_in_expr e name npara
	| MEMBEROFPTR (e,_) -> location_arg_in_expr e name npara
	| GNU_BODY (b) -> location_arg (snd b) name npara
	| EXPR_LINE (e,_,_) -> location_arg_in_expr e name npara
	|_ -> (*Printf.printf "1 OTHER\n" ;*)NOTHING



(** search argument of recursive in statement 
	@param stat		statement
	@param name		name of function.
	@param npara		number of concerned parameter
	@return			argument then recursive call
 *)
and location_arg stat name npara=
	match stat with
	COMPUTATION(e) -> (location_arg_in_expr e name npara)
	|RETURN(e) -> (location_arg_in_expr e name npara)
	|SEQUENCE (s1,s2) -> if (search_call name s1) then (location_arg s1 name npara) else (location_arg s2 name npara)
	| BLOCK (b) -> location_arg (snd b) name npara
	| IF (_,s1,s2) -> if (search_call name s1) then (location_arg s1 name npara) else (location_arg s2 name npara)
	| WHILE (_,s) -> location_arg s name npara
	| DOWHILE (_,s) -> location_arg s name npara
	| FOR (_,_,_,s) -> location_arg s name npara
	| SWITCH (_,s) -> location_arg s name npara
	| CASE (_,s) -> location_arg s name npara
	| DEFAULT (s) -> location_arg s name npara
	| LABEL (_,s) -> location_arg s name npara
	| STAT_LINE (s,_,_) -> location_arg s name npara
	|_ -> (*Printf.printf "2 OTHER\n" ;*)NOTHING



(** revers sign to binary operator
	@param bop	statement
	@return		binary_operateur
*)
and revers_sign_binary bop = match bop with
	EQ -> NE
	|NE -> EQ
	|LT -> GE
	|GT -> LE
	|LE -> GT
	|GE -> LT
	| _ -> failwith("frontc/iftofor.ml : argument revers_sign_binary out")



(** search last IF which contains call 
	@param stat		statement
	@param name		name of function
	@return			boolean
*)
and lastIF stat n=
match stat with
	BLOCK ((_,st)) ->  lastIF st n
	| SEQUENCE(s1,s2) -> (lastIF s1 n) || (lastIF s2 n)
	| IF (_,s1,s2) -> (not (search_call n s1)) && (not (search_call n s2))
	| WHILE (_,st) -> lastIF st n
	| DOWHILE (_,st) -> lastIF st n
	| FOR (_,_,_,st) -> lastIF st n
	| SWITCH (_,st) -> lastIF st n
	| CASE (_,st) -> lastIF st n
	| DEFAULT (st) -> lastIF st n
	| LABEL (_,st) -> lastIF st n
	| STAT_LINE (st,_,_) -> lastIF st n
	| _ ->(*Cprint.print_statement stat ; Printf.printf "lastIF OTHER\n" ;*)true



(** search if expression contains function parameter
	@param expr			expression
	@param nameParameter		name of parameter using in recursive
	@return				boolean
*)
and expr_contains_parameter expr nameParameter =
match expr with 
	UNARY (_,e) -> expr_contains_parameter e nameParameter
	| BINARY (_,e1,e2) -> (expr_contains_parameter e1 nameParameter) || (expr_contains_parameter e2 nameParameter)
	| QUESTION (e3,e1,e2) -> (expr_contains_parameter e1 nameParameter) || (expr_contains_parameter e2 nameParameter) || (expr_contains_parameter e3 nameParameter)
	| CAST (_,e) -> expr_contains_parameter e nameParameter
	| CALL (e,[]) -> expr_contains_parameter e nameParameter
	| CALL (e1,e2::l) -> let c=CALL(NOTHING,l) in (expr_contains_parameter e1 nameParameter) || (expr_contains_parameter e2 nameParameter) || (expr_contains_parameter c nameParameter)
	| COMMA ([]) -> false
	| COMMA (e::l) -> let c=COMMA(l) in expr_contains_parameter e nameParameter || (expr_contains_parameter c nameParameter)
	| VARIABLE (st) -> (compareString st nameParameter 0)
	| EXPR_SIZEOF (e) -> expr_contains_parameter e nameParameter
	| INDEX (e1,e2) -> (expr_contains_parameter e1 nameParameter) || (expr_contains_parameter e2 nameParameter)
	| MEMBEROF (e,_) -> expr_contains_parameter e nameParameter
	| MEMBEROFPTR (e,_) -> expr_contains_parameter e nameParameter
	| EXPR_LINE (e,_,_) -> expr_contains_parameter e nameParameter
	| _ -> (*Printf.printf "expr_cont OTHER\n" ;*)false



(** an unary expression become binary expression
	@param u		expression
	@return			expression
*)
and unary_become_binary u =
match u with 
	UNARY(NOT,BINARY(op,e1,e2)) -> BINARY((revers_sign_binary op),e1,e2)
	|UNARY(NOT,e) -> BINARY(NE,e,CONSTANT(CONST_INT("0")))
	|UNARY(op,e) -> BINARY(EQ,UNARY(op,e),CONSTANT(CONST_INT("0")))
	|_->NOTHING



(** statement separate in 3 building list during statement course
	@param stat		statement
	@param name		name of function
	@param before		list
	@param casRec		list
	@param after		list
	@return			boolean
*)
and beforeNowAfter stat nameFunction before casRec after =
(*Printf.printf "beforeNowAfter -> %s" nameFunction ;Cprint.print_statement stat ;*)
match stat with
	BLOCK (b) -> if (casRec == []) 
			then 
			(	(*Printf.printf "beforeNowAfter Bloc-> %s" nameFunction ;*)
				if  (search_call nameFunction (snd b))
				then beforeNowAfter (snd b) nameFunction before (casRec@[BLOCK (b)]) after
				else (before@ [BLOCK (b)],casRec,after)
			)
			else  (before,casRec,(after@ [BLOCK (b)])) 
	| SEQUENCE (s1,s2) -> if(search_call nameFunction s1) 
				then (beforeNowAfter s1 nameFunction before casRec (after@[SEQUENCE (NOP,s2)]))
				else (beforeNowAfter s2 nameFunction (before@[SEQUENCE (s1,NOP)]) casRec after)
	| IF (e,s1,s2) -> if (casRec == [])
			then
			( 	(*Printf.printf "beforeNowAfter 1-> %s" nameFunction ;*)
				if(search_call nameFunction s1 || search_call nameFunction s2)
				then 
				(	(*Printf.printf "beforeNowAfter 2-> %s" nameFunction ;*)
						let statement= (IF (e,s1,s2)) in
							if(apCall statement nameFunction)
							then 
								let re,apC=apCallStat statement nameFunction [] in
								(before, (casRec @ [re]), (after @ apC))
							else (before, (casRec @ [IF (e,s1,s2)]), after)	
				)
				else ((before@ [IF (e,s1,s2)]), casRec, after)
			)
			else (before, casRec, (after @ [IF (e,s1,s2)]))
	| STAT_LINE (st,_,_) -> beforeNowAfter st nameFunction before casRec after
	(*|STAT_LINE (s,_,_) -> apCall s nameFunction*)
	| stat->  (*Printf.printf "beforeNowAfter OTHER\n" ;*)if (casRec == []) then (before @ [stat],casRec,after) else (before,casRec,after@ [stat])



(** test if there is statement after call
@param stat			statement
@param nameFunction 		string
@ return bool
*)
and apCall stat nameFunction = match stat with
	 COMPUTATION (e)-> not (e_call nameFunction e)
	|BLOCK (_,c) -> apCall c nameFunction
	|SEQUENCE (a,b) -> if (search_call nameFunction a && b!=NOP) then true else apCall b nameFunction
	|IF (_,t,e) -> if (search_call nameFunction t) then apCall t nameFunction else apCall e nameFunction
	|WHILE (_,s) -> apCall s nameFunction
	|DOWHILE (_,s) -> apCall s nameFunction
	|FOR(_,_,_,s) -> apCall s nameFunction
	|RETURN (e) -> not (e_call nameFunction e)
	|SWITCH (_,s) -> apCall s nameFunction
	|CASE (_,s) -> apCall s nameFunction
	|DEFAULT (s) -> apCall s nameFunction
	|LABEL (_,s) -> apCall s nameFunction
	|STAT_LINE (s,_,_) -> apCall s nameFunction
	|(_) -> (* Printf.printf "search apCall OTHER\n" ; *)false

(** distinguish between statement after and before recursive call
@param stat					statement	
@param nameFunction				name of function
@param apC					nil list
@return (statement,statement list)		before,after
*)
and apCallStat stat nameFunction apC=
match stat with
	COMPUTATION (e)-> (COMPUTATION (e),apC)
	|BLOCK (b) -> let i=apCallStat (snd b) nameFunction apC in ((BLOCK (fst(b),(fst i))), snd i)
	|SEQUENCE (a,b) -> if (search_call nameFunction a) 
			then let i=apCallStat a nameFunction (apC@[b]) in (fst i , snd i)
			else let i= apCallStat b nameFunction apC in (SEQUENCE (a, fst i),snd i)
	|IF (c,t,e) -> if (search_call nameFunction t) 
			then let i=apCallStat t nameFunction apC in (IF (c,fst i,e) , snd i)
			else let i=apCallStat e nameFunction apC in (IF (c,t,fst i) , snd i)
	|WHILE (e,s) -> let i = apCallStat s nameFunction apC in WHILE (e,fst i), snd i
	|DOWHILE (e,s) -> let i = apCallStat s nameFunction apC in DOWHILE (e,fst i), snd i
	|FOR(a,b,c,s) -> let i = apCallStat s nameFunction apC in FOR(a,b,c,fst i),snd i
	|RETURN (e) -> RETURN (e) , apC
	|SWITCH (e,s) -> let i = apCallStat s nameFunction apC in SWITCH (e,fst i), snd i
	|CASE (e,s) -> let i = apCallStat s nameFunction apC in CASE (e,fst i), snd i
	|DEFAULT (s) -> let i = apCallStat s nameFunction apC in DEFAULT (fst i), snd i
	|LABEL (e,s) -> let i = apCallStat s nameFunction apC in LABEL (e,fst i), snd i 
	|STAT_LINE (s,e,f) -> let i = apCallStat s nameFunction apC in STAT_LINE (fst i,e,f), snd i 
	|other -> other , apC


(** reconstruct statement from list 
	@param l		list
	@return			statement
*)
and reconstruct l =
match l with
	  NOP ::_ -> NOP
	| COMPUTATION (e) ::_ -> COMPUTATION (e)
	| BLOCK (b) ::[] -> BLOCK(b)
	| BLOCK (b) ::l -> BLOCK (fst(b),reconstruct l)
	| SEQUENCE (NOP,s2) ::[] -> s2
	| SEQUENCE (s1,NOP) ::[] -> s1
	| SEQUENCE (NOP,s2) ::l -> SEQUENCE (reconstruct l,s2)
	| SEQUENCE (s1,NOP) ::l -> SEQUENCE (s1,reconstruct l)
	| SEQUENCE (s1,s2) ::_ -> SEQUENCE (s1,s2)
	| IF (e,s1,s2) ::_ -> IF (e,s1,s2)
	| WHILE (e,s) ::_ -> WHILE (e,s)
	| DOWHILE (e,s) ::_ -> DOWHILE (e,s)
	| FOR (e1,e2,e3,s) ::_ -> FOR (e1,e2,e3,s)
	| BREAK ::_ -> BREAK
	| CONTINUE ::_ -> CONTINUE
	| RETURN (e) ::_ -> RETURN (e)
	| SWITCH (e,s) ::_ -> SWITCH (e,s)
	| CASE (e,s) ::_ -> CASE (e,s)
	| DEFAULT (s) ::_ -> DEFAULT (s)
	| LABEL (st,s) ::_ -> LABEL (st,s)
	| GOTO (st) ::_ -> GOTO (st)
	| ASM (st) ::_ -> ASM (st)
	| GNU_ASM (a,b,c,d) ::_ -> GNU_ASM (a,b,c,d)
	| GNU_ASM_VOLATILE (a,b,c,d) ::_ -> GNU_ASM_VOLATILE (a,b,c,d)
	| STAT_LINE (s,i,st) ::_ -> STAT_LINE (s,i,st) 
	| [] -> NOP



(** do argument in position num *)
and doList (a,b,c) num = 
match num with
	1->a
	|2->b
	|3->c
	|_->[]



(**substitute call in expression
	@param expr			expression
	@param nameFunction		name of function
	@param ans			name of middle variable
	@return				expression
*)
and substituteCallInExpr exp nameFunction ans =
match exp with
	UNARY (u,e) -> UNARY(u,(substituteCallInExpr e nameFunction ans))
	|BINARY (opb,e1,e2) -> BINARY(opb, (substituteCallInExpr e1 nameFunction ans),(substituteCallInExpr e2 nameFunction ans))
	| QUESTION (e1,e2,e3) -> QUESTION((substituteCallInExpr e1 nameFunction ans),(substituteCallInExpr e2 nameFunction ans),(substituteCallInExpr e3 nameFunction ans))
	| CAST (bt,e) -> CAST (bt,(substituteCallInExpr e nameFunction ans)) 
	| CALL (VARIABLE(m),l) -> if (call_me nameFunction m) 
				then 
					VARIABLE(ans) 
				else CALL (VARIABLE(m),l)
	| COMMA (el) -> COMMA (List.fold_right (fun e r -> (substituteCallInExpr e nameFunction ans) :: r) [] el)
	| EXPR_SIZEOF (e) -> (substituteCallInExpr e nameFunction ans)
	| INDEX (e1,e2) -> INDEX((substituteCallInExpr e1 nameFunction ans),(substituteCallInExpr e2 nameFunction ans))
	| MEMBEROF (e,s) -> MEMBEROF ((substituteCallInExpr e nameFunction ans),s)
	| MEMBEROFPTR (e,s) -> MEMBEROFPTR ((substituteCallInExpr e nameFunction ans),s)
	| GNU_BODY (b) -> GNU_BODY (fst b, substituteCall (snd b) nameFunction ans )
	| EXPR_LINE(e,s,i) -> EXPR_LINE((substituteCallInExpr e nameFunction ans),s,i)
	| other -> other



(** substitute call in statement
	@param stat			statement
	@param nameFunction		name of function
	@param ans			name of middle variable
	@return				statement
*)
and substituteCall stat  nameFunction ans =
match stat with
	 COMPUTATION(CALL(VARIABLE(m),l)) -> if (call_me nameFunction m) then NOP else COMPUTATION(CALL(VARIABLE(m),l))
	| COMPUTATION(BINARY(ASSIGN,a,exp)) ->if (e_call nameFunction exp) 
					then SEQUENCE(COMPUTATION(BINARY(ASSIGN,a,substituteCallInExpr exp nameFunction ans)),COMPUTATION(BINARY(ASSIGN,VARIABLE(ans),a)))
					else COMPUTATION(BINARY(ASSIGN,a,substituteCallInExpr exp nameFunction ans))
	| COMPUTATION (e) -> COMPUTATION(substituteCallInExpr e nameFunction ans)
	| BLOCK (b) ->  BLOCK ((fst b),(substituteCall (snd b) nameFunction ans))
	| SEQUENCE (NOP,s2) -> substituteCall s2 nameFunction ans
	| SEQUENCE (s1,NOP) -> substituteCall s1 nameFunction ans
	| SEQUENCE (s1,s2) -> SEQUENCE (substituteCall s1 nameFunction ans,substituteCall s2 nameFunction ans)
	| IF (BINARY(bop,e1,e2),s1,s2) ->
					if(search_call nameFunction s1)
					then 
						substituteCall (IF(BINARY((revers_sign_binary bop),e1,e2),s1,s2)) nameFunction ans
					else
						if (search_call nameFunction s2)
						then
							if (lastIF s2 nameFunction)
							then ((*Printf.printf "lastIF 3 \n" ;*)(substituteCall s2 nameFunction ans))
							else IF (BINARY(bop,e1,e2), s1, (substituteCall s2 nameFunction ans))
						else
							IF (BINARY(bop,e1,e2),s1,s2)

	| IF(UNARY(uop,e),b,c) -> let u=UNARY(uop,e) in let next=IF((unary_become_binary u),b,c) in substituteCall next nameFunction ans
	| RETURN (e) -> if(e_call nameFunction e) then COMPUTATION(BINARY(ASSIGN,VARIABLE(ans),substituteCallInExpr e nameFunction ans)) else RETURN (e)
	| WHILE (e,s) -> WHILE (e, (substituteCall s nameFunction ans))
	| DOWHILE (e,s) -> DOWHILE (e,(substituteCall s nameFunction ans))
	| FOR (e1,e2,e3,s) -> FOR (e1,e2,e3,(substituteCall s nameFunction ans))
	| SWITCH (e,s) -> SWITCH (e,(substituteCall s nameFunction ans))
	| CASE (e,s) -> CASE (e,(substituteCall s nameFunction ans))
	| DEFAULT (s) -> DEFAULT ((substituteCall s nameFunction ans))
	| LABEL (st,s) -> LABEL (st,(substituteCall s nameFunction ans))
	| STAT_LINE (s,a,b) -> STAT_LINE ((substituteCall s nameFunction ans),a,b)
	| other -> other



(** return informations for recursive 
	@param stat			statement
	@param nameFunction		name of function
	@param	nameParameter		name of recursive parameter
	@param numpara			position of recursive parameter
	@return				((casZero,value of argument for recursive),(operateur condition, valeur condition))

(infoCall casRec nameFunction nameParameter numpara) 
*) 
and infoCall stat  nameFunction nameParameter numpara=
(*Cprint.print_statement stat ;Printf.printf "infoCall -> %s %s %d" nameFunction nameParameter numpara;*)
match stat with
	IF(BINARY(bop,e1,e2),b,c)->
			if (search_call nameFunction b)
			then 
				infoCall (IF(BINARY((revers_sign_binary bop),e1,e2),c,b)) nameFunction nameParameter numpara
			else 
				if (search_call nameFunction c)
				then
					if (lastIF c nameFunction) 
					then ((*Printf.printf "lastIF 2 \n" ;*)
						if (expr_contains_parameter e1 nameParameter)
						then ((b,(location_arg c nameFunction numpara)),(bop,e1))
						else ((b,(location_arg c nameFunction numpara)),(bop,e2)))
					else infoCall c nameFunction nameParameter numpara
				else failwith("frontc/iftofor.ml : not recursive")

	|IF(UNARY(uop,e),b,c) -> let u=UNARY(uop,e) in let next=IF((unary_become_binary u),b,c) in infoCall next nameFunction nameParameter numpara
	|BLOCK(d) -> infoCall (snd d) nameFunction nameParameter numpara
	|SEQUENCE(s1,s2) -> if(search_call nameFunction s1)
				then infoCall s1  nameFunction nameParameter numpara
				else infoCall s2  nameFunction nameParameter numpara
	| WHILE (_,s) -> infoCall s  nameFunction nameParameter numpara
	| DOWHILE (_,s) -> infoCall s  nameFunction nameParameter numpara
	| FOR (_,_,_,s) -> infoCall s  nameFunction nameParameter numpara
	| SWITCH (_,s) -> infoCall s  nameFunction nameParameter numpara
	| CASE (_,s) -> infoCall s  nameFunction nameParameter numpara
	| DEFAULT (s) -> infoCall s  nameFunction nameParameter numpara
	| LABEL (_,s) -> infoCall s  nameFunction nameParameter numpara
	| STAT_LINE (s,_,_) -> infoCall s  nameFunction nameParameter numpara
	
	|_ -> (*Cprint.print_statement stat ;*)failwith("frontc/iftofor.ml : not recursive")



(** change var n to i
	@param n		string
	@param i		string
	@param	expr		expression
	@return			expression
*)
and changeVarInExpr n i expr=
match expr with
	NOTHING -> NOTHING
	| UNARY (u,e) -> UNARY (u, changeVarInExpr n i e) 
	| BINARY (ASSIGN,e1,e2) ->BINARY (ASSIGN,e1,e2) 
	| BINARY (b,e1,e2) -> BINARY (b,changeVarInExpr n i e1,changeVarInExpr n i e2) 
	| QUESTION (e1,e2,e3)-> QUESTION (changeVarInExpr n i e1,changeVarInExpr n i e2,changeVarInExpr n i e3)
	| CAST (b,e) -> CAST (b,changeVarInExpr n i e)
	| CALL (e,l) -> CALL (changeVarInExpr n i e, (List.fold_right (fun e r->(changeVarInExpr n i e) :: r) l []))
	| COMMA (l) -> COMMA (List.fold_right (fun e r->(changeVarInExpr n i e) :: r) l [])
	| CONSTANT(c) -> CONSTANT(c)
	| VARIABLE (s) -> if (compareString s n 0) then VARIABLE (i) else VARIABLE (s)
	| EXPR_SIZEOF (e) -> EXPR_SIZEOF (e)
	| TYPE_SIZEOF (b) -> TYPE_SIZEOF (b)
	| INDEX (e1,e2) -> INDEX (changeVarInExpr n i e1,changeVarInExpr n i e2)
	| MEMBEROF (e,s) ->  MEMBEROF (changeVarInExpr n i e,s)
	| MEMBEROFPTR (e,s) ->  MEMBEROFPTR (changeVarInExpr n i e,s)
	| GNU_BODY (b) -> GNU_BODY (b)
	| EXPR_LINE (e,s,c) ->EXPR_LINE (changeVarInExpr n i e,s,c)


and changeVarInStat n i stat = 
match stat with
	COMPUTATION (e) -> COMPUTATION (changeVarInExpr n i e) 
	| BLOCK ((d,s)) -> BLOCK (d,(changeVarInStat n i s)) 
	| SEQUENCE (s1,s2) -> SEQUENCE ((changeVarInStat n i s1),(changeVarInStat n i s2))
	| IF (c,t,e) -> IF ((changeVarInExpr n i c),(changeVarInStat n i t),(changeVarInStat n i e))
	| WHILE (e,s) -> WHILE ((changeVarInExpr n i e),(changeVarInStat n i s))
	| DOWHILE (e,s) -> DOWHILE ((changeVarInExpr n i e),(changeVarInStat n i s))
	| FOR (a,b,c,s) ->  FOR ((changeVarInExpr n i a),(changeVarInExpr n i b),(changeVarInExpr n i c),(changeVarInStat n i s) )
	| RETURN (e) -> RETURN (changeVarInExpr n i e) 
	| SWITCH (e,s) -> SWITCH ((changeVarInExpr n i e),(changeVarInStat n i s))
	| CASE (e,s) -> CASE ((changeVarInExpr n i e),(changeVarInStat n i s))
	| other -> other



(** test if call is using in an binary operator  
	@param casRec			statement
	@param nameFunction		name of function
	@return				boolean
*)
and op_with_call casRec nameFunction= 
match casRec with
	COMPUTATION(BINARY(_,e1,e2)) -> (e_call nameFunction e1 || e_call nameFunction e2)
	(*|COMPUTATION(CALL(_)) -> false
	|RETURN(CALL(_)) -> false*)
	|RETURN(BINARY(_,e1,e2)) -> (e_call nameFunction e1 || e_call nameFunction e2)
	|BLOCK (_,s) -> op_with_call s nameFunction
	|SEQUENCE (a,b) -> (op_with_call b nameFunction) || (op_with_call a nameFunction)
	|IF (_,t,e) -> (op_with_call t nameFunction) || (op_with_call e nameFunction)
	|WHILE (_,s) -> op_with_call s nameFunction
	|DOWHILE (_,s) -> op_with_call s nameFunction
	|FOR(_,_,_,s) -> op_with_call s nameFunction
	|SWITCH (_,s) -> op_with_call s nameFunction
	|CASE (_,s) -> op_with_call s nameFunction
	|DEFAULT (s) -> op_with_call s nameFunction
	|LABEL (_,s) -> op_with_call s nameFunction
	|STAT_LINE (s,_,_) -> op_with_call s nameFunction
	(*|(_) -> false*)
	|_-> false
	

(** return an expression includes in statement 
	@param stat		statement
	@return			expression
*)
and comp_to_const stat nameFunction =
match stat with
	RETURN(BINARY(ASSIGN,VARIABLE(_),a))->a
	|RETURN (a) -> a
	|COMPUTATION(BINARY(ASSIGN,VARIABLE(_),a))->a
	|COMPUTATION(a) -> a
	|BLOCK(b) -> comp_to_const (snd b) nameFunction
	|SEQUENCE (a,b) -> if (containReturn a) then comp_to_const a nameFunction else comp_to_const b nameFunction
	|IF (_,t,e) -> if (containReturn t) then comp_to_const t nameFunction else comp_to_const e nameFunction
	|WHILE (_,s) -> comp_to_const s nameFunction 
	|DOWHILE (_,s) -> comp_to_const s nameFunction 
	|FOR(_,_,_,s) -> comp_to_const s nameFunction 
	|SWITCH (_,s) -> comp_to_const s nameFunction 
	|CASE (_,s) -> comp_to_const s nameFunction 
	|LABEL (_,s) -> comp_to_const s nameFunction 
	|STAT_LINE (s,_,_) -> comp_to_const s nameFunction 
	|_ -> failwith ("frontc/iftofor.ml : comp_to_const not return")


(**
*)
and containReturn stat = match stat with
	BLOCK (b) -> containReturn (snd b)
	|SEQUENCE (a,b) -> containReturn a || containReturn b
	|IF (_,t,e) -> (containReturn t) || (containReturn e)
	|WHILE (_,s) -> containReturn s
	|DOWHILE (_,s) -> containReturn s
	|FOR(_,_,_,s) -> containReturn s
	|RETURN (_) -> true
	|SWITCH (_,s) -> containReturn s
	|CASE (_,s) -> containReturn s
	|DEFAULT (s) -> containReturn s
	|LABEL (_,s) -> containReturn s
	|STAT_LINE (s,_,_) -> containReturn s
	|_ -> false


(** test if call is using in a return 
	@param casRec			statement
	@param nameFunction		name of function
	@return				boolean
*)
and return_loop casRec nameFunction =
match casRec with
	RETURN(CALL(_)) -> true
	|RETURN(BINARY(_,e1,e2)) -> (e_call nameFunction e1 || e_call nameFunction e2)
	|BLOCK (_,s) -> return_loop s nameFunction
	|SEQUENCE (a,b) -> (return_loop b nameFunction) || (return_loop a nameFunction)
	|IF (_,t,e) -> (return_loop t nameFunction) || (return_loop e nameFunction)
	|WHILE (_,s) -> return_loop s nameFunction
	|DOWHILE (_,s) -> return_loop s nameFunction
	|FOR(_,_,_,s) -> return_loop s nameFunction
	|SWITCH (_,s) -> return_loop s nameFunction
	|CASE (_,s) -> return_loop s nameFunction
	|DEFAULT (s) -> return_loop s nameFunction
	|LABEL (_,s) -> return_loop s nameFunction
	|STAT_LINE (s,_,_) -> return_loop s nameFunction
	|(_) -> false

(** create statement list of variable initialization 
@param dec	list of declaration
@return  	statement list
*)
and initparam dec=
match dec with
	DECDEF(_,_,[]) ::l-> initparam l
	|DECDEF(a,b,(name,_,_,exp)::r) ::l -> if (exp==NOTHING) then initparam (DECDEF(a,b,r) ::l)
					else (COMPUTATION(BINARY(ASSIGN,VARIABLE(name),exp))) :: (initparam (DECDEF(a,b,r) ::l))
	|[] -> []
	|_::l -> initparam l

(** reconstruct statement list in statement
*)
and constructInit liInit= match liInit with
	[e]-> e
	|a::b::[]->SEQUENCE(a,b)
	|e::l->SEQUENCE(e,constructInit l)
	|[]->NOP


(**
*)(*
and pre_post_caszero stat nameFunction =
match stat with
	COMPUTATION (e)-> 
	|BLOCK (b) -> 
	|SEQUENCE (a,b) -> 
	|IF (c,t,e) -> 
	|WHILE (e,s) -> 
	|DOWHILE (e,s) -> 
	|FOR(a,b,c,s) -> 
	|RETURN (e) -> 
	|SWITCH (e,s) -> 
	|CASE (e,s) -> 
	|DEFAULT (s) -> 
	|LABEL (e,s) -> 
	|STAT_LINE (s,e,f) -> 
	|other -> 
*)

(** remove return in statment
*)
and remove_return stat = match stat with
	BLOCK (_,RETURN(_)) ->NOP 
	|BLOCK (b) -> if(containReturn (snd b)) then remove_return (snd b) else BLOCK (b)
	|SEQUENCE (RETURN(_),_) -> NOP
	|SEQUENCE (a,RETURN(_)) -> a
	|SEQUENCE (a,b) -> if (containReturn a) then SEQUENCE ((remove_return a),b) 
			else 
				if (containReturn b) then SEQUENCE (a,(remove_return b)) 
				else SEQUENCE (a,b)
	|IF (c,RETURN(_),e) ->IF (c,NOP,e)
	|IF (c,t,RETURN(_)) ->IF (c,t,NOP)
	|IF (c,t,e) -> if(containReturn t) then IF (c,(remove_return t),e)
			else if (containReturn e) then IF (c,t,(remove_return e))
				else IF (c,t,e)
	|WHILE (_,RETURN(_)) -> NOP
	|WHILE (e,s) -> if(containReturn s) then WHILE (e,(remove_return s)) else WHILE (e,s)
	|DOWHILE (e,s) -> if(containReturn s) then DOWHILE (e,(remove_return s)) else DOWHILE (e,s)
	|FOR(_,_,_,RETURN(_)) -> NOP
	|FOR(a,b,c,s) -> if(containReturn s) then FOR(a,b,c,(remove_return s)) else FOR(a,b,c,s)
	|RETURN (_) -> NOP
	|SWITCH (e,s) -> if(containReturn s) then SWITCH (e,(remove_return s)) else SWITCH (e,s)
	|CASE (e,s) -> if(containReturn s) then CASE (e,(remove_return s)) else CASE (e,s)
	|DEFAULT (s) -> if(containReturn s) then DEFAULT ((remove_return s))  else DEFAULT (s) 
	|LABEL (e,s) -> if(containReturn s) then LABEL (e,(remove_return s)) else LABEL (e,s)
	|STAT_LINE (s,e,f) -> if(containReturn s) then STAT_LINE ((remove_return s),e,f) else STAT_LINE (s,e,f)
	|other -> other

(** return RETURN(e) statment
*)
and stat_return stat = match stat with
	BLOCK (b) -> if(containReturn (snd b)) then stat_return (snd b) else NOP
	|SEQUENCE (a,b) -> if (containReturn a) then (stat_return a)
			else stat_return b
	|IF (_,t,e) -> if(containReturn t) then stat_return t
			else stat_return e
	|WHILE (_,s) -> stat_return s
	|DOWHILE (_,s) -> stat_return s
	|FOR(_,_,_,s) -> stat_return s
	|RETURN (e) -> RETURN (e) 
	|SWITCH (_,s) -> stat_return s
	|CASE (_,s) -> stat_return s
	|DEFAULT (s) -> stat_return s
	|LABEL (_,s) -> stat_return s
	|STAT_LINE (s,_,_) -> stat_return s
	|other -> other

(** remove NOP statement
*)
and clean stat =match stat with
	COMPUTATION(NOTHING) -> NOP
	|RETURN(NOTHING) -> NOP
	|BLOCK (b) -> BLOCK (fst b,clean (snd b))
	|SEQUENCE (NOP,b) ->clean b
	|SEQUENCE (a,NOP) -> clean a
	|SEQUENCE (a,b) -> SEQUENCE (clean a, clean b) 
	|IF (c,t,e) -> IF (c,clean t, clean e)
	|WHILE (_,NOP) -> NOP
	|WHILE (e,s) -> WHILE (e,clean s)
	|DOWHILE (_,NOP) -> NOP
	|DOWHILE (e,s) -> DOWHILE (e,clean s)
	|FOR(_,_,_,NOP) -> NOP
	|FOR(a,b,c,s) -> FOR(a,b,c,clean s) 
	|SWITCH (_,NOP) -> NOP
	|SWITCH (e,s) -> SWITCH (e,clean s)
	|CASE (_,NOP) -> NOP
	|CASE (e,s) -> CASE (e,clean s)
	|DEFAULT (s) -> DEFAULT (clean s)
	|LABEL (_,NOP) -> NOP
	|LABEL (e,s) -> LABEL (e,clean s)
	|STAT_LINE (NOP,_,_) -> NOP
	|STAT_LINE (s,e,i) -> STAT_LINE (clean s,e,i) 
	|other -> other


(** modify body of function from if rec to for 
	@param stat			statement
	@param dec			declaration of variable
	@param nameFunction		name of function
	@param ans			string (name of middle variable)
	@param i			string (name of variable using in for)
	@param nameParameter		name of recursive parameter
	@param numPara			position of recursive parameter
	@return				statement
*)
and change_code stat dec nameFunction ans i nameParameter numpara=
 
	let r=beforeNowAfter stat nameFunction [] [] [] in
	let initVar= constructInit(initparam dec) in
 

	let before = changeVarInStat nameParameter i (reconstruct (doList r 1))


	and casRec = changeVarInStat nameParameter i (reconstruct (doList r 2))
	and afterRet = changeVarInStat nameParameter i (reconstruct (doList r 3)) in
	let after=remove_return(afterRet)
	and ret= stat_return afterRet in

	let paire = (infoCall casRec nameFunction nameParameter numpara) in
 
	let casNil = (fst (fst paire))
	and op =  (snd (fst paire))
	and cond = revers_sign_binary ((fst (snd paire)))
	and valcond = (snd (snd paire)) in 
	let casZero= remove_return(SEQUENCE(before,SEQUENCE((casNil),afterRet)) )in
	let casZ= SEQUENCE(before,SEQUENCE(casNil,afterRet)) in

 
	if (op_with_call casRec nameFunction)
	then
		if (return_loop casRec nameFunction)
		(* return + expr *)
		then
			SEQUENCE( 
				SEQUENCE(initVar,casZero),
				SEQUENCE(
					COMPUTATION(BINARY(ASSIGN,VARIABLE(ans),comp_to_const casZ nameFunction)), 
					SEQUENCE(
						SEQUENCE(
							FOR( 
							BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
							BINARY(cond,VARIABLE(i),valcond) , 
							BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
							SEQUENCE(
								SEQUENCE(initVar,before),
								substituteCall casRec nameFunction ans)
							),
							RETURN(VARIABLE(ans))
						),
						SEQUENCE(
							FOR( 
							BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
							BINARY(cond,VARIABLE(i),valcond) , 
							BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
							after
							),
							ret
						)
					)
				)
			)

		(* dans expr *)
		else
			SEQUENCE( 
				SEQUENCE(initVar,casZero),
				SEQUENCE(
					COMPUTATION(BINARY(ASSIGN,VARIABLE(ans),comp_to_const casZ nameFunction)), 
					SEQUENCE(
						FOR( 
						BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
						BINARY(cond,VARIABLE(i),valcond) , 
						BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
						SEQUENCE(
							SEQUENCE(initVar,before),
							substituteCall casRec nameFunction ans )
						),
						SEQUENCE(
							FOR( 
							BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
							BINARY(cond,VARIABLE(i),valcond) , 
							BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
							after
							),
							ret
						)	
					)
				)
			)
			

	else
		if (return_loop casRec nameFunction)
		(* return sans expr *)
		then
			SEQUENCE(
				SEQUENCE(initVar,casZero),
				SEQUENCE(
					COMPUTATION(BINARY(ASSIGN,VARIABLE(ans),comp_to_const casZ nameFunction)), 
					SEQUENCE(
						FOR( 
						BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
						BINARY(cond,VARIABLE(i),valcond) , 
						BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
						SEQUENCE(
							SEQUENCE(initVar,before),
							substituteCall casRec nameFunction ans )
						),
						SEQUENCE(
							FOR( 
							BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
							BINARY(cond,VARIABLE(i),valcond) , 
							BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
							after
							),
							ret
						)
					)
				)
			)
		
		(* cas simple *)
		else
			SEQUENCE(
				FOR( 
				BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
				BINARY(cond,VARIABLE(i),valcond) , 
				BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
				SEQUENCE(
					SEQUENCE(initVar,before),
					substituteCall casRec nameFunction ans
				)
				),
				SEQUENCE(
					SEQUENCE(initVar,casZero),
					SEQUENCE(
						FOR( 
						BINARY(ASSIGN,VARIABLE(i),VARIABLE(nameParameter)) , 
						BINARY(cond,VARIABLE(i),valcond) , 
						BINARY(ASSIGN,VARIABLE(i),(changeVarInExpr nameParameter i op)) ,
						after
						),
						ret
						)
				)
			)


(** do name of function parameter 
	@param n	single_name	
	@return		string list
*)
and name_parameter n = match n with
	(_,_,(_,PROTO(_,[(_,_,(a,_,_,_))],_),_,_))-> [a]
	|(b,c,(d,PROTO(e,(_,_,(a,_,_,_))::k,l),m,n)) -> a :: (name_parameter (b,c,(d,PROTO(e,k,l),m,n)))
	| _ -> failwith("frontc/iftofor.ml : name_parameter")


(** return list of parameter types 
	@param n	single_name	
	@return		string list
*)
and type_parameter n = match n with
	(_,_,(_,PROTO(_,[(a,_,_)],_),_,_))-> [a]
	|(b,c,(d,PROTO(e,(a,_,_)::h,i),j,k)) -> a :: (type_parameter (b,c,(d,PROTO(e,h,i),j,k)))
	| _ -> failwith("frontc/iftofor.ml : type_parameter")


(** from expression peer to string list*)
and list_string_of_paire c=
	match c with
	VARIABLE(a),VARIABLE(b) -> a::[b]
	|VARIABLE(a),_ -> [a]
	|_,VARIABLE(a) -> [a]
	|_,_ ->failwith ("frontc/iftofor.ml : list_string_of_paire")


(** search an element a in l1 return ((itself,this type ), his position) *)
and search a l1 l2 cpt=  
	match l1,l2 with 
	e::l , f::m -> if (compareString e a 0) then ((e,f),cpt) else search a l m (cpt+1)
	|_,_-> failwith ("frontc/iftofor.ml : search")


(** do name and type of recursive parameter
	@param l	expression list using in last if	
	@param lname	list of name parameters
	@param ltype	list of type parameters
	@return		((name,type),position)
*)
and parameter l lname ltype=
	match l with
	e::li ->if (List.mem e lname) then (search e lname ltype 1)
		else parameter li lname ltype
	|[] -> failwith ("frontc/iftofor.ml : parameter")


(** return couple expression from IF opb 
	@param stat		statement
	@param nameFunction	name of function
	@return 			(expression,expression)
*)
and argCond stat nameFunction=
	match stat with
	IF(BINARY(bop,e1,e2),b,c)->
			if (search_call nameFunction b)
			then 
				argCond (IF(BINARY((revers_sign_binary bop),e1,e2),c,b)) nameFunction 
			else 
				if (search_call nameFunction c)
				then
					if (lastIF c nameFunction) 
					then ( (e1,e2)	)
					else argCond c nameFunction 
				else failwith("frontc/iftofor.ml : argCond")

	|IF(UNARY(uop,e),b,c) -> let u=UNARY(uop,e) in let next=IF((unary_become_binary u),b,c) in argCond next nameFunction 
	|BLOCK(d) -> argCond (snd d) nameFunction
	|SEQUENCE(s1,s2) -> if(search_call nameFunction s1)
				then argCond s1 nameFunction
				else argCond s2 nameFunction
	| WHILE (_,s) -> argCond s nameFunction
	| DOWHILE (_,s) -> argCond s nameFunction
	| FOR (_,_,_,s) -> argCond s nameFunction
	| SWITCH (_,s) -> argCond s nameFunction
	| CASE (_,s) -> argCond s nameFunction
	| DEFAULT (s) -> argCond s nameFunction
	| LABEL (_,s) -> argCond s nameFunction
	| STAT_LINE (s,_,_) -> argCond s nameFunction
	|_ -> failwith("frontc/iftofor.ml : argCond 2")


(** return name of fonction *)
and nameFunction = function 
	(_, _, (a,_,_,_))->a



(** 
	@param def	definition	
	@return		string list (list of variables)
*)
and editListVarSuite def= 
match def with
	DECDEF (_,_,[])-> []
	|DECDEF (b,s,(n,_,_,_)::l) -> let a=DECDEF(b,s,l) in n:: (editListVarSuite a)
	|_ -> failwith("frontc/iftofor.ml : argument out editListVar")


(** edit list of variables *)
and editListVar defList =
match defList with
	[]->[]
	|e::l -> (editListVarSuite e) @ (editListVar l)



(** search if nomVar doesn't contain in l 
	@param l		string list
	@param nomVar		string
	@return			boolean
*)
and listContains l nomVar = 
match l with
	[] -> false
	|e::r -> (compareString e nomVar 0) || (listContains r nomVar)



(** add a character at the end of String*)
and addChar var= var^(Char.escaped 'a')



(** change one character in var *)
and changeVar var fin cpt =
let taille=String.length var in
	if (taille==cpt) then ""
	else
		if (cpt==fin) then (Char.escaped (char_of_int ((int_of_char (String.get var cpt))+1))) ^ (changeVar var fin (cpt+1))
		else (Char.escaped (String.get var cpt)) ^ (changeVar var fin (cpt+1))



(** modify a variable *)
and modifyVar var cpt =
	let taille= String.length var in
	if (cpt==taille) then addChar var
	else 
		if ((String.get var cpt) == 'z')
		then (modifyVar var (cpt+1))
		else (changeVar var cpt 0)



(** return name of variable possible *)
and createNameVar l nomVar =
	if (listContains l nomVar) then createNameVar l (modifyVar nomVar 0)
	else nomVar


(** add variable to declaration *)
and addVar l i typ= 
	let dec = DECDEF(typ,NO_STORAGE,[i,typ, [] ,NOTHING]) in [dec] @ l



(** modify body of function 
	@param sn	single_name	
	@param b	body
	@return		definition
*)
and makeover_function sn b = 
	let m=parameter (list_string_of_paire (argCond (snd b) (nameFunction sn))) (name_parameter sn) (type_parameter sn) in
 
	let nameParameter = fst (fst m) 
	and typ = (snd (fst m)) 
	and numpara = snd m in
	let l=(editListVar (fst b)) in 
	let ans = createNameVar l "a" in	
	let i = createNameVar (ans::l) "a" in

 

	FUNDEF(sn,
		(
		(addVar (addVar (fst b) ans typ) i (INT(LONG,SIGNED))),
		(clean (clean (change_code (snd b) (fst b)(nameFunction sn) ans i nameParameter numpara)))
		)
	)



(** transform loop in for 
	@param f	definition	
	@return		definition
*)
and loop_to_for = function 
	FUNDEF (sn,b) -> if (search_rec_ok (nameFunction sn) (snd b)) then makeover_function sn b
			else FUNDEF (sn,b)
	| other -> other



(** transform list loop in for  
	@param x	definition list
	@return		definition list
*)
and if_to_for (x:Cabs.definition list) = 
match x with
	[]-> []
	|e::[] -> let res = (loop_to_for e) in   [res]
	|e::l -> let res = (loop_to_for e) in  res :: (if_to_for l)


