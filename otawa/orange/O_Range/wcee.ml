(** Worst Case Execution Estimation.
    Estimate the worst case cost of execution of a source code in term of:
    {ul
    {- Operations
      {ul
      {- Simple (+, -, %, >>, ==...) }
      {- Multiplication }
      {- Division }
      }
    }
    {- Controls
      {ul
      {- Conditionnal branching }
      {- Unconditionnal branching }
      {- Computed branching }
      {- Method call }
      {- Method return }
      }
    }
    {- Memory accesses
      {ul
      {- Immediate }
      {- Load }
      {- Store }
      }
    }
    }

    The categories have been discussed during a IRIT/Continental meeting on November 24th 2014.
    On the source code, we assumed that:
    {ul
    {- there is no "goto" }
    {- switch cases are always broken at the end of the case }
    }
    This module relies on a FrontC abstract syntax tree.

    @author Pascal Sotin
*)

(** {3 Categories of costs} *)

(** Families of cost. *)
module CostFamily = struct
  type t = Operation | Control | Memory
  let print fmt f = Format.pp_print_string fmt (match f with
    | Operation -> "operation"
    | Control -> "control"
    | Memory -> "memory")
  let compare = Pervasives.compare
end

(** Cost items. Each cost item belongs to a family of cost. *)
module CostItem = struct
  type t =
  | SimpleOp
  | Mult
  | Div
  | CondBr
  | UncondBr
  | CalcBr
  | Call
  | Return
  | Address
  | Load
  | Store
  | UnknownCode
  let print fmt i = Format.pp_print_string fmt (match i with
    | SimpleOp -> "SimpleOp" | Mult -> "Mult" | Div -> "Div"
    | CondBr -> "CondBr" | UncondBr -> "UncondBr" | CalcBr -> "CalcBr" | Call -> "Call" | Return -> "Return"
    | Address -> "Address" | Load -> "Load" | Store -> "Store" | UnknownCode -> "UnknowCode")
  let compare = Pervasives.compare
  let family = function
    | SimpleOp | Mult | Div | UnknownCode -> CostFamily.Operation
    | CondBr | UncondBr | CalcBr | Call | Return -> CostFamily.Control
    | Address | Load | Store -> CostFamily.Memory
  let cycle_range = function
    | SimpleOp -> (0.5, 1.)
    | Mult -> (1., 5.)
    | Div -> (1., 10.)
    | CondBr -> (1., 1.)
    | UncondBr -> (0.5, 1.)
    | CalcBr -> (1., 4.)
    | Call -> (0.5, 10.)
    | Return -> (0.5, 10.)
    | Address -> (0.5, 1.)
    | Load -> (2., 20.)
    | Store -> (2., 20.)
    | UnknownCode -> (0., 1000.)
  let all = [SimpleOp; Mult; Div; CondBr; UncondBr; CalcBr; Call; Return; Address; Load; Store; UnknownCode]
end

(** {3 Counting operations} *)

(** Counts the number occurence of each cost item. *)
module ItemCount = struct
  module IMap = TMap.Make(CostItem)
  type t = int IMap.t

  (** {7 Adding to a count} *)

  let add = IMap.combine ( + )
  let add_list l e = List.fold_right (fun (count,item) acc ->
    IMap.update (fun n -> n + count) item acc) l e
  let add_items items = add_list [items]
  let add_one_item item = add_items (1,item)

  (** {7 Basic constructions} *)

  let nothing = IMap.make 0
  let is_nothing = IMap.has_no_special
  let list l = add_list l nothing
  let items items = add_items items nothing
  let one_item item = add_one_item item nothing

  (** Common part of two counts. *)
  let common = IMap.combine min

  (** Removes the first count from the second. *)
  let remove = IMap.combine (fun b a ->
    let rem = a - b in
    if rem < 0 then failwith "negative" else rem)

  let multiplyBy n =
    assert (n >= 0);
    IMap.map (fun x -> n * x)

  (** {7 From count to evaluation} *)

  let eval cost ct = IMap.fold_special (fun item nb acc -> acc +. (float_of_int nb) *. (cost item)) ct 0.
  let cheap_eval = eval (fun item -> 0.1 *. fst (CostItem.cycle_range item))
  let expen_eval = eval (fun item -> 10. *. snd (CostItem.cycle_range item))

  (** Partial ordering of two item counts. *)
  let order ct1 ct2 =
    let ctb = common ct1 ct2 in
    let ct1rem = remove ctb ct1 in
    let ct2rem = remove ctb ct2 in
    let open Maxima.PosetOrder in
    match is_nothing ct1rem, is_nothing ct2rem with (* point-wise ordering *)
    | true, true -> EQ
    | true, false -> LT
    | false, true -> GT
    | false, false ->
      match expen_eval ct1rem < cheap_eval ct2rem, expen_eval ct2rem < cheap_eval ct1rem with (* possible-costs-based ordering *)
      | true, true -> EQ
      | true, false -> LT
      | false, true -> GT
      | false, false -> UN
      
  (** Greatest lower bound. Maybe an approximation. *)
  let glb = common

  (** Least upper bound. *)
  let lub = IMap.combine max

  (** Pretty printer. *)
  let print = IMap.print CostItem.print Format.pp_print_int

  (** Full printing. *)
  let print_list = IMap.print_list ~first:"" ~firstbind:">> " ~last:"" ~sep:"@\n" CostItem.print CostItem.all Format.pp_print_int
end

(** Evaluation of a piece of code. *)
module Footprint = struct
  module MaxCount = Maxima.Make(ItemCount)

  type t = MaxCount.t
  let nothing = MaxCount.just ItemCount.nothing
  let one_item it = MaxCount.just (ItemCount.one_item it)
  let items items = MaxCount.just (ItemCount.items items)
  let list l = MaxCount.just (ItemCount.list l)
  let add mc1 mc2 =
    let mc = MaxCount.combine ~iso1:true ~iso2:true ItemCount.add mc1 mc2 in
    mc
  let add_all l = List.fold_left add nothing l
  let max = MaxCount.max
  let max_all = function
    | [] -> failwith "empty"
    | x::r -> List.fold_left max x r
  let decomp mc =
    let common = MaxCount.glb mc in
    let rem = MaxCount.map ~iso:true (ItemCount.remove common) mc in
    common, rem

  let worst_case mc = MaxCount.lub mc

  let multiplyBy n = MaxCount.map (ItemCount.multiplyBy n)

  let of_loop ~header:h ~count:n ~body:b =
    let h = add h (one_item CostItem.CondBr) in
    let b = add b (one_item CostItem.UncondBr) in
    let one_loop = add b h in
    let all_loops = multiplyBy n one_loop in
    add h all_loops

  let print fmt mc =
    let common, remainders = decomp mc in
    Format.printf "Basis: %a@\n" ItemCount.print common;
    List.iteri (fun i rem ->
      Format.printf "Comp %d: %a@\n" (i+1) ItemCount.print rem) remainders

  let size = MaxCount.cardinal
end

(** {3 Main algorithm} *)

type dataloc = Reg | Mem

(** Relying on some flow facts and the FrontC Abstract Syntax Trees of the program,
    this function computes the worst case execution evaluation of the program. *)
let analysis
    (ff: Balance.ff_input)
    (defs: Cabs.definition list)
    (entry: string)
    :
    Footprint.t
    =
  
  let rec fun_eval (fname: string) : Footprint.t =
    Format.printf "WCEE: starting evaluation of function %s@\n@?" fname;
    let bodyCost = try stmt_eval (Balance.function_def defs fname)
      with Not_found -> Footprint.one_item CostItem.UnknownCode in
    let callCost = Footprint.list [(1,CostItem.Call); (1,CostItem.Return)] in
    let res = Footprint.add bodyCost callCost in
    Format.printf "%a@\nWCEE: ending evaluation of function %s with %d maximal elements@\n@?" Footprint.print res fname (Footprint.size res);
    res
      
  and access mempen e =
    let (ecost,loc) = expr_eval e in
    match loc with
    | Reg -> ecost
    | Mem -> Footprint.add mempen ecost
  and read e = access (Footprint.one_item CostItem.Load) e
  and write e = access (Footprint.one_item CostItem.Store) e
  and read_write e = access (Footprint.list [(1,CostItem.Load); (1, CostItem.Store)]) e

  and expr_eval (expr: Cabs.expression) : Footprint.t * dataloc =
    let open Cabs in
    let open Footprint in
    match expr with
    | NOTHING -> nothing, Reg
    | UNARY (op,e) ->
      begin match op with
	| MINUS | PLUS | NOT | BNOT -> add_all [read e; one_item CostItem.SimpleOp], Reg
	| MEMOF -> read e, Mem
	| ADDROF -> nothing, Reg
	| PREINCR | PREDECR | POSINCR | POSDECR -> add_all [read_write e; one_item CostItem.SimpleOp], Reg
      end
    | BINARY (op,e1,e2) -> begin
      match op with
      | ADD | SUB
      | AND | OR
      | BAND | BOR | XOR | SHL | SHR
      | EQ | NE | LT | GT | LE | GE
	-> add_all [read e1; read e2; one_item CostItem.SimpleOp], Reg
      | MUL -> add_all [read e1; read e2; one_item CostItem.Mult], Reg
      | DIV | MOD -> add_all [read e1; read e2; one_item CostItem.Div], Reg
      | ASSIGN -> add_all [write e1; read e2], Reg
      | ADD_ASSIGN | SUB_ASSIGN
      | BAND_ASSIGN | BOR_ASSIGN | XOR_ASSIGN | SHL_ASSIGN | SHR_ASSIGN
	-> add_all [read_write e1; read e2; one_item CostItem.SimpleOp], Reg
      | MUL_ASSIGN -> add_all [read_write e1; read e2; one_item CostItem.Mult], Reg
      | DIV_ASSIGN | MOD_ASSIGN -> add_all [read_write e1; read e2; one_item CostItem.Div], Reg
    end
    | QUESTION (e_if,e_then,e_else) ->
      let c_then = add (read e_then) (list [(1,CostItem.CondBr); (1,CostItem.UncondBr)]) in
      let c_else = add (read e_else) (list [(1,CostItem.CondBr)]) in
      add_all [read e_if; max c_then c_else], Reg
    | CAST (_,e) -> read e, Reg
    | CALL (VARIABLE fname,el) ->
      let args = Footprint.add_all (List.map read el) in
      let calling = items (Pervasives.max 0 ((List.length el) - 4), CostItem.Store) in
      let execution = fun_eval fname in
      if false then Format.printf "Execution of %s evaluates to:@\n%a@\n" fname Footprint.print execution;
      add_all [args; calling; execution], Reg
    | CALL _ -> failwith "cannot resolve the call"
    | VARIABLE _ -> one_item CostItem.Address, Reg (* All local assumption. *)
    | CONSTANT (CONST_INT "0")
    | CONSTANT (CONST_INT "1") -> nothing, Reg
    | CONSTANT _ -> one_item CostItem.Address, Reg
    | EXPR_SIZEOF _
    | TYPE_SIZEOF _ -> nothing, Reg
    | INDEX (t,i) -> expr_eval (UNARY (MEMOF,(BINARY (ADD,t,i))))
    | MEMBEROF (e,_) -> expr_eval e
    | MEMBEROFPTR (e,_) -> expr_eval (UNARY (MEMOF,e))
    | EXPR_LINE (e,_,_) -> expr_eval e
    | COMMA le -> add_all (List.map read le), Reg
    | GNU_BODY _ -> failwith "cannot handle assembly code"

  and stmt_eval (stmt: Cabs.statement) : Footprint.t =
    let eval = stmt_eval in
    let open Footprint in
    let open Cabs in
    match stmt with
    | STAT_LINE (sub,_,_) -> eval sub
    | COMPUTATION e
    | RETURN e -> read e
    | NOP -> nothing
    | BLOCK (_,sub) -> eval sub (* WARNING: initialisation of the declared variables is not taken into account. *)
    | SEQUENCE (sub1,sub2) -> add (eval sub1) (eval sub2)
    | IF (e,s_then,s_else) ->
      let branch_cost (s_branch, extra) feasible =
	if not feasible
	then nothing
	else add_all ((eval s_branch)::extra) in
      add (read e) (max_all
		      (List.map2 branch_cost
			 [s_then, [one_item CostItem.CondBr; one_item CostItem.UncondBr];
			  s_else, [one_item CostItem.CondBr]]
			 (ff.Balance.branch_feasibility stmt)))
    | SWITCH (e,sub) ->
      add (read e) (eval sub) (* No break taken into account *)
    | CASE (_,sub) -> eval sub
    | DEFAULT sub -> eval sub
    | FOR (e_init,e_test,e_end,s_body) ->
      let wi = read e_init in
      let wt = read e_test in
      let we = read e_end in
      let wb = eval s_body in
      begin match ff.Balance.loop_bound stmt with
      | Some n ->
	add wi (of_loop ~header:wt ~count:n ~body:(add_all [wb; we]))
      | None -> failwith "Unbounded for loop" end
    | WHILE (e_test,s_body) ->
      let wt = read e_test in
      let wb = eval s_body in
      begin match ff.Balance.loop_bound stmt with
      | Some n -> of_loop ~header:wt ~count:n ~body:wb
      | None -> failwith "Unbounded while loop" end
    | DOWHILE (e_test,s_body) ->
      let wt = read e_test in
      let wb = eval s_body in
       begin match ff.Balance.loop_bound stmt with
      | Some n -> of_loop ~header:nothing ~count:(n+1) ~body:(add wb wt)
      | None -> failwith "Unbounded do while loop" end
    | BREAK -> nothing (* ignored *)
    | CONTINUE -> nothing (* ignored *)
    | GOTO _ -> failwith "unhandled statement: GOTO"
    | LABEL (_,sub) -> eval sub
    | ASM _ -> failwith "unhandled statement: ASM"
    | GNU_ASM _ -> failwith "unhandled statement: GNU ASM"
    | GNU_ASM_VOLATILE _ -> failwith "unhandled statement: GNU ASM VOLATILE"

  in
  (* Performing the analysis. *)
  Format.printf "Starting the worst-case execution estimation analysis@\n@?";
  let eval = fun_eval entry in
  Format.printf "Estimated cost for the function:@\n%a@\n" Footprint.print eval;
  let wc = Footprint.worst_case eval in
  Format.printf "Worst case detail:@\n%a@\n" ItemCount.print_list wc;
  Format.printf ">> Optimistic evaluation => %d@\n" (int_of_float (ItemCount.cheap_eval wc));
  Format.printf ">> Pessimistic evaluation => %d@\n" (int_of_float (ItemCount.expen_eval wc));
  eval
