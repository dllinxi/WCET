(** Condition analysis.
    Extracts interval constraints on the values of the variables of the program assuming
    that a given condition (FrontC expression without side-effect) holds.
*)

(** Nature of the constrained data. Assumed to be only integers. *)
module Data = struct
  type t = int
  let leq a b = a <= b
  let min_value = min_int
  let max_value = max_int
end

(** Extension of the data with a lowest and highest value. *)
module Bound = struct
  type t =
  | Below
  | Data of Data.t
  | Above
  let leq a b = match (a,b) with
    | _, Below
    | Above,_ -> false
    | Below, _
    | _,Above -> true
    | Data da, Data db -> Data.leq da db
  let min a b = if leq a b then a else b
  let max a b = if leq a b then b else a
  let to_data = function
    | Below -> Data.min_value
    | Data x -> x
    | Above -> Data.max_value
end

(** Intervals on the data (bounded or unbounded).
    No representation is given for the empty interval. *)
module Interval = struct
  type t = { low : Bound.t; high : Bound.t }
  exception Bottom
  (** Triggered when building an empty interval. *)
  let make low high =
    if Bound.leq low high
    then { low = low; high = high }
    else raise Bottom
  let unbounded = make Bound.Below Bound.Above
  let lower_bounded l = make (Bound.Data l) Bound.Above
  let upper_bounded h = make Bound.Below (Bound.Data h)
  let bounded l h = make (Bound.Data l) (Bound.Data h)
  let singleton x = make (Bound.Data x) (Bound.Data x)
  let join a b = make (Bound.min a.low b.low) (Bound.max a.high b.high)
  let meet a b = make (Bound.max a.low b.low) (Bound.min a.high b.high)
  let is_unbounded i = i = unbounded
  let to_data_range i = Bound.to_data i.low, Bound.to_data i.high
end

(** Expressions as Map keys. EXPR_LINE are stripped. *)
module Expr = struct
  type t = Make of Cabs.expression
  let compare (Make e1) (Make e2) = Pervasives.compare e1 e2
  let make expr =
    let rec annot_free expr =
      let rf = annot_free in
      let open Cabs in
      match expr with
      | NOTHING -> NOTHING
      | UNARY (o,e) -> UNARY (o,rf e)
      | BINARY (o,e1,e2) -> BINARY (o,rf e1,rf e2)
      | QUESTION (e1,e2,e3) -> QUESTION (rf e1,rf e2,rf e3)
      | CAST (t,e) -> CAST (t,rf e)
      | CALL (e,args) -> CALL(rf e, List.map rf args)
      | CONSTANT _ -> expr
      | COMMA el -> COMMA (List.map rf el)
      | EXPR_SIZEOF e -> EXPR_SIZEOF (rf e)
      | VARIABLE _ -> expr
      | TYPE_SIZEOF _ -> expr
      | INDEX (e1,e2) -> INDEX (rf e1,rf e2)
      | MEMBEROF (e,m) -> MEMBEROF (rf e,m)
      | MEMBEROFPTR (e,m) -> MEMBEROFPTR (rf e,m)
      | GNU_BODY _ -> failwith "Unhandled"
      | EXPR_LINE (e,_,_) -> rf e
    in
    Make (annot_free expr)
  let to_cabs (Make e) = e
end

module ExprMap = Map.Make (Expr)

(** Mapping from left values to intervals of data.
    Unbounded left values are not tracked.
*)
module Ranges = struct
  type t = Interval.t ExprMap.t
  let top = ExprMap.empty
  let expr_bound e i = ExprMap.add e i top
  let meet a b =
    ExprMap.merge (fun _ oia oib -> match oia, oib with
    | Some i, None
    | None, Some i -> Some i
    | Some ia, Some ib -> Some (Interval.meet ia ib)
    | _ -> failwith "merge") a b
  let join a b =
    ExprMap.merge (fun _ oia oib -> match oia, oib with
    | None, _
    | _, None -> None
    | Some ia, Some ib ->
      let ir = Interval.join ia ib in
      if Interval.is_unbounded ir then None else Some ir) a b
  let fold = ExprMap.fold
end

(** Utilities for the FrontC expressions interpreted as conditions. *)
module CondUtils = struct
  type t = Cabs.expression
  (** Turns an expression X into an expression Y such that Y <=> !X
      and such that Y is not of the form !Z. *)
  let rec push_not expr =
    let open Cabs in
    let mk_not e = UNARY (NOT,e) in
    match expr with
    | UNARY (NOT,e) -> e
    | BINARY (AND,e1,e2) -> BINARY (OR,mk_not e1,mk_not e2)
    | BINARY (OR,e1,e2) -> BINARY (AND,mk_not e1,mk_not e2)
    | BINARY (EQ,e1,e2) -> BINARY (NE,e1,e2)
    | BINARY (NE,e1,e2) -> BINARY (EQ,e1,e2)
    | BINARY (LT,e1,e2) -> BINARY (GE,e1,e2)
    | BINARY (GT,e1,e2) -> BINARY (LE,e1,e2)
    | BINARY (LE,e1,e2) -> BINARY (GT,e1,e2)
    | BINARY (GE,e1,e2) -> BINARY (LT,e1,e2)
    | EXPR_LINE (e,_,_) -> push_not e
    | _ -> BINARY (EQ,expr,CONSTANT (CONST_INT "0"))
  let interval_from_comparison cmp left right =
    let open Cabs in
    let flip = function
      | EQ -> EQ | NE -> NE
      | LT -> GT | GT -> LT
      | LE -> GE | GE -> LE
      | _ -> failwith "unhandled argument" in
    let rec rifc justFlipped cmp left right =
      match left, right with
      | EXPR_LINE (e,_,_), _ -> rifc false cmp e right
      | VARIABLE _, CONSTANT (CONST_INT ns) ->
	let e = Expr.make left in
	begin
	  try
	    let n = int_of_string ns in
	    let i = match cmp with
	      | EQ -> Interval.singleton n
	      | LT -> Interval.upper_bounded (n - 1)
	      | LE -> Interval.upper_bounded n
	      | GT -> Interval.lower_bounded (n + 1)
	      | GE -> Interval.lower_bounded n
	      | _ -> failwith "unhandled argument"
	    in
	    Some (e,i)
	  with Failure("int_of_string") -> None
	end
      | _ when not justFlipped -> rifc true (flip cmp) right left
      | _ -> None in
    rifc false cmp left right
end

exception InvalidCondition
exception InfeasibleCondition

let rec cond_to_ranges expr =
  let open Cabs in
  let rr = cond_to_ranges in
  match expr with
  | UNARY (NOT, e) -> cond_to_ranges (CondUtils.push_not e)
  | BINARY (AND,e1,e2) -> begin try Ranges.meet (rr e1) (rr e2)
    with Interval.Bottom -> raise InfeasibleCondition end
  | BINARY (OR,e1,e2) -> Ranges.join (rr e1) (rr e2)
  | BINARY (NE,_,_) -> Ranges.top
  | BINARY (op,e1,e2) when op = EQ || op = LT || op = GT || op = LE || op = GE ->
    begin match CondUtils.interval_from_comparison op e1 e2 with
    | None -> Ranges.top
    | Some (bounded_expr, interval) -> Ranges.expr_bound bounded_expr interval
    end
  | GNU_BODY _
  | NOTHING -> raise InvalidCondition
  | _ -> Ranges.top (* failwith "todo" *)

let constraints_from_condition
    (expr : Cabs.expression)
    (result : bool)
    :
    (Cabs.expression * (Data.t * Data.t)) list
    =
  let expr' = if result then expr else Cabs.UNARY (Cabs.NOT,expr) in
  let ranges = cond_to_ranges expr' in
  Ranges.fold (fun lv i acc ->
    (Expr.to_cabs lv, Interval.to_data_range i)::acc) ranges []
