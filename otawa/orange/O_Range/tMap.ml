(** Total maps.
    @author Pascal Sotin (irit.fr)
 *)

module Make = functor (Ord : Map.OrderedType) -> struct

module Mappe = struct
  include Map.Make(Ord)
  let print
      ?(first : (unit, Format.formatter, unit) format = ("[@[<hv>" : (unit, Format.formatter, unit) format))
      ?(sep : (unit, Format.formatter, unit) format = (";@ ":(unit, Format.formatter, unit) format))
      ?(last : (unit, Format.formatter, unit) format = ("@]]":(unit, Format.formatter, unit) format))
      ?(firstbind : (unit, Format.formatter, unit) format = ("" : (unit, Format.formatter, unit) format))
      ?(sepbind : (unit, Format.formatter, unit) format = (" => ":(unit, Format.formatter, unit) format))
      ?(lastbind : (unit, Format.formatter, unit) format = ("":(unit, Format.formatter, unit) format))
      (print_key:Format.formatter -> Ord.t -> unit)
      (print_data:Format.formatter -> 'b -> unit)
      (formatter:Format.formatter)
      (map:'b t)
      : unit
      =
    Format.fprintf formatter first;
    let firstitem = ref true in
    iter
      (begin fun key data ->
	if !firstitem then firstitem := false else Format.fprintf formatter sep;
	Format.fprintf formatter firstbind;
	print_key formatter key;
	Format.fprintf formatter sepbind;
	print_data formatter data;
	Format.fprintf formatter lastbind;
       end)
      map;
    Format.fprintf formatter last
end

module Print = struct
(** Printing functions for standard datatypes *)

  open Format

  let list
      ?(first=("[@[":(unit,Format.formatter,unit) format))
      ?(sep = (";@ ":(unit,Format.formatter,unit) format))
      ?(last = ("@]]":(unit,Format.formatter,unit) format))
      (print_elt: Format.formatter -> 'a -> unit)
      (fmt:Format.formatter)
      (list: 'a list)
      : unit
      =
    if list=[] then begin
      fprintf fmt first;
      fprintf fmt last;
    end
    else begin
      fprintf fmt first;
      let rec do_sep = function
      [e] -> print_elt fmt e
	| e::l -> (print_elt fmt e ; fprintf fmt sep; do_sep l)
	| [] -> failwith ""
      in
      do_sep list;
      fprintf fmt last;
    end
end

(** {3 Types} *)

(** The type of total maps. *)
type 'a t = {
  special: 'a Mappe.t;      (** The keys for which a specific value have been defined
				(distinct from the default value). *)
  default: 'a;              (** The default value. Assumed to be the value for
				all keys which have not been specified. *)
}

(** The type of keys for a total map. *)
type key =
| Special of Ord.t   (** A key for which the value is not the default one. *)
| Others             (** A symbol denoting all the keys that are not special. *)

  (** {3 Constructor} *)

let make d = { special = Mappe.empty; default = d; }

(** {3 Inspectors} *)

let default m = m.default
  
let special_bindings m = m.special
  
(*
let special_keys m = Mappe.maptoset m.special
*)
  
let special_count m = Mappe.cardinal m.special

let has_no_special m = Mappe.is_empty m.special
  
(** {3 Array-like operators} *)
  
let get k m =
  try Mappe.find k m.special
  with Not_found -> m.default
let set k v m = { m with special =
    if v = m.default then Mappe.remove k m.special
    else Mappe.add k v m.special }
let reset k m = { m with special = Mappe.remove k m.special }
let update f k m = set k (f (get k m)) m
    
(** {3 Printing} *)

let print
    ?(first : (unit, Format.formatter, unit) format = ("[@[<hv>" : (unit, Format.formatter, unit) format))
    ?(sep : (unit, Format.formatter, unit) format = (";@ ":(unit, Format.formatter, unit) format))
    ?(last : (unit, Format.formatter, unit) format = ("@]]":(unit, Format.formatter, unit) format))
    ?(firstbind : (unit, Format.formatter, unit) format = ("" : (unit, Format.formatter, unit) format))
    ?(sepbind : (unit, Format.formatter, unit) format = (" => ":(unit, Format.formatter, unit) format))
    ?(lastbind : (unit, Format.formatter, unit) format = ("":(unit, Format.formatter, unit) format))
    ?(others : (unit, Format.formatter, unit) format = ("*":(unit, Format.formatter, unit) format))
    print_key print_val fmt m =
  Mappe.print ~first:first ~sep:sep ~firstbind:firstbind ~sepbind:sepbind ~lastbind:lastbind ~last:""
    print_key print_val fmt m.special;
  if not (Mappe.is_empty m.special)
  then begin Format.fprintf fmt sep end;
  Format.fprintf fmt firstbind;
  Format.fprintf fmt others;
  Format.fprintf fmt sepbind;
  print_val fmt m.default;
  Format.fprintf fmt lastbind;
  Format.fprintf fmt last

let print_list
    ?(first : (unit, Format.formatter, unit) format = ("[@[<hv>" : (unit, Format.formatter, unit) format))
    ?(sep : (unit, Format.formatter, unit) format = (";@ ":(unit, Format.formatter, unit) format))
    ?(last : (unit, Format.formatter, unit) format = ("@]]":(unit, Format.formatter, unit) format))
    ?(firstbind : (unit, Format.formatter, unit) format = ("" : (unit, Format.formatter, unit) format))
    ?(sepbind : (unit, Format.formatter, unit) format = (" => ":(unit, Format.formatter, unit) format))
    ?(lastbind : (unit, Format.formatter, unit) format = ("":(unit, Format.formatter, unit) format))
    print_key key_list print_val fmt m =
  Print.list ~first:first ~sep:sep ~last:last
    (fun fmt key ->
      Format.fprintf fmt firstbind;
      print_key fmt key;
      Format.fprintf fmt sepbind;
      print_val fmt (get key m);
      Format.fprintf fmt lastbind) fmt key_list

(** {3 Operating on every bindings} *)

(** All the following operations consider all the bindings of the map, including the default binding.
    The default binding is always presented {b after} the special ones.
    These operations are proposed in two versions.
    {ul
    {- Two functions version : one for the special keys, one for the default.
    These functions have the usual name. }
    {- One function version : the parameter is of type [key].
    These functions have the usual name followed by "1". }} *)

let v1 op f = op (fun k v -> f (Special k) v) (fun v -> f Others v)

let fold fspe fdef m acc = fdef m.default (Mappe.fold fspe m.special acc)
let fold1 f = (v1 fold) f

let iter fspe fdef m = Mappe.iter fspe m.special; fdef m.default
let iter1 f = (v1 iter) f

exception Found
let mappe_exists p m = 
  try Mappe.iter (fun k v -> if p k v then raise Found) m; false
  with Found -> true
let mappe_find p m =
  Mappe.fold (fun k v acc ->
    if acc = None then (if p k v then Some (k,v) else None)
    else acc) m None

let exists pspe pdef m = mappe_exists pspe m.special || pdef m.default
let exists1 p = (v1 exists) p

let for_all pspe pdef m = not (exists (fun k v -> not (pspe k v)) (fun v -> not (pdef v)) m)
let for_all1 p = (v1 for_all) p  

let mapi fspe fdef m =
  let s' = Mappe.mapi fspe m.special in
  let d' = fdef m.default in
  { special = Mappe.filter (fun _ v -> v <> d') s'; default = d' }
let mapi1 f = (v1 mapi) f

let find pspe pdef m =
  match mappe_find pspe m.special with
  | Some (k,v) -> Some (Special k,v)
  | None -> if pdef m.default then Some (Others,m.default) else None
let find1 p = (v1 find) p

(** {3 Operating on the special bindings} *)

let fold_special f = fold f (fun _ x -> x)
let iter_special f = iter f (fun _ -> ())
let exists_special p = exists p (fun _ -> false)
let for_all_special p = for_all p (fun _ -> true)
let mapi_special f = mapi f (fun v -> v)
let find_special p m = mappe_find p m.special

let choose_special m =
  try Some (Mappe.choose m.special)
  with Not_found -> None

let choose_special_exn m =
  match choose_special m with
  | Some x -> x
  | None -> raise Not_found

(** {3 Operating on the values} *)

let exists_val p = exists (fun _ v -> p v) p
let for_all_val p = for_all (fun _ v -> p v) p
let map f = mapi (fun _ v -> f v) f
let find_val p = find (fun _ v -> p v) p
let find_special_val p = find_special (fun _ v -> p v)

(*
let values m = fold (fun _ v acc -> Sette.add v acc) Sette.add m
*)

(** {7 Operating on two maps} *)

let combine f m1 m2 =
  let d' = f m1.default m2.default in
  let s' = Mappe.merge (fun _ ov1 ov2 ->
    let v1 = match ov1 with None -> m1.default | Some x -> x in
    let v2 = match ov2 with None -> m2.default | Some x -> x in
    let v3 = f v1 v2 in
    if v3 = d' then None else Some v3) m1.special m2.special in
  { special = s'; default = d' }
  
let holds r m1 m2 = for_all_val (fun b -> b) (combine r m1 m2)

end
