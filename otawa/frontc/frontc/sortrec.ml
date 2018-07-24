(**
	Identify recursive functions and sort them.
	
	@author Noélie Bonjean
*)

open Frontc
open Cabs

type functionBody = EXTERN | FUNCBODY of statement

	
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

(** compare 2 String
	@param n	name of function
	@param m	name of call funcion
	@return		boolean
 *)
and call_me = fun n m -> 
	compareString n m 0 	

(** search if nomVar doesn't contain in l 
	@param l		string list
	@param nomVar		string
	@return			boolean
*)
and listContains l nomVar = 
match l with
	[] -> false
	|e::r -> (compareString e nomVar 0) || (listContains r nomVar)



(** return name of fonction *)
and nameFunction = function 
	(_, _, (a,_,_,_))->a






(** do name list of functions in file 
	@param x	definition list
	@return		string list
*)
and name_fun_list li =
match li with
	[] -> []
	|FUNDEF (sn,_)::l -> (nameFunction sn) :: (name_fun_list l)
	| _::l -> name_fun_list l

(** do name list of functions in file 
	@param x	definition list
	@return		string list
*)



and get_fun_body name li =
if li = [] then EXTERN
else
begin
	let (first, next) = (List.hd li, List.tl li) in
	match first with
		FUNDEF (proto, body)     | OLDFUNDEF (proto, (*decs*)_, body)                   ->	 	
			let (_ , _ , fct )=proto in
			let (nom,_,_,_) =fct in 
			
			if nom = name then 
			begin
				let (decs, stat) = body in  
				let nb = BLOCK (decs, stat) in
					FUNCBODY(nb)
			end
			else  get_fun_body name next
		|_-> get_fun_body name next
end  






(** print couple
	@param	c	couple	
	@return 	unit
*)
and print c = 
let (namef,((nbC,nb),liCall))=c in
	begin 
	output_string stdout namef ;
	output_string stdout (" :   ");
	output_string stdout (string_of_int nb) ;
	output_string stdout ("   ");
	output_string stdout (string_of_int nbC) ;
	output_string stdout ("    ");
	print_call  liCall ;
	output_string stdout ("\n")
	end



(** print call list 
*)
and print_call l = match l with 
	[] -> output_string stdout ("   ")
	|e::r -> begin output_string stdout e ; output_string stdout " " ;  print_call r end

and exist_call_expr name exp namefunList def=
match exp with
	CALL (VARIABLE(m),a)-> 
		if (List.mem name namefunList) then((*Printf.printf "111 exist_call_expr -> %s called from %s\n " name m; *)output_string stdout (" : mutual MultiLevelRec because of \n")  ; output_string stdout m ;true)
		else
		 	if (List.mem m namefunList) then false
			else 
			begin
				(*Printf.printf "exist_call_expr -> %s  called from %s \n "  m name;*)
					exist_call_expr name (COMMA(a)) namefunList  def  ||
		
				  ( match get_fun_body m def with 
							FUNCBODY(b)-> 
									if exist_call name b (List.append [m]  namefunList) def then true else false
							|_ ->  false
						 ) 
			end
		
	|UNARY (_,e) -> 		exist_call_expr name e namefunList def
	|BINARY (_,eg,ed)->   	exist_call_expr name eg namefunList def ||  exist_call_expr name ed namefunList  def 
	|QUESTION (e,f,g) ->  	exist_call_expr name e namefunList def ||  exist_call_expr name f namefunList def ||  exist_call_expr name g namefunList def
		 
	|CAST (_,e) -> 			exist_call_expr name e namefunList def
	|COMMA (e::[]) ->		exist_call_expr name e namefunList def
	|COMMA (e::l) -> let suite= COMMA(l) in
			 exist_call_expr name suite namefunList def || exist_call_expr name e namefunList def 
	|EXPR_SIZEOF (e) -> 	exist_call_expr name e namefunList def
	|INDEX (e,f) ->   		if exist_call_expr name e namefunList  def then true else exist_call_expr name f namefunList  def
	|MEMBEROF (e,_) -> 		exist_call_expr name e namefunList def
	|MEMBEROFPTR (e,_) -> 	exist_call_expr name e namefunList def
	|EXPR_LINE(e,_,_) -> 	exist_call_expr name e namefunList def
	|GNU_BODY(b) -> 		exist_call name (snd b) namefunList def
	| _ -> false



(** do exist of call in statement  
	@param stat		statement
	@param namefun		name of function
	@return			boolean
*)
and exist_call name stat namefun def =	
match stat with
 	COMPUTATION (e)-> 	exist_call_expr name e namefun def
	|BLOCK (_,s) -> 	exist_call name s namefun def
	|SEQUENCE (a,b) -> 	exist_call name a namefun def ||exist_call name b namefun def 
	|IF (c,t,e) ->  	exist_call name e namefun def || exist_call name t namefun def ||  exist_call_expr name c namefun def
	|WHILE (e,s) -> 	exist_call_expr name e namefun def || exist_call name s namefun def 
	|DOWHILE (e,s) -> 	exist_call_expr name e namefun def ||  exist_call name s namefun def
	|FOR(a,b,c,s) -> 	exist_call_expr name a namefun def || exist_call_expr name b namefun def || exist_call_expr name c namefun def || exist_call name s namefun def
	|RETURN (e) -> 		exist_call_expr name e namefun def
	|SWITCH (e,s) -> 	exist_call_expr name e namefun def || exist_call name s namefun def
	|CASE (e,s) -> 		exist_call_expr name e namefun def || exist_call name s namefun def
	|DEFAULT (s) -> 	exist_call name s namefun def
	|LABEL (_,s) -> 	exist_call name s namefun def
	|STAT_LINE (s,_,_) -> exist_call name s namefun def
	|(_) ->false


(** do number of call in expression and number call itself
	@param stat		statement
	@param namefun		name of function
	@return			string list
*)

and number_call_expr exp namefun =
match exp with
	CALL (VARIABLE(m),a)-> 
		let reg= number_call_expr (COMMA(a)) namefun  in
		let red = 		if (call_me namefun m) then ((1,1),[]) else ((1,0),[m]) in
		((fst (fst reg))+(fst (fst red)),(snd (fst reg))+(snd (fst red))), (snd reg) @ (snd red)
	|UNARY (_,e) -> number_call_expr e namefun
	|BINARY (_,eg,ed)-> 
			let reg = number_call_expr eg namefun 
			and red = number_call_expr ed namefun 
			in ((fst (fst reg))+(fst (fst red)),(snd (fst reg))+(snd (fst red))), (snd reg) @ (snd red)
	|QUESTION (e,f,g) -> 
			let re = number_call_expr e namefun 
			and rf = number_call_expr f namefun 
			and rg = number_call_expr g namefun
			in (((fst (fst re))+(fst (fst rf))+(fst (fst rg)),(snd (fst re))+(snd (fst rf))+(snd (fst rg))) , (snd re)@(snd rf)@(snd rg))
	|CAST (_,e) -> number_call_expr e namefun
	|COMMA (e::[]) -> number_call_expr e namefun
	|COMMA (e::l) -> let suite= COMMA(l) in
			 let rs= number_call_expr suite namefun 
			 and re= number_call_expr e namefun in
			 (((fst (fst re))+(fst (fst rs)) , (snd (fst re))+(snd (fst rs))), (snd re)@(snd rs))
	|EXPR_SIZEOF (e) -> number_call_expr e namefun
	|INDEX (e,f) -> let re = number_call_expr e namefun 
			and rf = number_call_expr f namefun 
			in (((fst (fst re))+(fst (fst rf)),(snd (fst re))+(snd (fst rf))), ((snd re)@(snd rf)))
	|MEMBEROF (e,_) -> number_call_expr e namefun
	|MEMBEROFPTR (e,_) -> number_call_expr e namefun
	|EXPR_LINE(e,_,_) -> number_call_expr e namefun
	|GNU_BODY(b) -> number_call (snd b) namefun
	| _ -> ((0,0),[])



(** do number of call in statement and number call itself
	@param stat		statement
	@param namefun		name of function
	@return			string list
*)
and number_call stat namefun =	
match stat with
 	COMPUTATION (e)-> number_call_expr e namefun
	|BLOCK (_,s) -> number_call s namefun
	|SEQUENCE (a,b) -> let ra= number_call a namefun
			and rb= number_call b namefun
			in (((fst (fst ra))+(fst (fst rb)),(snd (fst ra))+(snd (fst rb))) , (snd ra)@(snd rb))
	|IF (c,t,e) -> 
			let re = number_call e namefun 
			and rt = number_call t namefun 
			and rc = number_call_expr c namefun
			in (((fst (fst re))+(fst (fst rt))+(fst (fst rc)),(snd (fst re))+(snd (fst rt))+(snd (fst rc))), (snd re)@(snd rt)@(snd rc))
	|WHILE (e,s) -> let re = number_call_expr e namefun
			and rs = number_call s namefun 
			in  (((fst (fst re))+(fst (fst rs)) , (snd (fst re))+(snd (fst rs))), (snd re)@(snd rs))
	|DOWHILE (e,s) -> let re = number_call_expr e namefun
			and rs = number_call s namefun 
			in  (((fst (fst re))+(fst (fst rs)) , (snd (fst re))+(snd (fst rs))), (snd re)@(snd rs))
	|FOR(a,b,c,s) -> let ra = number_call_expr a namefun
			and rb = number_call_expr b namefun
			and rc = number_call_expr c namefun
			and rs = number_call s namefun 
			in ( ((fst (fst ra))+(fst (fst rb))+(fst (fst rc))+(fst (fst rs)),(snd (fst ra))+(snd (fst rb))+(snd (fst rc))+(snd (fst rs))) ,
			    (snd ra)@(snd rb)@(snd rc)@(snd rs) )
	|RETURN (e) -> number_call_expr e namefun
	|SWITCH (e,s) -> let re = number_call_expr e namefun
			and rs = number_call s namefun 
			in  (((fst (fst re))+(fst (fst rs)) , (snd (fst re))+(snd (fst rs))), (snd re)@(snd rs))
	|CASE (e,s) -> let re = number_call_expr e namefun
			and rs = number_call s namefun 
			in  (((fst (fst re))+(fst (fst rs)) , (snd (fst re))+(snd (fst rs))), (snd re)@(snd rs))
	|DEFAULT (s) -> number_call s namefun
	|LABEL (_,s) -> number_call s namefun
	|STAT_LINE (s,_,_) -> number_call s namefun
	|(_) -> ((0,0),[])


(** construct 4 list functions their caracteristics
	@param		((_,_),list) 
	@return 	list of list
*)
and egal_different lnamef linfo eComplexe eSimple eSans different = 
match lnamef, linfo with
	[],[] -> (eComplexe, eSimple, eSans, different)
	|n::ln,i::li -> let nb=(fst (fst i)) in
			if (nb == (snd (fst i))) 
			then 
				if (nb < 2)
				then
					if (nb==1)
					then egal_different ln li eComplexe (eSimple@[(n,i)]) eSans different
					else
						if (nb==0)
						then egal_different ln li eComplexe eSimple (eSans@[(n,i)])  different
						else failwith ("frontc/sortrec : egal_different info out")
				else egal_different ln li (eComplexe@[(n,i)]) eSimple eSans different
				
			else egal_different ln li eComplexe eSimple eSans (different@[(n,i)])
	|_ -> failwith ("frontc/sortrec : egal_different")



and rec_mutuelle diff alldiff eComplexe eSimple eSans mutuelle =
	match diff with
	[]-> (eComplexe, eSimple, eSans, mutuelle)
	|(namef,info)::l -> let f= call_in_other namef info alldiff in 
		if (compareString "" f 0) 
		then 
			let nb = snd (fst info) in
			if (nb<2)
			then
				if(nb==1)
				then rec_mutuelle l alldiff eComplexe (eSimple@[(namef,info)]) eSans mutuelle
				else
					if(nb==0)
					then rec_mutuelle l alldiff eComplexe eSimple (eSans@[(namef,info)]) mutuelle
					else failwith ("frontc/sortrec : rec_mutuelle info out")
			else rec_mutuelle l alldiff (eComplexe@[(namef,info)]) eSimple eSans mutuelle
		else rec_mutuelle l alldiff eComplexe eSimple eSans (mutuelle@[(namef,info)])


and call_in_other e einfo lcall =
	match lcall with
	[] -> ""
	|(namef,info)::r -> if ((listContains (snd info) e) && (listContains (snd einfo) namef) ) 
				then namef 
				else call_in_other e einfo r


and print_list l = match l with
	[] -> output_string stdout ("")
	| e::r -> begin print e ; print_list r end

	

and readFich l=match l with
	[] -> []
	| FUNDEF (sn,b)::r -> (number_call (snd b) (nameFunction sn)  ):: readFich r
	| _ ::r -> readFich r


and search_in_list name li=
match li with
	[] -> false
	|e::l -> if (compareString name (fst e) 0) then true else search_in_list name l



(** tell if def is not recursive function
	@param file	file
	@param def	fonction to test
	@return		boolean
*)
and without_rec def file =
	let (complexe, simple, without, different) = egal_different (name_fun_list file) (readFich file) [] [] [] [] in 
	let (_, _, without, _) = rec_mutuelle different different complexe simple without [] in
	match def with
		| FUNDEF(sn,_) ->
			let name = nameFunction sn in
			search_in_list name without
				(* Make the pattern exhaustive: *)
		| _ -> failwith("frontc/sortrec : Bad definition (a function is needed)")



(** tell if def is simple recursive function
	@param file	file
	@param def	fonction to test
	@return		boolean
*)
and simple_rec def file =
	let (complexe, simple, without, different) = egal_different (name_fun_list file) (readFich file) [] [] [] [] in 
	let (_, simple, _, _) = rec_mutuelle different different complexe simple without [] in
	match def with
		| FUNDEF(sn,_) ->
			let name = nameFunction sn in
			search_in_list name simple	
				(* Make the pattern exhaustive: *)
		| _ -> failwith("frontc/sortrec : Bad definition (a function is needed)")



(** tell if def is complex recursive function
	@param file	file
	@param def	fonction to test
	@return		boolean
*)
and complex_rec def file =
	let (complexe, simple, without, different) = egal_different (name_fun_list file) (readFich file) [] [] [] [] in 
	let (complexe, _, _, _) = rec_mutuelle different different complexe simple without [] in
	match def with
		| FUNDEF(sn,_) ->
			let name = nameFunction sn in
			search_in_list name complexe	
				(* Make the pattern exhaustive: *)
		| _ -> failwith("frontc/sortrec : Bad definition (a function is needed)")
		

		
	(** tell if def is mutual recursive function
	@param file	file
	@param def	fonction to test
	@return		boolean
*)
and mutual_rec def file =
	let (complexe, simple, without, different) = egal_different (name_fun_list file) (readFich file) [] [] [] [] in 
	let (_, _, _, mutual) = rec_mutuelle different different complexe simple without [] in
	match def with
		| FUNDEF(sn,_) ->
			let name = nameFunction sn in
			search_in_list name mutual	
		| _ -> failwith("frontc/sortrec : Bad definition (a function is needed)")



(*

and info file =
let (complex, simple, without, different) = egal_different (name_fun_list file) (readFich file) [] [] [] [] in 
 rec_mutuelle different different complex simple without [] 

and without_rec def infoFile =
	let (_, _, without, _) = infoFile in
	let FUNDEF(sn,_)=def in let name = nameFunction sn in
	search_in_list name without



and simple_rec def infoFile =
	let (_, simple, _, _) = infoFile in
	let FUNDEF(sn,_)=def in let name = nameFunction sn in
	search_in_list name simple	



and complex_rec def infoFile =
	let (complex, _, _, _) = infoFile in
	let FUNDEF(sn,_)=def in let name = nameFunction sn in
	search_in_list name complex	
		

	
and mutual_rec def infoFile =
	let (_, _, _, mutual) = infoFile in
	let FUNDEF(sn,_)=def in let name = nameFunction sn in
	search_in_list name mutual	

*)

(** Search for recursive function in a C source file.
	@param x the tail of the definition list, initialy the whole file
	@param file the whole file to search in
*)	
and test out x file  = 
match x with 
	[] -> output_string out ("")
	| FUNDEF (sn,b)::r -> let d=(FUNDEF (sn,b)) in 
			if (without_rec d file) 
			then  
			begin let name =nameFunction sn in  Printf.printf "\ntest -> %s \n " name ;
				output_string out name ;
				let multiLevel = (match get_fun_body name file with 
					FUNCBODY(b)-> (*Cprint.print_statement b ;*)
							if  exist_call name b [] file then ( output_string out (" : mutual MultiLevelRec\n")  ; true) else (output_string out (" : without\n") ; false)
					|_ ->  
				(output_string out (" : without\n"); false) ) in
				if multiLevel = false then test out r file 
			end
			else 
				if (simple_rec (FUNDEF (sn,b)) file) 
				then let name =nameFunction sn in begin output_string out name ; output_string out (" : simple\n") ; test out r file  end
				else 
					if (complex_rec (FUNDEF (sn,b)) file) 
					then let name =nameFunction sn in begin output_string out name ; output_string out (" : complex\n") ; test out r file  end
					else 
						if (mutual_rec (FUNDEF (sn,b)) file) 
						then let name =nameFunction sn in begin output_string out name ; output_string out (" : mutual\n") ; test out r file  end
						else failwith ("frontc/sortrec : no group")
	| _ ::r -> test out r file 



(** Search if it exists recursive function in a C source file.
	@param x the tail of the definition list, initialy the whole file
	@param file the whole file to search in
*)	
and hasRecursivity  x file  = 
match x with 
	[] -> false
	| FUNDEF (sn,b)::r -> let d=(FUNDEF (sn,b)) in 
			if (without_rec d file) 
			then 
			begin let name =nameFunction sn in 
				(*output_string out name ;*)
				let multiLevel = (match get_fun_body name file with 
					FUNCBODY(b)-> 
							if exist_call name b [] file then  true else false
					|_ ->   false ) in
				if multiLevel = false then hasRecursivity  r file else false 
			end
			else 
				if (simple_rec (FUNDEF (sn,b)) file) 
				then true
				else 
					if (complex_rec (FUNDEF (sn,b)) file) 
					then  true
					else 
						if (mutual_rec (FUNDEF (sn,b)) file) 
						then  true
						else failwith ("frontc/sortrec : no group")
	| _ ::r -> hasRecursivity r file 

(** Search if it exists recursive function but only single one in a C source file.
	@param x the tail of the definition list, initialy the whole file
	@param file the whole file to search in
*)	
and hasOnlySIngleRecursivity  x file  = 
match x with 
	[] -> true
	| FUNDEF (sn,b)::r -> let d=(FUNDEF (sn,b)) in 
			if (without_rec d file) 
			then let name =nameFunction sn in 
			begin 
				(*output_string out name ;*)
				let nomultiLevel = (match get_fun_body name file with 
					FUNCBODY(b)-> 
							if exist_call name b [] file then  false else true
					|_ ->   true ) in
				if nomultiLevel = true then hasOnlySIngleRecursivity  r file else false 
			end
			else 
				if (simple_rec (FUNDEF (sn,b)) file) 
				then  hasOnlySIngleRecursivity r file  
				else 
					if (complex_rec (FUNDEF (sn,b)) file) 
					then  false
					else 
						if (mutual_rec (FUNDEF (sn,b)) file) 
						then false
						else failwith ("frontc/sortrec : no group")
	| _ ::r -> hasOnlySIngleRecursivity r file 

(** Classified the recursivity type of the apploication
	Recurcivity application class : (0: no recursivity, 1: only single recursivity (that may be change into loop by calypso using --crec option), 2:others cases
	@param x the tail of the definition list, initialy the whole file
	@param file the whole file to search in
*)	

let applicationRecursivityClass x file =
	if hasRecursivity  x file = false then 0
	else	if hasOnlySIngleRecursivity  x file  then 1
		else 2






