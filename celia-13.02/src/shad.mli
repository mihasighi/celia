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


(** Types for Shad: experimental
*)

(** Types 
**)
module Typ : sig
  type t = Tvoid
end

(** Variables 
**)
module Var : sig
  type t = Apron.Var.t
  val of_string : string -> t
  val to_string : t -> string
end

(** Dimensions
**)
module Dim : sig
  type t = Apron.Dim.t
  type dimension = Apron.Dim.dimension
end

(** Environment
**)
module Environment : sig
  type t = Apron.Environment.t
  val make : Var.t array -> Var.t array -> t
  val add : t ->  Var.t array -> Var.t array -> t
end

(** Manager
**)
module Manager : sig
  type 'a t = 'a Apron.Manager.t
end

(** Constants
**)
module Coeff : sig
  type t = Apron.Coeff.t
  val s_of_int : int -> t
  val s_of_float : float -> t
  val print : Format.formatter -> t -> unit
end

(** Expressions 
**)

module Texpr0 : sig
  type t = Apron.Texpr0.t
  type unop = Apron.Texpr0.unop =
	      | Neg
	      | Cast
	      | Sqrt
  type binop = Apron.Texpr0.binop =
	       | Add
	       | Sub
	       | Mul
	       | Div
	       | Mod
  type typ = Apron.Texpr0.typ =
	     | Real
	     | Int
	     | Single
	     | Double
	     | Extended
	     | Quad 
  type round = Apron.Texpr0.round =
	       | Near
	       | Zero
	       | Up
	       | Down
	       | Rnd
  type expr = Apron.Texpr0.expr =
	      | Cst of Coeff.t
	      | Dim of Dim.t
	      | Unop of unop * expr * typ * round
	      | Binop of binop * expr * expr * typ * round

 val of_expr : expr -> t
 val to_expr : t -> expr
end

module Texpr1 : sig
  type t = Apron.Texpr1.t
  type unop = Apron.Texpr1.unop =
	      | Neg
	      | Cast
	      | Sqrt
		  
  type binop = Apron.Texpr1.binop =
	       | Add
	       | Sub
	       | Mul
	       | Div
	       | Mod

  type typ = Apron.Texpr1.typ =
	     | Real
	     | Int
	     | Single
	     | Double
	     | Extended
	     | Quad

  type round = Apron.Texpr1.round =
	       | Near
	       | Zero
	       | Up
	       | Down
	       | Rnd

  type expr = Apron.Texpr1.expr = 
    | Cst of Coeff.t
    | Var of Var.t
    | Unop of unop * expr * typ * round
    | Binop of binop * expr * expr * typ * round

  val cst : Environment.t -> Coeff.t -> t
  val var : Environment.t -> Var.t -> t
  val unop : Texpr0.unop -> t -> Texpr0.typ -> Texpr0.round -> t
  val binop : Texpr0.binop -> t -> t -> Texpr0.typ -> Texpr0.round -> t
      
end

(** Constraints
**)
module Tcons0 : sig
  type t = Apron.Tcons0.t
  type typ = 
    | EQ
    | SUPEQ
    | SUP
    | DISEQ
    | EQMOD of Apron.Scalar.t

end

module Tcons1 : sig
  type t = Apron.Tcons1.t
  type earray = Apron.Tcons1.earray
  type typ = 
    | EQ
    | SUPEQ
    | SUP
    | DISEQ
    | EQMOD of Apron.Scalar.t

  val make : Environment.t -> Tcons0.t -> t
  val get_env: t -> Environment.t
  val get_texpr1: t -> Texpr1.t
  val get_tcons0: t -> Tcons0.t

  val array_make : Environment.t -> int -> earray
  val array_get : earray -> int -> t
  val array_set : earray -> int -> t -> unit
  val array_length : earray -> int
  val array_print :
    ?first:(unit, Format.formatter, unit) format ->
    ?sep:(unit, Format.formatter, unit) format ->
    ?last:(unit, Format.formatter, unit) format ->
    Format.formatter -> earray -> unit
end

(** Formulas
**)
module Tform1 : sig
(** Shad formulas are SL3 formulas.
 *  At the top most level (Ftop_or), the formulas is a non empty disjunction
 *  of simple Shad formulas.
 *)
type formula = 
    { env: Apron.Environment.t; (** Declared ptr and data vars *)
      form: shadform list; (** Disjunction list *)
    }
    
(** A simple shad formula is a list of nodes and a conjunction of
* edge, pointer and sequence formulas.
*)
and shadform = 
    { nodes: Apron.Var.t array; (** The declared nodes, including null *)
      eform: edgeform list; (** Non empty list of separated edges *)
      pform: labform list; (** Labeling formulas *)
      dform: Apron.Tcons0.t list;
      (** Data constraints over the environment of scalars and nodes! *)
      uform: univform list; (** Universally quantified formulas *)
    }

(** Edge formula links two nodes
*)
and edgeform =
    Esegm of Apron.Var.t * Apron.Var.t
  | Enull of Apron.Var.t

(** Label formula links a ptr variable to anode or NULL
*)
and labform =
    Lnode of Apron.Var.t * Apron.Var.t
  | Lnull of Apron.Var.t

(** Universally quantified formuals have the special form
 *  forall y:Int. G(y,n) ==> U(y,n)
*)
and univform =
    { qvars: Apron.Var.t array; (** Array of quantified variables *)
      guard: guardform; (** Guard formula over qvars and nodes *)
      data: Apron.Tcons0.t list; (** Data constraints over scalars, nodes, and qvars *)
    }

and guardform =
    Gall of Apron.Var.t * Apron.Var.t (** Gall(y,n) *)
  | Gle2 of Apron.Var.t * Apron.Var.t * Apron.Var.t (** Gle2(y1,y2,n) *)
  | Gsucc2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t (** Gsucc2(y1,y2,n) *)
  | Gfst of Apron.Var.t * Apron.Var.t (** Gfst(y,n) *)
  | Glst of Apron.Var.t * Apron.Var.t (** Glst(y,n) *)
  | Geq2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Var.t (** Geq2(y1,n1,y2,n2) *)
  | Gsl2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Texpr0.t (** Gsl2(y1,n1,y2,n2,c) *)
  | Gsr2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Texpr0.t (** Gsr2(y1,n1,y2,n2,c) *)
  | Gother of Apron.Tcons0.t list (** other guards over qvars, nodes and prg vars *)

(*****************************************************************************)
(* Building constants *)
(*****************************************************************************)

val shadform_top: Apron.Environment.t -> shadform

val check_shadform: Apron.Environment.t -> shadform -> shadform

(*****************************************************************************)
(* Priting functions *)
(*****************************************************************************)

val print_smtlib_spec: Format.formatter -> formula -> unit

end


(** Abstact values 
**)
module Abstract1 : sig
  type 'a t = 'a Apron.Abstract1.t
end
