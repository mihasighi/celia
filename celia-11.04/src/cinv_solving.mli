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


(* This file is part of the Interproc analyzer, released under GPL license.
   Please read the COPYING file packaged in the distribution.

   Copyright (C) Mathias Argoud, Gaël Lalire, Bertrand Jeannet 2007.
   Copuright (C) Mihaela Sighireanu 2010.
*)
(** Solving the equations *)

(*  ********************************************************************* *)
(** {2 Options} *)
(*  ********************************************************************* *)

(* See @module{cinv_options} *)

(*  ********************************************************************* *)
(** {2 Forward analysis} *)
(*  ********************************************************************* *)

module Forward : sig
  val apply :
    Cinv_equation.graph ->
    'a Apron.Manager.t ->
    Cinv_equation.hedge -> 'a Apron.Abstract1.t array ->
    unit * 'a Apron.Abstract1.t
      (** Applying a transfer function, given
	- the equation graph ;
	- optionally, the result of a previous, backward analysis;
	- an APRON manager;
	- an hyperedge identifier;
	- an array of input abstract values
      *)

  val compute :
    Cinv_equation.graph ->
    'a Apron.Manager.t ->
    debug:int ->
    (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output
      (** Compute (post)fixpoint, given
	- the equation graph;
	- optionally, the result of a previous, backward analysis
	- an APRON manager;
	- a debug level
      *)
end
