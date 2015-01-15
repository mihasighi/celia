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
