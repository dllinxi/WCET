(** Sets of maximal values from a partially ordered set [X].

    This modules encode sets of maximal values from a partially ordered set [X].
    In other words a set does not contains two elements such that the first one is greater or equal to the second one.
    The objects encoded by this module coincide with the mathematical notion of ideals of a poset.

    @author Pascal Sotin (irit.fr)
*)

(** Order in a partially ordered set. *)
module PosetOrder = struct
  (** The possible outcome of comparing two elements of a partially ordered set. *)
  type t =
  | LT (** Less than. *)
  | EQ (** Equal. *)
  | GT (** Greater than. *)
  | UN (** Unordered. *)

  (** Pretty printing. *)
  let print fmt o = Format.pp_print_string fmt (match o with
    | LT -> "<"
    | EQ -> "="
    | GT -> ">"
    | UN -> "!~")
end

(** Signature of a partially ordered set [X]. *)
module type POSET = sig
  (** Type of the elements of the partially ordered set. *)
  type t

  (** Ordering two elements. *)
  val order : t -> t -> PosetOrder.t

  (** Greatest lower bound.
      The greatest value least than to values. *)
  val glb : t -> t -> t

  (** Least upper bound.
      The smallest value greater than to values. *)
  val lub : t -> t -> t

  (** Pretty-printing. *)
  val print : Format.formatter -> t -> unit
end

(** Make. *)
module Make = functor (P : POSET) -> struct
  (** Sets of maximal parameterized values. Encoded as a list.
      One should be able to come back from ['a] to [P.t]. *)
  type 'a max_set = 'a list

  (** Sets of maximal values from the poset. *)
  type t = P.t max_set

  let print fmt set =
    List.iteri (fun rg x ->
      if rg = 0
      then Format.fprintf fmt "[%a " P.print x
      else Format.fprintf fmt "; %a" P.print x)
      set;
    Format.pp_print_string fmt "]"

  (** Size of the set. *)
  let cardinal set = List.length set

  (** Lifting a value to a set. *)
  let just x = [x]

  (** Extending a parameterized set with an element.
      According to the provided ordering the element might be added or not.
      If the element is added other elements might be remove.
      The provided ordering must agree with [P.order].
  *)
  let extend_gen order m x =
    let rec re = function
      | [] -> [x]
      | y::r ->
	let open PosetOrder in
	match order x y with
	| LT | EQ -> y::r (* x is discarded. *)
	| GT -> re r      (* y is discarded. *)
	| UN -> y::(re r) in
    re m  
  
  (** [extend_gen] specialized to maximal sets of [P.t]. *)
  let extend = extend_gen P.order

  (** Applies a function to all the elements.
      If no order is provided then it is assumed that [a] unrelated to [b] implies [f a] unrelated to [f b].
  *)
  let map_gen ?(oorder=None) f m =
    match oorder with
    | None -> List.map f m
    | Some order -> List.fold_left (fun acc x -> extend_gen order acc (f x)) [] m

  (** [map_gen] specialized to maximal sets of [P.t].
      [iso] set to [true] means that [a] unrelated to [b] implies [f a] unrelated to [f b].
  *)
  let map ?(iso=false) f m =
    if iso then map_gen ~oorder:None f m
    else map_gen ~oorder:(Some P.order) f m

  (** Union of two sets. Non maximal elements will be removed.
  *)
  let max m1 m2 =
    let m1' = map_gen (fun x -> m1,x) m1 in
    let m2' = map_gen (fun x -> m2,x) m2 in
    let order (ma,x) (mb,y) =
      if ma == mb then PosetOrder.UN
      else P.order x y in
    let m' = List.fold_left (extend_gen order) m1' m2' in
    let m = map_gen (fun (_,x) -> x) m' in
    m

  (** Greatest lower bound.
      The greatest value least than every element of the set.
  *)
  let glb = function
    | x::r -> List.fold_left P.glb x r
    | [] -> failwith "empty"

   (** Least upper bound.
      The least value greater than every element of the set.
  *)
  let lub = function
    | x::r -> List.fold_left P.lub x r
    | [] -> failwith "empty"     

  (** Combines two sets with a function [f] and a given order.
      The provided ordering must agree with [P.order].
  *)
  let combine_gen order f m1 m2 =
    List.fold_left (fun acc x1 ->
      List.fold_left (fun acc x2 -> extend_gen order acc (f x1 x2)) acc m2) [] m1

  (** Combines two sets with a function [f].
      {ul
      {- [iso1] set to [true] means that [a] unrelated to [b] implies [f a c] unrelated to [f b c] for any [c].}
      {- [iso2] set to [true] means that [a] unrelated to [b] implies [f c a] unrelated to [f c b] for any [c].}
      }
  *)
  let combine ?(iso1=false) ?(iso2=false) f m1 m2 = match iso1, iso2 with
    | false, false -> combine_gen P.order f m1 m2
    | true, false -> map_gen (fun (_,r) -> r)
      (combine_gen
	 (fun (xa,ra) (xb,rb) -> if xa == xb then PosetOrder.UN else P.order ra rb)
	 (fun x y -> (x, f x y)) m1 m2)
    | false, true -> map_gen (fun (_,r) -> r)
      (combine_gen
	 (fun (ya,ra) (yb,rb) -> if ya == yb then PosetOrder.UN else P.order ra rb)
	 (fun x y -> (y, f x y)) m1 m2)
    | true, true -> map_gen (fun (_,_,r) -> r)
      (combine_gen
	 (fun (xa,ya,ra) (xb,yb,rb) -> if xa == xb || ya == yb then PosetOrder.UN else P.order ra rb)
	 (fun x y -> (x, y, f x y)) m1 m2)

end
