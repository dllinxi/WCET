(** A module for computing {b balancing informations} about a program.
    Such computation takes as input:
    {ul
    {- The abstract syntax tree as computed by FrontC }
    {- Flow facts as computed by oRange }
    }
    The computation outputs the list of {e conditional statements} of the program
    together with a cost information about {e each possible branch}.
    This list is {e sorted} according to the difference of cost between the lightest and heaviest branch.
    @author Pascal Sotin
    @author Jakob Zwirchmayr
*)

(** {3 Utilities} *)

(** {7 General utilities} *)

let rec extract f = function
  | [] -> raise Not_found
  | e::r -> match f e with
    | None -> extract f r
    | Some x -> x

let rec min_max f = function
  | [] -> raise (Invalid_argument "empty list")
  | [x] -> f x, f x 
  | x::r -> let min_r, max_r = min_max f r in
            min (f x) min_r, max (f x) max_r

let rec fold1 f = function
  | [] -> raise (Invalid_argument "empty list")
  | [x] -> x
  | x::r -> f x (fold1 f r)

(** {7 FrontC utilities} *)

let function_def (defs: Cabs.definition list) (fname: string) =
  let body = extract (function
    | Cabs.FUNDEF ((_,_,(name,_,_,_)),(_,body))
    | Cabs.OLDFUNDEF ((_,_,(name,_,_,_)),_,(_,body)) when name = fname -> Some body
    | _ -> None) defs
  in body

let print_expr fmt (expr: Cabs.expression) =
  Cprint.current := "";
  Cprint.current_len := 0;
  Cprint.line := "";
  Cprint.line_len := 0;
  Cprint.print_expression expr ~-1;
  Cprint.commit ();
  Format.pp_print_string fmt !Cprint.line

let output_expr_ffx oc (expr: Cabs.expression) =
  let str = Format.asprintf "%a" print_expr expr in
  output_string oc (Coutput.pcdata_from_string str)

let condition_test (stmt: Cabs.statement) =
  match stmt with
  | Cabs.IF (e,_,_)
  | Cabs.SWITCH (e,_) -> e
  | _ -> raise (Invalid_argument "not a condition")

let direct_location (stmt: Cabs.statement) =
  match stmt with
  | Cabs.STAT_LINE (_,f,l) -> Some (f,l)
  | _ -> None

(** {3 Types} *)

(** The flow facts requiered for the balancing analysis. *)
type ff_input = {
  loop_bound: Cabs.statement -> int option;
(**
   Provide information about a given loop.
   {ul
   {- Returns [Some n]: each time the loop is reached, it iterates {e at most} [n] times}
   {- Returns [None]: no loop bound could be computed for the loop}
   {- Raises [Not_found]: the loop statement does not exist in the flow facts}
   {- Raises [Invalid_argument]: the parameter must be [FOR], [WHILE] or [DOWHILE]}
   } *)
  branch_feasibility: Cabs.statement -> bool list;
(**
   Provide information about a given conditional.
   {ul
   {- Returns [ [b1, b2...] ]: the {e feasibility} of the first, second... branches of the conditional are respectively [b1], [b2]...}
   {- Raises [Not_found]: the conditional statement does not exist in the flow facts}
   {- Raises [Invalid_argument]: the parameter must be [IF] or [CASE]}
   }
*)
  branch_id: Cabs.statement -> string;
(**
   Associate a conditional a statement to its oRange identifier.
   "IF-?" is used when no identifier is known.
*)
}

(** Location in the source code. *)
module Loc = struct
  (** A location in the source code. *)
  type t = { file: string; (** File name. *)
	     line: int;    (** Line number. *)
	   }

  (** Uses a Cabs.STAT_LINE statement to build a location. *)
  let of_stat_line = function
    | Cabs.STAT_LINE (_,f,l) -> { file = f; line = l }
    | _ -> raise (Invalid_argument "not a STAT_LINE")
  (** Prints a location. *)
  let print fmt loc = Format.fprintf fmt "%s:%d" loc.file loc.line
  let output_ffx oc loc = Printf.fprintf oc " source=\"%s\" line=\"%d\"" loc.file loc.line
  let of_statement s = match direct_location s with
    | None -> None
    | Some (f,l) -> Some { file = f; line = l }
end

(** Tracks contextual information. *)
module Ctx = struct
  (** Type of the contextual information. *)
  type t = {
    cur: Loc.t option; (** A location when fresh enough. *)
    fname: string;     (** The enclosing function name. *)
  }

  (** Type indicating the distance between a context and a descendant of it. *)
  type distance =
  | Close    (** Denotes that the location is still valid. *)
  | Distant  (** Denotes that the location cannot be inherited from the context. *)

  (** Prints a location option. None means "unknown location". *)
  let print fmt (ctx: t) = Format.fprintf fmt "at %a in %s"
    (fun fmt -> function
    | None -> Format.pp_print_string fmt "unknown location"
    | Some loc -> Loc.print fmt loc) ctx.cur
    ctx.fname
  let output_ffx oc ctx =
    begin match ctx.cur with
    | None -> ()
    | Some loc -> Loc.output_ffx oc loc
    end
    
  (** The empty context. *)
  let empty = { cur = None; fname = "<toplevel>" }
  (** Updating the context by a function call. *)
  let through_function_call (ctx: t) (fname: string) = { cur = None; fname = fname }
  (** Updating the context by descending in a statement.
      This might invalidate the location or not. *)
  let through_located_statement (dist: distance) (ctx: t) (stmt: Cabs.statement) =
    let latest_loc = match dist with
      | Close -> ctx.cur
      | Distant -> None in
    match stmt with
    | Cabs.STAT_LINE _ -> {ctx with cur = Some (Loc.of_stat_line stmt) }
    | _ -> { ctx with cur = latest_loc }
  (** Descend and invalidate the location. *)
  let through_statement = through_located_statement Distant
  (** Descend and keep the location valid. *)
  let through_close_statement = through_located_statement Close
end

(** A module for descending in the Abstract Syntax Tree while keeping a context. *)
module CtxNode = struct
  (** The type of a node in the contextual Abstract Syntax Tree. *)
  type t = {
    statement: Cabs.statement; (** The statement carried by the node.
			           Corresponds to a fragment of the original Abstract Syntax Tree. *)
    context: Ctx.t;            (** The context in which this statement was reached.
				   Due to the presence of functions, the complete
				   Abstract Syntax Tree is a Directed Acyclic Graph and a node can
				   be reached through different contexts. *)
  }

  (** Retrives the statement. *)
  let statement n = n.statement
  (** Retrives the context. *)
  let context n = n.context
  (** Provide the context obtained if we descend through the node. *)
  let descent_context (n: t) = Ctx.through_statement n.context n.statement
  (** Gives the node reached after descending in a given statement. *)
  let descent (n: t) (stmt: Cabs.statement) =
    { statement = stmt;
      context = descent_context n
    }
  (** Gives the node reached after descending in a given close statement.
      The location of the father is kept for the son. *)
  let close_descent (n: t) (stmt: Cabs.statement) =
    { statement = stmt;
      context = Ctx.through_close_statement n.context n.statement;
    }
  (** Gives the initial node of a function starting in a given context. *)
  let function_call (ctx: Ctx.t) (fname: string) (body: Cabs.statement) =
    { statement = body;
      context = Ctx.through_function_call ctx fname;
    }
  let loop_descent (n: int) (node: t) (stmt: Cabs.statement) =
    let ctx = descent_context node in
    { statement = stmt;
      context = ctx;
    }
  let output_ffx oc n =
    let ct = Ctx.output_ffx oc n.context in
    ct, condition_test n.statement
end

(** Code "weight".
    Encoded by an integer.
    A high value is a hint of a high contribution to the WCET. *)
type weight = int

(** Evaluation of a condition. *)
module CondEval = struct
  (** Result of the analysis of a conditional. *)
  type t = {
    condition: CtxNode.t;                (** The statement in the abstract syntax tree. Only [Cabs.IF] or [Cabs.SWITCH] statements. *)
    id: string;                          (** Orange internal identifier. *)
    branches: (string * weight * Loc.t option) list;
                                         (** List of the feasible branches together with their respective weight.
					     Must be non-empty. *)
  }
  let compare = Pervasives.compare
  let delta (n,ce) =
    let low, high = min_max (fun (_,w,_) -> w) ce.branches in
    n * (high - low)
  let delta_compare a b = (delta b) - (delta a)
  let make node id brs = { condition = node; id = id; branches = brs }
  let delta_print fmt (n,cond) = Format.fprintf fmt "Delta %d %a : %a // %a"
    (delta (n,cond))
    Ctx.print (CtxNode.context cond.condition)
    (fun fmt -> List.iter (fun (br,w,_) -> Format.fprintf fmt "%s=%d; " br w)) cond.branches
    print_expr (condition_test (CtxNode.statement cond.condition))
  let output_delta_ffx oc pad (n,ce) =
    let d = delta (n,ce) in
    Printf.fprintf oc "%s<conditional id=\"%s\"" pad ce.id;
    let (), expr = CtxNode.output_ffx oc ce.condition in
    Printf.fprintf oc " delta=\"%d\">\n" d;
    Printf.fprintf oc "%s  <condition expression=\"%a\" count=\"%d\"/>\n" pad output_expr_ffx expr n;
    List.iter (fun (br,w,ol) ->
      Printf.fprintf oc "%s  <case cond=\"%c\"" pad (match br with "then" -> '1' | "else" -> '0' | _ -> '?');
      begin match ol with
      | None -> ()
      | Some l -> Loc.output_ffx oc l end;
      Printf.fprintf oc " weight=\"%d\"/>\n" w) ce.branches;
    Printf.fprintf oc "%s</conditional>\n" pad
end

(** Sets of deltas *)
module CondSet = struct
  module Count = TMap.Make(CondEval)
  type t = int Count.t
  let empty = Count.make 0
  let add = Count.combine ( + )
  let scale n = Count.map (fun x -> x * n)
  let max = Count.combine max (* Trickier if keys partially overlap, eg. contextual delta *)
  let add_cond s ce = Count.update (fun x -> x + 1) ce s
  let cardinal = Count.special_count
  let to_list s = Count.fold_special (fun x n acc -> (n,x)::acc) s []
end

(** Evaluation of a code fragment. *)
module CodeEval = struct
  (** The total weight of the code fragment (including the functions called). *)
  type t = weight * CondSet.t
  let of_int n = n,CondSet.empty
  let zero = of_int 0
  let one = of_int 1
  let add (a,sa) (b,sb) = a+b,CondSet.add sa sb
  let scale n (a,s) = n*a,CondSet.scale n s
  let max (a,sa) (b,sb) = max a b,CondSet.max sa sb
  (** Evaluation of a loop, providing the weights of the body and header
      together with a bound on the number of iterations. *)
  let of_loop ~header:h ~count:n ~body:b = add h (scale n (add h b))
  (** Evaluation of a set of alternatives (at least one). *)
  let of_alternatives n id br =
    let w,s = fold1 max (snd (List.split br)) in
    w, CondSet.add_cond s (CondEval.make n id (List.map (fun ((name,ol),(w,_)) -> name,w,ol) br))
  (** Sum of weights *)
  let sum = List.fold_left add zero
  let weight = fst
  let cond_count (_,s) = CondSet.cardinal s
  let sorted_conds (_,s) =
    let l = CondSet.to_list s in
    List.sort CondEval.delta_compare l
end

(** {3 Main algorithm} *)

(** Relying on some flow facts and the FrontC Abstract Syntax Trees of the program, this function
    computes the conditional constructs reachable from a given entry function,
    from the most unbalanced ones to the most balanced. *)
let analysis
    (ff: ff_input)
    (defs: Cabs.definition list)
    (entry: string)
    :
    (string * CodeEval.t)
    =

  (* Three mutually recursive functions : expr -> function -> statement -> expr.
     The evaluation of the functions are cached.
     The conditions met are registered. *)

  (* Cache for the function analysis. *)
  let finfo = Hashtbl.create 10 in
  (* Analysis of a function. *)
  let rec fun_eval (ctx: Ctx.t) (fname: string) : CodeEval.t
      =
    try Hashtbl.find finfo (ctx,fname)
    with Not_found -> try 
      let body = 	
	function_def defs fname in
      let eval = stmt_eval (CtxNode.function_call ctx fname body) in
      Hashtbl.add finfo (ctx,fname) eval;
      eval
    with Not_found -> CodeEval.of_int 10 (* Default weight when function not found failwith (fname^" is not a known function") in*)
  

  (* Analysis of an expression. *)
  and expr_eval (ctx: Ctx.t) (expr: Cabs.expression) : CodeEval.t
      =
    let eval = expr_eval ctx in (* Evaluation in the same context. *)
    let open Cabs in
    let open CodeEval in
    match expr with
    | NOTHING -> zero
    | UNARY (_,e) -> add one (eval e)
    | BINARY (_,e1,e2) -> sum [one; eval e1; eval e2]
    | QUESTION (e_if,e_then,e_else) -> add (eval e_if) (max (eval e_then) (eval e_else))
    | CALL (VARIABLE fname,el) ->
      let args = CodeEval.sum (List.map eval el) in
      let calling = of_int (List.length el) in
      let execution = fun_eval ctx fname in
      sum [args; calling; execution]
    | CALL _ -> failwith "cannot resolve the call"
    | CONSTANT _ -> zero
    | VARIABLE _ -> one
    | EXPR_SIZEOF _
    | TYPE_SIZEOF _
    | CAST _ -> zero
    | COMMA el -> CodeEval.sum (List.map eval el)
    | INDEX (e1,e2) -> sum [one; eval e1; eval e2]
    | MEMBEROF (e,_) -> eval e
    | MEMBEROFPTR (e,_) -> add one (eval e)
    | EXPR_LINE (e,_,_) -> eval e
    | GNU_BODY _ -> failwith "cannot handle assembly code"
  (* WARNING: the evaluation of expressions is far from realistic. *)

  (* Analysis of a statement (and substatements). *)
  and stmt_eval (node: CtxNode.t) : CodeEval.t
      =
    let stmt = CtxNode.statement node in
    let expr_eval_here e = expr_eval (CtxNode.context node) e in
    let expr_eval_under e = expr_eval (CtxNode.descent_context node) e in
    let eval s = stmt_eval (CtxNode.descent node s) in
    let eval_close s = stmt_eval (CtxNode.close_descent node s) in
    let eval_multi s n = stmt_eval (CtxNode.loop_descent n node s) in
    let open Cabs in
    let open CodeEval in
    match stmt with
    | STAT_LINE (sub,_,_) -> eval sub
    | COMPUTATION e
    | RETURN e -> expr_eval_here  e
    | NOP -> zero
    | BLOCK (_,sub) -> eval_close sub (* WARNING : initialisation of the declared variables is not taken into account. *)
    | SEQUENCE (sub1,sub2) ->
      let eval1 = eval_close sub1 in
      let eval2 = eval sub2 in
      add eval1 eval2
    | IF (e,s_then,s_else) ->
      let we = expr_eval_here e in

      let branches = List.concat 
            (List.map2 
                (fun (br,sub) feasible ->
                    if not feasible then []
                    else [(br,Loc.of_statement sub),eval sub])
                    [("then",s_then); ("else",s_else)] (ff.branch_feasibility stmt)
            ) 
      in
      add we (CodeEval.of_alternatives node (ff.branch_id stmt) branches)
    | FOR (e_init,e_test,e_end,s_body) ->
        let wi = expr_eval_here e_init in
        let wt = expr_eval_under e_test in
        begin match ff.loop_bound stmt with
        | Some 0 -> add wi wt
        | Some n ->
          	let wbody = eval_multi s_body n in
          	let we = expr_eval_under e_end in
          	add wi (CodeEval.of_loop ~header:we ~count:n ~body:wbody)
        | None -> failwith "Unbounded for loop" end
    | WHILE (e,s_body) ->
        let we = expr_eval_under e in
        begin match ff.loop_bound stmt with
          | Some 0 -> we
          | Some n ->
              let eval_body = eval_multi s_body n in
              CodeEval.of_loop ~header:we ~count:n ~body:eval_body
          | None -> failwith "Unbounded while loop" end
    | BREAK ->
        (* HACKHACKHACK: fix breaks -- the count for 3 ops so decrease 2 *) zero
    | _ -> failwith "TODO"

  in
  (* Performing the analysis. *)
  let eval = fun_eval Ctx.empty entry in
  (* Synthesizing the list of conditionals. *)
  entry,eval

module Output = struct
  let to_ffx
      (oc : out_channel)
      ((entry_point, eval) : string * CodeEval.t)
      : unit
      =
    let conds = CodeEval.sorted_conds eval in
    let print = output_string oc in
    print "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
    print "<flowfacts>\n";
    Printf.fprintf oc "  <function name=\"%s\" weight=\"%d\">\n" entry_point (CodeEval.weight eval);
    List.iter (CondEval.output_delta_ffx oc "    ") conds;
    print "  </function>\n";
    print "</flowfacts>\n";
    
end
