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



(* Import from Cil *)
open Cil_types
open Cil
open Cilutil
module FCExtlib=Extlib
module FCAst_info=Ast_info
module FCProject=Project
module FCParameters=Parameters

open Db_types
open Visitor

open Common

(*****************************************************************************)
(* Check intlist type declaration                                            *)
(*****************************************************************************)
(* Module for visitor *)
class check_intlist_type =
  object(_self)
    inherit Visitor.generic_frama_c_visitor (Project.current())
      (Cil.inplace_visit())

  method vglob_aux = function
    | GCompTag (compinfo,_) when compinfo.cstruct ->
      begin
	Celia_options.feedback "See struct %s " compinfo.cname;
	if (String.compare compinfo.cname "intlist_") != 0 then
	  Celia_options.feedback "...ignored"
	else
	  Celia_options.feedback "...ok"
        ;
	DoChildren
      end

    | GType (typeinfo,_) ->
      begin
	Celia_options.debug "See typedef %s " typeinfo.tname;
	if (String.compare typeinfo.tname "intlist") != 0 then
	  Celia_options.debug "...ignored"
	else
	  Celia_options.debug "...ok"
        ;
	DoChildren
      end
    | GFun _ | GAnnot _ | GVar _ | GVarDecl _ -> DoChildren
    | GCompTag _ | GCompTagDecl _ | GEnumTagDecl _
    | GEnumTag _ | GAsm _ | GPragma _ | GText _ -> SkipChildren    
  end

(* Function encapsulating the visitor *)
let check_intlist_type f =
  let vis = new check_intlist_type in Visitor.visitFramacFile vis f

(*****************************************************************************)
(* Check entry function definition                                           *)
(*****************************************************************************)
(* Module for visitor *)
class check_entryfun_type =
  object(_self)
    inherit Visitor.generic_frama_c_visitor (Project.current())
      (Cil.inplace_visit())

  method vglob_aux = function
    | GFun (fundec,_) ->
      begin
	Celia_options.feedback "See function %s " fundec.svar.vname;
	if (String.compare fundec.svar.vname (FCParameters.MainFunction.get())) != 0 then
	  begin
	    Celia_options.feedback "...ignored";
	    SkipChildren
	  end
	else
	  begin
	    Celia_options.feedback "...ok";
	    DoChildren
	  end
      end

    | GType _ | GAnnot _ | GVar _ | GVarDecl _
    | GCompTag _ | GCompTagDecl _ | GEnumTagDecl _
    | GEnumTag _ | GAsm _ | GPragma _ | GText _ -> SkipChildren 

  method vvdec vinfo = 
    if vinfo.vglob then
      SkipChildren
    else (* only variables from the input function are visited then *)
    begin
      Celia_options.feedback "Variable %s" vinfo.vname;
      match vinfo.vtype with
	| TInt _ -> SkipChildren
	| TNamed (tinfo,_) ->
	    begin
	      if (String.compare tinfo.tname "intlist") != 0 then
		Celia_options.abort "Unsupported type %s" tinfo.tname
	      else
		Celia_options.feedback "Allowed type %s" tinfo.tname
	      ;
	      SkipChildren
	    end 
	| TFun _ -> DoChildren
	| TVoid _ | TFloat _ | TBuiltin_va_list _
	| TPtr _ | TArray _ | TComp _
	| TEnum _ -> 
	    Celia_options.abort "Unsupported type for variable %s" vinfo.vname
    end
  end

(* Function encapsulating the visitor *)
let check_entryfun_type f =
  let vis = new check_entryfun_type in Visitor.visitFramacFile vis f

(*****************************************************************************)
(* Check restrictions on the Cil AST                                         *)
(*****************************************************************************)
(* Module for visitor *)
class check_ast =
  object(_self)
    inherit Visitor.generic_frama_c_visitor (Project.current())
      (Cil.inplace_visit())

  method vfile _f =
      DoChildren
  end

(* Function encapsulating the visitor *)
let check_ast f =
  let vis = new check_ast in Visitor.visitFramacFile vis f;
  Globals.Functions.iter (fun kf -> ignore (Kernel_function.get_spec kf))


(*****************************************************************************)
(* Check restrictions on the Cil AST                                         *)
(*****************************************************************************)

(* Main *)
let check file =
  if checking then check_types file;
  (* Check that intlist type exists with field next and data. *)
  (* TODO: This shall be done more precisely in the future. *)
  Celia_options.debug "Check intlist type";
  check_intlist_type file;
  (* Check that entry function use only numeric and intlist type *)
  (* TODO: This shall look at all the call graph in the future. *)
  Celia_options.debug "Check entry function";
  check_entryfun_type file; (* kernel option -main *)
  check_ast file;
  if checking then check_types file

(*
Local Variables:
compile-command: "LC_ALL=C make -C ../.. -j bin/toplevel.byte"
End:
*)

