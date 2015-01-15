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

open Format
open Num

(* native types *)

let operator_of_native t =
  match t with
  | Tunit -> `Unit
  | Tboolean -> `Boolean
  | Tinteger -> `Integer

let operator_of_type = function
  | LCTnative n -> operator_of_native n
  | LCTenum _ -> `Integer
  | LCTany -> assert false (* TODO? *)
  | LCTnull | LCTpointer _ -> `Pointer

let unit_type = LCTnative Tunit
let boolean_type = LCTnative Tboolean
let integer_type = LCTnative Tinteger
let null_type = LCTnull
let any_type = LCTany

(* types *)

let root_is_union rt = false

let root_name st =
  st.lc_struct_info_hroot.lc_struct_info_name


let struct_root st =
  match st.lc_struct_info_hroot.lc_struct_info_root with
    | Some vi -> vi
    | None ->
        raise (Invalid_argument
                 ("struct_root in clim_pervasives.ml ("
                  ^st.lc_struct_info_name^", "
                  ^st.lc_struct_info_hroot.lc_struct_info_name^")"))

let pointer_class_root = function
  | LCtag(st, _) -> struct_root st
  | LCroot vi -> vi


let string_of_native t =
  match t with
    | Tunit -> "unit"
    | Tinteger -> "integer"
    | Tboolean -> "boolean"

let rec print_type fmt t =
  match t with
    | LCTnative n -> fprintf fmt "%s" (string_of_native n)
    | LCTenum ri -> fprintf fmt "%s" ri.lc_enum_info_name
    | LCTpointer(pc, ao, bo) ->
        begin match pc with
          | LCtag({ lc_struct_info_name = name }, [])
          | LCroot { lc_root_info_name = name } ->
              fprintf fmt "%s" name
          | LCtag({ lc_struct_info_name = name }, params) ->
              fprintf fmt "%s<%a>" name
                (Pp.print_list Pp.comma print_type) params
        end;
        begin match ao, bo with
          | None, None ->
              fprintf fmt "[..]"
          | Some a, None ->
              fprintf fmt "[%s..]" (Num.string_of_num a)
          | None, Some b ->
              fprintf fmt "[..%s]" (Num.string_of_num b)
          | Some a, Some b ->
              if Num.eq_num a b then
                fprintf fmt "[%s]" (Num.string_of_num a)
              else
                fprintf fmt "[%s..%s]"
                  (Num.string_of_num a) (Num.string_of_num b)
        end
    | LCTnull -> fprintf fmt "(nulltype)"
    | LCTany -> fprintf fmt "(anytype)"

(* operators *)
let string_of_op = function
  | `Blt -> "<"
  | `Bgt -> ">"
  | `Ble -> "<="
  | `Bge -> ">="
  | `Beq -> "=="
  | `Bneq -> "!="
  | `Badd -> "+"
  | `Bsub -> "-"
  | `Bmul -> "*"
  | `Bdiv -> "/"
  | `Bmod -> "%"
  | `Bland -> "&&"
  | `Blor -> "||"
  | `Bimplies -> "==>"
  | `Biff -> "<=>"
  | `Uminus -> "unary -"
  | `Unot -> "not"

let string_of_op_type = function
  | `Integer -> "integer"
  | `Unit -> "unit"
  | `Boolean -> "boolean"
  | `Pointer -> "pointer"


