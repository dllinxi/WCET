(** constante -  To be precise. Not sure it is used... 
**
** Project: 			Frontc
** File: 			constante.ml
** Version:			1.0
** Date:			18.06.2008
** Author:			Bonjean Noelie
**
*)


(** result : a^n *)
let rec puissance = fun a n ->
match n with
	0->1
	|_-> a*(puissance a (n-1))
;;

(** convert char to decimal *)
let convert_H_to_D = fun nb ->
let valeur=(int_of_char nb) in
	if (valeur>=(int_of_char '0') && valeur<=(int_of_char '9')) then valeur - (int_of_char '0')
	else
		if(valeur>=(int_of_char 'A') && valeur<=(int_of_char 'Z')) 
		then valeur - (int_of_char 'A')+ 10
		else valeur - (int_of_char 'a')+ 10
;;

(** convert String into decimal *)
let rec decimal = fun s taille cpt ->
let nb=(String.get s cpt) in
	if (cpt==(taille-1)) then (convert_H_to_D nb)
	else ((convert_H_to_D nb)*puissance 10 (taille-cpt-1)) + (decimal s taille (cpt+1))
;;

(** convert hexadecimal into decimal *)
let rec hexadecimal = fun s taille cpt ->
let nb=(String.get s cpt) in
	if (cpt==(taille-1)) then (convert_H_to_D nb)
	else ((convert_H_to_D nb)*puissance 16 (taille-cpt-1)) + (hexadecimal s taille (cpt+1))
;;

(** convert octal into decimal *)
let rec octal = fun s taille cpt ->
let nb=(String.get s cpt) in
	if (cpt==(taille-1)) then (convert_H_to_D nb)
	else ((convert_H_to_D nb)*puissance 8 (taille-cpt-1)) + (octal s taille (cpt+1))
;;

(** test if number is negative *)
let neg = fun s -> ( (String.get s 0) == '-' ) 
;;

(** return Position of exposant *)
let rec posExposant = fun s cpt ->
let c=(String.get s cpt) in 
	if ((c == 'e') || (c == 'E')) 
	then cpt+1
	else 
		if (cpt == ((String.length s)-1))
		then cpt+3
		else posExposant s (cpt+1)
;;

(** return Position of point *)
let rec posPoint = fun s cpt ->
let c=(String.get s cpt) in 
	if (c == '.') 
	then cpt+1
	else posPoint s (cpt+1)
;;

(** return String between 2 positions *)
let numberAt = fun s fin ->
	let rec loop = fun cpt ->
	if (fin==cpt) then ""
	else (Char.escaped (String.get s cpt))^(loop (cpt+1))
in loop 
;;

(** point after exposant ? *)
let rec pointAfter = fun s  cpt->
	let c=(String.get s cpt) in 
	if (c == '.') 
	then true
	else 
		if (c == 'e' || c == 'E')
		then false
		else pointAfter s (cpt+1)
;;

(** remove first char of string *)
let removeFirstChar = fun s ->
	numberAt s (String.length s) 1
;;

(** remove last char of string *)
let removeLastChar = fun s ->
	numberAt s ((String.length s)-1) 0
;;

let longfct = fun s ->
	let lastChar= String.get s ((String.length s)-1) in
	(lastChar == 'l' || lastChar == 'L')
;;

let floatfct = fun s ->
	let lastChar= String.get s ((String.length s)-1) in
	(lastChar == 'f' || lastChar == 'F')
;;


(** make over String (CONST_INT) to int *)
let string_to_int = fun i -> 
	if ((String.length i)>1) 
	then
		if (longfct i)
		then
			(* long number *)
			let l=(removeLastChar i) in
			if (neg l) 
			then
				(* negative number *)
				let nb=(removeFirstChar l) in
				if((String.length nb)>1)
				then
					let first=(String.get nb 0) and second=(String.get nb 1) in
					if (first == '0')
					then
						if (second == 'x' || second == 'X')
						then 
							(* negative hexadecimal number *)
							0 - (hexadecimal nb (String.length nb) 2)
						else 
							(* negative octal number *)
							0 - (octal nb (String.length nb) 1)
					else 
						(* negative decimal number *)
						0 - (decimal nb (String.length nb) 0)
				else
					(* negative figure *)
					0-(decimal nb (String.length nb) 0)
					
			else
				if ((String.length l)>1)
				then

					let first=(String.get l 0) and second=(String.get l 1) in
					if (first == '0')
					then
						if (second == 'x' || second == 'X')
						then 
							(* positive hexadecimal number *)
							hexadecimal l (String.length l) 2
						else 
							(* positive hexadecimal number *)
							octal l (String.length l) 1
					else 
						(* positive decimal number *)
						(decimal l (String.length l) 0)
				else
					(decimal l (String.length l) 0)
				
		else
			(* int number *)
			if (neg i) 
			then
				(* negative number *)
				let nb=(removeFirstChar i) in
				if((String.length nb)>1)
				then
					let first=(String.get nb 0) and second=(String.get nb 1) in
					if (first == '0')
					then
						if (second == 'x' || second == 'X')
						then 
							(* negative hexadecimal number *)
							0 - (hexadecimal nb (String.length nb) 2)
						else 
							(* negative octal number *)
							0 - (octal nb (String.length nb) 1)
					else 
						(* negative decimal number *)
						0 - (decimal nb (String.length nb) 0)
				else
					(* negative figure *)
					0-(decimal nb (String.length nb) 0)
					
			else
				let first=(String.get i 0) and second=(String.get i 1) in
				if (first == '0')
				then
					if (second == 'x' || second == 'X')
					then 
						(* positive hexadecimal number *)
						hexadecimal i (String.length i) 2
					else 
						(* positive hexadecimal number *)
						octal i (String.length i) 1
				else 
					(* positive decimal number *)
					(decimal i (String.length i) 0)
	else
		(* positive figure *)
		(decimal i (String.length i) 0)
;;


(** make over String (CONST_FLOAT) to Float *)
let string_to_float = fun f ->
	if (floatfct f)
	then
		let newf=(removeLastChar f) in
		(float_of_int (decimal newf (String.length newf) 0))
	else
		if(longfct f)
		then
			let newf=(removeLastChar f) in
			if (neg newf)
			then
				(* negative number *)
				let nbf= (removeFirstChar newf) in
				let first=(String.get nbf 0) in
				if (first == '.')
				then
					(* .figures[exposant] *)
					let nb=(removeFirstChar nbf) in
					if ((String.contains nb 'e') || (String.contains nb 'E'))
					then
						(* .figures Exposant *)
						let before=(numberAt nb ((posExposant nb 0)-1) 0) and after=(numberAt nb (String.length nb) (posExposant nb 0)) in
						if (neg after)
						then 
							let p=(removeFirstChar after) in
							0. -. (float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else 
							0. -. (float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) *. (float_of_int (puissance 10 (decimal after(String.length after) 0)))
						
					else
						(* .figures *)
						0. -. (float_of_int (decimal nb (String.length nb) 0)) /. (float_of_int (puissance 10 (String.length nb)))
				else
					(* figures ... *)
					if (pointAfter nbf 0)
					then
						(* figures. ...*)
						if ((String.length nbf) == (posPoint nbf 0))
						then
							(* figures. *)
							let taille= (String.length nbf)-1 in
							0. -. (float_of_int (decimal (numberAt nbf taille 0) taille 0))
						else
							if ((posPoint nbf 0) == ((posExposant nbf 0)-1))
							then
								(* figures.Exposant *)
								let chiffres=(numberAt nbf ((posPoint nbf 0)-1) 0) and exposant=(numberAt nbf (String.length nbf) (posExposant nbf 0)) in
								if (neg exposant)
								then
									let p= (removeFirstChar exposant) in
									0. -. (float_of_int (decimal chiffres (String.length chiffres) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
								else
									0. -. (float_of_int (decimal chiffres (String.length chiffres) 0)) *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								
							else
								(* figures.figures [Exposant] *)
								if ((String.contains nbf 'e') || (String.contains nbf 'E'))
								then
									(* figures.figures Exposant *)
									let beforePoint = (numberAt nbf ((posPoint nbf 0)-1) 0)
									and afterPoint =(numberAt nbf ((posExposant nbf 0)-1) (posPoint nbf 0))
									and exposant = (numberAt nbf (String.length nbf) (posExposant nbf 0)) 
									in
									let nb= (float_of_int (decimal beforePoint (String.length beforePoint) 0)) +. ((float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))) in
									if (neg exposant)
									then 
										let p= (removeFirstChar exposant) in
										0. -. nb /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
									else 
										0. -. nb *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								else
									(* figures.figures *)
									let beforePoint = (numberAt nbf ((posPoint nbf 0)-1) 0)
									and afterPoint =(numberAt nbf (String.length nbf) (posPoint nbf 0)) in
									0. -. (float_of_int (decimal beforePoint (String.length beforePoint) 0)) -. (float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))
									
					else
						(* figures Exposant *)
						let before=(numberAt nbf ((posExposant nbf 0)-1) 0) and after=(numberAt nbf (String.length nbf) (posExposant nbf 0)) in
						if (neg after)
						then
							let p=(removeFirstChar after) in
							0. -. (float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else
							0. -. (float_of_int (decimal before (String.length before) 0)) *. (float_of_int (puissance 10 (decimal after (String.length after) 0)))
		
		
			else 
				(* positive number *)
				let first=(String.get newf 0) in
				if (first == '.')
				then
					(* .figures[exposant] *)
					let nb=(removeFirstChar newf) in
					if ((String.contains nb 'e') || (String.contains nb 'E'))
					then
						(* .figures Exposant *)
						let before=(numberAt nb ((posExposant nb 0)-1) 0) and after=(numberAt nb (String.length nb) (posExposant nb 0)) in
						if (neg after)
						then 
							let p=(removeFirstChar after) in
							(float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else 
							(float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) *. (float_of_int (puissance 10 (decimal after(String.length after) 0)))
						
					else
						(* .figures *)
						(float_of_int (decimal nb (String.length nb) 0)) /. (float_of_int (puissance 10 (String.length nb)))
				else
					(* figures ... *)
					if (pointAfter newf 0)
					then
						(* figures. ...*)
						if ((String.length newf) == (posPoint newf 0))
						then
							(* figures. *)
							let taille= (String.length newf)-1 in
							(float_of_int (decimal (numberAt newf taille 0) taille 0))
						else
							if ((posPoint newf 0) == ((posExposant newf 0)-1))
							then
								(* figures.Exposant *)
								let chiffres=(numberAt newf ((posPoint newf 0)-1) 0) and exposant=(numberAt newf (String.length newf) (posExposant newf 0)) in
								if (neg exposant)
								then
									let p= (removeFirstChar exposant) in
									(float_of_int (decimal chiffres (String.length chiffres) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
								else
									(float_of_int (decimal chiffres (String.length chiffres) 0)) *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								
							else
								(* figures.figures [Exposant] *)
								if ((String.contains newf 'e') || (String.contains newf 'E'))
								then
									(* figures.figures Exposant *)
									let beforePoint = (numberAt newf ((posPoint newf 0)-1) 0)
									and afterPoint =(numberAt newf ((posExposant newf 0)-1) (posPoint newf 0))
									and exposant = (numberAt newf (String.length newf) (posExposant newf 0)) 
									in
									let nb= (float_of_int (decimal beforePoint (String.length beforePoint) 0)) +. ((float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))) in
									if (neg exposant)
									then 
										let p= (removeFirstChar exposant) in
										nb /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
									else 
										nb *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								else
									(* figures.figures *)
									let beforePoint = (numberAt newf ((posPoint newf 0)-1) 0)
									and afterPoint =(numberAt newf (String.length newf) (posPoint newf 0)) in
									(float_of_int (decimal beforePoint (String.length beforePoint) 0)) +. (float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))
									
					else
						(* figures Exposant *)
						let before=(numberAt newf ((posExposant newf 0)-1) 0) and after=(numberAt newf (String.length newf) (posExposant newf 0)) in
						if (neg after)
						then
							let p=(removeFirstChar after) in
							(float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else
							(float_of_int (decimal before (String.length before) 0)) *. (float_of_int (puissance 10 (decimal after (String.length after) 0)))
	
	
		else
			(* pas d'ajout de lettre en fin *)
			if (neg f)
			then
				(* negative number *)
				let nbf= (removeFirstChar f) in
				let first=(String.get nbf 0) in
				if (first == '.')
				then
					(* .figures[exposant] *)
					let nb=(removeFirstChar nbf) in
					if ((String.contains nb 'e') || (String.contains nb 'E'))
					then
						(* .figures Exposant *)
						let before=(numberAt nb ((posExposant nb 0)-1) 0) and after=(numberAt nb (String.length nb) (posExposant nb 0)) in
						if (neg after)
						then 
							let p=(removeFirstChar after) in
							0. -. (float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else 
							0. -. (float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) *. (float_of_int (puissance 10 (decimal after(String.length after) 0)))
						
					else
						(* .figures *)
						0. -. (float_of_int (decimal nb (String.length nb) 0)) /. (float_of_int (puissance 10 (String.length nb)))
				else
					(* figures ... *)
					if (pointAfter nbf 0)
					then
						(* figures. ...*)
						if ((String.length nbf) == (posPoint nbf 0))
						then
							(* figures. *)
							let taille= (String.length nbf)-1 in
							0. -. (float_of_int (decimal (numberAt nbf taille 0) taille 0))
						else
							if ((posPoint nbf 0) == ((posExposant nbf 0)-1))
							then
								(* figures.Exposant *)
								let chiffres=(numberAt nbf ((posPoint nbf 0)-1) 0) and exposant=(numberAt nbf (String.length nbf) (posExposant nbf 0)) in
								if (neg exposant)
								then
									let p= (removeFirstChar exposant) in
									0. -. (float_of_int (decimal chiffres (String.length chiffres) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
								else
									0. -. (float_of_int (decimal chiffres (String.length chiffres) 0)) *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								
							else
								(* figures.figures [Exposant] *)
								if ((String.contains nbf 'e') || (String.contains nbf 'E'))
								then
									(* figures.figures Exposant *)
									let beforePoint = (numberAt nbf ((posPoint nbf 0)-1) 0)
									and afterPoint =(numberAt nbf ((posExposant nbf 0)-1) (posPoint nbf 0))
									and exposant = (numberAt nbf (String.length nbf) (posExposant nbf 0)) 
									in
									let nb= (float_of_int (decimal beforePoint (String.length beforePoint) 0)) +. ((float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))) in
									if (neg exposant)
									then 
										let p= (removeFirstChar exposant) in
										0. -. nb /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
									else 
										0. -. nb *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								else
									(* figures.figures *)
									let beforePoint = (numberAt nbf ((posPoint nbf 0)-1) 0)
									and afterPoint =(numberAt nbf (String.length nbf) (posPoint nbf 0)) in
									0. -. (float_of_int (decimal beforePoint (String.length beforePoint) 0)) -. (float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))
									
					else
						(* figures Exposant *)
						let before=(numberAt nbf ((posExposant nbf 0)-1) 0) and after=(numberAt nbf (String.length nbf) (posExposant nbf 0)) in
						if (neg after)
						then
							let p=(removeFirstChar after) in
							0. -. (float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else
							0. -. (float_of_int (decimal before (String.length before) 0)) *. (float_of_int (puissance 10 (decimal after (String.length after) 0)))
		
		
			else 
				(* positive number *)
				let first=(String.get f 0) in
				if (first == '.')
				then
					(* .figures[exposant] *)
					let nb=(removeFirstChar f) in
					if ((String.contains nb 'e') || (String.contains nb 'E'))
					then
						(* .figures Exposant *)
						let before=(numberAt nb ((posExposant nb 0)-1) 0) and after=(numberAt nb (String.length nb) (posExposant nb 0)) in
						if (neg after)
						then 
							let p=(removeFirstChar after) in
							(float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else 
							(float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (String.length before))) *. (float_of_int (puissance 10 (decimal after(String.length after) 0)))
						
					else
						(* .figures *)
						(float_of_int (decimal nb (String.length nb) 0)) /. (float_of_int (puissance 10 (String.length nb)))
				else
					(* figures ... *)
					if (pointAfter f 0)
					then
						(* figures. ...*)
						if ((String.length f) == (posPoint f 0))
						then
							(* figures. *)
							let taille= (String.length f)-1 in
							(float_of_int (decimal (numberAt f taille 0) taille 0))
						else
							if ((posPoint f 0) == ((posExposant f 0)-1))
							then
								(* figures.Exposant *)
								let chiffres=(numberAt f ((posPoint f 0)-1) 0) and exposant=(numberAt f (String.length f) (posExposant f 0)) in
								if (neg exposant)
								then
									let p= (removeFirstChar exposant) in
									(float_of_int (decimal chiffres (String.length chiffres) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
								else
									(float_of_int (decimal chiffres (String.length chiffres) 0)) *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								
							else
								(* figures.figures [Exposant] *)
								if ((String.contains f 'e') || (String.contains f 'E'))
								then
									(* figures.figures Exposant *)
									let beforePoint = (numberAt f ((posPoint f 0)-1) 0)
									and afterPoint =(numberAt f ((posExposant f 0)-1) (posPoint f 0))
									and exposant = (numberAt f (String.length f) (posExposant f 0)) 
									in
									let nb= (float_of_int (decimal beforePoint (String.length beforePoint) 0)) +. ((float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))) in
									if (neg exposant)
									then 
										let p= (removeFirstChar exposant) in
										nb /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
									else 
										nb *. (float_of_int (puissance 10 (decimal exposant (String.length exposant) 0)))
								else
									(* figures.figures *)
									let beforePoint = (numberAt f ((posPoint f 0)-1) 0)
									and afterPoint =(numberAt f (String.length f) (posPoint f 0)) in
									(float_of_int (decimal beforePoint (String.length beforePoint) 0)) +. (float_of_int (decimal afterPoint (String.length afterPoint) 0)) /. (float_of_int (puissance 10 (String.length afterPoint)))
									
					else
						(* figures Exposant *)
						let before=(numberAt f ((posExposant f 0)-1) 0) and after=(numberAt f (String.length f) (posExposant f 0)) in
						if (neg after)
						then
							let p=(removeFirstChar after) in
							(float_of_int (decimal before (String.length before) 0)) /. (float_of_int (puissance 10 (decimal p (String.length p) 0)))
						else
							(float_of_int (decimal before (String.length before) 0)) *. (float_of_int (puissance 10 (decimal after (String.length after) 0)))

;;




