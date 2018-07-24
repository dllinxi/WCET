(** extractinfoPtr - Extracts information in order to treat pointers. Search of domain (restriction, unknown...). Domain analysis on pointers. 
Strong hypthesis: when pointer domain is unknown, if the pointed variable is modifyed, only global, static variables having the same "type" of the pointer variable are set to top.
Domain can be: - completely defined (pointed variable known), - subset of domain (several pointed variables are possible), - unknown 

*) 
open Cabs
open Cvariables
 
open Printf
open Cprint
open Rename
open Util


let (trace :bool ref) = ref false
let (globalPtr : string list ref)=ref []
(*
add_list_bodyForPtr   v
let setalreadyDefFunction =*)



(* pb with struct *)

let filterSameTypePtrOrArray listeDesVar typ exp=
List.filter(fun x-> let isPtrOrArray = isArrayOrPtr x in 

				let hasTheSameType  = if existAssosPtrNameType x then (getAssosPtrNameType x) = typ  else true in 
			
				if isPtrOrArray && hasTheSameType then true 
				else (	(*if isPtrOrArray then Printf.printf "filterSameTypePtrOrArray taborptr %s " x; pb of casting ???*)let (t,_) = isStructAndGetIt typ in
						if  t then Printf.printf "filterSameTypePtrOrArray  %s " x;false)
				
				)listeDesVar





let  isArray  var  =  existAssosArrayIDsize var 

let isPtr var = List.mem_assoc var !listeAssosPtrNameType 


let rec listWithoutv l v = 
match l with
		| [] ->[]
		| np::sp -> if np = v then sp else np::(listWithoutv sp v)


module type PtrDomain = sig
	type ptrDom
	(*type ptrType*)
	type ptrContext  

	(* constants *)
	val noref: ptrDom
	val top: ptrDom
 
	 
	(* utility *)
	val is_noref: ptrContext -> bool
    val is_top: ptrContext -> bool
	val is_refer : ptrContext -> bool
	val print: ptrContext -> unit
	val addAllContext : string ->ptrContext -> ptrContext
	val addMultiAllContext : string list->ptrContext -> ptrContext

	(*vval setNewContext : string  -> ptrContext
	val joinContext : string ->ptrContext -> ptrContext = add*)
	
	val getDomain : ptrContext -> ptrDom
	val getType: ptrContext -> newBaseType 
	val init:    newBaseType -> ptrContext 
	val settop : newBaseType -> ptrContext 
	val setRef : newBaseType -> string list -> string list -> ptrContext 
	val getVal : ptrContext -> string list
	val getRef : ptrContext -> string list
	val setExtern : newBaseType ->  ptrContext 

	(*val isOnlyOnVar: ptrContext -> bool
	val getOnlyOnVar: ptrContext -> string*)
	(*val setInitVal : string -> ptrDom
	val setInitPtr : string -> ptrDom*)
end

module MyPtrDomain : PtrDomain = struct
	type ptrDom = NOREF |ALL| REFER of string list (*variables pointed bu it into all context*) * string list (* pointers used to set it in all context*) 
	(*type ptrType =  newBaseType *)(*its basetype*) 
	type ptrContext = PTRC of newBaseType * ptrDom
    let noref = NOREF 
	let top = ALL

	let getDomain v= let PTRC(_,x) = v in x

	let is_top v= getDomain v = ALL
	let is_noref v = getDomain v = NOREF
	let is_refer v = match getDomain v with REFER(_,_) -> true |_-> false
	 
	let addAllContext v d =	   
		match  d with
			|PTRC(t,NOREF) -> if  isPtr v then  PTRC(t, REFER( [],[v])) else PTRC(t, REFER([v], []))
			| PTRC(t,ALL) -> d
			| PTRC(t,REFER(varl, refl)) ->
					if  isPtr v then  if List.mem v refl then  PTRC(t,REFER(varl, refl)) else  PTRC(t,REFER(varl, v::refl))
					else if List.mem v varl then  PTRC(t,REFER(varl, refl)) else  PTRC(t,REFER(v::varl, refl))

	let rec addMultiAllContext lv d = (* nessesary pointer or array list *)
		if lv = [] then d else addAllContext (List.hd lv) (addMultiAllContext  (List.tl lv) d ) (* when all ???*)


	let getType v= let PTRC(t,_) = v in  t
					 
	let init ty = PTRC(ty, NOREF)
	let setExtern ty = PTRC(ty, REFER( [],["__extern"]))


	let settop ty = PTRC(ty, ALL)
	let setRef ty v r = PTRC(ty, REFER (v,r))

	let getVal v = match getDomain v with
			REFER(varl, _) ->   varl  |_->[]

	let getRef v = match getDomain v with
		  REFER(_, refl) ->   refl  |_->[]



   (* let is_noref v = v = NOREF*)

	let print v =  
		match getDomain v with
			|NOREF -> Printf.printf "\t\tNOREF" (*  if *ptr is used when it is nodef then it has to be considered as ALL or sumup of all contect it depends on ???*)
			|ALL -> Printf.printf "\t\tALL variable  and array of its type \n" (* catch an exeption no because it may be because local ...?*);(* print_expression (expVaToExp exp) 0; printfBaseType (getType v) *)
			|REFER(varl, refl) -> 
				if varl != [] then ( Printf.printf "\t assigntoAdressOf:" ;List.iter (fun x -> Printf.printf "\n\t\t%s " x)varl;  Printf.printf "\n" );
				if refl != [] then ( Printf.printf "\t assigntoOtherPtr:" ;List.iter (fun x -> Printf.printf "\n\t\t%s " x)refl; Printf.printf "\n" )
end

(*

module AllPtrContext(D: PtrDomain) = struct
	type t = (string * D.ptrContext) list
 	let empty : t = []
	let get s n = List.assoc n s
	let mem s n = List.mem_assoc n s
	let rec set s n v t =
		match s with
		| [] ->[(n, MyPtrDomain.addAllContext v (MyPtrDomain.init t))]
		| (np, vp)::sp ->
			if n = np then (np,MyPtrDomain.addAllContext v vp)::sp else
			if n < np then  (n, MyPtrDomain.addAllContext v (MyPtrDomain.init t))::s 
			else	 (np, vp)::(set sp n v t)	 

	let rec setMulti s n v t =
		match s with
		| [] ->[(n, MyPtrDomain.addMultiAllContext v (MyPtrDomain.init t))]
		| (np, vp)::sp ->
			if n = np then (np,MyPtrDomain.addMultiAllContext v vp)::sp else
			if n < np then (n, MyPtrDomain.addMultiAllContext v (MyPtrDomain.init t))::s 
			else (np, vp)::(setMulti sp n v t)

	let rec print s =
	match s with
	|[]->Printf.printf "\n"
	|   (np, vp)::sp ->Printf.printf "\n\t%s\n" np; MyPtrDomain.print vp; print sp
end 

*)

let (myChangePtr:  string     list ref)= ref []
let (myChangeUpperLoopPtr :  string     list ref)= ref []
let (myChangeVar :  string     list ref)= ref []
let (assocPtrChangeVar :  (string *string list)    list ref)= ref []







module LocalPtrContext(D: PtrDomain) = struct
	type t = (string * D.ptrContext) list
 	let empty : t = []
	let get s n = List.assoc n s
	let mem s n = List.mem_assoc n s
	let alwayget s n = 
		
		 let myType = if existAssosPtrNameType n then getAssosPtrNameType n else  INT_TYPE in
		 if mem s n = false then 
			 MyPtrDomain.settop myType
		else get s n

(* n is necessary into s*)
	let rec join s n values ptrvalues t add =
 

		match s with
		| [] ->if add then [(n, MyPtrDomain.setRef  t values    ("__extern"::ptrvalues))]  else []
		| (np, vp)::sp ->
			if n = np then 
				 
					if MyPtrDomain.is_noref vp then 
						(np, MyPtrDomain.setRef  (MyPtrDomain.getType vp) values    ("__extern"::ptrvalues))::sp   
					else if MyPtrDomain.is_top vp then (np, vp)::sp   
						 else (np, MyPtrDomain.setRef (MyPtrDomain.getType vp) 
									( union ( MyPtrDomain.getVal vp) values)  (union  (MyPtrDomain.getRef  vp) ptrvalues))::sp   
				
				
			else
				if n < np then  if add then (n, MyPtrDomain.setRef  t values    ("__extern"::ptrvalues))::s else s 
				else	 (np, vp)::(join sp n values ptrvalues t add)	 


	let rec unionPL s1 s2 global input=
	match s1 with
				| [] ->(*List.filter(fun(x,_)->List.mem np input || List.mem np global )*)s2
				| (np, vp)::sp -> 
			(	match s2 with
				|[]->s1
				| (np2, vp2)::sp2 ->

						 (*|| (List.mem np input = false && List.mem np global = false )*)  
						if np2= np then (np, vp)::(unionPL  sp sp2 global input) 
						else
							if  np2< np then (np2, vp2)::(unionPL  s1 sp2 global input)
							else (np, vp)::(unionPL sp s2 global input))
					 

	let rec set s n values ptrvalues t add=
					match s with
					| [] -> if add then [(n, MyPtrDomain.setRef  t values    ptrvalues)]    else []
					| (np, vp)::sp ->
						if n = np then  (np,  MyPtrDomain.setRef (MyPtrDomain.getType vp)   values ptrvalues)::sp   
						else
							if n < np then if add then (n, MyPtrDomain.setRef  t values    ptrvalues)::s    else s 
							else (np, vp)::(set sp n values ptrvalues t add)




	let rec filterGlobal s=
	List.filter(fun (n,_)->isPtr n)s

	let rec init s n d =
		match s with
		| [] ->[(n, d)]
		| (np, vp)::sp ->
			(*if n = np then (np,MyPtrDomain.addAllContext v (MyPtrDomain.init t))::sp else not possible*)
			if n < np then (n, d)::s 
			else (np, vp)::(init sp n d)

(*	let rec joinMulti s n v t =
		match s with
		| [] ->(*[(n, MyPtrDomain.addMultiAllContext v (MyPtrDomain.init t))]*)[]
		| (np, vp)::sp ->
			if n = np then (np,MyPtrDomain.addMultiAllContext v vp)::sp else
			if n < np then(* (n, MyPtrDomain.addMultiAllContext v (MyPtrDomain.init t))::*)s 
			else (np, vp)::(joinMulti sp n v t)



	let rec setMulti s n v t =
		match s with
		| [] ->(*[(n, MyPtrDomain.addMultiAllContext v (MyPtrDomain.init t))]*)[]
		| (np, vp)::sp ->
			if n = np then (np,MyPtrDomain.addMultiAllContext v (MyPtrDomain.init t))::sp else
			if n < np then (* (n, MyPtrDomain.addMultiAllContext v (MyPtrDomain.init t))::*)s 
			else (np, vp)::(setMulti sp n v t)*)



(* to compose an upper loop to istCallContext case loop function uper loop*)	
	let rec composeSet s1 globalAndInput (*inputVar*) =
 		match globalAndInput with
			| [] -> s1
			|_->	
				match s1 with
				| [] -> []
				| (name, cs1)::ns1 -> (* !alreadyAffectedGlobales*)
					(* either name is an input or an global then extern may be replaced by the value into globalAndInput *)
					(* else nothing to do *)
					(*if List.mem name !alreadyAffectedGlobales || List.mem name inputVar then
					begin*)
						 let nf =
								if MyPtrDomain.is_top cs1 then  cs1
								else 
									if MyPtrDomain. is_noref  cs1 then  
										if mem globalAndInput name = false then cs1 (*not possible*)
										else get  globalAndInput name

									else  
									begin
										if mem globalAndInput name = false || List.mem "__extern" (MyPtrDomain.getRef cs1) = false then cs1 (*not possible*)
										else 
										begin
											 
											let externSet = get  globalAndInput name in
											if MyPtrDomain.is_top externSet  then  cs1
											else 
												if MyPtrDomain.is_noref  externSet then cs1
										 		else
													MyPtrDomain.setRef (MyPtrDomain.getType cs1) 
													(union (MyPtrDomain.getVal cs1) (MyPtrDomain.getVal externSet)) 
													
													(	union (	(listWithoutv (MyPtrDomain.getRef cs1) "__extern") ) 	(MyPtrDomain.getRef externSet)	)									
										end 
									end in
								(name,nf)::composeSet ns1  globalAndInput  
					(*end
					else (name, cs1)::composeSet ns1 globalAndInput inputVar *)
(* the same ptr in the same order *)
(*compare two set *)
let rec areTheSAme s1 s2 =

 		match (s1, s2) with
			| ([],[]) ->  true 
			|((name, cs1)::ns1,(name2, cs2)::ns2)->	
				
									let fisrtSame =
										 if (MyPtrDomain.is_top cs1 && MyPtrDomain.is_top cs2) || 
										 (MyPtrDomain.is_noref  cs1 && MyPtrDomain.is_noref cs2) then true
	
										else if MyPtrDomain.is_refer cs1 && MyPtrDomain.is_refer cs2 then 
											  begin
												let (v1, v2) = (MyPtrDomain.getVal cs1, MyPtrDomain.getVal cs2) in
												let (r1, r2) = (MyPtrDomain.getRef cs1, MyPtrDomain.getRef cs2) in

												 (inclus v1 v2 ) &&  (inclus r1 r2 ) &&  inclus v2  v1 &&  inclus  r2 r1
											    end else false in
									  if fisrtSame then areTheSAme ns1 ns2 else false
		|_-> false




(* the same ptr in the same order *)
	let rec joinSet s1 s2 =  
		match s1 with
		| [] -> [](*[]s2*)
	    | (name, cs1)::ns1 ->
			 match s2 with
				| [] -> s1
				| (_, cs2)::ns2 ->
		 
					let nf =
						if MyPtrDomain.is_top cs1 || MyPtrDomain.is_top cs2 then cs1
						else if MyPtrDomain. is_noref  cs1 then cs2 
								else if MyPtrDomain. is_noref  cs2 then cs1 (* stange pointer on parameter lost*)
									else   MyPtrDomain.setRef (MyPtrDomain.getType cs1) (union (MyPtrDomain.getVal cs1) (MyPtrDomain.getVal cs2)) (union (MyPtrDomain.getRef cs1) (MyPtrDomain.getRef cs2))
																
					in (name,nf)::joinSet ns1 ns2

(* the same ptr in the same order *)
	let rec joinSetSubSet s1 s2 = 
	 	match s1 with
		| [] ->[](* []s2*)
	    | (name, cs1)::ns1 ->
			if mem s2 name then (name, get s2 name)::(joinSetSubSet  ns1  s2) else  (name, cs1)::(joinSetSubSet  ns1  s2)
			  
				

	let rec initIntervalAnalyse poiteurUsed input externValue globalPtr =
	if poiteurUsed = [] then []
	else
	begin
		
		let (x, next) = (List.hd poiteurUsed, List.tl poiteurUsed)  in
		let myType = if existAssosPtrNameType x then getAssosPtrNameType x else  INT_TYPE in
		let myDom =  	(*if List.mem x input then  D.setExtern myType
						else *)	if mem externValue x then get externValue x (* the external context *)
						 		else(*  if  List.mem x poiteurUsed then*) D.setExtern myType (*else D.settop (getAssosPtrNameType x )*)
					  in
		init (initIntervalAnalyse next input externValue globalPtr)  x myDom
					
				 
	end

(* get all the variable that may be assign by this pointer *)

	let isOnlyextern s n=
 	let md = alwayget s n   in 
			if MyPtrDomain.is_refer md then  
			begin

				let (mv,myptr) = (MyPtrDomain.getVal md, MyPtrDomain.getRef md)  in
				if mv != [] ||myptr=[]||  List.mem "__extern" myptr = false then false
				else  if   List.tl myptr = [] then  true else false
				
			end else false

	let hasLinkToGlobalPtr   = ref false
	let rec  getAllAdress s n t ln =
		if List.mem n !alreadyAffectedGlobales then  hasLinkToGlobalPtr:= true;
		if n = "__extern" then MyPtrDomain.setRef t []  ["__extern"] 
		else
		begin
			let md = alwayget s n   in 
			if MyPtrDomain.is_refer md then 
			begin

				let (mv,myptr) = (MyPtrDomain.getVal md, MyPtrDomain.getRef md)  in
				if List.mem "__extern" myptr = false then
					getAll s   (union [n] ln) t (MyPtrDomain.getVal md) myptr
				else
				begin
					let newList =  (listWithoutv myptr "__extern") in

					 
					if mv = []&& newList = [] then   MyPtrDomain.setRef t []  ["__extern"]  
					else
					begin  
						let yy =  getAll s (union ("__extern"::[n]) ln) t mv newList in 
						if MyPtrDomain.is_refer yy then MyPtrDomain.setRef t (MyPtrDomain.getVal yy) (union ["__extern"] (MyPtrDomain.getRef yy)) else yy
					end 
				end
			end
			else md
		end

 

			
		 
				
				
					(* a noref ptr is assign to a parameter we keep the pointer *)

	and getAll s ln t varl refl = 
		if refl = []  then  MyPtrDomain.setRef t varl [] (*pointer not initialised only into global *)
		else MyPtrDomain.setRef t varl ["__extern"]
			(*begin
				let (f, o) = (List.hd refl , List.tl refl) in
				if List.mem f ln = false then
				begin
					let md = alwayget s f   in
					if MyPtrDomain. is_noref  md then
					begin
						let xx = getAll s   (union [f] ln) t varl o in
						if MyPtrDomain.is_refer xx then MyPtrDomain.setRef t (MyPtrDomain.getVal xx) (union [f] (MyPtrDomain.getRef xx))
						else MyPtrDomain.settop t 									
					end
					else
						if  MyPtrDomain.is_top md then md 
						else 
						begin
							
								let yy = getAllAdress s f t ln in 
								if MyPtrDomain.is_refer yy then
								begin
									let (vl2, rl2) = (MyPtrDomain.getVal yy, MyPtrDomain.getRef yy ) in
									let xx = getAll s (union [f] ln) t varl o in
									if MyPtrDomain.is_refer xx then
									begin
										let (vl3, rl3) = (MyPtrDomain.getVal xx ,MyPtrDomain.getRef xx ) in
										 MyPtrDomain.setRef t (union vl2  vl3) (union rl2  rl3)
									end
									else  MyPtrDomain.settop t  	
								end
								else  MyPtrDomain.settop t  		
						end   
				end   
				else getAll s ln t varl o
	end*)

	and 
		isOnlyOne l = if l = [] then false else if List.tl l = [] then true else false
	and isempty l = if l = [] then true else false
	and classified  s n  globalNotEnum=
		 
		let t = if existAssosPtrNameType n then getAssosPtrNameType n else  INT_TYPE in
		let allAssign = getAllAdress s  n t [] in
		let l =getAllV allAssign globalNotEnum in
	 l



	and getAllV allAssign globalNotEnum =
		if MyPtrDomain.is_refer allAssign then
		begin
			let (vl2, rl2) = (MyPtrDomain.getVal allAssign, MyPtrDomain.getRef allAssign ) in
			if rl2 != [] then 
			begin 
				(*Printf.printf "je retourne quoi des var ou des expression To do ou toutes les globales dans un premier temps"; *)
				(*let vg = getGlobalByType t in*)  
				
				 union vl2 globalNotEnum  
			end (* je retourne quoi des var ou des expression*)
			else vl2
		end
		else  globalNotEnum

 
	let rec getALLPTRDomain ptrDomain changedPtr =
		 List.map(fun x-> 
					let (values, ptrvalues) =  
			 				
							let isD = alwayget ptrDomain x  in
								if MyPtrDomain.is_refer isD then ( MyPtrDomain.getVal isD , MyPtrDomain.getRef isD) else ([],["__extern"] )
							in
					let t = if existAssosPtrNameType x then getAssosPtrNameType x else  INT_TYPE in
					(x, (t, values , ptrvalues))


				) changedPtr  


	let rec  getAllOfAllPtrAdress ptrDomain changedPtr phase2=
			myChangeVar:=[];hasLinkToGlobalPtr:=false;

			let gne = (List.filter (fun x-> List.mem x !listEnumId = false && List.mem_assoc x !listeAssosPtrNameType = false ) !alreadyAffectedGlobales) in
			let globalPtrAssignDomain = List.map(fun x-> 
					let t = if existAssosPtrNameType x then getAssosPtrNameType x else  INT_TYPE in
					(x, getAllAdress ptrDomain x t [])


				) 

				changedPtr in	
			let (c,a)= getAllOfAllPtrAdressaux globalPtrAssignDomain gne phase2 in
			assocPtrChangeVar := a;
			myChangeVar := c;
			globalPtrAssignDomain


	and getAllOfAllPtrAdressaux globalPtrAssignDomain gne phase2=
				match globalPtrAssignDomain with
				| [] -> ([],[])
				| (name, cs1)::ns1 ->

						let t = if List.mem_assoc name !listAssocIdType then (List.assoc name !listAssocIdType) 
								else if existAssosPtrNameType name then getAssosPtrNameType name else  INT_TYPE in 
						let gnen = if phase2 = false || !hasLinkToGlobalPtr = true then mayBeAssignVar gne t else ["__extern"] in
						(*if !hasLinkToGlobalPtr=false && phase2 then Printf.printf "%s   restrictHHHHHHH\n"name else if phase2 then Printf.printf "%s    rOOOOO\n" name;*)
						let (lv,(nextl,nexta)) = (getAllV cs1 gnen ,getAllOfAllPtrAdressaux ns1 gnen phase2) in
				  		(union lv nextl , (name,lv)::nexta)


 
							 



	let rec setGlobalToTop s listOfGlobalToSet=
	match s with
		| [] -> []
	    | (name, cs1)::ns1 -> 
			if List.mem name (*!alreadyAffectedGlobales*)listOfGlobalToSet then 
			begin
(*Printf.printf "setGlobalToTop %s\n" name;*)
				(name, MyPtrDomain.setExtern  (MyPtrDomain.getType cs1))::(setGlobalToTop ns1 listOfGlobalToSet) 

			end
			else (name, cs1)::(setGlobalToTop ns1 listOfGlobalToSet) 
			 
 


	let rec print s =
	match s with
	|[]->Printf.printf "\n"
	|   (np, vp)::sp ->Printf.printf "\t%s\n" np; MyPtrDomain.print vp; print sp

end	

module LocalAPContext  = LocalPtrContext(MyPtrDomain)


(*

module APContext  = AllPtrContext(MyPtrDomain)
let myAP =  ref []*)


let (callsListEvalp:  (string * LocalAPContext.t)   list ref)= ref []
let (callsListEvalLoop:  (int * LocalAPContext.t)   list ref)= ref []

let (callsListEvalLoopInnerPtr:  (int * LocalAPContext.t)   list ref)= ref []
let (callsListEvalLoopInnerPtrCurrent:  (int * LocalAPContext.t)   list ref)= ref []

let upperLoop = ref 0
let (callsListEvalLoopInner:  (int * int)   list ref)= ref []

let myCurrentPtrContext = ref []
let myCurrentPtrContextAux = ref []
let intoLoopCurrent = ref false

(*

let rec extractMemAssignAndPtrAssign assignList =
if assignList != [] then 
begin
	let (affect, suite) = (List.hd assignList, List.tl assignList) in
	let lm =
	(match affect with
	 VAR ( id, exp,_,_)  ->	  
		if List.mem_assoc id !listeAssosPtrNameType then 
		begin 
			let (myType,isptr)
						 = if existAssosPtrNameType id then (getAssosPtrNameType id, true) else 
							(Printf.printf "extractMemAssignAndPtrAssign NOTYPE...";(INT_TYPE, false)) in
			if isptr  then
			begin
				let (nl,_) = getVarPtrDep id exp myType in
				if nl != []  then  
				begin
					 
					if List.tl nl = [] then (*only one var *)
					begin
						(*Printf.printf "extractMemAssignAndPtrAssign only one var...%s\n" (List.hd listeDesVar);*)
						  myAP :=  APContext.set !myAP id   (List.hd nl) myType  (* the ptr on record member ???*) 
					end
					else  myAP :=  APContext.setMulti !myAP id nl myType ; 
				end;
			end
		end  ; []
	| TAB ( id, _, _,_,_) -> if isPtr id then  [id] else [] |  MEMASSIGN ( id, _, _,_,_)->  [id]	 (*voir*)
	| IFVF ( _, i1, i2) 		->  let (l1,l2) = (extractMemAssignAndPtrAssign [i1]  ,   extractMemAssignAndPtrAssign [i2]) in   	union l1 l2  
	| IFV ( _, i1) 				->  extractMemAssignAndPtrAssign [i1]  
	| BEGIN (liste)				->    extractMemAssignAndPtrAssign liste   		
	| FORV ( _, _, _, _, _, _, i,_) ->  extractMemAssignAndPtrAssign [i]   			
	| APPEL (num, e, nom, s, CORPS c,v,r,_) -> 
			let liste1 = match c with BEGIN(e)-> e |e->[e] in
			let corps = if liste1 = [] then  get_fct_body   nom else liste1 in			
			extractMemAssignAndPtrAssign corps  
	| APPEL (num, e, nom, s, ABSSTORE a,v,r,_) ->   extractMemAssignAndPtrAssign [BEGIN (listeAsToListeAffect a) ]  
	) in

	let lf = extractMemAssignAndPtrAssign suite in union lm lf

 end
else []*)


let externptr listeptr listinpu =
List.filter (fun x-> if isPtr x then true else false )listeptr


let filterGlobalAndInput listeptr listinpu =
List.filter (fun (x,_)->   List.mem x listinpu || List.mem x !alreadyAffectedGlobales   )listeptr

let phaseinit = ref true

let getValuesToSet v s =
		if  isPtr v then 
		begin
				let isD = LocalAPContext.alwayget s v  in
				if MyPtrDomain.is_refer isD then 
					
						( MyPtrDomain.getVal isD , 
							(if List.mem "__extern"  (MyPtrDomain.getRef isD) then 
								union [v] (listWithoutv (MyPtrDomain.getRef isD) "__extern") 
								else MyPtrDomain.getRef isD) 
						) 
					
				else ([],[v] )
		end else ([v],[] ) 



let rec getAllValuesToSEt  nl s =
				match nl with
				| [] ->  ([],[])
				| name::ns1 -> let (listAV,listAP) =getAllValuesToSEt   ns1 s in
							   let(firstV,firtsP) =getValuesToSet name s in
				(union listAV firstV, union firtsP listAP)


let majPtrvarAssign id exp beforePtr into add=
	if List.mem_assoc id !listeAssosPtrNameType then 
	begin 
				let (myType,isPtr) = 	if existAssosPtrNameType id then (getAssosPtrNameType id ,true)
								else ((*Printf.printf "extractMemAssignAndPtrAssign NOTYPE...";*)(INT_TYPE,false)) in
				if isPtr then
				begin
					let (nl, _) = getVarPtrDep id exp myType in
				
					if nl != []  then  
					begin
						let (values, ptrvalues) =   getAllValuesToSEt   nl beforePtr  in
 						if into = false || !phaseinit = false then 
								LocalAPContext.set beforePtr id   values ptrvalues myType  add
						else
								LocalAPContext.join beforePtr id   values ptrvalues myType add
					end
					else  beforePtr 
				end
				else  beforePtr 
	end else  beforePtr 




let  new_instVarAndPtr id exp  =
		myCurrentPtrContext := majPtrvarAssign id exp !myCurrentPtrContext !intoLoopCurrent (*false*) true;
		 VAR(id, (if !isIntoSwithch = false then exp else EXP(NOTHING)), [], [])

let hasChangePTRInto = ref false  
let firstPass = ref true  

let rec fixPointPtr  assign beforePtr     =
	callsListEvalLoopInnerPtrCurrent := [];
	hasChangePTRInto:= false;
	let  fc = (localPtrAnalyse assign  beforePtr  true false) in   
	 
	let res = (LocalAPContext.joinSet fc   beforePtr ) in
	let isChange = !hasChangePTRInto && (LocalAPContext.areTheSAme res beforePtr) = false in

 (*LocalAPContext.print res; 
		LocalAPContext.print beforePtr ;*)
(*if (res = beforePtr)=false  then  Printf.printf "Current   change\n";*)
(*if !hasChangePTRInto then  Printf.printf "Inner   change\n"; *)
 
	if isChange then  (Printf.printf "Inner   change\n";  fixPointPtr  assign res  ;);
	hasChangePTRInto:= false; 




and onassignPtrAnalyse assign beforePtr  into  add=(* return  new prt interval list*)

 	match assign with
		VAR ( id, exp,l,u)  ->	  
		majPtrvarAssign id exp beforePtr into add	 			
		| TAB ( id, j, k,_,_)   |  MEMASSIGN ( id, j, k,_,_)->   	 beforePtr
 (* si unique alors remplacer var par *ptr...?? sinon rempacare par SET (var,*ptr) ou multi toute celles qui correspondent*)

		| IFVF ( e, i1, i2) 		-> 
				
				let fc  = (localPtrAnalyse [i1]  beforePtr   into false) in
				let  ec= (localPtrAnalyse [i2]  beforePtr   into false) in
			 
				 LocalAPContext.joinSet   fc ec
		| IFV ( e, i1) 		-> 	let  fc = (localPtrAnalyse [i1]  beforePtr   into false) in  
							
								(LocalAPContext.joinSet fc beforePtr )
		| BEGIN (liste)		-> (localPtrAnalyse liste beforePtr  into false)
		| FORV (a, b, c, d, e,f, i,_) ->
					if into = false then hasChangePTRInto:= false;
					(*if into = false then  upperLoop := a;*)
					let  fc = (localPtrAnalyse [i]  beforePtr  true false ) in  
					let res =	(LocalAPContext.joinSet fc   beforePtr ) 	in 
					let prev = !hasChangePTRInto in
					
					let result =
						if !phaseinit = false &&  List.mem_assoc a !callsListEvalLoop = false  then
						begin
								hasChangePTRInto := prev || (res = beforePtr) = false ;
								if into = false then
								begin 
									 	if !hasChangePTRInto  then 
										begin
												
												if !firstPass then
												begin
													(*Printf.printf "boucle %d  CHANGE\n" a;*)
													
													firstPass := false;
													fixPointPtr [i] res   ;
													firstPass := true;
											
													callsListEvalLoopInnerPtr:= List.append !callsListEvalLoopInnerPtr !callsListEvalLoopInnerPtrCurrent;
													callsListEvalLoopInnerPtrCurrent := [];
													callsListEvalLoop:=(a,res)::!callsListEvalLoop;
											
												end;
												res
										end
										else
										begin
											callsListEvalLoopInnerPtr:= List.append !callsListEvalLoopInnerPtr !callsListEvalLoopInnerPtrCurrent;
											callsListEvalLoopInnerPtrCurrent := [];
											callsListEvalLoop:=(a,beforePtr)::!callsListEvalLoop;
											beforePtr
										end

								end
								else 
								begin
									(*Printf.printf "boucle %d  ADD\n" a; if (fc = beforePtr)=false  then begin Printf.printf "Current   change\n";
									LocalAPContext.print fc;
									LocalAPContext.print beforePtr end;*)
								 
									 if List.mem_assoc a !callsListEvalLoopInnerPtrCurrent  = false then 
										callsListEvalLoopInnerPtrCurrent:=(a,res)::!callsListEvalLoopInnerPtrCurrent ;
									res
								

								end
							end
							else  (LocalAPContext.joinSet fc   beforePtr )  in
					
		
					result  (*???*)				
		| APPEL (num, e, nom, s, CORPS c,v,r,_) ->
			let memc = !hasChangePTRInto in
			hasChangePTRInto:=false; 
			let res =
				if AFContext.mem  !myAF nom = false 
				then LocalAPContext.setGlobalToTop beforePtr ( changeGlobalListPtr  nom (CORPS c) [])
				else 
		 		begin
					let f = AFContext.get  !myAF nom in
					let (input, used) =    (MyFunCont.getInput f,MyFunCont.getOtherUsed f)  in
					 
					if used = []   then   beforePtr (* neither pointer assignment or used => not pointer used*)
					else
					begin
						let poiteurUsed = externptr used input   in 

						 if poiteurUsed = []   then   beforePtr
						 else
						 begin
								 
								let newptr = localPtrAnalyse [e] beforePtr  into true  in
								let before =LocalAPContext.initIntervalAnalyse   poiteurUsed   input  newptr !globalPtr  in
								
								let after =if List.mem_assoc nom !callsListEvalp then 
				 								  LocalAPContext.composeSet  (filterGlobalAndInput (List.assoc nom !callsListEvalp) input) before  
											else
												localPtrAnalyse [c] before  into  false 
											in(*passer new*)				 
								 LocalAPContext.joinSetSubSet beforePtr after
						 end
	
					end
				end in
				hasChangePTRInto := memc;

				res
 				(*if List.mem_assoc nom !callsListEvalp then begin let  (l,l2)=(List.assoc nom !callsListEvalp) in if l = [] then 
					LocalAPContext.joinSetSubSet beforePtr l end else beforePtr*)
				(* to eval from the without call function *)
			
		| APPEL (num, e, nom, s, ABSSTORE a,v,r,_) ->
		let res =	LocalAPContext.setGlobalToTop beforePtr ( changeGlobalListPtr  nom (ABSSTORE a) []) in

		res

			

			
and localPtrAnalyse assignList beforePtr into  add =
	if assignList != [] then 
	begin
		let (affect, suite) = (List.hd assignList, List.tl assignList) in
		let lm= onassignPtrAnalyse affect beforePtr   into  add in
 
		let neptr=  localPtrAnalyse suite lm   into add  in
 
		 neptr
	end 
	else  beforePtr





let memAssign assignList =
	let changed = assignVar assignList in
	myChangePtr:=List.map (fun id-> String.sub id 1 ((String.length id)-1) )

					(List.filter(fun id->
							 if  String.length id > 1 then 
									if (String.sub id  0 1)="*" then if isPtr (String.sub id 1 ((String.length id)-1) ) then true else false else false
							else false)changed)
 


(* the var assign after an instruction setting a pointer are after instructions but for loop is may be all the var assign into its upper loop
into the current function if global are 
*)

let rec assignAfterAssign assignList  afterList usedafterList into =(* return rewritting assign,      assign  var list*)
if assignList = [] then ([],afterList,usedafterList)
else
begin
	let (assign, next) = (List.hd assignList, List.tl assignList) in
	
 	match assign with
		VAR ( id, e,_,_)  ->	let ou =   listeDesVarsDeExpSeules  (expVaToExp e) in 
			let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in 
			if List.mem_assoc id !listeAssosPtrNameType 
			then
			begin
				 let fid =
					if isPtr id then  
						(match e with
							EXP(CONSTANT(	CONST_COMPOUND(_)))-> ["*"^id] 
							 
						(* if a field of struct is assigned it is either a conmpoud either multiple not inportant*)
							|_-> [id]  )

				 	else [id] in
				
				 (VAR(id,e, after,used)::newassign, union fid after, union ou used) 
			end
			else 
				(VAR(id,e, after,used)::newassign
				,(if isAddedVar id then after else union [id] after),
				  union ou used) 
		| TAB ( id, i,e,_,_)->  let ou = union  (listeDesVarsDeExpSeules  (expVaToExp e)) (listeDesVarsDeExpSeules  (expVaToExp i)) in 
			let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in   
			let fid = 	if  String.length id > 1 then 
								if (String.sub id  0 1)="*" then  String.sub id 1 ((String.length id)-1) else id
							else id  in 
			if isPtr fid then (TAB ( id, i,e,after,used)::newassign,   union ["*"^fid] after,union ou used) else (TAB ( id, i,e,after,used)::newassign, union [id] after,union ou used)

		|    MEMASSIGN ( id,i,e,_,_)->   let ou = union  (listeDesVarsDeExpSeules  (expVaToExp e)) (listeDesVarsDeExpSeules  (expVaToExp i)) in 
			let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in   
			let fid = 	if  String.length id > 1 then 
								if (String.sub id  0 1)="*" then  String.sub id 1 ((String.length id)-1) else id
							else id  in 
			if isPtr fid then 
			(MEMASSIGN ( id, i,e,after,used)::newassign,   union ["*"^fid] after, union ou used) else (MEMASSIGN ( id, i,e,after,used)::newassign, union [id] after,union ou used)
		| IFVF ( e, i1, i2) 		-> 	
				let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in 
					let (a1, f1,u1) = assignAfterAssign [i1] after used into  in 
					let (a2, f2, u2) = assignAfterAssign [i2] after used into  in 
			(IFVF ( e, BEGIN a1, BEGIN a2) ::newassign,   union( union f1 f2) after, union  ( union u1 u2)   used)

		| IFV ( e, i1) 		-> 	
			let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in  
			let (a1, f1,u1) = assignAfterAssign [i1] after used into  in 			 
			(IFV ( e, BEGIN a1 ) ::newassign,   union  f1   after, union  u1   used)

		| BEGIN (liste)		->  
			let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in 
			let (a1, f1,u1) = assignAfterAssign liste after used into  in 			 
			(BEGIN (a1)	::newassign,   union  f1   after, union  u1   used)

		| FORV (a, b, c, d, e,f, i,cp) -> 	
			if into = false then
			begin
				let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in 
				let (_, f1,u1) = assignAfterAssign [i] after used true  in 		(*into the firts*)
				let (afterandinner,afterandinnerused) = (union  f1   after, union  u1   used) in
				let (a2, _,_) = assignAfterAssign [i] afterandinner afterandinnerused true  in 	(*into the inner*)		 
				(FORV (a, b, c, d, e,f, BEGIN a2,cp)::newassign,   afterandinner, afterandinnerused)
			end
			else
			begin
				let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in 
				let (a1, f1,u1) = assignAfterAssign [i] after used into  in 		
				 	 
				(FORV (a, b, c, d, e,f, BEGIN a1,cp)::newassign,   union  f1   after, union  u1   used)

			end
		| APPEL (num, e, nom, s, CORPS c,v,r,_) -> (* peut être corps vide pour qu'il ailles le chercher ? *)

						

			let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in 
			(*let sorties = (match s with BEGIN(sss)-> sss |_->[]) in
			let listVar = if List.mem_assoc nom !callsListEvalAssignVar then   List.assoc nom !callsListEvalAssignVar  else assignVar [c] in
			let globalPtr = List.filter(fun x->   List.mem x !alreadyAffectedGlobales) listVar  in*)
			let (fctass,fused) = assignVarAndUsed [APPEL (num, e, nom, s, CORPS c,v,r,[])] in
			
			 (APPEL (num, e, nom, s, CORPS (BEGIN(get_fct_bodyForPtr   nom)),v,r,used)::newassign,union fctass after, union fused used)

		| APPEL (num, e, nom, s, ABSSTORE a,v,r,l) ->let (newassign, after,used) = assignAfterAssign next afterList usedafterList into  in 
						
			  let (fctass,fused)  = assignVarAndUsed [APPEL (num, e, nom, s, ABSSTORE a,v,r,l)] in
			  (APPEL (num, e, nom, s, ABSSTORE a,v,r,used) ::newassign,union fctass after,union fused used)(*REVOIR*)
end 

 
let evalPtrEffectOnFunctionBody name assign f(*functionContext*)=
	let (input, used) =  (MyFunCont.getInput f,MyFunCont.getOtherUsed f) in
 
	if used = []   then   [] (* neither pointer assignment or used => not pointer used*)
	else
	begin
		let poiteurUsed = externptr used input   in 
 
		(*   pointer used are only output parameters voir si parametre est un pointeur sur globale et que la globale est aussi utilisée directement*)
		if poiteurUsed != [] then
		begin


		(*	let globalPtr = List.filter(fun x-> List.mem x !alreadyAffectedGlobales)poiteurUsed in*)
		(*	let globalPtr = List.filter(fun x->  isPtr x)!alreadyAffectedGlobales in*)

 
			let initPtrList = LocalAPContext.initIntervalAnalyse poiteurUsed input (*!myAP current context*)[] !globalPtr(*externValue*) in
			(*LocalAPContext.print initPtrList;*)
			myCurrentPtrContext := initPtrList;myCurrentPtrContextAux:=!myCurrentPtrContext;
			(*let globalPtrAssignDomain = List.map(fun x-> (x, (LocalAPContext.getAllAdress initPtrList x ))) globalPtr in*)
			
			(*if globalPtr != [] then Printf.printf "function used globalptr %s\n" name;
			(*List.iter (fun x->  Printf.printf "%s " x) globalPtr; *)
			LocalAPContext.print globalPtrAssignDomain; *)
			 
			hasChangePTRInto:=false; 

 	 		let x = localPtrAnalyse assign initPtrList  false false in

(*LocalAPContext.print x; *)

(*Printf.printf "\nfunction %s\n" name;*)
			 
 
			 x
 		end else []
	end  


						 
				 



