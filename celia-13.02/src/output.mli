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



open Lexing
open Format

type constant =
  | Prim_void
  | Prim_int of string
  | Prim_real of string
  | Prim_bool of bool
(*
  | Prim_string of string
*)

val fprintf_constant : Format.formatter -> constant -> unit

type term = 
  | LConst of constant
  | LApp of string * term list
  | LVar of string
  | LVarAtLabel of string * string     (*r x@L *)
  | Tnamed of string * term
  | TIf of term * term * term

val fprintf_term : Format.formatter -> term -> unit

type logic_type = 
    { logic_type_name : string;
      logic_type_args : logic_type list;
    }
  
type assertion = 
  | LTrue | LFalse
  | LAnd of assertion * assertion
  | LOr of assertion * assertion
  | LIff of assertion * assertion
  | LNot of assertion
  | LImpl of assertion * assertion
  | LIf of term * assertion * assertion
  | LLet of string * term * assertion
      (*r warning: only for Coq assertions *)
  | LForall of string * logic_type * trigger list list * assertion
      (*r forall x:t.a *)
  | LExists of string * logic_type * trigger list list * assertion
      (*r exists x:t.a *)
  | LPred of string * term list
  | LNamed of string * assertion
and trigger =
  |LPatP of assertion
  |LPatT of term


val make_var : string -> term

val make_or : assertion -> assertion -> assertion
val make_and : assertion -> assertion -> assertion
val make_and_list : assertion list -> assertion
val make_or_list : assertion list -> assertion
val make_forall_list : (string * logic_type) list -> trigger list list -> assertion -> assertion
val make_impl : assertion -> assertion -> assertion
val make_equiv : assertion -> assertion -> assertion

val fprintf_logic_type : Format.formatter -> logic_type -> unit
val fprintf_assertion : Format.formatter -> assertion -> unit
val fprintf_triggers : Format.formatter -> trigger list list -> unit

(*s types *)

type why_type = 
  | Prod_type of string * why_type * why_type      (*r (x:t1)->t2 *)
  | Base_type of logic_type
  | Ref_type of why_type
  | Annot_type of 
      assertion * why_type * 
      string list * string list * assertion * ((string * assertion) list)
	(*r { P } t reads r writes w { Q | E => R } *)


val base_type : string -> why_type
val int_type : why_type
val bool_type : why_type
val unit_type : why_type

val fprintf_type : bool -> Format.formatter -> why_type -> unit

(*s expressions *)

type variant = term * string option

type opaque = bool

type assert_kind = [`ASSERT | `CHECK]

type expr =
  | Cte of constant
  | Var of string
  | And of expr * expr
  | Or of expr * expr
  | Not of expr
  | Void
  | Deref of string
  | If of expr * expr * expr
  | While of 
      expr (* loop condition *)
      * assertion (* invariant *) 
      * variant option (* variant *) 
      * expr list (* loop body *)
  | Block of expr list
  | Assign of string * expr
  | Let of string * expr * expr
  | Let_ref of string * expr * expr
  | App of expr * expr
  | Raise of string * expr option
  | Try of expr * string * string option * expr
  | Fun of (string * why_type) list * 
      assertion * expr * assertion * ((string * assertion) list)
  | Triple of opaque * 
      assertion * expr * assertion * ((string * assertion) list)
  | Assert of assert_kind * assertion * expr
  | Label of string * expr
  | BlackBox of why_type
  | Absurd
  | Loc of Lexing.position * expr
;;

val make_or_expr : expr -> expr -> expr
val make_and_expr : expr -> expr -> expr
val make_app : string -> expr list -> expr 
val make_logic_app : string -> expr list -> expr
val make_while : expr -> assertion -> variant option -> expr -> expr
val make_label : string -> expr -> expr
val make_pre : assertion -> expr -> expr

val fprintf_variant : Format.formatter -> (variant option -> unit)

val fprintf_expr : Format.formatter -> expr -> unit
val fprintf_expr_list : Format.formatter -> expr list -> unit
val fprintf_expr_end_list : Format.formatter -> expr list -> unit

type why_decl =
  | Param of bool * string * why_type         (*r parameter in why *)
  | Def of string * expr               (*r global let in why *)
  | Logic of bool * string * (string * logic_type) list * logic_type    (*r logic decl in why *)
  | Predicate of bool * string * (string * logic_type) list * assertion  
  | Inductive of bool * string * (string * logic_type) list *  
      (string * assertion) list (*r inductive definition *)
  | Axiom of string * assertion         (*r Axiom *)
  | Goal of string * assertion         (*r Goal *)
  | Function of bool * string * (string * logic_type) list * logic_type * term
  | Type of string * string list
  | Exception of string * logic_type option


val get_why_id : why_decl -> string
val iter_why_decl : (string -> unit) -> why_decl -> unit

type state = [`TODO | `RUNNING | `DONE ];;

type 'a decl = { mutable state : state; decl : 'a };;

val fprint_logic_arg : Format.formatter -> (string * logic_type) -> unit
val fprintf_why_decl : Format.formatter -> why_decl -> unit
val fprintf_why_decls : Format.formatter -> why_decl list -> unit

(*s locs table *)

type kind =
  | VarDecr
  | ArithOverflow
  | DownCast
  | IndexBounds
  | PointerDeref
  | UserCall
  | DivByZero
  | AllocSize
  | Pack
  | Unpack
  | FPoverflow

val reg_pos : string ->
           ?id:string ->
           ?kind:kind ->
           ?name:string -> ?formula:string -> Loc.position -> string
val print_kind : Format.formatter -> kind -> unit
val print_pos : Format.formatter -> unit

(*
  Local Variables: 
  compile-command: "LC_ALL=C make -j -C .. bin/jessie.byte"
  End: 
*)
