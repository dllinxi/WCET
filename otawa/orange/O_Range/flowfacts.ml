(** Module for registering the flowfacts.
    See also [Orange.LISTENER].
    @author Pascal Sotin
*)

module LoopId = struct
  type t = int
  let compare = Pervasives.compare
end

module LoopMap = Map.Make(LoopId)

module Bound = struct
  type t =
  | Bounded of int
  | Unbounded
  let bounded n = Bounded n
  let unbounded = Unbounded
  let worst b1 b2 = match b1, b2 with
    | Unbounded, _
    | _, Unbounded -> Unbounded
    | Bounded b1, Bounded b2 -> Bounded (max b1 b2)
  let of_expression_evaluee ee =
    if Cvarabs.estDefExp ee
    then Bounded (Cvarabs.getIntVal ee)
    else Unbounded
  let dump oc = function
    | Unbounded -> Printf.fprintf oc "unbounded"
    | Bounded n -> Printf.fprintf oc "<= %d" n
  let to_option = function
    | Unbounded -> None
    | Bounded n -> Some n
end

(** A listener that registers only the loop bounds. *)
module Coarse (* : Orange.LISTENER *) = struct
  type t = {
    loop_bound : Bound.t LoopMap.t
  }
  let worst_loop_bound = LoopMap.merge (fun _ b1 b2 -> match (b1,b2) with
    | None, Some b
    | Some b, None -> Some b
    | Some b1, Some b2 -> Some (Bound.worst b1 b2)
    | None, None -> assert false)
  
  (** {3 Implementation} *)

  let null = { loop_bound = LoopMap.empty }
  let onLoop x id _ _ _ max _ _ _ _ _ _ _ =
    { loop_bound = worst_loop_bound x.loop_bound (LoopMap.singleton id (Bound.of_expression_evaluee max)) }
  let concat x y =
    { loop_bound = worst_loop_bound x.loop_bound y.loop_bound }
	

  (** {3 Dummy implementation} *)

  let onBegin x = x
  let onEnd x = x
  let onFunction x _ _ _ _ = x
  let onCall x _ _ _ _ _ _ _ _ _ = x
  let onReturn x = x
  let onLoopEnd x = x
  let onIfT x _ _ = x
  let onIfF x _ _ = x
  let onIf x _ _ _ _ _ _ _ _ _ _ = x
  let onFunctionEnd x = x
  let onIfEnd x = x
  let onIfTEnd x = x
  let onIfFEnd x = x

  (** {3 Output} *)

  let dump oc x =
    let dump_loop_bound oc =
      LoopMap.iter (fun id b ->
	Printf.fprintf oc "Loop %d: %a\n" id Bound.dump b) in
    if LoopMap.is_empty x.loop_bound
    then
      Printf.fprintf oc "No loops\n"
    else begin
      Printf.fprintf oc "## Loop bounds ##\n";
      dump_loop_bound oc x.loop_bound
    end

  let loop_bound x = x.loop_bound
end

module CStmt = struct
  type t = Cabs.statement
  let compare = Pervasives.compare
end

module CStmtMap = Map.Make(CStmt)

(** Loop information rattached to the AST. *)
module LoopInfo = struct
  type t = {
    cabs_id : LoopId.t CStmtMap.t;
    id_bound : Bound.t LoopMap.t;
  }

  let make_cabs_id file =
    let counter = ref 1 in
    let map = ref CStmtMap.empty in
    let open Cabs in
    let rec process_file f = List.iter process_def f
    and process_def = function
      | FUNDEF (_,b)
      | OLDFUNDEF (_,_,b) -> process_body b
      | _ -> ()
    and process_body (_,s) = process_stmt s
    and process_stmt s =
      let number () =
	map := CStmtMap.add s !counter !map;
	incr counter in
      match s with
      | BLOCK b -> process_body b
      | SEQUENCE (s1,s2)
      | IF (_,s1,s2) -> process_stmt s1; process_stmt s2
      | WHILE (_,s) -> number (); process_stmt s
      | DOWHILE (_,s) -> number (); process_stmt s
      | FOR (_,_,_,s) -> number (); process_stmt s
      | SWITCH (_,s)
      | CASE (_,s)
      | DEFAULT s
      | LABEL (_,s)
      | STAT_LINE (s,_,_) -> process_stmt s
      | _ -> () in
    process_file file;
    !map

  let make file results =
    let cabs_id = make_cabs_id file in
    let id_bound = Coarse.loop_bound results in
    { cabs_id = cabs_id;
      id_bound = id_bound }

  let to_ff_input x =
    let loop_bound stmt =
      let id = CStmtMap.find stmt x.cabs_id in
      let b = LoopMap.find id x.id_bound in
      Bound.to_option b in
    let branch_feasibility = function
      | Cabs.IF _ -> [true; true]
      | Cabs.CASE _ -> failwith "todo"
      | _ -> raise (Invalid_argument "not a conditional") in
    let branch_id stmt = "IF-?" in
    let open Balance in
    { loop_bound = loop_bound;
      branch_feasibility = branch_feasibility;
      branch_id = branch_id }
end
