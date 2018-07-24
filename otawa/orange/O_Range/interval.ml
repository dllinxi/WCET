(* interval -- use Frontc CAML  Tools and computation of intervals used to resolve increment value. 
**
** Project:	O_Range
** File:	interval.ml
** Version:	1.1
** Date:	2009
** Author:	Marianne de Michiel
*)

open Cabs
open Frontc

let version = "interval.ml Marianne de Michiel"

open Cprint
open Cexptostr
open Cvarabs
open Cvariables
open Constante
open Util
 
type boundType =	
		BINCLUDE  of float 
	| 	BNOTINCLUDE of float 
	|	INFINI  
type intervalType = INTERVALLE of boundType * boundType | EMPTY 

let getInfoOfCondBINARY el er var =

	let val1 = calculer (EXP (BINARY(SUB,el,er))) !infoaffichNull [] 1 in 

		if (estAffine var val1)   then 
		begin
				let (a,b) = calculaetbAffineForne var val1 in	
				let (var1, var2) = (evalexpression a , evalexpression b) in
				let isLeft = if estStricPositif var1 then true else false in
 				let bound = if (estNul var1 )= false then calculer (EXP(expressionEvalueeToExpression (Quot(Diff(ConstFloat("0.0"), var2),var1))))!infoaffichNull [] 1 else NOCOMP in
				(true, isLeft,bound)

		end
		else (false, false,ConstInt("0"))





(* in case of the result is composed from more than on intreval the result will be the biggest interval comtaining the real resuly *)







let rec inter firstInterval secondInterval =
match firstInterval with 
	  EMPTY -> EMPTY
	|INTERVALLE(INFINI,INFINI)-> secondInterval
	|INTERVALLE(INFINI,BINCLUDE fs)->
			(
				match secondInterval with 
				EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> INTERVALLE(INFINI,BINCLUDE (min ss fs))
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> if fs < ss then firstInterval else secondInterval
				|INTERVALLE(BINCLUDE si,INFINI)->if si<= fs then INTERVALLE(BINCLUDE si,BINCLUDE fs) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,INFINI)->if fs > si then INTERVALLE(BNOTINCLUDE si,BINCLUDE fs) else EMPTY
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> if fs >= si then INTERVALLE(BINCLUDE si,BINCLUDE (min ss fs)) else EMPTY
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
						if fs >= si then if fs < ss then INTERVALLE(BINCLUDE si,BINCLUDE fs) 
						else INTERVALLE(BINCLUDE si,BNOTINCLUDE ss) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> if fs > si then  INTERVALLE(BNOTINCLUDE si,BINCLUDE (min ss fs)) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
						if fs > si then if fs < ss then INTERVALLE(BNOTINCLUDE si,BINCLUDE fs) 
						else INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss) else EMPTY
			)
	|INTERVALLE(INFINI,BNOTINCLUDE fs)->
			(
				match secondInterval with 
				  EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)->  if fs <= ss then  firstInterval else secondInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> INTERVALLE(INFINI,BNOTINCLUDE (min ss fs))
				|INTERVALLE(BINCLUDE si,INFINI)->if si< fs then INTERVALLE(BINCLUDE si,BNOTINCLUDE fs) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,INFINI)->
						if fs > si then INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE fs) else EMPTY				
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> 
						if fs > si then if fs <= ss then INTERVALLE(BINCLUDE si,BNOTINCLUDE fs) else INTERVALLE(BINCLUDE si,BINCLUDE ss) else EMPTY  
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)->   
						if fs > si then INTERVALLE(BINCLUDE si,BNOTINCLUDE (min ss fs)) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
						if fs > si then 
							if fs <= ss then INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE fs) else INTERVALLE(BNOTINCLUDE si,BINCLUDE ss) 
						else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->  
						if fs > si then  INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE (min ss fs)) else EMPTY
			)
	|INTERVALLE(BINCLUDE fi,INFINI)->
			(
				match secondInterval with EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> inter secondInterval firstInterval  
				|INTERVALLE(INFINI,BNOTINCLUDE ss)->   inter secondInterval firstInterval 
				|INTERVALLE(BINCLUDE si,INFINI)->  INTERVALLE(BINCLUDE   (max si fi),INFINI)
				|INTERVALLE(BNOTINCLUDE si,INFINI)->
						if fi <= si then secondInterval else firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> if fi <= ss then   INTERVALLE(BINCLUDE (max si fi),BINCLUDE ss)   else EMPTY 
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)->   if fi < ss then INTERVALLE(BINCLUDE (max si fi),BNOTINCLUDE ss)  else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
						if fi <= ss  then if fi <= si then INTERVALLE(BNOTINCLUDE si,BINCLUDE ss) else INTERVALLE(BINCLUDE fi,BINCLUDE ss) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->  
						if fi < ss  then 
							if fi <= si then INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss) else INTERVALLE(BINCLUDE fi,BINCLUDE ss) 
						else EMPTY
			)

	|INTERVALLE(BNOTINCLUDE fi,INFINI)->
			(
				match secondInterval with EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> inter secondInterval firstInterval  
				|INTERVALLE(INFINI,BNOTINCLUDE ss)->   inter secondInterval firstInterval 
				|INTERVALLE(BINCLUDE si,INFINI)->  inter secondInterval firstInterval 
				|INTERVALLE(BNOTINCLUDE si,INFINI)-> INTERVALLE(BNOTINCLUDE   (max si fi),INFINI)
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> 
						if fi < ss  then if fi < si then INTERVALLE(BINCLUDE si,BINCLUDE ss) else INTERVALLE(BNOTINCLUDE fi,BINCLUDE ss) else EMPTY
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
						if fi < ss then if fi < si then INTERVALLE(BINCLUDE si,BNOTINCLUDE ss) else INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE ss) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> if fi < ss then   INTERVALLE(BNOTINCLUDE (max si fi),BINCLUDE ss)   else EMPTY 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->   if fi < ss then   INTERVALLE(BNOTINCLUDE (max si fi),BNOTINCLUDE ss)   else EMPTY 
			)


	|INTERVALLE(BINCLUDE fi,BINCLUDE fs)->
			(
				match secondInterval with EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> 
					let newi = max fi si in
					let news = min fs ss in
					if news >= newi then INTERVALLE(BINCLUDE newi,BINCLUDE news) else EMPTY
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
					let newi = max fi si in
					let news = min fs ss in
					if news = ss then if news > newi then INTERVALLE(BINCLUDE newi,BNOTINCLUDE news) else EMPTY
					else if news >= newi then INTERVALLE(BINCLUDE newi,BINCLUDE news) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					let newi = max fi si in
					let news = min fs ss in
					if newi = si then if news > newi then INTERVALLE(BNOTINCLUDE newi,BINCLUDE news) else EMPTY
					else if news >= newi then INTERVALLE(BINCLUDE newi,BINCLUDE news) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					let newi = max fi si in
					let news = min fs ss in
					if news = ss then 
						if news > newi then 
							if newi = si then INTERVALLE (BNOTINCLUDE newi,BNOTINCLUDE news) else INTERVALLE(BINCLUDE newi,BNOTINCLUDE news) 
						else EMPTY
					else if newi = si then if news > newi then INTERVALLE(BNOTINCLUDE newi,BINCLUDE news) else EMPTY
						 else if news >= newi then INTERVALLE(BINCLUDE newi,BINCLUDE news) else EMPTY
			)
	|INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)->
		(
				match secondInterval with EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
					let newi = max fi si in
					let news = min fs ss in
					if news > newi then INTERVALLE(BINCLUDE newi,BNOTINCLUDE news) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					let newi = max fi si in
					let news = min fs ss in
					if newi = si then 
						if news = fs then 
							if news > newi then INTERVALLE(BNOTINCLUDE newi,BNOTINCLUDE news) else EMPTY
						else 
							if news > newi then INTERVALLE(BNOTINCLUDE newi,BINCLUDE news) else EMPTY
					else (* min = BINCLUDE fi *)
						if news = fs then if news > newi then INTERVALLE(BINCLUDE newi,BNOTINCLUDE news) else EMPTY
						else (* max = BINCLUDE ss *)
							if news >= newi then INTERVALLE(BINCLUDE newi,BINCLUDE news) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					let newi = max fi si in
					let news = min fs ss in
					if newi = si then 
						if news > newi then INTERVALLE(BNOTINCLUDE newi,BNOTINCLUDE news) else EMPTY
					else (* min = BINCLUDE fi *)
						if news > newi then INTERVALLE(BINCLUDE newi,BNOTINCLUDE news) else EMPTY
						 
			)
	|INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs)->
	(
				match secondInterval with EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					let newi = max fi si in
					let news = min fs ss in
					if news > newi then INTERVALLE(BNOTINCLUDE newi,BINCLUDE news) else EMPTY
						 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					let newi = max fi si in
					let news = min fs ss in
					if news = ss then 
						if news > newi then INTERVALLE(BNOTINCLUDE newi,BNOTINCLUDE news) else EMPTY
					else 
						if news >= newi then INTERVALLE(BNOTINCLUDE newi,BINCLUDE news) else EMPTY
						 
			)
	|INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)->
		(
				match secondInterval with EMPTY -> EMPTY
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> inter  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> inter  secondInterval firstInterval
						 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					let newi = max fi si in
					let news = min fs ss in
					if news > newi then INTERVALLE(BNOTINCLUDE newi,BNOTINCLUDE news) else EMPTY
			)



let rec union firstInterval secondInterval = 
match firstInterval with 
	  EMPTY -> secondInterval
	|INTERVALLE(INFINI,INFINI)->firstInterval
	|INTERVALLE(INFINI,BINCLUDE fs)->
			(
				match secondInterval with 
				EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)-> INTERVALLE(INFINI,BINCLUDE (max ss fs))
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> if fs >= ss then firstInterval else secondInterval
				|INTERVALLE(BINCLUDE si,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(BNOTINCLUDE si,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->  INTERVALLE(INFINI,BINCLUDE (max ss fs))
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> if ss > fs then INTERVALLE(INFINI,BNOTINCLUDE ss) else INTERVALLE(INFINI,BINCLUDE fs)
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)->  INTERVALLE(INFINI,BINCLUDE (max ss fs))
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)-> if ss > fs then INTERVALLE(INFINI,BNOTINCLUDE ss) else INTERVALLE(INFINI,BINCLUDE fs)
			)
	|INTERVALLE(INFINI,BNOTINCLUDE fs)->
			(
				match secondInterval with 
				  EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)->if fs > ss then firstInterval else secondInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> if fs >= ss then firstInterval else secondInterval
				|INTERVALLE(BINCLUDE si,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(BNOTINCLUDE si,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->  if fs > ss then INTERVALLE(INFINI,BNOTINCLUDE fs) else INTERVALLE(INFINI,BINCLUDE ss)
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)->   INTERVALLE(INFINI,BNOTINCLUDE (max ss fs))  
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)->   if fs > ss then INTERVALLE(INFINI,BNOTINCLUDE fs) else INTERVALLE(INFINI,BINCLUDE ss)
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)-> INTERVALLE(INFINI,BNOTINCLUDE (max ss fs))  
			)
	|INTERVALLE(BINCLUDE fi,INFINI)->
			(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BNOTINCLUDE ss)->   INTERVALLE(INFINI,INFINI)
				|INTERVALLE(BINCLUDE si,INFINI)-> INTERVALLE (BINCLUDE   (min si fi),INFINI)
				|INTERVALLE(BNOTINCLUDE si,INFINI)->if fi > si then secondInterval else firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->  INTERVALLE(BINCLUDE   (min si fi),INFINI)
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)->   INTERVALLE(BINCLUDE   (min si fi),INFINI)
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)->  if fi >= si then INTERVALLE (BNOTINCLUDE si    ,INFINI) else INTERVALLE(BINCLUDE  fi  ,INFINI)
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->  if fi >= si then  INTERVALLE(BNOTINCLUDE si    ,INFINI) else INTERVALLE(BINCLUDE  fi  ,INFINI)
			)

	|INTERVALLE(BNOTINCLUDE fi,INFINI)->
			(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BNOTINCLUDE ss)->  INTERVALLE (INFINI,INFINI) 
				|INTERVALLE(BINCLUDE si,INFINI)->  union secondInterval firstInterval 
				|INTERVALLE(BNOTINCLUDE si,INFINI)-> INTERVALLE(BNOTINCLUDE   (min si fi),INFINI)
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->  if fi < si then INTERVALLE(BNOTINCLUDE   fi,INFINI) else INTERVALLE(BINCLUDE si,INFINI)
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)->  if fi < si then INTERVALLE(BNOTINCLUDE   fi,INFINI) else INTERVALLE(BINCLUDE si,INFINI)
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> INTERVALLE(BNOTINCLUDE   (min si fi),INFINI)
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->   INTERVALLE(BNOTINCLUDE  ( min si fi),INFINI)
			)


	|INTERVALLE(BINCLUDE fi,BINCLUDE fs)->
			(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->   INTERVALLE(BINCLUDE (min fi si),BINCLUDE (max fs ss)) 
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
					if ss > fi then   INTERVALLE(BINCLUDE  (min fi si),BNOTINCLUDE ss) 	else  INTERVALLE (BINCLUDE ( min fi si),BINCLUDE fs)  
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					if fi > si then INTERVALLE(BNOTINCLUDE si,BINCLUDE (max fs ss))  
					else INTERVALLE(BINCLUDE fi,BINCLUDE (max fs ss))  
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					 
					if ss > fs then  
							if fi > si then INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss) else INTERVALLE(BINCLUDE fi,BNOTINCLUDE ss) 
					else if fi > si then   INTERVALLE(BNOTINCLUDE si,BINCLUDE fs)   else   INTERVALLE(BINCLUDE fi,BINCLUDE fs) 
			)
	|INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)->
		(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)->  INTERVALLE (BINCLUDE (min fi si) ,BNOTINCLUDE (max fs ss) )  
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					 
				 
					if fi > si then 
						if ss < fs then 
							 INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE fs) 
						else 
							 INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)  
					else (* min = BINCLUDE fi *)
						if ss < fs then INTERVALLE (BINCLUDE fi,BNOTINCLUDE fs) 
						else (* max = BINCLUDE ss *)
							 INTERVALLE (BINCLUDE fi,BINCLUDE ss) 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
				 
					 
					if fi > si then
						  INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE (max fs ss))  
					else (* min = BINCLUDE fi *)
						 INTERVALLE(BINCLUDE fi,BNOTINCLUDE (max fs ss)) 
						 
			)
	|INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs)->
	(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)->  
				 INTERVALLE(BNOTINCLUDE (min fi si),BINCLUDE (max fs ss) ) 
						 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
			  
					if fs < ss then 
						 INTERVALLE(BNOTINCLUDE (min fi si),BNOTINCLUDE ss)  
					else 
						 INTERVALLE (BNOTINCLUDE (min fi si),BINCLUDE fs) 
						 
			)
	|INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)->
		(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> INTERVALLE(INFINI,INFINI)
				|INTERVALLE(INFINI,BINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,INFINI)->union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> union  secondInterval firstInterval
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> union  secondInterval firstInterval
						 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)-> 
				 INTERVALLE(BNOTINCLUDE (min fi si) ,BNOTINCLUDE (max fs ss))  
			)


let rec diff firstInterval secondInterval = (* secondInterval must be included into firstInterval*)
match firstInterval with 
	  EMPTY -> EMPTY
	|INTERVALLE(INFINI,INFINI)-> 
			(
				match secondInterval with 
				EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(INFINI,BINCLUDE ss)-> INTERVALLE(BNOTINCLUDE ss, INFINI)
				|INTERVALLE(INFINI,BNOTINCLUDE ss)-> INTERVALLE(BINCLUDE ss, INFINI)
				|INTERVALLE(BINCLUDE si,INFINI)-> INTERVALLE( INFINI,BNOTINCLUDE si)
				|INTERVALLE(BNOTINCLUDE si,INFINI)-> INTERVALLE( INFINI,BINCLUDE si)
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->  firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> firstInterval
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)->  firstInterval
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)-> firstInterval
			)

	|INTERVALLE(INFINI,BINCLUDE fs)->
			(
				match secondInterval with 
				EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(BINCLUDE si,_)-> INTERVALLE( INFINI,BNOTINCLUDE si)
				|INTERVALLE(BNOTINCLUDE si,_)-> INTERVALLE( INFINI,BINCLUDE si)
				|INTERVALLE(_,BINCLUDE ss)-> if ss >= fs then EMPTY else INTERVALLE(BNOTINCLUDE ss, BINCLUDE fs)
				|INTERVALLE(_,BNOTINCLUDE ss)->if ss > fs then EMPTY else INTERVALLE(BINCLUDE ss, BINCLUDE fs)
			)
	|INTERVALLE(INFINI,BNOTINCLUDE fs)->
			(
				match secondInterval with 
				  EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(BINCLUDE si,_)-> INTERVALLE( INFINI,BNOTINCLUDE si)
				|INTERVALLE(BNOTINCLUDE si,_)-> INTERVALLE( INFINI,BINCLUDE si)

				|INTERVALLE(_,BINCLUDE ss)->if ss >= fs then EMPTY else INTERVALLE(BNOTINCLUDE ss, BNOTINCLUDE fs)
				|INTERVALLE(_,BNOTINCLUDE ss)-> if ss >= fs then EMPTY else INTERVALLE(BINCLUDE ss, BNOTINCLUDE fs)
			)
	|INTERVALLE(BINCLUDE fi,INFINI)->
			(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(_,BINCLUDE ss)-> INTERVALLE(BNOTINCLUDE ss, INFINI)
				|INTERVALLE(_,BNOTINCLUDE ss)-> INTERVALLE  (BINCLUDE ss, INFINI)
				|INTERVALLE(BINCLUDE si,_)->  if si > fi then INTERVALLE(BINCLUDE fi,BNOTINCLUDE si) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,_)-> if si >= fi then INTERVALLE(BINCLUDE fi,BINCLUDE si) else EMPTY
				
			)

	|INTERVALLE(BNOTINCLUDE fi,INFINI)->
			(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(INFINI,INFINI)-> firstInterval
				|INTERVALLE(_,BINCLUDE ss)-> INTERVALLE(BNOTINCLUDE ss, INFINI)
				|INTERVALLE(_,BNOTINCLUDE ss)-> INTERVALLE   (BINCLUDE ss, INFINI)
				|INTERVALLE(BINCLUDE si,_)->  if si > fi then INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE si) else EMPTY
				|INTERVALLE(BNOTINCLUDE si,_)-> if si > fi then INTERVALLE(BNOTINCLUDE fi,BINCLUDE si) else EMPTY
				
			)


	|INTERVALLE(BINCLUDE fi,BINCLUDE fs)->
			(
				match secondInterval with EMPTY -> firstInterval
				
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->   
					if si != fi && fs != ss then firstInterval
					else
						if si = fi then if ss = fs then EMPTY else INTERVALLE(BNOTINCLUDE ss, BINCLUDE fs) 
						else  if fi = si then EMPTY else  INTERVALLE(BINCLUDE fi, BNOTINCLUDE si)
					
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
					if si != fi then firstInterval
					else INTERVALLE(BINCLUDE ss, BINCLUDE fs)  
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					if   fs != ss then firstInterval
					else  INTERVALLE(BINCLUDE fi, BINCLUDE si)
					
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)-> firstInterval
				|_-> firstInterval
			)
	|INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)->
		(
				match secondInterval with EMPTY -> firstInterval
				
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->   
					if si != fi then firstInterval
					else if ss = fs then EMPTY else INTERVALLE(BNOTINCLUDE ss, BINCLUDE fs)  
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
					if si != fi && fs != ss then firstInterval
					else
						if si = fi then if ss = fs then EMPTY else  INTERVALLE(BINCLUDE ss, BNOTINCLUDE fs) 
						else  if fi = si then EMPTY else  INTERVALLE(BINCLUDE fi, BNOTINCLUDE si)
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> firstInterval 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					if   fs != ss then firstInterval
					else  INTERVALLE(BINCLUDE fi, BINCLUDE si)
				|_-> firstInterval
			)
	|INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs)->
	(
				match secondInterval with EMPTY -> firstInterval
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->  
					if   fs != ss then firstInterval
					else if fi = si then EMPTY else  INTERVALLE(BNOTINCLUDE fi, BNOTINCLUDE si)

				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> firstInterval
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					if si != fi && fs != ss then firstInterval
					else
					begin
						if si = fi then 
							 if ss = fs then EMPTY else INTERVALLE(BNOTINCLUDE ss, BINCLUDE fs) 
						else 
							 if fi = si then EMPTY else  INTERVALLE(BNOTINCLUDE fi, BINCLUDE si)
					end
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					if si != fi then firstInterval
					else  INTERVALLE(BINCLUDE ss, BINCLUDE fs)  
	
			|_-> firstInterval
			)
	|INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)->
		(
				match secondInterval with EMPTY -> firstInterval
				
				|INTERVALLE(BINCLUDE si,BINCLUDE ss)->   firstInterval
				|INTERVALLE(BINCLUDE si,BNOTINCLUDE ss)-> 
					if   fs != ss then firstInterval
					else if fi = si then EMPTY else  INTERVALLE(BNOTINCLUDE fi, BNOTINCLUDE si)
				 
				|INTERVALLE(BNOTINCLUDE si,BINCLUDE ss)-> 
					if si != fi then firstInterval
					else  if ss = fs then EMPTY else INTERVALLE(BNOTINCLUDE ss, BNOTINCLUDE fs)  
				 
				|INTERVALLE(BNOTINCLUDE si,BNOTINCLUDE ss)->
					if si != fi && fs != ss then firstInterval
					else
						if si = fi then  if ss = fs then EMPTY else INTERVALLE(BINCLUDE ss, BNOTINCLUDE fs) 
						else  if fi = si then EMPTY else  INTERVALLE(BNOTINCLUDE fi, BINCLUDE si)
				|_-> firstInterval
			)


(*The inforamtions are isAffine fonction of x (var) and if is affine then is variable on the left part of the binary expression and the affine function is ax+b op 0 then -b/a*)
(*

module Intervalle = Set.Make( FloatOrder ) 
let getDefValue value: floatofset  = 
 (getDefValue value)

module FloatOrder : Set.OrderedType = struct
  type t = float
  let compare = Pervasives.compare
  
end;;*)

let equal   firstInterval secondInterval = 
match firstInterval with 
	  EMPTY ->( match secondInterval with 
				EMPTY -> true
				|_-> false)
	|INTERVALLE(INFINI,INFINI)-> ( match secondInterval with 
				INTERVALLE(INFINI,INFINI) -> true
				|_-> false)
	|INTERVALLE(INFINI,BINCLUDE fs)->
			(
				match secondInterval with 
				 INTERVALLE(INFINI,BINCLUDE ss)-> if ss = fs then true else false |_-> false
				 
			)
	|INTERVALLE(INFINI,BNOTINCLUDE fs)->
			(
				match secondInterval with 
				 INTERVALLE(INFINI,BNOTINCLUDE ss)-> if ss = fs then true else false |_-> false
				 
			)
	|INTERVALLE(BINCLUDE fi,INFINI)->
			(
				match secondInterval with 
				INTERVALLE (BINCLUDE si,INFINI)-> if fi = si then true else false |_-> false
				 
			)

	|INTERVALLE(BNOTINCLUDE fi,INFINI)->
			(
				match secondInterval with 
				INTERVALLE (BNOTINCLUDE si,INFINI)-> if fi = si then true else false |_-> false
				 
			)
	|INTERVALLE(BINCLUDE fi,BINCLUDE fs)->
			(
				match secondInterval with 
				INTERVALLE (BINCLUDE si,BINCLUDE ss)-> if fi = si&& fs =ss then true else false |_-> false
				 
			)
	|INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)->
			(
				match secondInterval with 
				INTERVALLE (BINCLUDE si,BNOTINCLUDE ss)-> if fi = si&& fs =ss then true else false |_-> false
				 		 
			)
	|INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs)->
			(
				match secondInterval with 
				INTERVALLE (BNOTINCLUDE si,BINCLUDE ss)-> if fi = si&& fs =ss then true else false |_-> false
				 		 
			)
	|INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)->
			(
				match secondInterval with 
				INTERVALLE (BNOTINCLUDE si,BNOTINCLUDE ss)-> if fi = si&& fs =ss then true else false |_-> false
				 		 
			) 


let isMember value intervalle =
match intervalle with 
	  EMPTY -> false 
	|INTERVALLE(INFINI,INFINI)->  true 
	|INTERVALLE(INFINI,BINCLUDE fs)-> value <= fs
	|INTERVALLE(INFINI,BNOTINCLUDE fs)->value < fs
	|INTERVALLE(BINCLUDE fi,INFINI)->value >= fi
	|INTERVALLE(BNOTINCLUDE fi,INFINI)->value > fi
	|INTERVALLE(BINCLUDE fi,BINCLUDE fs)-> value >= fi && value <= fs
	|INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)->value >= fi && value < fs
	|INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs)->value > fi && value <= fs
	|INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)->value > fi && value < fs

let remove value intervalle =
match intervalle with 
	  EMPTY -> intervalle 
	|INTERVALLE(INFINI,INFINI)->  intervalle 
	|INTERVALLE(INFINI,BINCLUDE fs)-> if value = fs then INTERVALLE(INFINI,BNOTINCLUDE fs)  else intervalle
	|INTERVALLE(INFINI,BNOTINCLUDE fs)-> intervalle
	|INTERVALLE(BINCLUDE fi,INFINI)-> if value = fi then INTERVALLE(BNOTINCLUDE fi,INFINI)  else intervalle
	|INTERVALLE(BNOTINCLUDE fi,INFINI)-> intervalle
	|INTERVALLE(BINCLUDE fi,BINCLUDE fs)-> if value = fi then INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs) else if value = fs then INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)  else intervalle
	|INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)->if value = fi then INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs) else intervalle
	|INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs)-> if value = fs then INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)  else intervalle
	|INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)->intervalle

let print_interval intervalle =
match intervalle with 
	  EMPTY -> Printf.printf "NEVER\n"
	|INTERVALLE(INFINI,INFINI)->  Printf.printf "ALWAYS\n"
	|INTERVALLE(INFINI,BINCLUDE fs)-> Printf.printf " INTERVAL : <= %f\n" fs
	|INTERVALLE(INFINI,BNOTINCLUDE fs)-> Printf.printf " INTERVAL : < %f\n" fs
	|INTERVALLE(BINCLUDE fi,INFINI)-> Printf.printf " INTERVAL : >= %f\n" fi
	|INTERVALLE(BNOTINCLUDE fi,INFINI)-> Printf.printf " INTERVAL : > %f\n" fi
	|INTERVALLE(BINCLUDE fi,BINCLUDE fs)-> Printf.printf " INTERVAL : %f<= value <= %f\n" fi fs
	|INTERVALLE(BINCLUDE fi,BNOTINCLUDE fs)->Printf.printf " INTERVAL : %f<= value < %f\n" fi fs
	|INTERVALLE(BNOTINCLUDE fi,BINCLUDE fs)-> Printf.printf " INTERVAL : %f< value <= %f\n" fi fs
	|INTERVALLE(BNOTINCLUDE fi,BNOTINCLUDE fs)->Printf.printf " INTERVAL : %f< value < %f\n" fi fs


let getLower intervalle =
match intervalle with 
	  EMPTY -> NOCOMP
	|INTERVALLE(INFINI,_)->  NOCOMP
	|INTERVALLE(BINCLUDE fi,_)->  RConstFloat fi
	|INTERVALLE(BNOTINCLUDE fi,_)->  RConstFloat ( fi  +. 0.000001(*min_float*))

let getUpper intervalle =
match intervalle with 
	  EMPTY -> NOCOMP
	|INTERVALLE(_,INFINI)->  NOCOMP
	|INTERVALLE(_,BINCLUDE fs)-> RConstFloat fs
	|INTERVALLE(_,BNOTINCLUDE fs)->  RConstFloat (fs -. 0.000001(*min_float*))


let rec restictIntervalFromCond exp var  interval =
	match exp with
	NOTHING -> interval
	| UNARY (op, e) ->
		(match op with
		 NOT -> 
			let ensnot = restictIntervalFromCond e var  interval   in  
			if  equal interval ensnot then interval else diff interval ensnot 
		| _ -> interval )
	| BINARY (op, el, er) ->
		(match op with
		  LT ->

			let (isAffine, isLeft, value ) = getInfoOfCondBINARY el er var in
			if isAffine && value != NOCOMP then
			begin 
				 
				if isLeft then
				begin (*var < value*) 
					  
					 if estDefExp value then 
						inter  (INTERVALLE(INFINI,BNOTINCLUDE ( getDefValue  value))) interval  
					 else interval (* interval inter ]-infini, val1[*)
				end
				else
				begin (* value < var*)
					 if estDefExp value then inter (INTERVALLE(BNOTINCLUDE( getDefValue  value),INFINI)) interval  else interval  
				end
			end
			else interval
		| LE -> 
			let (isAffine, isLeft, value) = getInfoOfCondBINARY el er var in
			if isAffine && value != NOCOMP then
			begin
				 
				if isLeft then
				begin (*var <= value*) 
					 if estDefExp value then inter (INTERVALLE(INFINI,BINCLUDE ( getDefValue  value))) interval  else interval  
				end
				else
				begin (* value <= var*)
					 if estDefExp value then inter (INTERVALLE(BINCLUDE( getDefValue  value),INFINI)) interval  else interval  
				end
			end
			else interval
		| GT -> 
			let (isAffine, isLeft, value) = getInfoOfCondBINARY el er var in
			if isAffine && value != NOCOMP then
			begin
		 
				if isLeft then
				begin (*var > value*) 
					 if estDefExp value then inter (INTERVALLE(BNOTINCLUDE( getDefValue  value),INFINI)) interval else interval  
				end
				else
				begin (* value > var*)
					 if estDefExp value then inter (INTERVALLE(INFINI,BNOTINCLUDE ( getDefValue  value)))   interval else interval  
				end
			end
			else interval
		| GE -> 
			let (isAffine, isLeft, value) = getInfoOfCondBINARY el er var in
			if isAffine && value != NOCOMP then
			begin
				 
				if isLeft then
				begin (*var >= value*) 
					 if estDefExp value then  inter (INTERVALLE(BINCLUDE( getDefValue  value),INFINI)) interval else interval  
				end
				else
				begin (* value >= var*)
					 if estDefExp value then  inter (INTERVALLE(INFINI,BINCLUDE ( getDefValue  value)))   interval else interval  
				end
			end
			else interval
		| EQ -> 
			let (isAffine, isLeft, value) = getInfoOfCondBINARY el er var in
			if isAffine && value != NOCOMP then  
			begin
				 
				if estDefExp value then 
				begin
					let fvalue = ( getDefValue  value) in
					if isMember fvalue interval then  (INTERVALLE (BINCLUDE fvalue,BINCLUDE fvalue))  else EMPTY
				end
   				else interval  
			end
			else interval



		| NE -> 
			let (isAffine, isLeft, value) = getInfoOfCondBINARY el er var in
			if isAffine && value != NOCOMP then
			begin
			 
				if estDefExp value then   remove   ( getDefValue  value)  interval  else interval  
			end
			else interval
		| AND ->  inter ( restictIntervalFromCond el var  interval) ( restictIntervalFromCond er var  interval )
		| OR -> union( restictIntervalFromCond el var  interval) ( restictIntervalFromCond er var  interval )

		| _ -> interval
		)
		| _ -> interval
















