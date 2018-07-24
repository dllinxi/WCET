(** Merge many C files into a single one, with the right transformations.

	Checking a list of C files is done in four steps:
		- step -1: geting informations about extern an global variables.
		- step 0: removing redundants declarations  (typedefs, struct, union,
				  enum, global variables), and putting up the definition of
				  extern variables.
		- step 1: geting informations and statistics in the files about
				  conflicting typedef and static elements, and conflicting
				  struct / enum / union.
		- step 2: using them to rename conflicting elements.

	This module define also some utility functions.

	The main entry points of this module are the functions check and merge.

	@author Florian Birée <florian\@biree.name>
	@version 0.2
*)

(** This modules work on C Cabs.file values (produced by Frontc). *)
open Cabs

(** Generic functions to manage a C Cabs file.

	Those functions works only on top-level definitions.
*)
	
	(* is a declaration type a function declaration type  *)
	let rec isProto typ =
		 match typ with
		 
						  PROTO (_, _, _) | OLD_PROTO (_, _, _) ->	true
						| GNU_TYPE (_, typ) ->isProto typ
						| TYPE_LINE (_, _, _typ) -> isProto typ
						| _ -> false  

(** Check a single C source file.

	Keep only definitions that respect the predicate in the tree,
	then apply (corrector definition) on the definition.
	@param predicate return if the definition is kept.
	@param corrector return the correct definition.
	@param file the C source file to check.
	@return the checked file.
*)
let rec file_checker = fun predicate corrector file ->
	match file with
		| elt :: file_tail -> if (predicate elt)
			then (corrector elt) :: (file_checker predicate corrector file_tail)
			else (file_checker predicate corrector file_tail)
		| [] -> []

(** Check a list of c source files.

	Keep only definitions that respect the predicate in the trees,
	then apply (corrector definition) on the trees, and return the
	list of modified trees.
	@param predicate return if the definition is kept.
	@param corrector return the correct definition.
	@param file_list the C source file to check.
	@return the checked file.
*)
let rec file_list_checker = fun predicate corrector file_list ->
	List.map (file_checker predicate corrector) file_list

(** Transform definitions in a single C source file.
	@param f function that takes the previous transformation result and the
			current definition, and that must return the next transformation
			result.
	@param start the inital value passed as previous transformation result to f.
	@param file the C source file to be transformed.
	@return the transformed C source file.
*)
let file_transform = fun f start file ->
	(List.fold_left f start file)

(** Transform definitions in a list of c source files.
	@param f function that takes the previous transformation result and the
			current definition, and that must return the next transformation
			result.
	@param start the inital value passed as previous transformation result to f.
	@param file_list the list of C source file to be transformed.
	@return the list of transformed C source file.
*)
let file_list_transform = fun f start file_list ->
	(List.fold_left (file_transform f) start file_list)

(** Utility function to add a name in a name table.

	A name table is a list of (name, count).
	@param name_table the initial name.
	@param name the name to add in the table.
	@return the modified name table.
*)
let rec add_name = fun name_table name ->
	match name_table with
		| (oth_name, count) :: t -> if name = oth_name
			then (name, count + 1) :: t
			else (oth_name, count) :: (add_name t name)
		| [] -> (name, 1) :: []


(** Utility function to get the list of multiple names in a name table.
	@param name_table the name table to get the names.
	@return the list of names that have a > 1 count in the name table.
*)
let keep_multiple = fun name_table ->
	(List.fold_left
		(fun mult_list entry ->
			let (name, count) = entry
			in if count > 1 then name :: mult_list else mult_list)
		[]
		name_table
	)

(** Utility function to return if a name is in a name table.
	@param name_table the table to search in.
	@param name the name to look for.
	@return if the name is in the table.
*)
let rec is_name_in_table = fun name_table name ->
	match name_table with
		| (a_name, _) :: t -> (a_name = name) or (is_name_in_table t name)
		| [] -> false

(** Utility functions to manage Cabs.name list elements. *)

(** Return if the given name is in a Cabs.name list
	@param name the name (string) to look for
	@param cabsnames the Cabs.name list to look en
	@return if name is in the list
*)
let rec name_is_in = fun name cabsnames ->
	match cabsnames with
		| (name0, _, _, _) :: t ->
			if name0 = name
			then true
			else (name_is_in name t)
		| [] -> false

(** Return if the given name is the only one in the list of Cabs.name
	@param name the name (string) to look for
	@param cabsnames the Cabs.name list to look in
	@return if name is the only one in the list
*)
let name_is_lonely = fun name cabsnames ->
	match cabsnames with
		| (name0, _, _, _) :: [] -> (name = name0)
		| _ -> false

(** Remove a name from a Cabs.name list
	@param name the name to remove
	@param cabsnames the Cabs.name list where the name must be removed
	@return the Cabs.name list without the name
*)
let name_remove = fun name cabsnames ->
	List.filter (function (name2, _, _, _) -> name <> name2) cabsnames

(** Get the list of string names for a Cabs.name list
	@param cabsnames the Cabs.name list
	@return the list of names as a string list
*)
let strings_of_cabsnames = fun cabsnames ->
	List.map (function (name, _, _, _) -> name) cabsnames

(** Fold_left on a Cabs.name list
	@param f the function ('a -> string -> 'a) to apply on each name
	@param first the first 'a element
	@param cabsnames the Cabs.name list to iterate on
	@return the resulting 'a element
*)
let name_fold_left = fun f first cabsnames ->
	List.fold_left f first (strings_of_cabsnames cabsnames)

(** Get the Cabs.name element of a name in a Cabs.name list
	@param name the name to get the corresponding Cabs.name
	@param cabsnames the Cabs.name list to search in
	@return the corresponding Cabs.name
*)
let rec get_cabsname_of_name = fun name cabsnames ->
	match cabsnames with
		| cname :: t ->
			let (name0, _, _, _) = cname
			in if name0 = name
				then cname
				else get_cabsname_of_name name t
		| [] -> failwith("Name not found")

(** Type to manage various kind of C elements. *)
type elt =
	| E_DEF of definition list
	| E_BASETYPE of base_type
	| E_SINGLE_NAME of single_name
	| E_NAME_GROUP of name_group
	| E_BODY of body
	| E_EXPR of expression

(** Generic renaming function.

	For general purpose, see the rename function.

	@param element a C element to look for names to rename.
	@param rn_name function that is applied on all names.
	@return the resulting element.
*)
let rec generic_rename = fun element rn_name ->

	(* recursive calls to generic rename, with type convertion *)
	let generic_rn_def = fun deflist ->
		match generic_rename (E_DEF(deflist)) rn_name with
			E_DEF(result) -> result
			| _ -> failwith("Bad answer") in
	let generic_rn_basetype = fun basetype ->
		match generic_rename (E_BASETYPE(basetype)) rn_name with
			E_BASETYPE(result) -> result
			| _ -> failwith("Bad answer") in
	let generic_rn_single_name = fun sn ->
		match generic_rename (E_SINGLE_NAME(sn)) rn_name with
			E_SINGLE_NAME(result) -> result
			| _ -> failwith("Bad answer") in
	let generic_rn_name_group = fun ng ->
		match generic_rename (E_NAME_GROUP(ng)) rn_name with
			E_NAME_GROUP(result) -> result
			| _ -> failwith("Bad answer") in
	let generic_rn_body = fun body ->
		match generic_rename (E_BODY(body)) rn_name with
			E_BODY(result) -> result
			| _ -> failwith("Bad answer") in
	let generic_rn_expr = fun expr ->
		match generic_rename (E_EXPR(expr)) rn_name with
			E_EXPR(result) -> result
			| _ -> failwith("Bad answer") in

	(* renaming in a constant *)
	let rn_constant = fun constant ->
		match constant with
			| CONST_INT(name) -> CONST_INT(rn_name name)
			| CONST_FLOAT(name) -> CONST_FLOAT(rn_name name)
			| CONST_CHAR(name) -> CONST_CHAR(rn_name name)
			| CONST_STRING(name) -> CONST_STRING(rn_name name)
			| CONST_COMPOUND(expr_list) ->
				CONST_COMPOUND(List.map generic_rn_expr expr_list)
			| RCONST_FLOAT f -> RCONST_FLOAT f
			| RCONST_INT i -> RCONST_INT i in

	(* renaming in an expression *)
	let rec rn_expr = fun expr ->
		match expr with
			UNARY(op, u_expr) ->
				UNARY(op, (rn_expr u_expr))
			| BINARY(op, expr1, expr2) ->
				BINARY(op, (rn_expr expr1), (rn_expr expr2))
			| QUESTION(expr1, expr2, expr3) ->
				QUESTION((rn_expr expr1), (rn_expr expr2), (rn_expr expr3))
			| CAST(base_type, c_expr) ->
				CAST((generic_rn_basetype base_type), (rn_expr c_expr))
			| CALL(c_expr, expr_list) ->
				CALL((rn_expr c_expr), List.map rn_expr expr_list)
			| COMMA(expr_list) ->
				COMMA(List.map rn_expr expr_list)
			| CONSTANT(cst) ->
				CONSTANT(rn_constant cst)
			| VARIABLE(name) ->
				VARIABLE(rn_name name)
			| EXPR_SIZEOF(s_expr) ->
				EXPR_SIZEOF(rn_expr s_expr)
			| TYPE_SIZEOF(base_type) ->
				TYPE_SIZEOF(generic_rn_basetype base_type)
			| INDEX(expr1, expr2) ->
				INDEX((rn_expr expr1), (rn_expr expr2))
			| MEMBEROF(m_expr, name) ->
				MEMBEROF((rn_expr m_expr), (rn_name name))
			| MEMBEROFPTR(m_expr, name) ->
				MEMBEROFPTR((rn_expr m_expr), (rn_name name))
			| GNU_BODY(body) ->
				GNU_BODY(generic_rn_body body)
			| EXPR_LINE(exprl, str, num) ->
				EXPR_LINE((rn_expr exprl), str, num)
			| other -> other in

	(* renaming in a proto *)
	let rn_proto = fun proto ->
		let (base_type, single_name_list, other_param) = proto in
		let new_sn_list = (List.map generic_rn_single_name single_name_list)
		in ((generic_rn_basetype base_type), new_sn_list, other_param)
		in

	(* renaming in an old K&R proto *)
	let rn_old_proto = fun old_proto ->
		let (base_type, str_list, other_param) = old_proto
		in (generic_rn_basetype base_type, List.map rn_name str_list,
			other_param) in

	(* renaming in an enum item *)
	let rn_enum_item = fun enum_item ->
		let (str, expr) = enum_item
		in ((rn_name str), (rn_expr expr)) in

	(* renaming in gnu_attr *)
	let rec rn_gnu_attr = fun gnu_attr ->
		match gnu_attr with
			GNU_CALL(str,call_gnu_attrs) ->
				GNU_CALL(
					(rn_name str),
					(List.map rn_gnu_attr call_gnu_attrs)
					)
			| GNU_ID(str) -> GNU_ID(rn_name str)
			| other -> other in

	(* renaming in gnu_attrs *)
	let rn_gnu_attrs = fun gnu_attrs ->
		List.map rn_gnu_attr gnu_attrs in

	(* renaming in a base type *)
	let rec rn_base_type = fun base_type ->
		match base_type with
			PTR(ptr_base_type) ->
				PTR(rn_base_type ptr_base_type)
			| RESTRICT_PTR(ptr_base_type) ->
				RESTRICT_PTR(rn_base_type ptr_base_type)
			| ARRAY(a_base_type, expr) ->
				ARRAY((rn_base_type a_base_type), (rn_expr expr))
			| STRUCT(name, name_group_list) ->
				STRUCT(
					(rn_name name),
					(List.map generic_rn_name_group name_group_list)
				)
			| UNION(name, name_group_list) ->
				UNION(
					(rn_name name),
					(List.map generic_rn_name_group name_group_list)
				)
			| PROTO(proto) ->
				PROTO(rn_proto proto)
			| OLD_PROTO(old_proto) ->
				OLD_PROTO(rn_old_proto old_proto)
			| NAMED_TYPE(name) ->
				NAMED_TYPE(rn_name name)
			| ENUM(name, enum_item_list) ->
				ENUM((rn_name name), (List.map rn_enum_item enum_item_list))
			| CONST(c_base_type) ->
				CONST(rn_base_type c_base_type)
			| VOLATILE(v_base_type) ->
				VOLATILE(rn_base_type v_base_type)
			| GNU_TYPE(gnu_attrs, g_base_type) ->
				GNU_TYPE(
					(rn_gnu_attrs gnu_attrs),
					(rn_base_type g_base_type)
				)
			| other -> other in

	(* renaming a cabs_name (type name in Cabs) *)
	let rn_cabs_name = fun cabs_name ->
		let (name, base_type, gnu_attrs, expr) = cabs_name
		in ((rn_name name),
			(rn_base_type base_type),
			(rn_gnu_attrs gnu_attrs),
			(rn_expr expr)) in

	(* renaming in a single_name *)
	let rn_single_name = fun single_name ->
		let (base_type, storage, cabs_name) = single_name
		in ((rn_base_type base_type), storage, (rn_cabs_name cabs_name)) in

	(* renaming in a name_group *)
	let rn_name_group = fun name_group ->
		let (base_type, storage, cabs_name_list) = name_group
		in let res_name_list = List.map rn_cabs_name cabs_name_list
		in ((rn_base_type base_type), storage, res_name_list) in

	(* renaming in statement *)
	let rec rn_statement = fun statement ->
		match statement with
			COMPUTATION(expr) ->
				COMPUTATION(rn_expr expr)
			| BLOCK(body) ->
				BLOCK(generic_rn_body body)
			| SEQUENCE(stat1, stat2) ->
				SEQUENCE(rn_statement stat1, rn_statement stat2)
			| IF(expr, stat1, stat2) ->
				IF(rn_expr expr, rn_statement stat1, rn_statement stat2)
			| WHILE(expr, stat) ->
				WHILE(rn_expr expr, rn_statement stat)
			| DOWHILE(expr, stat) ->
				DOWHILE(rn_expr expr, rn_statement stat)
			| FOR(expr1, expr2, expr3, stat) ->
				FOR(
					(rn_expr expr1),
					(rn_expr expr2),
					(rn_expr expr3),
					(rn_statement stat)
					)
			| RETURN(expr) ->
				RETURN(rn_expr expr)
			| SWITCH(expr, stat) ->
				SWITCH((rn_expr expr), (rn_statement stat))
			| CASE(expr, stat) ->
				CASE((rn_expr expr), (rn_statement stat))
			| DEFAULT(stat) ->
				DEFAULT(rn_statement stat)
			| LABEL(str, stat) ->
				LABEL(str, (rn_statement stat))
			| STAT_LINE(stat, str, num) ->
				STAT_LINE((rn_statement stat), str, num)
			| other -> other in

	(* renaming in a body *)
	let rec rn_body = fun body ->
		let (def_list, statement) = body
		in ((generic_rn_def def_list), (rn_statement statement)) in

	(* renaming in a definition list *)
	let rec rn_def_list = fun def_list ->
		match def_list with
			FUNDEF(single_name,body) :: t ->
				FUNDEF(
					(rn_single_name single_name),
					(rn_body body)
				)::(rn_def_list t)
			| OLDFUNDEF(single_name,name_group_list,body) :: t ->
				OLDFUNDEF(
					(rn_single_name single_name),
					(List.map rn_name_group name_group_list),
					(rn_body body)
				) :: (rn_def_list t)
			| DECDEF(name_group) :: t ->
					DECDEF(rn_name_group name_group) :: (rn_def_list t)
			| TYPEDEF(name_group,gnu_attrs) :: t ->
				TYPEDEF(
					(rn_name_group name_group),
					(rn_gnu_attrs gnu_attrs)
				) :: (rn_def_list t)
			| ONLYTYPEDEF(name_group) :: t ->
				ONLYTYPEDEF(rn_name_group name_group) :: (rn_def_list t)
			| [] -> []

	(* dispatch generic elements -- generic_remane *)
	in match element with
		E_DEF(def_list) -> E_DEF(rn_def_list def_list)
		| E_BASETYPE(base_type) -> E_BASETYPE(rn_base_type base_type)
		| E_SINGLE_NAME(single_name) ->
			E_SINGLE_NAME(rn_single_name single_name)
		| E_NAME_GROUP(group_name) -> E_NAME_GROUP(rn_name_group group_name)
		| E_BODY(body) -> E_BODY(rn_body body)
		| E_EXPR(expr) -> E_EXPR(rn_expr expr)

(** Change all occurences of a name to another in a C source file.
	This will replace names without checking the type of the elements
	(the file will keep the same naming scheme).

	This function hopes that the new name doesn't currently exists.

	@return the resulting file.
*)
let rename = fun old_name new_name tree ->
	(* return the named, changed if needed *)
	let rn_name = fun name ->
		if old_name = name
			then new_name
			else name
	(* convert the tree into a generic element *)
	in match (generic_rename (E_DEF(tree)) rn_name) with
		E_DEF(new_tree) -> new_tree
		| _ -> failwith("Not implemented")

(** Step -1: statisticals functions for globals variables: *)

(** Find the name tables of all global variables and extern variables.
	@param file_list the list of C files.
	@return the name table of all globals and all extern in the list of files.
*)
let globals_tables = fun file_list ->
	let counter = fun tables def ->
		let (globtbl, exttbl) = tables
		in match def with
			| DECDEF(base_type, storage, cabs_names) ->
				(* exclude function pointers *)
				(let has_fun_ptr = (List.fold_left
					(fun has cabs_name ->
						(has ||
						let (_, btype, _, _) = cabs_name
						in match btype with
							| PTR(PROTO(_)) | PTR(OLD_PROTO(_)) -> true
							| _ -> false
						)
					) false cabs_names)
				in if has_fun_ptr then (globtbl, exttbl)
				else let names = strings_of_cabsnames cabs_names
				in match base_type with
					| PROTO(_) | OLD_PROTO(_) -> (globtbl, exttbl)
					| _ -> 
						
	
						 
							(*if storage = EXTERN	then		List.iter (fun (name, pt , _,_)-> 
 								if isProto pt  then
																	Printf.eprintf "\ndéclaration de fonction externe 1 %s \n" name
												  )  cabs_names;*)

							if storage = EXTERN
							then (globtbl, (List.fold_left add_name exttbl  names))
							else if storage <> STATIC
							then ((List.fold_left add_name globtbl names), exttbl)
							else (globtbl, exttbl)
						 
				)
			| _ -> (globtbl, exttbl)
	in file_list_transform counter ([], []) file_list

(** Find the list of conflicting globals and to-be-simplified-externs
	@param globtbl table name of globals variables.
	@param exttbl table name of extern definitions.
	@return the names tables of conflicting globs and to-be-simplifed externs.
*)
let conflicts_glob_extern_tables = function (globtbl, exttbl) ->
	let rec dispatch = fun allglobtbl confglob ext ->
		match allglobtbl with
			| (name, count) :: t ->
				if (is_name_in_table exttbl name)
					then (dispatch t confglob  ((name, count) :: ext))
					else (dispatch t ((name, count) :: confglob) ext)
			| [] -> (confglob, ext)
	in let (glob_tbl, ext_tbl) = (dispatch globtbl [] [])
	in (glob_tbl, ext_tbl)

(** Step 0: remove redundants definitions: *)

(** Remove redundants definitions of struct/enum/unions
	@param onlytypedef the name_group of an onlytypedef
	@param file_tail the tail of the file
	@param list_tail the tail of the list of files
	@return (onlytypedef definition, file_tail, list_tail)
*)
let check_onlytypedef = fun onlytypedef file_tail list_tail ->
	(* return if the definition must be kept
		(the definition is removed if it has the same name_group than the first
		onlytypedef)
	*)
	let predicate = fun def ->
		match def with
			| ONLYTYPEDEF(name_group) ->
				(name_group <> onlytypedef)
			| _ -> true
	in (
		ONLYTYPEDEF(onlytypedef),
		(file_checker predicate (fun def -> def) file_tail),
		(file_list_checker predicate (fun def -> def) list_tail)
	)

(** Remove redundants typedefs
	@param typdef the (name_group, gnu_attrs) of the typedef
	@param file_tail the tail of the file
	@param list_tail the tail of the list of files
	@return (typedef definition, file_tail, list_tail)
*)
let check_typedef = fun typedef file_tail list_tail ->
	let (name_group, gnu_attrs) = typedef
	in let (base_type, storage, cabs_names) = name_group
	(* return if the definition must be kept
		(the definition is removed if it has one name, and this name is the one
		of the first typedef, and everyelse is similar to the first one)
	*)
	in let predicate = fun name def ->
		match def with
			| TYPEDEF((base_type0, storage0, cabs_names0), gnu_attrs0) ->
				not (
					(base_type = base_type0) &&
					(storage = storage0) &&
					(name_is_lonely name cabs_names0) &&
					(gnu_attrs = gnu_attrs0)
				)
			| _ -> true
	(* change a definition if needed
		it remove the name of the first typedef if other parameters are the
		same
	*)
	in let corrector = fun name def ->
		match def with
			| TYPEDEF((base_type0, storage0, cabs_names0), gnu_attrs0) ->
				if (base_type = base_type0 && storage = storage0 &&
											gnu_attrs = gnu_attrs0)
				then TYPEDEF(
					(base_type, storage, (name_remove name cabs_names0)),
					gnu_attrs)
				else TYPEDEF((base_type0, storage0, cabs_names0), gnu_attrs0)
			| other -> other
	(* apply checks on all names of the current typedef *)
	in let file_tail_chk = (name_fold_left
		(fun file name -> file_checker (predicate name) (corrector name) file)
		file_tail
		cabs_names
	)
	in let list_tail_chk = (name_fold_left
		(fun files name -> file_list_checker (predicate name) (corrector name)
			files)
		list_tail
		cabs_names
	)
	in (TYPEDEF(name_group, gnu_attrs), file_tail_chk, list_tail_chk)

let extern_table = ref []
let global_table = ref []
(** Replace the first extern declarations of a variable by its definition, and
	remove other declarations, and also remove multiples definition of globals
	variable.
	@param decdef the name_group of the decdef
	@param file_tail the tail of the file
	@param list_tail the tail of the list of files
	@return (decdef definition, file_tail, list_tail)
*)


	 

let check_decdef = fun decdef file_tail list_tail ->
	let (base_type, storage, cabs_names) = decdef

	(* Do the check on one name *)
	in let check_name = fun elements name ->
		let (new_names, old_names, file_tail, list_tail) = elements
		in let cur_cabsname = (get_cabsname_of_name name cabs_names)
		(* -------- manage extern variable -------- *)
		in if (is_name_in_table !extern_table name)
			then let def_name = ref cur_cabsname
			(* remove other declarations of extern variables, and keep a ref
				to the definition to be able to put it up *)

			in let predicate = fun def ->
				match def with
					| DECDEF(base_type0, EXTERN, cabs_names0) ->
					
						not (name_is_lonely name cabs_names0)
					| DECDEF(base_type0, storage0, cabs_names0) ->
						let _ = def_name := (
							if (name_is_in name cabs_names0)
							then (get_cabsname_of_name name cabs_names0)
							else !def_name
						) in not (name_is_lonely name cabs_names0)
					| _ -> true
			(* remove the name of the external variable in other declarations *)
			in let corrector = fun def ->
				match def with
					| DECDEF(base_type0, storage0, cabs_names0) ->
						DECDEF(base_type0, storage0,
							(name_remove name cabs_names0))
					| other -> other
			in (
				!def_name :: new_names,
				(name_remove name old_names),
				file_checker predicate corrector file_tail,
				file_list_checker predicate corrector list_tail
			)

		(* -------- manage globals variable --------- *)
		else if (is_name_in_table !global_table name)
			(* remove declarations with only the global variable *)
			then let predicate = fun def ->
				match def with
					| DECDEF(base_type0, storage0, cabs_names0) ->
						not (
							(base_type = base_type0) &&
							(storage = storage0) &&
							(name_is_lonely name cabs_names0)
						)
					| _ -> true
			(* remove the name from the global declarations *)
			in let corrector = fun def ->
				match def with
					| DECDEF(base_type0, storage0, cabs_names0) ->
						if (base_type = base_type0 && storage = storage0)
						then DECDEF(base_type, storage,
									(name_remove name cabs_names0))
						else DECDEF(base_type0, storage0, cabs_names0)
					| other -> other
			in (
				cur_cabsname :: new_names,
				(name_remove name old_names),
				file_checker predicate corrector file_tail,
				file_list_checker predicate corrector list_tail
			)
		(* other declarations *)
		else (new_names, old_names, file_tail, list_tail)

	(* Iterate checks on all names *)
	in let (new_names, old_names, file_tail_chk, list_tail_chk) =
		(name_fold_left
			check_name
			([], cabs_names, file_tail, list_tail)
			cabs_names
		)
	(* return the result *)
	in match (new_names, old_names) with
		| [], old_names ->
			(DECDEF((base_type, storage, old_names)),
			file_tail_chk, list_tail_chk)
		| new_names, [] ->
			(DECDEF((base_type, NO_STORAGE, new_names)),
			file_tail_chk, list_tail_chk)
		| new_names, old_names ->
			(DECDEF((base_type, NO_STORAGE, new_names)),
			DECDEF((base_type, storage, old_names)) :: file_tail_chk,
			list_tail_chk)

(** Check of a list of C source files to remove redundancies
	@param file_list the list of C source files.
	@return the checked list of C source files.
*)
let check_step0 = fun file_list ->
	(* dispatchs checks for each type of declarations,
		return (cur_def, file_tail, list_tail)
	*)
	let dispatch_checks = fun cur_def file_tail list_tail ->
		match cur_def with
			| ONLYTYPEDEF(onlytypedef) ->
				(check_onlytypedef onlytypedef file_tail list_tail)
			| TYPEDEF(name_group, gnu_attrs) ->
				(check_typedef (name_group, gnu_attrs) file_tail list_tail)
			| DECDEF(decdef) ->
				(check_decdef decdef file_tail list_tail)
			| other -> (other, file_tail, list_tail)

	(* apply checks on a file, return the file and the tail of the list of
		files *)
	in let rec check_file = fun cur_file list_tail ->
		match cur_file with
			| cur_def :: file_tail ->
				let (cur_def_chk, file_tail_chk, list_tail_chk) =
					(dispatch_checks cur_def file_tail list_tail)
				in let (file_tail_rec, list_tail_rec) =
					(check_file file_tail_chk list_tail_chk)
				in (cur_def_chk :: file_tail_rec, list_tail_rec)
			| [] -> ([], list_tail)

	(* apply checks on a list of files, return the checked list of files*)
	in let rec check_file_list = fun cur_file_list ->
		match cur_file_list with
			| cur_file::list_tail ->
				let (cur_file_chk, list_tail_chk) =
					(check_file cur_file list_tail)
				in cur_file_chk :: (check_file_list list_tail_chk)
			| [] -> []
	in check_file_list file_list

(** Step 1: statisticals functions: *)

(** Find the name table of all typedef.
	@param file_list the list of C files.
	@return the name table of all typedefs in the list of files.
*)
let typedef_table = fun file_list ->
	let counter = fun typetbl def ->
		match def with
			| TYPEDEF((_, _, cabs_names), _) ->
				let names = List.map (function (n, _, _, _) -> n) cabs_names
				in List.fold_left add_name typetbl names
			| _ -> typetbl
	in file_list_transform counter [] file_list

(** Find the name table of static declarations.
	@param file_list the list of C files.
	@return the name table of all static declarations in the list of files.
*)
let static_fun_table = fun file_list ->
	let counter = fun statictbl def ->
		match def with
			| FUNDEF((_, STATIC, cabs_name), _) ->
				let (name, _, _, _) = cabs_name
				in add_name statictbl name
			| OLDFUNDEF((_, STATIC, cabs_name), _, _) ->
				let (name, _, _, _) = cabs_name
				in add_name statictbl name
			| DECDEF(_, STATIC, cabs_names) ->
				name_fold_left add_name statictbl cabs_names
			| _ -> statictbl
	in file_list_transform counter [] file_list

(** Find the name table of struct/enum/union declarations.
	@param file_list the list of C files.
	@return the name table of all struct/enum/union declarations in the list of
			files.
*)
let onlytypedef_table = fun file_list ->
	let counter = fun onlytable def ->
		match def with
			| ONLYTYPEDEF(base_type, _, _) ->
				(match base_type with
					| STRUCT("",_) | STRUCT(_,[]) | UNION("",_) | UNION(_,[]) ->
						onlytable
					| STRUCT(name, _) | UNION(name, _) ->
						add_name onlytable name
					| ENUM("", _) | ENUM(_, []) -> onlytable
					| ENUM(name, _) -> add_name onlytable name
					| _ -> onlytable
				)
			| _ -> onlytable
	in file_list_transform counter [] file_list

(** Step 2: functions to rename conflicting declarations: *)

(** Resolve a name conflict in a list of c source files.
	@param prefix the string that will be added to the name, in addition of a
					numeric identifier.
	@param file_list the list of c source files.
	@param conf_name the name to rename.
	@return the resulting list of c source files, without conflicts.
*)
let resolve_conflicts = fun prefix existing_table file_list conf_name ->
	let new_name = fun id ->
		prefix ^ string_of_int(id) ^ "__" ^ conf_name
	in let rec get_free_id = fun id ->
		if is_name_in_table existing_table (new_name id)
			then get_free_id (id + 1)
			else id
	in let rec resolve_file = function (a_file_list, id) ->
		match a_file_list with
			| file :: t ->
				let id = get_free_id id
				in ((rename conf_name (new_name id) file) ::
					(resolve_file (t, id+1)))
			| [] -> []
	in resolve_file (file_list, 0)

(** Main functions: *)

(** The check function get a list of C source files.

	It will check the list of C source trees in four steps:
	    - search global or extern definitions to be simplified;
		- remove redundants definitions;
		- compute the list of elements to be renamed;
		- rename them.
	@param prefix the string to be added to the name of conflicting elements.
	@param file_list the list of C source files.
	@return the list of checked c source files.

	(Then see the merge function to concat them into one c source tree.)

	This is the main entry point of Mergec.
*)
let check = fun prefix file_list ->
	(* step -1: get the list of globals variables and external variables *)
	let (glob, ext) = (globals_tables file_list)
	in let confglob_tbl, extern_tbl = (conflicts_glob_extern_tables (glob, ext))
	in let _ = extern_table := extern_tbl
	in let _ = global_table := confglob_tbl
	(* step 0: remove redundants typedefs and external *)
	in let file_list0 = check_step0 file_list
	(* step 1: get the list of conflictings typedefs,static,enum,struct,union *)
	in let typedef_tbl = typedef_table file_list0
	in let static_fun_tbl = static_fun_table file_list0
	in let only_tbl = onlytypedef_table file_list0
	in let existing_names = typedef_tbl @ static_fun_tbl @ only_tbl
	in let conf_names = ((keep_multiple typedef_tbl) @
						(keep_multiple static_fun_tbl) @
						(keep_multiple only_tbl))
	(* step 2: rename conflicting definitions *)
	in let file_list2 =
		(List.fold_left
			(resolve_conflicts prefix existing_names)
			file_list0
			conf_names
		)
	in file_list2

(** The removeDuplicatedExtern function remove duplicated extern global declaration into file
	@param file_list a list of parsed  C  files  
	@return the resulting list of C files without duplicated extern.
*)

let extern_tableName = ref [] (* list of extern name declaration *)
let rec removeDuplicatedExtern   file_list =
match  file_list with 
[] -> [] 
| fp::sp ->removeExternIntoOneParsedFile fp :: (removeDuplicatedExtern  sp)
	 



and removeExternIntoOneParsedFile file =
match  file with 
[] -> [] 
| def::sp ->
	 
		match def with
			
			 DECDEF(a, EXTERN, cabs_names) -> 
	
			   let newlist = List.filter ( fun (name, _ , _,_)->
					if List.mem name !extern_tableName then 
						false 
					else (extern_tableName:=name::!extern_tableName;  true)
					)cabs_names in
				if newlist = [] then removeExternIntoOneParsedFile  sp else DECDEF(a, EXTERN, newlist) ::(removeExternIntoOneParsedFile  sp)
			| _ -> def::(removeExternIntoOneParsedFile  sp)


	
	

(** The merge function merge a list of c source file into one c source file.

	It is recomended to give a list of c source files that have been checked
	by the check function.

	@param file_list the list of C source file to merge.
	@return the resulting C source file.
*)
let merge = fun file_list ->
	List.concat file_list
