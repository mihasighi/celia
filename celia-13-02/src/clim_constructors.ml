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

class positioned ~pos =
object
  method pos: Loc.position =
    match pos with None -> Loc.dummy_position | Some pos -> pos
end

class typed ~typ =
object
  method typ: lc_type = typ
end

class labeled ~label =
object
  val mutable llab: label option = label
  method label = llab
  method set_label lab = llab <- lab
end

class marked ~mark =
object
  method mark: string = mark
end

class identifier ?pos name =
object
  inherit positioned pos
  method name: string = name
end

class ['a] node_positioned ?pos node =
object
  inherit positioned pos
  method node: 'a = node
end

class ptype ?(pos = Loc.dummy_position) node =
object
  inherit [ptype_node] node_positioned ~pos node
end

class pexpr ?(pos = Loc.dummy_position) node =
object
  inherit [pexpr_node] node_positioned ~pos node
end

class pexpr_with ?pos ?node e =
  let pos = match pos with None -> e#pos | Some pos -> pos in
  let node = match node with None -> e#node | Some node -> node in
  pexpr ~pos node

class nexpr ?(pos = Loc.dummy_position) ?label node =
object
  inherit labeled label
  inherit [nexpr_node] node_positioned ~pos node
end

class nexpr_with ?pos ?node e =
  let pos = match pos with None -> e#pos | Some pos -> pos in
  let node = match node with None -> e#node | Some node -> node in
  let llab = e#label in
  nexpr ~pos ~label:llab node

class ['expr] ptag ?(pos = Loc.dummy_position) node =
object
  inherit ['expr ptag_node] node_positioned ~pos node
end

class ['expr] decl ?(pos = Loc.dummy_position) node =
object
  inherit ['expr decl_node] node_positioned ~pos node
end

(*
class expr ?(pos = Loc.dummy_position) ~typ ?(mark="") 
  ?original_type node =
object
  inherit typed typ
  inherit marked mark
  inherit [expr_node] node_positioned ~pos node
  method original_type =
    match original_type with None -> typ | Some original_type -> original_type
end

class expr_with ?pos ?typ ?mark ?node ?original_type e =
  let pos = match pos with None -> e#pos | Some pos -> pos in
  let typ = match typ with None -> e#typ | Some typ -> typ in
  let mark =
    match mark with None -> e#mark | Some mark -> mark
  in
  let node = match node with None -> e#node | Some node -> node in
  let original_type = match original_type with
    | None -> e#original_type
    | Some original_type -> original_type
  in
  expr ~pos ~typ ~mark ~original_type node

*)
