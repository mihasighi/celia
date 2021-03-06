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
open Ast_info
open Extlib
open Cil_types

(* for Boolean *)
open Clim

(* Utility functions *)
open Common
open Format

(* Exception used when the translation fails *)
exception CtePredicate of bool

(* Utility functions *)
let print_loc (fmt:Format.formatter) (loc:Cil_types.location) =
  Format.fprintf fmt "at line %d:%d"  (fst loc).Lexing.pos_lnum (fst loc).Lexing.pos_cnum

let array_exists varr name =
  Array.fold_left (fun b v -> b || ((String.compare (Apron.Var.to_string v) name)==0)) false varr


(*****************************************************************************)
(* ACSL-SL3 to Shad translation of terms                                   *)
(*****************************************************************************)

let product f t1 t2 =
  List.fold_right
    (fun x acc -> List.fold_right (fun y acc -> f x y :: acc) t2 acc) t1 []

let term2texpr0 env 
    (t:Cil_types.term)
    : Apron.Texpr0.t
    = 
  let e1l = (Cil2apron.coerce_type env t) in
  if (List.length e1l) >= 1 then (List.hd e1l).Apron.Texpr1.texpr0 else (Cil2apron.zero_expr env).Apron.Texpr1.texpr0

let texpr02var
    (env:Apron.Environment.t)
    (te0:Apron.Texpr0.t)
    : Apron.Var.t
    =
  let e = (Apron.Texpr0.to_expr te0) in
  match e with
    | Apron.Texpr0.Dim(d) ->
      (Apron.Environment.var_of_dim env d)
    | _ ->
      assert(false)

let lvar2node
    (lvname:string)
    : Apron.Var.t
    =
  (* typechecking done by ACSL *)
  Apron.Var.of_string lvname

let lvar2int
    (lvname:string)
    : Apron.Var.t
    =
  (* typechecking done by ACSL *)
  Apron.Var.of_string lvname

(** The term shall be a variable denoting a prg var or a node *)
let rec term2var_ptr
    (env:Apron.Environment.t)
    (t:Cil_types.term)
    : Apron.Var.t
    =
  match t.term_node with
    | TLval(TVar lv, TNoOffset) -> (Apron.Var.of_string lv.lv_name)
      
    | TLval(TResult _,TNoOffset) ->
      (Apron.Var.of_string "__return")

    | Tapp(li, _, []) -> 
      if (String.compare li.l_var_info.lv_name "nilNode") == 0 
      then (Apron.Var.of_string "nilNode")
      else (Apron.Var.of_string "?p")

    | Tapp(li, _, [t1]) ->
      if (String.compare li.l_var_info.lv_name "sloc") == 0
      then (term2var_ptr env t1)
      else (Apron.Var.of_string "?f(p)")

    | Tnull -> (Apron.Var.of_string "nilNode")
     
    (* | Told t1 ->
      let v = term2var_ptr env t1 in
      (Apron.Var.of_string ((Apron.Var.to_string v)^"_0"))
    *)
  
    | Tat(t1,_) -> (* TODO: check correctly the label *)
      (term2var_ptr env t1)
  
    | _ -> (Apron.Var.of_string "??")
      

(** The term shall be a variable denoting a position or a (not nil) node *)
let term2var_int_or_node
    (env:Apron.Environment.t)
    (yarr:Apron.Var.t array)
    (t:Cil_types.term)
    : Apron.Texpr0.t
    =
  let te1 = (term2texpr0 env t) in
  match t.term_node, t.term_type with
    | TLval(TVar lv, TNoOffset), Ctype(ty) -> 
      (* typechecking done by Cil *)
      begin
	(if (ty == Cil.intType) &&
	    not(array_exists yarr lv.lv_name)
	 then
	    warning "Cil2shad.guardform: incorrect instantiation of position var (%s) in guard@." lv.lv_name
	);
	te1
      end
     
    | _ -> te1


(*****************************************************************************)
(* ACSL-SL3 to Shad translation of predicates                                *)
(*****************************************************************************)

let quant2nodes
    (qv:Cil_types.quantifiers)
    : Apron.Var.t array
    = 
  Array.of_list (List.map (fun lv -> (lvar2node lv.lv_name)) qv)

let quant2int
    (qv:Cil_types.quantifiers)
    : Apron.Var.t array
    = 
  Array.of_list (List.map (fun lv -> (lvar2int lv.lv_name)) qv)

let pred2shadform_data
    (penv:Apron.Environment.t)
    (ploc:Cil_types.location)
    (p:Cil_types.predicate)
    : Apron.Tcons0.t list
    =
  try
    let bexpr = Cil2apron.pred penv p in
    let tc1l = Cil2apron.mkcons_list penv bexpr in
    let dform = List.map (fun tc1 -> tc1.Apron.Tcons1.tcons0) tc1l in
    dform
  with Cil2apron.CtePredicate _ ->
      unsupported "Cil2shad.shadform_data: unknown data constraint (%a)@." print_loc ploc

let rec pred2guardform
    (penv:Apron.Environment.t)
    (yarr:Apron.Var.t array)
    (ploc:Cil_types.location)
    (p:Cil_types.predicate)
    : Shad.guardform
    =
  match p with
      Papp(pinfo,_,tl) ->
	begin
	  let dg = Shad.Gother([]) in
	  try
            let args = List.map (fun t -> (List.map (term2var_int_or_node penv yarr) [t])) tl in
            let all_args = List.fold_right (product (fun x y -> x::y)) args [[]] in
            List.fold_left
              (fun g lp -> pred2guardform_app penv yarr ploc pinfo lp g) dg all_args
	  with (CtePredicate _)-> 
	    dg
	end

    | _ ->
      let tc0l = pred2shadform_data penv ploc p in
      Shad.Gother(tc0l)

and pred2guardform_app 
    (penv:Apron.Environment.t)
    (yarr:Apron.Var.t array)
    (ploc:Cil_types.location)
    (pinfo:Cil_types.logic_info)
    (tl:Apron.Texpr0.t list)
    (dg:Shad.guardform)
    : Shad.guardform
    =
  let gname = pinfo.l_var_info.lv_name in
  let tl_length = (List.length tl) in
  if tl_length == 2 
  then (* Gall, Gfst, Glst *)
    begin
      let y = texpr02var penv (List.nth tl 0) in
      let n = texpr02var penv (List.nth tl 1) in
      if (array_exists yarr (Apron.Var.to_string y))
      then
	(match gname with
	  | "Gall" -> Shad.Gall(y,n)
	  | "Gfst" -> Shad.Gfst(y,n)
	  | "Glst" -> Shad.Glst(y,n)
	  | _ -> 
	    unsupported "Cil2shad.guardform: (binary) guad predicates supported are Gall|Gfst|Glst (%a)@." print_loc ploc
	)
      else begin
	warning "Cil2shad.guardform: (binary) guard predicate does not use a position variable y (%a)@." print_loc ploc;
	dg
      end
    end
  else if tl_length == 3
  then
    begin
      let y1 = texpr02var penv (List.nth tl 0) in
      let y2 = texpr02var penv (List.nth tl 1) in
      let n = texpr02var penv (List.nth tl 2) in
      if (array_exists yarr (Apron.Var.to_string y1)) &&
	(array_exists yarr (Apron.Var.to_string y2))
      then
	(match gname with
	  | "Gle2" -> Shad.Gle2(y1,y2,n)
	  | "Gsucc2" -> Shad.Gsucc2(y1,y2,n)
	  | _ -> 
	    unsupported "Cil2shad.guardform: (ternary) guad predicates supported are Gle2|Gsucc2 (%a)@." print_loc ploc
	)
      else begin
	warning "Cil2shad.guardform: (ternary) guard predicate does not use a position variable y (%a)@." print_loc ploc;
	dg
      end
    end
  else if tl_length == 4
  then
    begin
      let y1 = texpr02var penv (List.nth tl 0) in
      let y2 = texpr02var penv (List.nth tl 2) in
      let n1 = texpr02var penv (List.nth tl 1) in
      let n2 = texpr02var penv (List.nth tl 3) in
      if (array_exists yarr (Apron.Var.to_string y1)) &&
	(array_exists yarr (Apron.Var.to_string y2))
      then
	(match gname with
	  | "Geq2" -> Shad.Geq2(y1,n1,y2,n2)
	  | _ -> 
	    unsupported "Cil2shad.guardform: (4-ary) guad predicate supported is Geq2 (%a)@." print_loc ploc
	)
      else begin
	warning "Cil2shad.guardform: (4-ary) guard predicate does not use a position variable y (%a)@." print_loc ploc;
	dg
      end
    end
  else if tl_length == 5
  then
    begin
      let y1 = texpr02var penv (List.nth tl 0) in
      let y2 = texpr02var penv (List.nth tl 2) in
      let n1 = texpr02var penv (List.nth tl 1) in
      let n2 = texpr02var penv (List.nth tl 3) in
      let c = (List.nth tl 4) in
      if (array_exists yarr (Apron.Var.to_string y1)) &&
	(array_exists yarr (Apron.Var.to_string y2))
      then
	(match gname with
	  | "Gsr2" -> Shad.Gsr2(y1,n1,y2,n2,c)
	  | "Gsl2" -> Shad.Gsl2(y1,n1,y2,n2,c)
	  | _ -> 
	    unsupported "Cil2shad.guardform: (5-ary) guad predicates supported are Gsl2|Gsr2 (%a)@." print_loc ploc
	)
      else begin
	warning "Cil2shad.guardform: (5-ary) guard predicate does not use a position variable y (%a)@." print_loc ploc;
	dg
      end
    end
  else
      unsupported "Cil2shad.guardform: %d-ary guad predicate (%a)@." tl_length print_loc ploc

let rec pred2shadform_forall
    (penv:Apron.Environment.t)
    (shf:Shad.shadform)
    (ploc:Cil_types.location)
    (qv:Cil_types.quantifiers)
    (p:Cil_types.predicate)
    : Shad.shadform
    =
  match p with 
      Pforall(qv1,pn) ->
	pred2shadform_forall penv shf pn.loc (qv@qv1) pn.content

    | Pimplies(pg,pu) ->
      let yarr = quant2int qv in (* build list of position vars *)
      let nenv = Apron.Environment.add penv yarr [||] in
      let gform = pred2guardform nenv yarr pg.loc pg.content in 
      let dform = pred2shadform_data nenv pu.loc pu.content in
      let uform = { Shad.qvars = yarr; Shad.guard = gform; Shad.data = dform; } in
      { Shad.nodes=shf.Shad.nodes;
	Shad.eform=shf.Shad.eform;
	Shad.pform=shf.Shad.pform;
	Shad.dform=shf.Shad.dform;
	Shad.uform=shf.Shad.uform@[uform];
      }

    | _ ->
      let _ = 
	unsupported "Cil2shad.pred2shadform_forall: predicate not allowed (%a)@." print_loc ploc
      in
      shf

let pred2shadform_sep
    (shf:Shad.shadform)
    (ploc:Cil_types.location)
    (tl:Apron.Var.t list)
    : Shad.shadform
    =
  (* Test that all nodes are referred in the separated clause, excluding nilNode *)
  if (Array.length shf.Shad.nodes) == (List.length tl) then 
    begin 
      (Array.iter (fun n -> 
	if (List.exists (fun nl -> ((String.compare (Apron.Var.to_string n) (Apron.Var.to_string nl)) == 0)) tl)
	then ()
	else warning "Cil2shad.shadform_separated: separated predicate does not list node %s (%a)@."
		   (Apron.Var.to_string n) print_loc ploc
       )
	 shf.Shad.nodes
      );
      shf
    end
  else
    unsupported "Cil2shad.shadform_separated: separated predicate must list all nodes %a@." print_loc ploc


(** Build a Shad formula from an atomic constraint and fill shf argument.
*)
let pred2shadform_rel
    (penv:Apron.Environment.t)
    (shf:Shad.shadform)
    (ploc:Cil_types.location)
    (p:Cil_types.predicate)
    : Shad.shadform
    =
  let tc0l = pred2shadform_data penv ploc p in 
  { Shad.nodes=shf.Shad.nodes;
    Shad.eform=shf.Shad.eform;
    Shad.pform=shf.Shad.pform;
    Shad.dform=(shf.Shad.dform)@tc0l;
    Shad.uform=shf.Shad.uform;
  }

(** Build a Shad formula from a predefined predicate and fill shf argument.
*)
let pred2shadform_app 
    (shf:Shad.shadform)
    (ploc:Cil_types.location)
    (pinfo:Cil_types.logic_info)
    (vl:Apron.Var.t list)
    : Shad.shadform
    =
  match pinfo.l_var_info.lv_name, vl with
    | "acyclic", [v] ->
      let nv = Apron.Var.of_string ("n_"^(Apron.Var.to_string v)) in
      let ev = Shad.Enull(nv) in
      let lv = Shad.Lnode(v,nv) in
      { Shad.nodes=(Array.append [|nv|] shf.Shad.nodes);
	Shad.eform=ev::shf.Shad.eform;
	Shad.pform=lv::shf.Shad.pform;
	Shad.dform=shf.Shad.dform;
	Shad.uform=shf.Shad.uform;
      }

    | "cyclic", [v] ->
      let nv = Apron.Var.of_string ("n_"^(Apron.Var.to_string v)) in
      let ev = Shad.Esegm(nv,nv) in
      let lv = Shad.Lnode(v,nv) in
      { Shad.nodes=(Array.append [|nv|] shf.Shad.nodes);
	Shad.eform=ev::shf.Shad.eform;
	Shad.pform=lv::shf.Shad.pform;
	Shad.dform=shf.Shad.dform;
	Shad.uform=shf.Shad.uform;
      }

    | "ls", n1::[n2] -> 
      let e = if (String.compare (Apron.Var.to_string n2) "nilNode") == 0 
	then Shad.Enull(n1) else Shad.Esegm(n1,n2)
      in
      { Shad.nodes=shf.Shad.nodes;
	Shad.eform=e::shf.Shad.eform;
	Shad.pform=shf.Shad.pform;
	Shad.dform=shf.Shad.dform;
	Shad.uform=shf.Shad.uform;
      }

    | "label", v::[n] ->
      let l = if (String.compare (Apron.Var.to_string n) "nilNode") == 0 
	then Shad.Lnull(v) else Shad.Lnode(v,n)
      in
      { Shad.nodes=shf.Shad.nodes;
	Shad.eform=shf.Shad.eform;
	Shad.pform=l::shf.Shad.pform;
	Shad.dform=shf.Shad.dform;
	Shad.uform=shf.Shad.uform;
      }
	
    | "sep", _ ->
      pred2shadform_sep shf ploc vl
	
    | "sorted", _
    | "reach",_ ->
      unsupported "Cil2shad.pred2shadform: sorted|reach predicate not supported (%a)@." print_loc ploc

    | _ ->
      unsupported "Cil2shad.pred2shadform: user-defined predicate not known (%a)@." print_loc ploc

(** Build a Shad formula by filling the shf argument.
*)
let rec pred2shadform_and 
    (penv:Apron.Environment.t)
    (shf:Shad.shadform)
    (ploc:Cil_types.location)
    (p:Cil_types.predicate)
    : Shad.shadform
    =
  match p with
      Ptrue -> shf
    | Pfalse -> 
      warning "Cil2shad.shadform_and: Pfalse predicate removes Shad formula at %s@." (Clim.Loc.string ploc);
      Shad.shadform_top penv

    | Pand(pn1,pn2) ->
      let shf1 = pred2shadform_and penv shf pn1.loc pn1.content in
      let shf2 = pred2shadform_and penv shf1 pn2.loc pn2.content in
      shf2

    | Papp(pinfo,_,tl) ->
      begin
	try
          let args = List.map (fun t -> (List.map (term2var_ptr penv) [t])) tl in
          let all_args = List.fold_right (product (fun x y -> x::y)) args [[]] in
          List.fold_left
            (begin fun f lp -> 
	      match lp with
		  [_] | _::[_] -> pred2shadform_app f ploc pinfo lp
		| _ -> raise (CtePredicate false)
	     end) shf all_args
	with (CtePredicate _)-> shf
      end

    | Prel _ ->
      pred2shadform_rel penv shf ploc p

    | Pforall(qv,pn) ->
      pred2shadform_forall penv shf ploc qv pn.content

    | Pseparated tl ->
      begin
	try
          let args = List.map (fun t -> (List.map (term2var_ptr penv) [t])) tl in
          let all_args = List.fold_right (product (fun x y -> x::y)) args [[]] in
          List.fold_left (fun f lp -> pred2shadform_sep f ploc lp) shf all_args
	with (CtePredicate _)-> shf
      end

    | _ ->
      let _ = unsupported "Cil2shad.pred2shadform_and: predicate not allowed (%a)@." print_loc ploc
      in
      Shad.shadform_top penv

(** Build a Shad formula starting with some node declaration.
*)
let rec pred2shadform_exists
    (penv:Apron.Environment.t)
    (ploc:Cil_types.location)
    (qv:Cil_types.quantifiers)
    (p:Cil_types.predicate)
    : Shad.shadform
    =
  match p with
      Pexists(qv1,pn) ->
	pred2shadform_exists penv pn.loc (qv@qv1) pn.content

    | Pand _ | Prel _ | Pforall _ | Papp _ ->
      let narr = quant2nodes qv in (* build list of node vars *)
      let nenv = Apron.Environment.add penv [||] narr in
      let sht = Shad.shadform_top nenv in
      let shf = { Shad.nodes=narr; Shad.eform=[]; Shad.pform=[]; Shad.dform=sht.Shad.dform; Shad.uform=[]; } in
      let shfr = pred2shadform_and nenv shf ploc p in
      Shad.check_shadform nenv shfr
      
    | _ ->
      let _ = 
	unsupported "Cil2shad.pred2shadform_exists: predicate not allowed (%a)@." print_loc ploc
      in
      Shad.shadform_top penv


let rec pred2shadform
    (penv:Apron.Environment.t)
    (ploc:Cil_types.location)
    (p:Cil_types.predicate)
    : Shad.shadform list
    =
  match p with
      Pfalse -> []

    | Ptrue -> [(Shad.shadform_top penv)]

    | Por(pn1,pn2) -> 
      let sfl1 = pred2shadform penv pn1.loc pn1.content in
      let sfl2 = pred2shadform penv pn2.loc pn2.content in
      sfl1@sfl2

    | Pexists(qv,pn) ->
      [(pred2shadform_exists penv ploc qv pn.content)]
      
    | Pand _ -> (** no nodes declared *)
      [(pred2shadform_and penv (Shad.shadform_top penv) ploc p)]

    | Papp _ ->
      [(pred2shadform_and penv (Shad.shadform_top penv) ploc p)]

    | _ ->
      unsupported "Cil2shad.pred2shadform: only Ptrue|Pfalse|Pexists|Por|Pand|Papp supported!"


let pred2formula
    (penv:Apron.Environment.t)
    (ploc:Cil_types.location)
    (p:Cil_types.predicate)
    : Shad.formula
    =
  let sf_list = pred2shadform penv ploc p in
  { Shad.env=penv;
    Shad.form=sf_list;
  }


(*****************************************************************************)
(* Main entry for translation *)
(*****************************************************************************)

let rec convert_spec 
    (ploc:Cinv_equation.point)
    (penv:Apron.Environment.t)
    (iplist:Cil_types.identified_predicate list) 
    : Apron.Tcons1.earray
    =
  let spec_file = ploc.Cinv_equation.pos.Lexing.pos_fname in
  let spec_line = ploc.Cinv_equation.pos.Lexing.pos_lnum in
  let spec_sid = ploc.Cinv_equation.sid in
  (Celia_options.feedback "Collect constraint at %s:%d sid:%d (of %d predicates)" 
     spec_file spec_line spec_sid (List.length iplist));  
  (** The environment is given by the formals *)
  
  (** Build the constraint from the line number (lnumber - 0) = 0 *)
  let eline = (Apron.Texpr1.cst penv (Apron.Coeff.s_of_int spec_line)) in
  let cstr = (Cil2apron.mkcons eline Req (Cil2apron.zero_expr penv)) in
  let carray = Apron.Tcons1.array_make penv 1 in
  let _ = Apron.Tcons1.array_set carray 0 cstr in

  (** Build the name of the file containing the constraint *)
  let fn_spec = "pan/spec_"^(string_of_int spec_line)^".smt" in
  let ch_spec = open_out fn_spec in
  let fmt = Format.formatter_of_out_channel ch_spec in
  let ptop = match iplist with ip::_ -> ip | _ -> notimplemented "Cil2shad: list of predicated" in
  let shad_spec = 
    try pred2formula penv ptop.ip_loc ptop.ip_content 
    with (CtePredicate b) -> 
      if b then (pred2formula penv ptop.ip_loc Ptrue) 
      else (pred2formula penv ptop.ip_loc Pfalse)
  in
  begin
    (* print spec as assertion *)
    Shad.print_smtlib_spec fmt shad_spec;
    flush ch_spec;
    close_out ch_spec;
    carray
  end
