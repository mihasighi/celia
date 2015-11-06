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


open Clim_env
open Clim_ast
(* open Clim_fenv *)

class identifier :
  ?pos:Loc.position ->
  string -> object method pos : Loc.position method name : string end

class ['a] node_positioned :
  ?pos:Loc.position ->
  'a -> object method pos : Loc.position method node : 'a end

class ptype :
  ?pos:Loc.position ->
  ptype_node ->
  object method pos : Loc.position method node : ptype_node end

class pexpr :
  ?pos:Loc.position ->
  pexpr_node ->
  object method pos : Loc.position method node : pexpr_node end

class pexpr_with :
  ?pos:Loc.position ->
  ?node:pexpr_node ->
  < pos : Loc.position; node : pexpr_node; .. > -> pexpr

class nexpr :
  ?pos:Loc.position ->
  ?label:label ->
  nexpr_node ->
  object
    val mutable llab : label option
    method pos : Loc.position
    method label : label option
    method node : nexpr_node
    method set_label : label option -> unit
  end

class ['a] ptag :
  ?pos:Loc.position ->
  'a ptag_node ->
  object method pos : Loc.position method node : 'a ptag_node end

class ['a] decl :
  ?pos:Loc.position ->
  'a decl_node ->
  object method pos : Loc.position method node : 'a decl_node end

(*
class expr :
  ?pos:Loc.position ->
  typ:lc_type ->
  ?mark:string ->
  ?original_type:lc_type ->
  expr_node ->
  object
    method pos : Loc.position
    method mark : string
    method node : expr_node
    method original_type : lc_type
    method typ : lc_type
  end
class expr_with :
  ?pos:Loc.position ->
  ?typ:lc_type ->
  ?mark:string ->
  ?node:expr_node ->
  ?original_type:lc_type ->
  < pos : Loc.position; mark : string; node : expr_node;
    original_type : lc_type; 
    typ : lc_type; .. > ->
  expr
*)


