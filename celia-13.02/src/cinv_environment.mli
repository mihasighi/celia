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

(** Define types and functions to deal with global declarations *)

(**************************************************************************)
(* Types                                                                  *)
(**************************************************************************)
(** Clim types follows the Cil types, with some simplifications *)
type typeinfo = {
  torig_name: string; (** The original name as found in C file. *)
  tname: string; (** The name. Used to order the types. *)
  ttype: typ; (** The type description. *)
  tloc: Loc.floc; (** A location where it has been defined. *)
  tid: typeid; (** The unique identifier. *)
}

(** Type description *)
and typ =
    TVoid 
  (** Void type. *)

  | TInt of ikind 
  (** An integer type. The kind specifies the sign and width. *)

  | TFloat of fkind
  (** A floating-point type. The kind specifies the precision. *)

  | TPtr of typeid
  (** Pointer type. Reference to a type into the table. *)
  
  | TArray of typeid * int option
  (** Array type. It indicates the base type and the array length. *)

  | TComp of compinfo
  (** A struct -- not yet union -- type. *)

(** Various kinds of integers *)
and ikind =
    IBool       (** [_Bool] *)
  | IChar       (** [char] *)
  | ISChar      (** [signed char] *)
  | IUChar      (** [unsigned char] *)
  | IInt        (** [int] *)
  | IUInt       (** [unsigned int] *)
  | IShort      (** [short] *)
  | IUShort     (** [unsigned short] *)
  | ILong       (** [long] *)
  | IULong      (** [unsigned long] *)
  | ILongLong   (** [long long] (or [_int64] on Microsoft Visual C) *)
  | IULongLong  (** [unsigned long long] (or [unsigned _int64] on Microsoft
                    Visual C) *)

(** Various kinds of floating-point numbers *)
and fkind =
    FFloat      (** [float] *)
  | FDouble     (** [double] *)
  | FLongDouble (** [long double] *)

(** Type identifier is an index in the {typ_table} *)
and typeid = int

(** The definition of a structure or union type. *)
and compinfo = {
  cstruct: bool;
    (** True if struct, False if union *)
  corig_name: string;
    (** Original name as found in C file. *)
  cname: string;
    (** The name. Always non-empty. Do not include "struct" or "union" *)
  ckey: int;
    (** A unique integer. *)
  cloc: Loc.floc;
    (** The location where it has been defined *)
  cfields: fieldid list;
    (** Information about the fields. Each fieldinfo has a
      * pointer back to the host compinfo. *)
  crecfields: fieldid list;
    (** The recursive fields, i.e., fields of type pointer to this compinfo *)
  }

(** Information about a struct/union field. *)
and fieldinfo = {
  fcomp: typeid;
    (** The host structure that contains this field; it is unique. *)
  forig_name: string;
    (** original name as found in C file. *)
  fname: string;
    (** The name of the field, desambiguated. *)
  ftype: typeid;
    (** The type *)
  fattr: int;
    (** The attributes for this field: next/prev *)
  fid: fieldid;
    (** The unique identifier *)
  floc: Loc.floc;
    (** The location where this field is defined *)
  fsize_in_bits: int option;
    (** Recall the size of the original field. *)
  foffset_in_bits: int option;
    (** Store the offset at which the field starts in the structure. *)
  fpadding_in_bits: int option;
    (** Store the size of the padding that follows the field, if any. *)
}

and fieldid = int

(** Information about variables. *)
and varinfo = {
  vname: string;
    (** Variable name. *)
  vtyp: typeid;
    (** Variable declaration type. *)
  vid: int;
    (** Variable unique identifier. *)
}

(**************************************************************************)
(* Stores                                                                 *)
(**************************************************************************)
    
(* API for the store of types *)
val init_types: unit
val add_type: string -> typeinfo -> typeid
val get_typeinfo: string -> typeinfo
val get_typeid: string -> typeid
val get_type: typeid -> typeinfo

(* API for the store of fields *)  
val add_field: fieldinfo -> fieldid
val get_fieldid: string -> fieldid
(*
val get_field: fieldinfo -> fieldid
*)
val get_fieldinfo: fieldid -> fieldinfo

(* Export environment of types *)
val fprint_types: unit -> unit


(* API for the store of vars *)
val add_var: string -> typeid -> bool -> int
