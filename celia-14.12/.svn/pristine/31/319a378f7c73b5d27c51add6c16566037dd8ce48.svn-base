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

open Clim_env
open Clim_ast
open Clim_pervasives

open Format
open Pp

let const fmt c =
  match c with
    | LCCvoid -> fprintf fmt "()"
    | LCCinteger s -> fprintf fmt "%s" s
    | LCCboolean b -> fprintf fmt "%B" b
    | LCCnull -> fprintf fmt "null"

let label fmt l =
  match l with
    | LabelName s -> fprintf fmt "%s" s.label_info_name
    | LabelHere -> fprintf fmt "Here" 
    | LabelPre -> fprintf fmt "Pre" 
    | LabelOld -> fprintf fmt "Old" 
    | LabelPost -> fprintf fmt "Post" 

let rec ptype fmt t =
  match t#node with
    | LCPTnative n -> fprintf fmt "%s" (string_of_native n)
    | LCPTidentifier (s,args) -> fprintf fmt "%s%a" s (print_list_delim lchevron rchevron comma ptype) args
    | LCPTpointer (name,params,ao, bo) ->
	begin match ao, bo with
	  | None, None ->
	      fprintf fmt "%s%a[..]" name ptype_params params
	  | Some a, None ->
	      fprintf fmt "%s%a[%s..]" name ptype_params params
                (Num.string_of_num a)
	  | None, Some b ->
	      fprintf fmt "%s%a[..%s]" name ptype_params params
                (Num.string_of_num b)
	  | Some a, Some b ->
	      if Num.eq_num a b then
		fprintf fmt "%s%a[%s]" name ptype_params params
                  (Num.string_of_num a)
	      else
		fprintf fmt "%s%a[%s..%s]" name ptype_params params
		  (Num.string_of_num a) (Num.string_of_num b)
	end

and ptype_params fmt l = print_list_delim lchevron rchevron comma ptype fmt l

and asrt_kind fmt = function
  | Aassert -> fprintf fmt "assert"
  | Ahint -> fprintf fmt "hint"
  | Aassume -> fprintf fmt "assume"
  | Acheck -> fprintf fmt "check"

and address_kind fmt = function
  | Addr_absolute -> fprintf fmt "absolute_"
  | Addr_pointer -> fprintf fmt ""

(*
Local Variables: 
compile-command: "LC_ALL=C make -j -C .. bin/jessie.byte"
End: 
*)
