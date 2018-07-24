# 1 "templater.mll"
 

(** Templater allows to generate files from templates.
	This templates may contains special token that are replaced
	by values retrieved from a dictionnary.

	Basically, an expression "$(identifier)" is replaced by a text
	found in the dictionnary.

	Templates language support support conditional statement
	in form "$(if identifier) ... $(end)" or
	"$(if identifier) ... $(else) ... $(end)". The identifier is looked
	in the dictionnary and must be resolved as a boolean.

	Loops are allowed using "$(foreach identifier) ... $(end)". In this
	case, the identifier must be resolved to a collection and the loop body
	is generated as many times there is elements in the collection.
	Identifiers contained in the body are resolved against special
	dictionnaries associated with each collection element.

	Finally, notes that "$$" expression is reduceded to "$$".
	*)

(** Type of dictionnaries. *)
type dict_t = (string * value_t) list

(** Values of a dictionnary *)
and  value_t =
	  TEXT of (out_channel -> unit)						(** function called when identifier is found *)
	| COLL of ((dict_t -> unit) -> dict_t -> unit)		(** collection : argument function must be called for each element
															with a dictionnary fixed for the current element. *)
	| BOOL of (unit -> bool)							(** boolean value *)
	| FUN of (out_channel -> string -> unit)			(** function value *)


type state_t =
	| TOP
	| THEN
	| ELSE
	| FOREACH


(** Perform text evaluation (and function if any)
	@param out	Out channel.
	@param dict	Used dictionnary.
	@param id	Text identifier. *)
let do_text out dict id =
	let p =  try String.index id ':' with Not_found -> -1 in
	try
	
	(* function call *)
	if p >= 0 then
		let id = String.sub id 0 p in
		(match List.assoc id dict with
		  FUN f ->
			(try f out (String.sub id (p + 1) ((String.length id) - p - 1))
			with Not_found -> failwith ("error in generation with "^id))
		| _ -> failwith (id ^ " is not a text !"))
	
	(* simple variable eveluation *)	
	else
		match List.assoc id dict with
		  TEXT f ->
			(try f out
			with Not_found -> failwith (Printf.sprintf "uncaught Not_found in generation with \"%s\"" id))
		| _ -> failwith (id ^ " is not a text !")
	
	(* not existing symbol *)
	with Not_found ->
		List.iter (fun (n, _) -> Printf.printf "=>[%s]\n" n) dict;
		Printf.printf "<=[%s]\n" id;
		failwith (id ^ " is undefined !")


(** Get a collection.
	@param dict	Dictionnary to look in.
	@param id	Identifier.
	@return		Found collection function. *)
let do_coll dict id =
	try
		(match List.assoc id dict with
		  COLL f -> f
		| _ -> failwith (id ^ " is not a collection"))
	with Not_found ->
		failwith (id ^ " is undefined !")


(** Get a boolean value.
	@param dict		Dictionnary to look in.
	@param id		Identifier.
	@return			Boolean value. *)
let do_bool dict id =
	try
		(match List.assoc id dict with
		  BOOL f -> f ()
		| _ -> failwith (id ^ " is not a boolean"))
	with Not_found ->
		false



# 104 "templater.ml"
let __ocaml_lex_tables = {
  Lexing.lex_base = 
   "\000\000\246\255\247\255\001\000\001\000\001\000\255\255\002\000\
    \003\000\004\000\005\000\249\255\008\000\009\000\010\000\012\000\
    \011\000\022\000\023\000\024\000\000\000\254\255\016\000\017\000\
    \018\000\001\000\253\255\019\000\020\000\002\000\252\255\030\000\
    \057\000\059\000\060\000\003\000\063\000\004\000\251\255\250\255\
    \002\000\248\255\006\000\254\255\255\255\031\000\249\255\250\255\
    \034\000\022\000\255\255\011\000\006\000\002\000\004\000\008\000\
    \000\000\000\000\021\000\008\000\254\255\064\000\253\255\011\000\
    \028\000\034\000\005\000\252\255\028\000\035\000\008\000\251\255\
    \041\000\250\255\251\255\042\000\031\000\255\255\020\000\032\000\
    \024\000\023\000\037\000\042\000\041\000\037\000\133\000\254\255\
    \134\000\253\255\044\000\038\000\252\255";
  Lexing.lex_backtrk = 
   "\255\255\255\255\255\255\008\000\008\000\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\001\000\255\255\255\255\255\255\
    \255\255\002\000\255\255\255\255\255\255\003\000\255\255\255\255\
    \255\255\255\255\255\255\005\000\255\255\004\000\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \005\000\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\001\000\255\255\002\000\255\255\255\255\
    \255\255\255\255\003\000\255\255\255\255\255\255\004\000\255\255\
    \255\255\255\255\255\255\004\000\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255";
  Lexing.lex_default = 
   "\002\000\000\000\000\000\255\255\255\255\007\000\000\000\007\000\
    \007\000\007\000\007\000\000\000\007\000\007\000\007\000\007\000\
    \007\000\007\000\019\000\019\000\255\255\000\000\007\000\007\000\
    \007\000\255\255\000\000\007\000\007\000\255\255\000\000\007\000\
    \033\000\033\000\036\000\255\255\036\000\255\255\000\000\000\000\
    \255\255\000\000\043\000\000\000\000\000\047\000\000\000\000\000\
    \255\255\255\255\000\000\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\000\000\255\255\000\000\255\255\
    \255\255\255\255\255\255\000\000\255\255\255\255\255\255\000\000\
    \074\000\000\000\000\000\255\255\255\255\000\000\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\000\000\
    \255\255\000\000\255\255\255\255\000\000";
  Lexing.lex_trans = 
   "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\021\000\026\000\030\000\039\000\038\000\067\000\
    \044\000\060\000\071\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\018\000\
    \007\000\007\000\000\000\000\000\004\000\006\000\041\000\032\000\
    \060\000\005\000\255\255\011\000\011\000\011\000\011\000\003\000\
    \040\000\011\000\011\000\011\000\011\000\011\000\018\000\007\000\
    \007\000\011\000\011\000\025\000\011\000\029\000\032\000\011\000\
    \011\000\020\000\007\000\048\000\007\000\007\000\050\000\011\000\
    \007\000\062\000\049\000\066\000\070\000\075\000\077\000\092\000\
    \000\000\000\000\076\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\007\000\034\000\007\000\007\000\000\000\000\000\007\000\
    \062\000\057\000\011\000\058\000\035\000\035\000\009\000\010\000\
    \037\000\031\000\008\000\015\000\061\000\056\000\014\000\016\000\
    \022\000\054\000\023\000\017\000\012\000\024\000\055\000\063\000\
    \028\000\064\000\013\000\051\000\053\000\059\000\068\000\052\000\
    \065\000\069\000\090\000\027\000\078\000\080\000\088\000\081\000\
    \079\000\082\000\083\000\084\000\085\000\086\000\087\000\089\000\
    \091\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\087\000\089\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \001\000\255\255\255\255\255\255\255\255\255\255\255\255\000\000\
    \255\255\255\255\255\255\255\255\255\255\000\000\000\000\000\000\
    \255\255\255\255\255\255\255\255\255\255\000\000\255\255\255\255\
    \255\255\000\000\000\000\000\000\000\000\000\000\255\255\046\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\073\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\255\255\000\000\255\255\255\255\000\000\000\000\255\255\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000";
  Lexing.lex_check = 
   "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\020\000\025\000\029\000\035\000\037\000\066\000\
    \042\000\059\000\070\000\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\017\000\
    \018\000\019\000\255\255\255\255\000\000\004\000\040\000\031\000\
    \059\000\004\000\005\000\007\000\008\000\009\000\010\000\000\000\
    \003\000\012\000\013\000\014\000\016\000\015\000\017\000\018\000\
    \019\000\022\000\023\000\024\000\027\000\028\000\031\000\017\000\
    \018\000\019\000\032\000\045\000\033\000\034\000\048\000\031\000\
    \036\000\061\000\048\000\065\000\069\000\072\000\075\000\091\000\
    \255\255\255\255\075\000\255\255\255\255\255\255\255\255\255\255\
    \255\255\032\000\032\000\033\000\034\000\255\255\255\255\036\000\
    \061\000\056\000\032\000\057\000\033\000\034\000\005\000\005\000\
    \036\000\008\000\005\000\014\000\052\000\055\000\013\000\015\000\
    \009\000\053\000\009\000\016\000\010\000\023\000\054\000\051\000\
    \027\000\051\000\012\000\049\000\049\000\058\000\063\000\049\000\
    \064\000\068\000\078\000\022\000\076\000\076\000\079\000\080\000\
    \076\000\081\000\082\000\083\000\084\000\085\000\086\000\088\000\
    \090\000\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\086\000\088\000\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \000\000\005\000\007\000\008\000\009\000\010\000\042\000\255\255\
    \012\000\013\000\014\000\016\000\015\000\255\255\255\255\255\255\
    \022\000\023\000\024\000\027\000\028\000\255\255\017\000\018\000\
    \019\000\255\255\255\255\255\255\255\255\255\255\031\000\045\000\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\072\000\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\032\000\255\255\033\000\034\000\255\255\255\255\036\000\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255";
  Lexing.lex_base_code = 
   "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\001\000\000\000\004\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \002\000\003\000\004\000\000\000\005\000\000\000\023\000\015\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000";
  Lexing.lex_backtrk_code = 
   "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\004\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\015\000\000\000\023\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000";
  Lexing.lex_default_code = 
   "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\001\000\001\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \007\000\007\000\010\000\000\000\018\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000";
  Lexing.lex_trans_code = 
   "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
    \000\000\000\000\000\000\000\000\000\000\000\000";
  Lexing.lex_check_code = 
   "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\018\000\019\000\032\000\033\000\034\000\036\000\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \018\000\019\000\032\000\033\000\034\000\036\000\255\255\255\255\
    \255\255\018\000\019\000\032\000\033\000\034\000\036\000\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
    \018\000\019\000\032\000\033\000\034\000\036\000";
  Lexing.lex_code = 
   "\255\001\255\255\000\001\255\002\255\255\003\255\002\255\255\000\
    \002\255\002\255\003\255\255\000\003\255";
}

let rec scanner out dict state lexbuf =
  lexbuf.Lexing.lex_mem <- Array.make 4 (-1) ;   __ocaml_lex_scanner_rec out dict state lexbuf 0
and __ocaml_lex_scanner_rec out dict state lexbuf __ocaml_lex_state =
  match Lexing.new_engine __ocaml_lex_tables __ocaml_lex_state lexbuf with
      | 0 ->
# 108 "templater.mll"
   ( output_char out '$'; scanner out dict state lexbuf )
# 364 "templater.ml"

  | 1 ->
let
# 110 "templater.mll"
                            id
# 370 "templater.ml"
= Lexing.sub_lexeme lexbuf (lexbuf.Lexing.lex_start_pos + 10) lexbuf.Lexing.lex_mem.(0) in
# 111 "templater.mll"
   (
		let buf = Buffer.contents (scan_end (Buffer.create 1024) 0 lexbuf) in
		let f = do_coll dict id in
		f (fun dict -> scanner out dict FOREACH (Lexing.from_string buf)) dict;
		scanner out dict state lexbuf
	)
# 379 "templater.ml"

  | 2 ->
# 119 "templater.mll"
 ( () )
# 384 "templater.ml"

  | 3 ->
# 122 "templater.mll"
 (	if state = THEN then skip out dict 0 lexbuf
		else failwith "'else' out of 'if'" )
# 390 "templater.ml"

  | 4 ->
let
# 125 "templater.mll"
                          id
# 396 "templater.ml"
= Lexing.sub_lexeme lexbuf (lexbuf.Lexing.lex_start_pos + 6) lexbuf.Lexing.lex_mem.(0) in
# 126 "templater.mll"
 (	if not (do_bool dict id) then scanner out dict THEN lexbuf
		else skip out dict 0 lexbuf;
		scanner out dict state lexbuf )
# 402 "templater.ml"

  | 5 ->
let
# 130 "templater.mll"
                      id
# 408 "templater.ml"
= Lexing.sub_lexeme lexbuf (lexbuf.Lexing.lex_start_pos + 5) lexbuf.Lexing.lex_mem.(0) in
# 131 "templater.mll"
 (	if do_bool dict id then scanner out dict THEN lexbuf
		else skip out dict 0 lexbuf;
		scanner out dict state lexbuf )
# 414 "templater.ml"

  | 6 ->
let
# 135 "templater.mll"
                    id
# 420 "templater.ml"
= Lexing.sub_lexeme lexbuf (lexbuf.Lexing.lex_start_pos + 2) (lexbuf.Lexing.lex_curr_pos + -1) in
# 136 "templater.mll"
 ( do_text out dict id; scanner out dict state lexbuf )
# 424 "templater.ml"

  | 7 ->
# 139 "templater.mll"
 ( comment out dict state lexbuf )
# 429 "templater.ml"

  | 8 ->
let
# 141 "templater.mll"
       c
# 435 "templater.ml"
= Lexing.sub_lexeme_char lexbuf lexbuf.Lexing.lex_start_pos in
# 142 "templater.mll"
 ( output_char out c; scanner out dict state lexbuf )
# 439 "templater.ml"

  | 9 ->
# 145 "templater.mll"
 ( () )
# 444 "templater.ml"

  | __ocaml_lex_state -> lexbuf.Lexing.refill_buff lexbuf; 
      __ocaml_lex_scanner_rec out dict state lexbuf __ocaml_lex_state

and comment out dict state lexbuf =
    __ocaml_lex_comment_rec out dict state lexbuf 42
and __ocaml_lex_comment_rec out dict state lexbuf __ocaml_lex_state =
  match Lexing.engine __ocaml_lex_tables __ocaml_lex_state lexbuf with
      | 0 ->
# 149 "templater.mll"
  ( scanner out dict state lexbuf )
# 456 "templater.ml"

  | 1 ->
# 151 "templater.mll"
  ( comment out dict state lexbuf )
# 461 "templater.ml"

  | __ocaml_lex_state -> lexbuf.Lexing.refill_buff lexbuf; 
      __ocaml_lex_comment_rec out dict state lexbuf __ocaml_lex_state

and skip out dict cnt lexbuf =
    __ocaml_lex_skip_rec out dict cnt lexbuf 45
and __ocaml_lex_skip_rec out dict cnt lexbuf __ocaml_lex_state =
  match Lexing.engine __ocaml_lex_tables __ocaml_lex_state lexbuf with
      | 0 ->
# 155 "templater.mll"
   ( skip out dict cnt lexbuf )
# 473 "templater.ml"

  | 1 ->
# 157 "templater.mll"
   ( skip out dict (cnt + 1) lexbuf )
# 478 "templater.ml"

  | 2 ->
# 159 "templater.mll"
 ( skip out dict (cnt + 1) lexbuf )
# 483 "templater.ml"

  | 3 ->
# 161 "templater.mll"
 ( if cnt = 0 then () else skip out dict (cnt -1) lexbuf )
# 488 "templater.ml"

  | 4 ->
# 163 "templater.mll"
 (	if cnt = 0 then scanner out dict ELSE lexbuf
		else skip out dict cnt lexbuf )
# 494 "templater.ml"

  | 5 ->
# 166 "templater.mll"
 ( skip out dict cnt lexbuf )
# 499 "templater.ml"

  | 6 ->
# 168 "templater.mll"
 ( failwith "unclosed if" )
# 504 "templater.ml"

  | __ocaml_lex_state -> lexbuf.Lexing.refill_buff lexbuf; 
      __ocaml_lex_skip_rec out dict cnt lexbuf __ocaml_lex_state

and scan_end buf cnt lexbuf =
    __ocaml_lex_scan_end_rec buf cnt lexbuf 72
and __ocaml_lex_scan_end_rec buf cnt lexbuf __ocaml_lex_state =
  match Lexing.engine __ocaml_lex_tables __ocaml_lex_state lexbuf with
      | 0 ->
let
# 172 "templater.mll"
          s
# 517 "templater.ml"
= Lexing.sub_lexeme lexbuf lexbuf.Lexing.lex_start_pos (lexbuf.Lexing.lex_start_pos + 2) in
# 173 "templater.mll"
   ( Buffer.add_string buf s; scan_end buf cnt lexbuf )
# 521 "templater.ml"

  | 1 ->
let
# 174 "templater.mll"
                       s
# 527 "templater.ml"
= Lexing.sub_lexeme lexbuf lexbuf.Lexing.lex_start_pos (lexbuf.Lexing.lex_start_pos + 10) in
# 175 "templater.mll"
   ( Buffer.add_string buf s; scan_end buf (cnt + 1) lexbuf )
# 531 "templater.ml"

  | 2 ->
let
# 176 "templater.mll"
                  s
# 537 "templater.ml"
= Lexing.sub_lexeme lexbuf lexbuf.Lexing.lex_start_pos (lexbuf.Lexing.lex_start_pos + 5) in
# 177 "templater.mll"
 ( Buffer.add_string buf s; scan_end buf (cnt + 1) lexbuf )
# 541 "templater.ml"

  | 3 ->
let
# 178 "templater.mll"
              s
# 547 "templater.ml"
= Lexing.sub_lexeme lexbuf lexbuf.Lexing.lex_start_pos (lexbuf.Lexing.lex_start_pos + 6) in
# 179 "templater.mll"
 ( if cnt = 0 then buf
	else (Buffer.add_string buf s; scan_end buf (cnt - 1) lexbuf) )
# 552 "templater.ml"

  | 4 ->
let
# 181 "templater.mll"
       c
# 558 "templater.ml"
= Lexing.sub_lexeme_char lexbuf lexbuf.Lexing.lex_start_pos in
# 182 "templater.mll"
 ( Buffer.add_char buf c; scan_end buf cnt lexbuf )
# 562 "templater.ml"

  | 5 ->
# 184 "templater.mll"
 ( failwith "unclosed foreach" )
# 567 "templater.ml"

  | __ocaml_lex_state -> lexbuf.Lexing.refill_buff lexbuf; 
      __ocaml_lex_scan_end_rec buf cnt lexbuf __ocaml_lex_state

;;

# 187 "templater.mll"
 
(** Perform a template generation.
	@param dict		Dictionnary to use.
	@param in_path	Input template path.
	@param out_path	Path of the output file. *)
let generate_path dict in_path out_path =
	let output = open_out out_path in
	let input = open_in in_path in
	scanner output dict TOP (Lexing.from_channel input);
	close_in input;
	close_out output


(** Perform a template generation.
	@param dict		Dictionnary to use.
	@param template	Template name (take from SOURCE_DIRECTORY/templates)
	@param out_path	Path of the output file. *)
let generate dict template out_path =
	generate_path dict (Irg.native_path (Config.source_dir ^ "/templates/" ^ template)) out_path

# 595 "templater.ml"
