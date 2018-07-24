(* TCL -- Calipso Main library interface.
**
** Project: 		Calipso
** File: 			calipso.ml
** Version:			3.0
** Date:			7.20.99
** Author:			Hugues Cassé
**
*)

exception InternalError

(** Calipso is an OCAML library useful to transform a C source code by removing
	goto, break, continue, and simplifying return and switch.
*)


(*
** Argument definition
*)

(** Constructor for Calipso options *)
type option =
	  Verbose
		(** Switch on the verbose mode *)
	| UseMask
		(** Use bitfield masks to handle labels *)
	| RemoveGoto
		(** Remove the goto statements *)
	| RemoveBreak
		(** Remove the break statements *)
	| RemoveContinue
		(** Remove the continue statements *)
	| RemoveReturn
		(** Remove the return statements *)
	| RemoveSwitch of Reduce.switch_action
		(** Set the reducing method for switch statements
			(Reduce.NO: don't reduce; Reduce.RAW: reduce rawly; Reduce.KEEP:
			reduce but keep the regular ones; Reduce.REDUCE: use a faster
			method for regular ones.) *)
	| Strategy of Algo.strategy_kind
		(** Set the algorithme strategy to organize sequences
			(Algo.LEFT: organize sequences to the left; Algo.RIGHT: organize to
			the right; Algo.WEIGHTED: use the weight) *)
	| RemoveRecursive
		(** Transform (if possible) recursive function into loops (iftofor). *)

let rm_goto = ref false
let verbose_mode = ref false
let rm_rec = ref false

(* process_defs definition list -> definition list
**		Remove "goto" and statement alike in the definition list.
*)
let rec process_defs (defs : Cabs.definition list) =
	let process_func typ name body =
		Gen.reset ();
		let body' = Reduce.reduce body typ in
		if !rm_goto then Algo.remove body' else body' in
	match defs with
	[] -> []
	| (Cabs.FUNDEF ((base, sto, (id, proto, [], exp)), body))::fol ->
		let _ = if !verbose_mode then prerr_string (id ^ "()\n") in
		(match proto with
		Cabs.PROTO (typ, pars, ell) ->
			(Cabs.FUNDEF (
				(base, sto, (id, proto, [], exp)),
				process_func typ id body))
			::(process_defs fol)
		| _ -> raise InternalError)
	| (Cabs.OLDFUNDEF ((base, sto, (id, proto, [], exp)), decs, body))::fol ->
		let _ = if !verbose_mode then prerr_string (id ^ "()\n") in
		(match proto with
		Cabs.OLD_PROTO (typ, pars, ell) ->
			(Cabs.OLDFUNDEF (
				(base, sto, (id, proto, [], exp)),
				decs,
				process_func typ id body))
		::(process_defs fol)
		| _ -> raise InternalError)
	| def::fol -> def::(process_defs fol)


(** Process operations on an abstract C source file.
	
	This function provide the full set of options.
	
	@param defs the definition list to be processed.
	@param options the option list.
	@return the processed definition list.
*)
let process_remove (defs : Cabs.definition list) options =
	let rec set_option opt_list =
		match opt_list with
			| Verbose :: t ->
				let _ = verbose_mode := true
				in set_option t
			| UseMask :: t ->
				let _ = Algo.use_mask := true
				in set_option t
			| RemoveGoto :: t ->
				let _ = rm_goto := true
				in set_option t
			| RemoveBreak :: t ->
				let _ = Reduce.remove_break := true
				in set_option t
			| RemoveContinue :: t ->
				let _ = Reduce.remove_continue := true
				in set_option t
			| RemoveReturn :: t ->
				let _ = Reduce.remove_return := true
				in set_option t
			| RemoveSwitch(Reduce.KEEP) :: t ->
				let _ = Reduce.remove_switch := Reduce.KEEP
				in let _ = Algo.regular_switch := true
				in set_option t
			| RemoveSwitch(meth) :: t ->
				let _ = Reduce.remove_switch := meth
				in set_option t
			| Strategy(strategy) :: t ->
				let _ = Algo.strategy := strategy
				in set_option t
			| RemoveRecursive :: t ->
				let _ = rm_rec := true
				in set_option t
			| [] -> ()
	in let _ = set_option options
	in let defs = process_defs defs
	in if (!rm_rec)
		then (Iftofor.if_to_for defs)
		else defs

(** Process a standard remove, by removing goto, break, continue, return and
	reducing switch with the Reduce.REDUCE method.
	@param defs the definition list to be processed.
	@return the processed definition list.
*)
let process_standard_remove (defs : Cabs.definition list) =
	process_remove defs [RemoveGoto; RemoveBreak; RemoveContinue; RemoveReturn;
							RemoveSwitch(Reduce.REDUCE); Strategy(Algo.LEFT)]

(** Process a subtle remove, by removing goto, break, continue, return and
	reducing switch with the Reduce.KEEP method.
	@param defs the definition list to be processed.
	@return the processed definition list.
*)
let process_subtle_remove (defs : Cabs.definition list) =
	process_remove defs [RemoveGoto; RemoveBreak; RemoveContinue; RemoveReturn;
							RemoveSwitch(Reduce.KEEP); Strategy(Algo.LEFT)]

