			
(isMultiInc2=false) &&(typeinc1 =MULTI) &&  (typeinc2 =DIVI) 

let isLeft1 = List.mem (List.hd l) (listeDesVarsDeExpSeules exp1) in
					let isLeft2 = List.mem (List.hd (List.tl l)) (listeDesVarsDeExpSeules exp1) in
					let oppose =  (isLeft1 &&  isLeft2 = false) || (isLeft2 &&  isLeft1 = false) in




let control inc1 inc2 oppose isMultiInc1 isMultiInc2=




if oppose &&  haveTheSameType inc1 inc2 then true

(getIncType inc1) = (getIncType inc2) 

let haveTheInvesType inc1 inc2 =
					let value =
						if oppose then 
							if typeinc1 =POSITIV||typeinc1 =NEGATIV then
							( calculer (EXP(BINARY(SUB,getIncValue inc1,getIncValue inc2)))  !infoaffichNull [] 1)
							else calculer (EXP(BINARY(DIV,getIncValue inc1,getIncValue inc2)))  !infoaffichNull [] 1
						else
	  						if typeinc1 =POSITIV||typeinc1 =NEGATIV then
								if isLeft1 then
								begin
									let newexp = ((remplacerValPar  (List.hd (List.tl l)) (getIncValue inc2) (remplacerValPar  (List.hd l) (getIncValue inc1) exp1))) in
									calculer (EXP(newexp))  !infoaffichNull [] 1

								end
								else
								begin
									let newexp = UNARY(MINUS,(remplacerValPar  (List.hd (List.tl l)) (getIncValue inc2) (remplacerValPar  (List.hd l) (getIncValue inc1) exp2))) in
									calculer  (EXP(newexp))   !infoaffichNull [] 1
								end
