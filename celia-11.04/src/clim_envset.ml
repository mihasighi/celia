(**************************************************************************)
(*                                                                        *)
(*  The Why platform for program certification                            *)
(*  Copyright (C) 2002-2008                                               *)
(*    Romain BARDOU                                                       *)
(*    Jean-François COUCHOT                                               *)
(*    Mehdi DOGGUY                                                        *)
(*    Jean-Christophe FILLIÂTRE                                           *)
(*    Thierry HUBERT                                                      *)
(*    Claude MARCHÉ                                                       *)
(*    Yannick MOY                                                         *)
(*    Christine PAULIN                                                    *)
(*    Yann RÉGIS-GIANAS                                                   *)
(*    Nicolas ROUSSET                                                     *)
(*    Xavier URBAIN                                                       *)
(*                                                                        *)
(*  This software is free software; you can redistribute it and/or        *)
(*  modify it under the terms of the GNU Library General Public           *)
(*  License version 2, with the special exception on linking              *)
(*  described in file LICENSE.                                            *)
(*                                                                        *)
(*  This software is distributed in the hope that it will be useful,      *)
(*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *)
(*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *)
(*                                                                        *)
(**************************************************************************)



open Clim_stdlib
open Clim_env

module type OrderedType =
sig
  type t
  val equal : t -> t -> bool
  val compare : t -> t -> int
end

module type OrderedHashedType =
sig
  type t
  val equal : t -> t -> bool
  val compare : t -> t -> int
  val hash : t -> int
end

module ChoiceOrd(A : OrderedHashedType)(B : OrderedHashedType) =
struct
  type t = A of A.t | B of B.t
  let equal = function
    | A a1,A a2 -> A.equal a1 a2
    | B b1,B b2 -> B.equal b1 b2
    | _ -> false
  let compare = function
    | A a1,A a2 -> A.compare a1 a2
    | B b1,B b2 -> B.compare b1 b2
    | A _,_ -> 1
    | B _,_ -> -1
  let hash = function
    | A a -> A.hash a
    | B b -> B.hash b
end

module StringSet = Set.Make(String)

module StringMap = Map.Make(String)

(* used names (in order to rename identifiers when necessary) *)
let used_names = Hashtbl.create 97

let mark_as_used x = 
  Hashtbl.add used_names x ()

let () = 
  List.iter mark_as_used 
    [ (* Why keywords *)
      "absurd"; "and"; "array"; "as"; "assert"; "axiom"; "begin";
      "bool"; "do"; "done"; "else"; "end"; "exception"; "exists";
      "external"; "false"; "for"; "forall"; "fun"; "function"; "goal";
      "if"; "in"; "int"; "invariant"; "label"; "let"; "logic"; "not";
      "of"; "or"; "parameter"; "predicate"; "prop"; "raise"; "raises";
      "reads"; "real"; "rec"; "ref"; "result"; "returns"; "then"; "true"; "try";
      "type"; "unit"; "variant"; "void"; "while"; "with"; "writes"; "init";
      (* Why prelude *)
      "exp" ; "log" ; "sin" ; "cos" ; "tan" ; "sqr_real" ;
      (* jessie generated names *)
      (* "global" ; "alloc"  *)
    ]

let is_used_name n = Hashtbl.mem used_names n

let use_name ?local_names n = 
  if is_used_name n then raise Exit; 
  begin match local_names with 
    | Some h -> if StringSet.mem n h then raise Exit 
    | None -> () 
  end;
  mark_as_used n;
  n

let rec next_name ?local_names n i = 
  let n_i = n ^ "_" ^ string_of_int i in
  try use_name ?local_names n_i 
  with Exit -> next_name ?local_names n (succ i)

let get_unique_name ?local_names n = 
  try use_name ?local_names n 
  with Exit -> next_name ?local_names n 0

let is_pointer_type t =
  match t with
    | LCTnull -> true
    | LCTpointer _ -> true
    | _ -> false

let is_nonnull_pointer_type t =
  match t with
    | LCTpointer _ -> true
    | _ -> false

let is_integral_type = function
  | LCTnative Tinteger -> true
  | LCTenum _ -> true 
  | LCTnative _ | LCTlogic _ | LCTpointer _ | LCTnull | LCTany
  | LCTtype_var _ -> false

let is_embedded_field fi =
  match fi.lc_field_info_type with
    | LCTpointer(_,Some _,Some _) -> true
    | _ -> false

module type Tag =
  sig
    type t
    val tag_of_val : t -> int
  end

module OrderedHashedTypeOfTag (X:Tag) : OrderedHashedType with type t = X.t =
struct
  type t = X.t
  let compare v1 v2 = Pervasives.compare (X.tag_of_val v1) (X.tag_of_val v2)
  let equal v1 v2 = (X.tag_of_val v1) = (X.tag_of_val v2)
  let hash v = Hashtbl.hash (X.tag_of_val v)
end

module VarOrd = OrderedHashedTypeOfTag 
  (struct type t = var_info
          let tag_of_val x = x.lc_var_info_tag
   end)

module VarSet = Set.Make(VarOrd)

module VarMap = Map.Make(VarOrd)

module StructOrd =
  struct type t = struct_info
	 let compare st1 st2 = 
	   Pervasives.compare 
	     st1.lc_struct_info_name st2.lc_struct_info_name
	 let equal st1 st2 =
	   st1.lc_struct_info_name = st2.lc_struct_info_name
	 let hash st =
	   Hashtbl.hash st.lc_struct_info_name 
  end

module StructSet = Set.Make(StructOrd)

module StructMap = Map.Make(StructOrd)

module VariantOrd = struct
  type t = root_info
  let compare v1 v2 = 
    Pervasives.compare v1.lc_root_info_name v2.lc_root_info_name
  let equal v1 v2 =
    v1.lc_root_info_name = v2.lc_root_info_name
  let hash v =
    Hashtbl.hash v.lc_root_info_name 
end

module VariantSet = Set.Make(VariantOrd)

module VariantMap = Map.Make(VariantOrd)

module FieldOrd =  OrderedHashedTypeOfTag 
  (struct type t = field_info
          let tag_of_val x = x.lc_field_info_tag
   end)

module FieldSet = Set.Make(FieldOrd)

module FieldMap = Map.Make(FieldOrd)
module FieldTable = Hashtbl.Make(FieldOrd)

module MemClass = 
struct
  type t = mem_class
  let equal fv1 fv2 = match fv1,fv2 with
    | LCmem_field a1,LCmem_field a2 -> FieldOrd.equal a1 a2
    | LCmem_plain_union b1,LCmem_plain_union b2 -> VariantOrd.equal b1 b2
    | LCmem_bitvector,LCmem_bitvector -> true
    | _ -> false
  let compare fv1 fv2 = match fv1,fv2 with
    | LCmem_field a1,LCmem_field a2 -> FieldOrd.compare a1 a2
    | LCmem_plain_union b1,LCmem_plain_union b2 -> VariantOrd.compare b1 b2
    | LCmem_bitvector,LCmem_bitvector -> 0
    | LCmem_field _,_ -> 1
    | _,LCmem_field _ -> -1
    | LCmem_plain_union _,_ -> 1
    | _,LCmem_plain_union _ -> -1
  let hash = function
    | LCmem_field a -> FieldOrd.hash a
    | LCmem_plain_union b -> VariantOrd.hash b
    | LCmem_bitvector -> 0
end

module MemClassSet = Set.Make(MemClass)

module AllocClass = 
struct
  type t = alloc_class
  let equal fv1 fv2 = match fv1,fv2 with
    | LCalloc_root st1,LCalloc_root st2 -> VariantOrd.equal st1 st2
    | LCalloc_bitvector,LCalloc_bitvector -> true
    | _ -> false
  let compare fv1 fv2 = match fv1,fv2 with
    | LCalloc_root a1,LCalloc_root a2 -> VariantOrd.compare a1 a2
    | LCalloc_bitvector,LCalloc_bitvector -> 0
    | LCalloc_root _,_ -> 1
    | _,LCalloc_root _ -> -1
  let hash = function
    | LCalloc_root a -> VariantOrd.hash a
    | LCalloc_bitvector -> 0
end

(* TODO: take into account type parameters *)
module PointerClass = 
struct
  type t = pointer_class
  let equal fv1 fv2 = match fv1,fv2 with
    | LCtag(st1,_),LCtag(st2,_) -> StructOrd.equal st1 st2
    | LCroot vi1,LCroot vi2 -> VariantOrd.equal vi1 vi2
    | _ -> false
  let compare fv1 fv2 = match fv1,fv2 with
    | LCtag(a1,_),LCtag(a2,_) -> StructOrd.compare a1 a2
    | LCroot b1,LCroot b2 -> VariantOrd.compare b1 b2
    | LCtag _,_ -> 1
    | _,LCtag _ -> -1
  let hash = function
    | LCtag(a,_) -> StructOrd.hash a
    | LCroot b -> VariantOrd.hash b
end

module ExceptionOrd =  OrderedHashedTypeOfTag 
  (struct type t = exception_info
          let tag_of_val x = x.lc_exception_info_tag
   end)  

module ExceptionSet = Set.Make(ExceptionOrd)

module ExceptionMap = Map.Make(ExceptionOrd)

module LogicLabelOrd =
  struct type t = label
	 let compare = (Pervasives.compare : label -> label -> int)
  end

module LogicLabelSet = Set.Make(LogicLabelOrd)

module TypeVarOrd = OrderedHashedTypeOfTag 
  (struct type t = type_var_info
          let tag_of_val x = x.lc_type_var_info_tag
   end)

module TypeVarMap = Map.Make(TypeVarOrd)

(*
Local Variables: 
compile-command: "LC_ALL=C make -C .. bin/jessie.byte"
End: 
*)
