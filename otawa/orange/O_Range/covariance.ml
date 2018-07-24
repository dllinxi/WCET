(* covariance -- use Frontc CAML. Retrieve information for loop normalisation in case of covariance (called with increment.ml)
**
** Project:	O_Range
** File:	covariance.ml
** Version:	1.1
** Date:	 
** Author:	Marianne de Michiel
*)

open Cabs
open Frontc

let version = "covariance.ml Marianne de Michiel"

open Cprint
open Cexptostr
open Cvarabs
open Cvariables
open Constante
open Interval
open Util
open Rename
open ExtractinfoPtr
open Increment

type covCoef = SINGLE of float|MULTIP of float*float

type increaseAG = CLASNOINC | CLASNODEF | ARITHGEO of covCoef*covCoef*float


(* coef arithmetic*)

let minimimOf a b c d =
min (min a b)( min c d)

 
let maximumOf a b c d =
max (max a b) (max c d)



let addcov c1 c2 = (*add operator for covariance coefficients *)
match(c1, c2) with
	(SINGLE f1, SINGLE f2) -> SINGLE (f1 +. f2)
	| (SINGLE f1, MULTIP (f21, f22) ) -> MULTIP (f1 +. f21, f1 +. f22)
	| (MULTIP (f21, f22) ,SINGLE f1) -> MULTIP (f1 +. f21, f1 +. f22)
	| (MULTIP (f11, f12), MULTIP (f21, f22) ) -> MULTIP 
			(	minimimOf (f11 +. f21) (f11 +. f22)  (f12 +. f21) (f12 +. f22) , 
				maximumOf (f11 +. f21) (f11 +. f22)  (f12 +. f21) (f12 +. f22)
			)


let mulcov c1 c2 = (*mul operator for covariance coefficients *)
match(c1, c2) with
	(SINGLE f1, SINGLE f2) -> SINGLE (f1 *. f2)
	| (SINGLE f1, MULTIP (f21, f22) ) ->if f1 = 0.0 then SINGLE 0.0 else MULTIP (f1 *. f21, f1 *. f22)
	| (MULTIP (f21, f22) ,SINGLE f1) -> if f1 = 0.0 then SINGLE 0.0 else MULTIP (f1 *. f21, f1 *. f22)
	| (MULTIP (f11, f12), MULTIP (f21, f22) ) -> MULTIP (minimimOf  (f11 *. f21) (f11 *. f22)  (f12 *. f21) (f12 *. f22) , maximumOf (f11 *. f21) (f11 *. f22)  (f12 *. f21) (f12 *. f22))

let inf c1 cte = (*inf operator for covariance coefficients *)
match c1 with
	SINGLE f1  ->  f1 < cte
	| MULTIP (f11, f12) -> min f11 f12<cte

let sup c1 cte = (*sup operator for covariance coefficients *)
match c1 with
	SINGLE f1  ->  f1 > cte
	| MULTIP (f11, f12) -> max f11 f12>cte

let isArithmeticForm c1 c2 = (*isArithmeticForn if the two coef are equal to 1.0 *)
match(c1, c2) with
	(SINGLE f1, SINGLE f2) -> f1 = 1.0 && f2 =1.0
	| (SINGLE f1, MULTIP (f21, f22) ) ->  f1 = 1.0 && f21 = 1.0 && f22  = 1.0
	| (MULTIP (f21, f22) ,SINGLE f1) -> f1 = 1.0 && f21 = 1.0 && f22  = 1.0
	| (MULTIP (f11, f12), MULTIP (f21, f22) ) ->f11 = 1.0 && f21 = 1.0 && f22  = 1.0 && f12 = 1.0

let isConstantForm c1 = (*isConstantForm if the   coef are equal to 0.0 *)
match c1 with
	SINGLE f1  -> f1 = 0.0  
	| MULTIP (f21, f22)  ->   f21 = 0.0 && f22 = 0.0 


let getMin c1 = 
match c1 with
	SINGLE f1  -> f1  
	| MULTIP (f21, _)  ->   f21 

let issimpleCoefArithGeo q k interval x =
match (q,k) with
	(SINGLE a,SINGLE b)  -> 
		if a = 1.0 (*arithmetic*)|| a=0.0 (*constant*)|| b = 0.0  (*geo*) then (false, 0.0)
		else
		begin
			let (  croissant, decroissant) =  (  b > 0.0 && a > 1.0,  b < 0.0 && a < 1.0) in	
(*Printf.printf "2 recherche   correction %f  \n"   b; *)
			(croissant || decroissant, b /. ( 1.0 -. a))
		end
	|_->  (false, 0.0)
			

let isArithmeticRes q k interval var = (*isArithmeticForn if the two coef are equal to 1.0 *)
(*let isMulti2 = !hasSETCALL in*)
isMultiInc := true;
match q with
	SINGLE f1  -> 
			if f1 = 1.0 then
			( match k with
				SINGLE kcte ->  (*Printf.printf " ARITH  q%f k%f\n" f1 kcte ;*)
							if kcte >0.0 then (INC(POSITIV,CONSTANT(RCONST_FLOAT kcte)), var,0.0)
							else if kcte< 0.0 then (INC(NEGATIV,CONSTANT(RCONST_FLOAT kcte)), var,0.0)
								 else (NOINC,"others",0.0)  
							
				| MULTIP (kcte1, kcte2) -> 
							(* Printf.printf " ARITH  q%f k SET (%f, %f)\n" f1 kcte1 kcte2;*)
							if kcte1 >0.0 && kcte2 >0.0  then
									(INC(POSITIV, CONSTANT(RCONST_FLOAT  (min kcte1 kcte2 )) ), var,0.0)
							else if  kcte1 <0.0 && kcte2 <0.0  then ( INC(NEGATIV,CONSTANT(RCONST_FLOAT  (max kcte1 kcte2 ))), var,0.0)
								 else (NOINC,"others",0.0)    )

			else if f1 = 0.0 then (NOINC,"others",0.0)  
				 else 
				 begin  
				( match k with					
					SINGLE kcte ->  
							if kcte = 0.0 then  (*	Printf.printf " GEO  q%f k%f\n" f1 kcte ;*)
								if f1 > 1.0 then  (INC(MULTI,CONSTANT(RCONST_FLOAT f1))	, var,0.0)
								else if f1<1.0 then (INC(DIVI,CONSTANT(RCONST_FLOAT f1))	, var,0.0)
									else (NOINC,"others",0.0)  
							else  (*NODEFINC,var*)
							begin (*ARITHGEO 0<f1<1 ou f1>1 et kcte !=0 *)
								let minValue = getLower interval in
								let (value, croissant, decroissant) =  (f1, kcte > 0.0 && f1 > 1.0,  kcte < 0.0 && f1 < 1.0) in
								if estDefExp minValue = false then 
									if croissant then  (INC(MULTI,CONSTANT(RCONST_FLOAT f1))	, var,0.0)
									else (NODEFINC,var,0.0) 
								else
								begin
									let minFValue = getDefValue minValue in
								(*	Printf.printf " ARITHGEO   q%f k%f %f minFValue\n" f1 kcte minFValue;*)
									if croissant  && minFValue >= 0.0 then 
											(INC(POSITIV,CONSTANT(RCONST_FLOAT   (minFValue *. f1 -. minFValue +. kcte) )), var,0.0)  
									else if decroissant && minFValue >= 0.0  then 
											(INC(DIVI,CONSTANT(RCONST_FLOAT  f1)), var,if minFValue = 0.0  then kcte else 0.0)  
											 (*car kcte <0 *)
										 
										 else  (NODEFINC,var,0.0) 
								end
							end 
							
					| MULTIP (kcte1, kcte2) -> 
						let (value, croissant, decroissant) =  (f1,min kcte1 kcte2 > 0.0 && f1 > 1.0, max kcte1 kcte2 < 0.0 && f1 < 1.0) in
						(*Printf.printf " ARITHGEO  q%f k SET (%f, %f)\n" f1 kcte1 kcte2;*)
						let minValue = getLower interval in
						if estDefExp minValue = false then 
									if croissant then  (INC(MULTI,CONSTANT(RCONST_FLOAT f1))	, var,0.0)  
									else (NODEFINC,var,0.0)  
						else
						begin
							let minFValue = getDefValue minValue in
							
							if croissant  && minFValue >= 0.0 then (*(INC(POSITIV,CONSTANT(RCONST_FLOAT 
											 (minFValue *. f1 -. minFValue +. min kcte1 kcte2))), var)  *)
									 (INC(MULTI,CONSTANT(RCONST_FLOAT f1))	, var,0.0)  
							else if decroissant && minFValue >= 0.0  then 
									(INC(DIVI,CONSTANT(RCONST_FLOAT  f1)), var,if minFValue = 0.0  then (max kcte1 kcte2 ) else 0.0) 
										(*car kcte <0  règles*)
									 
								 else  (NODEFINC,var,0.0)  
						end
				)	
				end
					
	| MULTIP (f11, f12) ->let mini = min f11 f12 in  
						  let maxi = max f11 f12 in  
			if mini >1.0 then
			begin
			( match k with
				SINGLE kcte ->  	(*Printf.printf "  k%f\n"   kcte ;*)
					if kcte = 0.0 then    (INC(MULTI,CONSTANT(RCONST_FLOAT mini))	, var,0.0)   (*GEO*)
					else 	if kcte > 0.0 then  
							begin
								(*(INC(POSITIV,CONSTANT(RCONST_FLOAT   kcte)), var) *) (INC(MULTI,CONSTANT(RCONST_FLOAT mini))	, var,0.0)    
							end
					  	 	else   (NODEFINC,var,0.0)  
							
				| MULTIP (kcte1, kcte2) -> (*Printf.printf "   k(%f, %f) \n"   kcte1 kcte2;*)
					let  minik = min kcte1 kcte2 in
					if minik> 0.0  then  (INC(MULTI,CONSTANT(RCONST_FLOAT mini))	, var,0.0)  (*(INC(POSITIV,CONSTANT(RCONST_FLOAT  minik)), var) *)
					else (NODEFINC,var,0.0)  
 
				)	
				end
				else 	if mini = 1.0 then (*increase*)
						begin
							( match k with
								SINGLE kcte -> (* Printf.printf "  k%f\n"   kcte ;*) 
									if kcte > 0.0 then   (INC(POSITIV,CONSTANT(RCONST_FLOAT   kcte)), var,0.0)    (*revoir avec les règles*)
									else (NODEFINC,var,0.0)   
							
								| MULTIP (kcte1, kcte2) -> (*Printf.printf "   k(%f, %f) \n"   kcte1 kcte2;*)
											 let  minik = min kcte1 kcte2 in
											 if minik> 0.0  then (INC(POSITIV,CONSTANT(RCONST_FLOAT  minik)), var,0.0)  
											 else  (NODEFINC,var,0.0)  

												
								)	
						end

				else
						if maxi = 1.0 then  (*decrease*)
						begin
							( match k with
								SINGLE kcte -> (* Printf.printf "  k%f\n"   kcte ;*) 
									if kcte < 0.0 then   (INC(NEGATIV,CONSTANT(RCONST_FLOAT   kcte)), var,0.0)    (*revoir avec les règles*)
									else (NODEFINC,var,0.0)   
							
								| MULTIP (kcte1, kcte2) -> (*Printf.printf "   k(%f, %f) \n"   kcte1 kcte2;*)
											 let  minik = max kcte1 kcte2 in
											 if minik< 0.0  then (INC(NEGATIV,CONSTANT(RCONST_FLOAT  minik)), var,0.0)  
											 else  (NODEFINC,var,0.0)  

												
								)	
						end

		(*0<mini<1*)	else begin
								let minValue = getLower interval in
								if estDefExp minValue = false then (NODEFINC,var,0.0)  
								else
								begin
									let minFValue = getDefValue minValue in
							
									if  minFValue >= 0.0  then

									( match k with
										SINGLE kcte ->   
											if kcte <= 0.0 then  

												 (INC(DIVI,CONSTANT(RCONST_FLOAT  mini)), var, if minFValue = 0.0  then  kcte else 0.0)    (*revoir avec les règles*)
											else  (NODEFINC,var,0.0)  
							
										| MULTIP (kcte1, kcte2) -> (*Printf.printf "   k(%f, %f) \n"   kcte1 kcte2;*)
											let  minik = min kcte1 kcte2 in
											if minik<= 0.0  then  
												(INC(DIVI,CONSTANT(RCONST_FLOAT  mini)), var,if minFValue = 0.0  then (max kcte1 kcte2 ) else 0.0)  
											else  (NODEFINC,var,0.0) 

										)

									else  (NODEFINC,var,0.0) 
								end
	
						end

	



let isGeometricForm c1 c2 = (*isGeometricForm if k_{10} = k_{20} = 0 then the increment form of X is *)
match(c1, c2) with
	(SINGLE f1, SINGLE f2) -> f1 = 0.0 && f2 =0.0
	| (SINGLE f1, MULTIP (f21, f22) ) ->  f1 = 0.0 && f21 = 0.0 && f22  = 0.0
	| (MULTIP (f21, f22) ,SINGLE f1) ->f1 = 0.0 && f21 = 0.0 && f22  = 0.0
	| (MULTIP (f11, f12), MULTIP (f21, f22) ) ->f11 = 0.0 && f21 = 0.0 && f22  = 0.0 && f12 = 0.0



let haveTheSameComportmentAri c1 c2 = (*haveTheSameComportmentAri : either strictly increasing or decreasing for an arithmetic form *)
match(c1, c2) with
	(SINGLE f1, SINGLE f2) -> f1 *. f2 > 0.0
	| (SINGLE f1, MULTIP (f21, f22) ) ->  f1 *. f21 > 0.0  && f1 *. f22  > 0.0
	| (MULTIP (f21, f22) ,SINGLE f1) -> f1 *. f21 > 0.0  && f1 *. f22  > 0.0
	| (MULTIP (f11, f12), MULTIP (f21, f22) ) ->sup (mulcov c1 c2) 0.0

let haveTheSameComportmentGeo c1 c2 = (*haveTheSameComportmentGeo : either strictly increasing or decreasing for an geometric form *)
match(c1, c2) with
	(SINGLE f1, SINGLE f2) -> (f1> 1.0 && f2 > 1.0) || (f1< 1.0 && f2 < 1.0)
	| (SINGLE f1, MULTIP (f21, f22) ) ->  (f1> 1.0 && f21 > 1.0 && f22 >1.0 ) || (f1< 1.0 && f21 < 1.0 && f22 <1.0)
	| (MULTIP (f21, f22) ,SINGLE f1) -> (f1> 1.0 && f21 > 1.0 && f22 >1.0 ) || (f1< 1.0 && f21 < 1.0 && f22 <1.0)
	| (MULTIP (f11, f12), MULTIP (f21, f22) ) ->(f11> 1.0 && f12> 1.0 && f21 > 1.0 && f22 >1.0 ) || (f11< 1.0 && f12<1.0 && f21 < 1.0 && f22 <1.0)


let compose c1 c2 =
match(c1, c2) with
	(SINGLE f1, SINGLE f2) -> if f1 = f2 then SINGLE f1 else MULTIP (f1 ,f2 )
	| (SINGLE f1, MULTIP (f21, f22) ) ->  if f1 < min f21 f22 then MULTIP (f1 ,max f21 f22 ) else if  f1 > max f21 f22 then MULTIP (min f21 f22 , f1 ) else c2
	| (MULTIP (f21, f22) ,SINGLE f1) -> if f1 < min f21 f22 then MULTIP (f1 ,max f21 f22 ) else if  f1 > max f21 f22 then MULTIP (min f21 f22 , f1 ) else c1
	| (MULTIP (f11, f12), MULTIP (f21, f22) ) -> MULTIP ( minimimOf  f11 f12 f21 f22,maximumOf  f11 f12 f21 f22)



(* k_{10} * k_{20} >0 (the two functiosn are either strictly increasing or decreasing)*)



(*   covariance *)

(* 
vector : list of assos varriable initial coefficient (from the initialisation of the variable created for covariance )
assocVectorVar : list of assos varriable current coefficient (from the current analysed assignment of X)
isfirst : boolean true : get initial values of q where q is a constant the such as the coef of variable var1 into vector = q* its coef into assocVectorVar
pq : previous value of q if isfirst = false else undifined

return  : q if all q are ok and equal else notfound ie (false,0.0)

*)
 
let  rec getQofvector 	assocVectorVar 	vector isfirst pq =
 	match vector with
		| [] ->  (true,pq)
		| (x,v)::n ->
			if isfirst then  
			begin
				let (ok,q1)= getQ assocVectorVar x v  in
 
				if ok && q1 != 0.0 then getQofvector 	assocVectorVar 	n false q1
				else (false,0.0)
			end
			else
			begin
				let (ok,q1)= getQ assocVectorVar 	x v  in

 
				if ok && q1 = pq && q1 != 0.0 then  getQofvector 	assocVectorVar 	n false q1 
				else (false,0.0)
			end


 
(* if q <= 0 return notfound ie (false ... ) else the value of q=k2/.k *)
and getQ assocVectorVar 	x k =
if List.mem_assoc x assocVectorVar && k != 0.0 then
begin
 

	let k2 = List.assoc x assocVectorVar in
 
	if k2 *. k > 0.0 then (true,k2/.k) else (false,0.0)
end
else (false,0.0)

(* if q_{1} = q_{2} =1 then the increment form of X is

 if (...)
       X=X+ k_{10}
 else
       X=X+ k_{20}

    *
      if k_{10} * k_{20} >0 (the two functiosn are either strictly increasing or decreasing) ⇒ the resulting increment may be lower estimated by (+, SET(k_{10}, k_{20}))
    *
      else it is undefined or constant

*)

 



let rec isassignedvarnextaux  listOfCovariantVar   inst = (*return a new inst list where each assignment of var of listOfCovariantVar are completed by instructiontoadd *)

		match inst with
		| [] -> false
		| n1::t1 ->
	 		let boolean = isassignedvarnextaux  listOfCovariantVar  t1 in
		 	( match n1 with
					VAR ( id, _,_,_) | TAB ( id, _, _,_,_)   |  MEMASSIGN ( id, _, _,_,_) ->	
						if List.mem id listOfCovariantVar  then  true
				 		else boolean
					 
					| IFVF ( _, _, _) 	 
					| IFV ( _, _) 		-> 	if intersection  listOfCovariantVar ( assignVar [n1]) = []then boolean else false 
					| BEGIN (liste)		-> boolean ||   isassignedvarnextaux  listOfCovariantVar  liste
 					|_->boolean 
		)			 



let   cardinal l = List.length l
(*getNumberOfTerms   exp avec exp type estAffine*)


(* we have a constant form this form may change on the way to the end of the loop test if this change leads to a quit the loop ? *)
(*inputs :
	 beforeInst : the instruction after the constant assignment, 
	 nextInst : the instruction after the constant assignment, 
	 var : the new loop variable
	 cond : the loop condition
	 covvarAssig: the covariant assigment 
	 listcovvar: list of covariant variables
	vector : initial coefficient vector for covariant variables*)
let isConstantFormLeadingToQuitTheLoop     beforeInst nextInst  var  cond    covvarAssig listcovvar vector  =
	let las = evalStore (new_instBEGIN  (List.append beforeInst nextInst)) [] [] !myCurrentPtrContext in
	let assign =expVaToExp( applyStoreVA covvarAssig las) in		
	let ( nbterms ,assocVectorVar, hasconstant ) =getVectorAndNumberOfTerms  assign listcovvar  in 
	let (ok,q1) = 	if assocVectorVar != [] then  getQofvector 	assocVectorVar 	vector true 0.0  else (true,0.0) in		
	let k11 =  calculer (EXP(replaceAllValByZeroBut "--noOne"  listcovvar  assign))  !infoaffichNull [] (-1) in	
	if ok =false || estDefExp k11 = false then false
	else 
	begin
		let k1 = getDefValue k11 in   						
		if  q1= 0.0 then
		begin  		
							let ncond = calculer (EXP(remplacerValPar var (CONSTANT (RCONST_FLOAT k1)) cond)) !infoaffichNull [] (-1) in
							let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
															|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
							if isExecutedV = false then true else false
		end
		else 
			(* on peut aussi sortie à cause d'un boolean*)
			false
	end










(* il faut ajouter les cas de sortie*)
let rec getIncOfInstListCOV  vector  listOfCovariantVar  inst completList interval previous var cond firstcond hasAndCond=  
 	match inst with
		| [] -> (true,CLASNOINC,previous, false)(* return is ok, increment , list of instructions after last assignment of a listOfCovariantVar var, ifendoutput cf output because test of loop is false after*)
		| firstInst::nextInst ->
			match firstInst with
				VAR (id, exp,_,_) ->  
					 if  List.mem id listOfCovariantVar (*&& (isassignedvarnextaux listOfCovariantVar nextInst =false)*) then 
					 begin
						beforeAffin := firstInst;
						let instbefore = (List.append previous [firstInst] ) in
						let listcovvar = List.map (fun (x,_)-> x) vector in
						let las = evalStore (new_instBEGIN(instbefore)) [] [] !myCurrentPtrContext in
						(*afficherLesAffectations instbefore;Printf.printf "expva :\n" ; 	print_expVA !covvarAssign; flush(); space();*)
						let assign =expVaToExp( applyStoreVA !covvarAssign las) in		
						(*print_expression assign 0; new_line();	
						(*Printf.printf "recherche  dans liste :\n" ;afficherListeAS las;new_line ();Printf.printf "fin s liste :\n" ;*)
							Printf.printf " VAR  member\n"  ;	*)	
					 	let ( nbterms ,assocVectorVar, hasconstant ) =getVectorAndNumberOfTerms  assign listcovvar  in 

														
						let correction = if hasconstant then ((cardinal listcovvar ) +1 ) * nbterms else (cardinal listcovvar )  * nbterms in
						

						(*  if ok a assocVectorVar is a list of assos of (var1,k1), (var2,k2) ...
							 where each k1 ... are coef of var 1 coefficients else [] *)
						let (ok,q1) = 	if assocVectorVar != [] then  getQofvector 	assocVectorVar 	vector true 0.0  else (true,0.0) in		
	 					let k11 =  calculer (EXP(replaceAllValByZeroBut "--noOne"  listcovvar  assign))  !infoaffichNull [] (-1) in	
						 
						if ok =false || estDefExp k11 = false then (false,CLASNODEF, [],false)
						else 
						begin
							let k1 = getDefValue k11 in   	
							let (correct, inc, after, isoutput) = 
								getIncOfInstListCOV  vector  listOfCovariantVar nextInst completList interval  [] var cond firstcond hasAndCond in
							
							if correct  then
							begin  
							( match inc with 
								CLASNOINC -> 
									if  q1= 0.0 then
									begin  		


										let ncond = calculer (EXP(remplacerValPar var (CONSTANT (RCONST_FLOAT k1)) cond)) !infoaffichNull [] (-1) in
										let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
															|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
										if isExecutedV = false then (true, ARITHGEO ( SINGLE q1,  SINGLE k1 , 0.0), after , true) else (false,CLASNODEF, [],false)
									end
									else 
									begin
										let value = if    q1 < 1.0 then truncate (1.0 /. q1)    else 0 in

										let coef = if value != 0 then  ((value - 1)* correction) / value else 0 in

										(true, ARITHGEO ( SINGLE q1,  SINGLE k1 ,    float(coef )), after , false)
										(*true, ARITHGEO ( SINGLE q1,  SINGLE k1 ), after , false*)


									end
								| CLASNODEF -> (false,CLASNODEF, [],false)
								| ARITHGEO (q2, k2,corr2)->  (*q_{1} *q_{2} X + k_{10}*q_{2}+ k_{20}*)  
									let q = mulcov (SINGLE q1) q2 in
									let k = addcov (mulcov (SINGLE k1) q2) k2 in
									 
	
									if  isConstantForm  (SINGLE q1) then
									begin
									(	match k with
										SINGLE f  ->  

											let value = if      q1 < 1.0 then  truncate (1.0 /. q1)  else 0  in

											let coef = if value != 0 then  ((value - 1)* correction) / value else 0 in
											
											let ncond=calculer(EXP(remplacerValPar var (CONSTANT (RCONST_FLOAT f)) cond)) !infoaffichNull [] (-1) in
											let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
																|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
											if isExecutedV = false then (true, ARITHGEO (  SINGLE q1,  k, float(coef)), after , true)
											else (false,CLASNODEF, [],false)
										|_->(false,CLASNODEF, [],false)
									)														 
									end
									else if  isConstantForm q2 then
										 begin			
											(match k2 with
												SINGLE f  ->  
													let min =  getMin q2 in 
													let value = if      min < 1.0 then    truncate (1.0 /. min)   else 0  in
													let coef = if value != 0 then  ((value - 1)* correction) / value else 0 in
												
													let ncond = 
														calculer (EXP(remplacerValPar var (CONSTANT(RCONST_FLOAT f)) cond)) !infoaffichNull [] (-1) in
													let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
																	|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
													if isExecutedV = false then (true, ARITHGEO (  q2,      k2,  float(coef)), after , true)
													else (false,CLASNODEF, [],false)
												|_->(false,CLASNODEF, [],false)
											)				 
										 end
 										 else 
										 begin
											 let min =  getMin q in 
											 	let value = if     min < 1.0 then  truncate (1.0 /. min)   else 0  in

											 let coef = if value != 0 then  ((value - 1)* correction) / value else 0 in
											 (true, ARITHGEO (q,  k, float(coef)),after, false ) (* arithmético géométrique*)

										 end
								)				  	
								end							
								else (false,CLASNODEF, [], false)
						end
					end
					else getIncOfInstListCOV  vector  listOfCovariantVar  nextInst completList interval (List.append previous [firstInst] ) var cond firstcond hasAndCond
				| TAB (id, _, _,_,_) -> 
					if  List.mem id listOfCovariantVar  then (false,CLASNODEF,  [], false) 
					else  getIncOfInstListCOV  vector  listOfCovariantVar nextInst completList interval  (List.append previous [firstInst] ) var cond firstcond hasAndCond
				| MEMASSIGN (id, _, _,_,_) -> 
					if  List.mem id listOfCovariantVar  then  (false,CLASNODEF,  [], false) 
					else getIncOfInstListCOV  vector  listOfCovariantVar nextInst completList interval  (List.append previous [firstInst] ) var cond firstcond hasAndCond
				| BEGIN liste ->
					(* attention on peut avoir non pas deux ensembles mais des cooef qui sont des set...*)
					let (correct1, inc1,after,out1) = getIncOfInstListCOV  vector  listOfCovariantVar liste completList interval   previous var cond firstcond hasAndCond in


					if correct1 then
						(match inc1 with 
							CLASNODEF -> (false,CLASNODEF, [], false)
							|CLASNOINC->
								getIncOfInstListCOV  vector  listOfCovariantVar nextInst completList interval  (List.append after [firstInst] ) var cond firstcond hasAndCond
							|ARITHGEO (q1, k1,corr1)->
								let (correct, inc, a2,out2) = 
									getIncOfInstListCOV vector listOfCovariantVar nextInst completList interval (List.append after [firstInst]) var cond firstcond hasAndCond in
								(* en fait les instructions qui suivent la dernière affectation ??? voir*)
								if correct  then
									( match inc with
										CLASNODEF -> (false,CLASNODEF, [], false)
										|CLASNOINC->(correct1, inc1,List.append after [firstInst] ,out1) 
										| ARITHGEO (q2, k2,corr2)-> (*q_{1} *q_{2} X + k_{10}*q_{2}+ k_{20}*) 
											let q = mulcov  q1 q2 in
											let k = addcov (mulcov  k1 q2) k2 in
											if  isConstantForm   q1 then
											begin
												(match k with
													SINGLE f  ->  
														let ncond =  
															calculer (EXP(remplacerValPar var (CONSTANT (RCONST_FLOAT f)) cond)) !infoaffichNull [] (-1) in
														let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
																		|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
														if isExecutedV = false then (true, ARITHGEO (   q1,  k , max corr1 corr2), after , true)
														else (false,CLASNODEF, [],false)
													|_->(false,CLASNODEF, [],false))
															 
											end
											else 	if  isConstantForm q2 then
													begin
													(match k2 with
														SINGLE f  ->  
															Printf.printf " VAR  member\n"  ;	
															let ncond =  calculer (EXP(remplacerValPar  var (CONSTANT (RCONST_FLOAT f)) cond))  !infoaffichNull [] (-1) in
															let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
																		|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
															if isExecutedV = false then (true, ARITHGEO (  q2,  k2 ,max corr1 corr2), after , true)
															else (false,CLASNODEF, [],false)
														|_->(false,CLASNODEF, [],false))
																	 
													end
 													else  (true, ARITHGEO (q, k,max corr1 corr2),after, false )(* arithmético géométrique*)
											)
										else (false,CLASNODEF, [], false))
					else (false,CLASNODEF, [], false)
				| IFVF (exp, i1, i2) ->

					let trueinterval = restictIntervalFromCond (getCondition(expVaToExp  exp)) var  interval in
					let falseinterval = restictIntervalFromCond ( UNARY (NOT,(getCondition(expVaToExp  exp)))) var  interval in
 	
					let (correct1, inc1,after,out1) = 
						getIncOfInstListCOV  vector  listOfCovariantVar  [i1] completList trueinterval   previous var cond firstcond hasAndCond in

(* when we have If instruction we can alse have boolean condition that leads to kit the loop*)
(*let isOkExe =   quitBecauseOfBoolean     (List.append previous [i1] ) nextInst  firstcond  in hasAndCond*)
					(*let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst cond hasAndCond in*)
					if correct1 = false  then 
					(	if hasAndCond then
							getIncOfAND vector  listOfCovariantVar  i1 i2 completList falseinterval  nextInst firstInst interval previous  var cond firstcond hasAndCond  after exp
						else (false,CLASNODEF, [], false)
					)
					else
					(	match inc1 with
						CLASNODEF -> 
							(	if hasAndCond then
									getIncOfAND vector  listOfCovariantVar  i1 i2 completList falseinterval  nextInst firstInst interval previous  var cond firstcond hasAndCond after exp
								else (false,CLASNODEF, [], false)
							)
							 
						|CLASNOINC-> 
							if hasAndCond then
								getIncOfAND vector  listOfCovariantVar  i1 i2 completList falseinterval  nextInst firstInst interval previous  var cond firstcond hasAndCond after exp
							else
							begin
								let (correct2, inc2, after2,_) = 
									getIncOfInstListCOV  vector  listOfCovariantVar  [i2] completList falseinterval   previous var cond firstcond hasAndCond in
								if correct2 = false then (false,CLASNODEF, [], false)
								else 
								( 
									match inc2 with  CLASNODEF -> (false,CLASNODEF, [], false)
									| CLASNOINC  -> (* covariant var are neither assigned into true or false alternate = depend only on next instructions*)
									   getIncOfInstListCOV vector listOfCovariantVar nextInst completList interval (List.append previous [firstInst]) var cond firstcond hasAndCond
									| ARITHGEO (q2, k2,corr2)-> (* change only into one of the alternate Printf.printf "TRUE ARITHGEO 0  quitBecauseOfBoolean \n" ;*)
										let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst cond hasAndCond firstcond in
										if isOkExe1  then (*the constant leads to quit the loop *)
																	( 	(true, ARITHGEO (  q2,  k2, corr2),  [IFVF (exp, BEGIN after, BEGIN after2)] , true))
										else (false,CLASNODEF, [],false)

								  )
							end

								
							| ARITHGEO (q1, k1,corr1)-> 		
								 let (correct2, inc2, after2, out2) = 
									getIncOfInstListCOV  vector  listOfCovariantVar  [i2] completList falseinterval   previous  var cond firstcond hasAndCond in
								 if correct2 = false  then (false,CLASNODEF, [],false)
								 else 
									(* j'ai un peu de pb avec le contenu de after je vais le considérer vide dans un premier temps c'est pessimiste*)
								(
									 match inc2 with
										CLASNODEF -> (false,CLASNODEF, [], false)
										|CLASNOINC(* change only into one of the alternate*)->  
											let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i2]) nextInst cond hasAndCond firstcond in
											if isOkExe1  then (*the constant leads to quit the loop *)
																( 	(true, ARITHGEO (  q1,  k1, corr1),  [IFVF (exp, BEGIN after, BEGIN after2)] , true))
											else (false,CLASNODEF, [],false)
										| ARITHGEO (q2, k2,corr2)->
											if (*q1 = q2 =1 :arithmetic form*) isArithmeticForm q1 q2 then  
												if (* k_{10} * k_{20} >0 (the two functiosn are either strictly increasing or decreasing) *)
													haveTheSameComportmentAri k1 k2
												then (* the resulting increment may be lower estimated by (+, SET(k_{10}, k_{20})) *)
													(true, ARITHGEO (  q2, compose k1 k2,max corr1 corr2),  [IFVF (exp, BEGIN after, BEGIN after2)] ,false) 
													(* arithmético géométrique*)
												else   (false,CLASNODEF, [], false)  
											else 
											(
												let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst cond hasAndCond firstcond in
												let isOkExe2 = quitBecauseOfBoolean  (List.append previous [i2]) nextInst cond hasAndCond firstcond in
												if  isConstantForm q1 || isOkExe1||  isConstantForm q2 || isOkExe2(*q1 =0 or  q2 =0 :one is constant*) then
												begin  
													if  isConstantForm q1  then  	(* ici il faut en fait évaluer la valeur de la variable en sortie de boucle quand on  passe dans ce cas*) 
													begin
	 													match k1 with SINGLE kval ->

															let isOkExe =   isConstantFormLeadingToQuitTheLoop     (List.append previous [i1] ) nextInst  var  cond    !covvarAssign  listOfCovariantVar vector   in
															(*let ncond =  
																calculer (EXP(remplacerValPar var (CONSTANT (RCONST_FLOAT kval)) cond)) !infoaffichNull [] (-1) in
															let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
																	|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
														Printf.printf "TRUE ARITHGEO 1  bool1 %b bool2 %b \n" isOkExe isExecutedV;	
															if isExecutedV = false then *)
															if isOkExe then (*the constant leads to quit the loop *)
																( 	(true, ARITHGEO (  q2,  k2,max corr1 corr2),  [IFVF (exp, BEGIN after, BEGIN after2)] , true))
															else (false,CLASNODEF, [],false)
														|_->  (false,CLASNODEF, [],false)
													end
													else 	
														if   isOkExe1 then	( 	(true, ARITHGEO (  q2,  k2,max corr1 corr2),  [IFVF (exp, BEGIN after, BEGIN after2)] , true)) 
														else (*isConstantForm q2 *)
															if  isConstantForm q2  then 
															begin
																match k2 with SINGLE kval -> (* ici il faut en fait évaluer la valeur de la variable en sortie de boucle quand on  passe dans ce cas*) 
																	let isOkExe =   isConstantFormLeadingToQuitTheLoop       (List.append previous [i2] )  nextInst  var  cond    !covvarAssign listOfCovariantVar vector    in
																	(*let ncond =  
																		calculer (EXP(remplacerValPar var (CONSTANT (RCONST_FLOAT kval)) cond)) !infoaffichNull [] (-1) in
																	let isExecutedV = (match ncond with Boolean(b)	->  if b = false then false  else true 
																			|_->if estDefExp ncond then if estNul ncond then false else true else true) in	
																		Printf.printf "TRUE ARITHGEO 2  bool1 %b bool2 %b \n" isOkExe isExecutedV;	
																	if isExecutedV = false then *)
																	if isOkExe then
																		(true, ARITHGEO (  q1,  k1,max corr1 corr2),  [IFVF (exp, BEGIN after, BEGIN after2)] , true)
																	else (false,CLASNODEF, [],false)
																|_->  (false,CLASNODEF, [],false)
															end
															else (true, ARITHGEO (  q1,  k1,max corr1 corr2),  [IFVF (exp, BEGIN after, BEGIN after2)] , true)
												end
												else 
													if isGeometricForm k1 k2   then														   
														if   haveTheSameComportmentGeo q1 q2 then (* ... (+, SET(k_{10}, k_{20})) arithmético géo *)
															(true, ARITHGEO (  compose q1 q2,  k1,max corr1 corr2),  [IFVF (exp, BEGIN after, BEGIN after2)] ,false)
														else (false,CLASNODEF, [], false) 
													else 	  
														if    (sup q1 1.0 && sup q1 1.0 && sup k1 0.0 && sup k2 0.0) ||
															(inf q1 1.0 && inf q1 1.0 && inf k1 0.0 && inf k2 0.0)
														then (true, ARITHGEO (  compose q1 q2, compose k1 k2,max corr1 corr2),  
																	[IFVF (exp, BEGIN after, BEGIN after2)] ,false)
														else (false,CLASNODEF, [], false)	
														 
											))
										)

				| IFV ( exp, i1) ->  
					let trueinterval = restictIntervalFromCond (getCondition(expVaToExp  exp)) var  interval in
				 

					let (correct1, inc1,_,_) = getIncOfInstListCOV  vector  listOfCovariantVar  [i1] completList trueinterval   previous  var cond firstcond hasAndCond in

					if correct1 = false  then 
					(
						if hasAndCond then
						begin
							let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst cond hasAndCond firstcond in
							if isOkExe1 then
		 						getIncOfInstListCOV  vector  listOfCovariantVar  nextInst completList interval (List.append previous [firstInst] ) var cond firstcond hasAndCond
							else  (false,CLASNODEF, [], false)
						end 
						else (false,CLASNODEF, [], false)
					)
					else
						(match inc1 with
							CLASNODEF -> 			
										if hasAndCond then
										begin
											let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst cond hasAndCond firstcond in
											if isOkExe1 then
						 						getIncOfInstListCOV  vector  listOfCovariantVar  nextInst completList interval (List.append previous [firstInst] ) var cond firstcond hasAndCond
											else  (false,CLASNODEF, [], false)
										end 
										else (false,CLASNODEF, [], false)
							|CLASNOINC->  getIncOfInstListCOV  vector  listOfCovariantVar nextInst completList interval  (List.append previous [firstInst] ) var cond firstcond hasAndCond
										   
							| ARITHGEO (q1, k1,_)->  (false,CLASNODEF, [], false)(* if constant some times it can be analysed*)
						)

(*les deux dernier FOR et CALL doivent être revus*)
				

				| FORV (num,id, _, _, _, _, body,_)->        
					let nbItMin = if  existAssosExactLoopInit  num then getAssosExactLoopInit num  else 0 in
					let (indirect1, inc1, var1, before1,incifexe) = (extractIncOfLoop var [body] id nbItMin (*nbItMin_{numLoop}*)completList) (List.append previous [firstInst] ) firstcond hasAndCond in
				 
					if incifexe =   NODEFINC then  (false,CLASNODEF, [], false)
					else
						if indirect1 = false && inc1 =  NOINC then 
							 getIncOfInstListCOV  vector  listOfCovariantVar nextInst completList interval  (List.append previous [firstInst] )  var cond firstcond hasAndCond
						else (false,CLASNODEF, [], false)


				| APPEL (_,_,name,s,c,_,_,_)->
					let (indirect1, inc1, var1, before1) = (getIncOfCall var name c completList s (List.append previous [firstInst] )  ) in
					if inc1 =   NODEFINC then  (false,CLASNODEF, [], false)
					else
						if indirect1 = false && inc1 =   NOINC then   
							 getIncOfInstListCOV  vector  listOfCovariantVar nextInst completList interval  (List.append previous [firstInst] ) var cond firstcond hasAndCond
						else  (false,CLASNODEF, [], false) 
		 

(* covariance *)
 

and  getIncOfAND vector  listOfCovariantVar  i1 i2 completList falseinterval  nextInst firstInst interval previous  var cond firstcond hasAndCond after exp=
		let isOkExe1 = quitBecauseOfBoolean  (List.append previous [i1]) nextInst cond hasAndCond firstcond in
		if isOkExe1 then
		begin
			let (correct2, inc2, after2, out2) = 
						getIncOfInstListCOV  vector  listOfCovariantVar  [i2] completList falseinterval   previous  var cond firstcond hasAndCond in
			let isOkExe2 = quitBecauseOfBoolean  (List.append previous [i2]) nextInst cond hasAndCond firstcond in

			if correct2 = false && isOkExe2 = false then (false,CLASNODEF, [], false)
			else
				if  isOkExe2 then getIncOfInstListCOV  vector  listOfCovariantVar  nextInst completList interval (List.append previous [firstInst] ) var cond firstcond hasAndCond
				else 
					if correct2 then (	(true, inc2,  [IFVF (exp, BEGIN after, BEGIN after2)] , true))
				 	else (false,CLASNODEF, [], false)
		end
		else (false,CLASNODEF, [], false)

(*if isOkExe1  then (*the constant leads to quit the loop *)
																( 	(true, inc2,  [IFVF (exp, BEGIN after, BEGIN after2)] , true))
											else (false,CLASNODEF, [],false)*)


and getcovariance  vector  listOfCovariantVar  inst completList cond previous x firstcond hasAndCond= (*return a new inst list where each assignment of var of listOfCovariantVar are completed by instructiontoadd *)

		let pred = !getOnlyBoolAssignment in
 		getOnlyBoolAssignment := false;
		setOnlyIncrement true;
		isMultiInc := false; 

		if intersection listOfCovariantVar !myChangeVar != [] then
		( 
			 
			expressionIncFor :=NOTHING;
			opEstPlus :=false;
			(false,NODEFINC,x, false, true, NOTHING) 

		)
		else
		(

				let interval  = restictIntervalFromCond cond x  (INTERVALLE(INFINI,INFINI)) in
				let (ok, inc1,l, out ) = getIncOfInstListCOV  vector  listOfCovariantVar  inst completList interval previous x cond firstcond hasAndCond in

				if ok  then  
				begin
					match inc1 with
					
					CLASNODEF ->
							expressionIncFor :=NOTHING;
							opEstPlus :=false;
							(false,NODEFINC,x, false, true, NOTHING) 
					| CLASNOINC-> 
							expressionIncFor :=NOTHING;
							opEstPlus :=false;
							(false,NOINC,x, false, true, NOTHING) 
					| ARITHGEO (q1, k1,corr1)->  
							if  isConstantForm q1 then 
							begin
								expressionIncFor :=NOTHING;
								opEstPlus :=false;
								(false,NODEFINC,x, false, true, NOTHING) 
							end
							else
							begin
								let (simpleCoefArithGeo, bsurUnMoina) = issimpleCoefArithGeo q1 (addcov k1 (SINGLE(corr1))) interval x in

								let (inc, var, cte)  = isArithmeticRes q1 k1 interval x in
								opEstPlus := getIsAddInc inc;
								expressionIncFor :=  getIncValue inc ;
								setOnlyIncrement false;
								getOnlyBoolAssignment := pred;
								if simpleCoefArithGeo then 
								begin
								(*	Printf.printf "TRUE ARITHGEO  cte%f \n" bsurUnMoina ;		*)					  	
									(out,inc,var, true , true, CONSTANT(RCONST_FLOAT bsurUnMoina))
								end
								else
								begin 
									 (*Printf.printf " ARITHGEO  cte%f \n" cte ;*)
						 			if bsurUnMoina = 0.0 then ( out,inc,var, (cte = 0.0) = false , true, NOTHING) else (false,NODEFINC,x, false, true, NOTHING) 
								end
							end
			
				end 
				else
				begin
					expressionIncFor :=NOTHING;
					opEstPlus :=false;
					(false,NODEFINC,x, false, true, NOTHING) 

				end
				
		)


(*			let (croissant,borneInf,borneSup,operateur,multiple,nnvar,nexp)= 
				rechercheConditionBinary (VARIABLE(varCond1)) varCond1 nop exp1 exp2 (union l listeVDeBoucle) avant dans cte t comp lv (union l listeVDeBoucle)  inst in 

getcovariance  vector  listOfCovariantVar  inst completList cond previous x =
*)


and getLoopVarIncOrCov v inst firstcond hasAndCond =


(*if !covvarAssign=MULTIPLE then *)  getLoopVarInc v inst (*else ...*)firstcond hasAndCond




