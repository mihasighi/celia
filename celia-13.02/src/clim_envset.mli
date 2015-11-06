(**************************************************************************)
(*                                                                        *)
(*  This file is part of CINV.                                            *)
(*                                                                        *)
(*  Copyright (C) 2009-2011                                               *)
(*    LIAFA (University of Paris Diderot and CNRS)                        *)
(*                                                                        *)
(*                                                                        *)
(*  you can redistribute it and/or modify it under the terms of the GNU   *)
(*  Lesser General Public License as published by the Free Software       *)
(*  Foundation, version 3.                                                *)
(*                                                                        *)
(*  It is distributed in the hope that it will be useful,                 *)
(*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *)
(*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *)
(*  GNU Lesser General Public License for more details.                   *)
(*                                                                        *)
(*  See the GNU Lesser General Public License version 3.                  *)
(*  for more details (enclosed in the file LICENSE).                      *)
(*                                                                        *)
(**************************************************************************)



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

open Clim_env
open Clim_stdlib

module StringSet : Set.S with type elt = string

module StringMap : Map.S with type key = string 

val get_unique_name : ?local_names:StringSet.t -> string -> string

val is_pointer_type : Clim_env.lc_type -> bool
val is_nonnull_pointer_type : Clim_env.lc_type -> bool
val is_integral_type: lc_type -> bool

val is_embedded_field : Clim_env.field_info -> bool

module VarOrd : OrderedHashedType with type t = var_info

module VarSet : Set.S with type elt = var_info

module VarMap : Map.S with type key = var_info

module StructSet : Set.S with type elt = struct_info

module StructMap : Map.S with type key = struct_info

module FieldOrd : OrderedHashedType with type t = field_info

module FieldSet : Set.S with type elt = field_info

module FieldMap : Map.S with type key = field_info

module FieldTable : Hashtbl.S with type key = field_info

module PointerClass : OrderedHashedType with type t = pointer_class

module LogicLabelSet : Set.S with type elt = label

(*
Local Variables: 
compile-command: "make -C .. bin/jessie.byte"
End: 
*)

