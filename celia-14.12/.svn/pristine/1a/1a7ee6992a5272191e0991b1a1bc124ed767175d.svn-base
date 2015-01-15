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


open Clim_stdlib
open Clim_env
open Clim_ast

(* types *)
val operator_of_native: native_type -> [> native_operator_type]
val operator_of_type: lc_type -> [> operator_type]

val unit_type : Clim_env.lc_type
val integer_type : Clim_env.lc_type
val boolean_type : Clim_env.lc_type
val null_type : Clim_env.lc_type
val any_type: lc_type

val root_is_union : Clim_env.root_info -> bool
val root_name : Clim_env.struct_info -> string
val struct_root : Clim_env.struct_info -> Clim_env.root_info
val pointer_class_root : pointer_class -> Clim_env.root_info

val string_of_native: Clim_env.native_type -> string
val print_type : Format.formatter -> Clim_env.lc_type -> unit


(* operators *)

val string_of_op: [< bin_op | unary_op] -> string
val string_of_op_type: [< operator_type] -> string



