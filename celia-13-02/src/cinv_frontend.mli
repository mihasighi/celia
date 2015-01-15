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


open Format
open Cinv_options
open Cinv_equation
open Cinv_solving

val parse_lexbuf : Format.formatter -> Lexing.lexbuf -> unit 

val compute_and_display :
  Format.formatter ->
  Cinv_equation.graph ->  
  Cinv_equation.graph ->  
  'a Apron.Manager.t -> 
  unit

val analyze_and_display :
  string -> 
  Format.formatter -> 
  Cinv_equation.graph -> 
  Cinv_equation.graph -> 
  unit
