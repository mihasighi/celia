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
open Ast_info
open Extlib
open Db_types

(* Import from Cinv *)
open Clim
open Cinv_equation

(* Utility functions *)
open Common
(* open Integer *)
open Format


(* Exception used when the translation fails *)
exception CtePredicate of bool

(*****************************************************************************)
(* Cil to Apron translation of operators                                      *)
(*****************************************************************************)

let unop = function
  | Neg -> Apron.Texpr1.Neg
  | BNot
  | LNot -> assert false

let binop = function
  | PlusA -> Apron.Texpr1.Add
  | PlusPI 
  | IndexPI -> assert false (*TODO: MS extract index correctly *)
  | MinusA -> Apron.Texpr1.Sub
  | MinusPI
  | MinusPP  -> assert false
  | Mult -> Apron.Texpr1.Mul
  | Div -> Apron.Texpr1.Div
  | Mod -> Apron.Texpr1.Mod
  | _ -> assert false

let relation = function
  | Lt -> Rlt
  | Gt -> Rgt
  | Le -> Rle
  | Ge -> Rge
  | Eq -> Req
  | Ne -> Rneq
  | _ -> assert false

let name_of_printf = "printf"
let is_printf_function v = isFunctionType v.vtype && v.vname = name_of_printf

let framac_field 
    (s:string)
    : 
    bool
    =
  let n = (String.compare (String.sub s 0 7) "struct_") in
  n = 0

(*****************************************************************************)
(* Smart constructors.                                                       *)
(*****************************************************************************)

let point_of_stmt
    (s:Cil_types.stmt)
    :
    Cinv_equation.point
    =
  let loc = get_stmtLoc s.Cil_types.skind in
  { pos=(fst loc); sid=s.Cil_types.sid; }

let point_of_instr
    (pos:Cil_types.location)
    (sid:int)
    :
    Cinv_equation.point
    =
  { pos=(fst pos); sid=sid; }

let point_of_block_if
    (s:Cil_types.stmt)
    :
    Cinv_equation.point
    =
  (* TODO: change the encoding below to translate correctly composed boolean 
   * conditions in IF statements *)
  let nsid = (-1) in
  { pos=(fst (get_stmtLoc s.skind)); sid=nsid; }

let point_of_block_else
    (s:Cil_types.stmt)
    :
    Cinv_equation.point
    =
  let nsid = (-2) in
  { pos=(fst (get_stmtLoc s.skind)); sid=nsid; }

let mkexpr_var env 
    (vname:string)
    :
    Apron.Texpr1.t
    =
  Apron.Texpr1.var env (Apron.Var.of_string vname)

let mkexpr_cst_int env 
    (i:int)
    :
    Apron.Texpr1.t
    =
  Apron.Texpr1.cst env (Apron.Coeff.s_of_int i)

let mkexpr_cst_ptr env
    (f:float)
    :
    Apron.Texpr1.t
    =
  Apron.Texpr1.cst env (Apron.Coeff.s_of_float f)

let null_expr env = mkexpr_cst_ptr env 0.0
let zero_expr env = mkexpr_cst_int env 0
let one_expr env = mkexpr_cst_int env 1
let true_cons env = Apron.Tcons1.make (zero_expr env) Apron.Tcons1.EQ 
let false_cons env = Apron.Tcons1.make (one_expr env) Apron.Tcons1.EQ 

(* Translates lvalue terms using the convention:
   x->data  :   cast_i,n(x)
   x->next  :   cast_f,n(x)
*)
let mkexpr_field
    (fname:string) 
    (x:Apron.Texpr1.t)
    :
    Apron.Texpr1.t
    =
  match fname with
    "next" -> 
      Apron.Texpr1.unop Apron.Texpr1.Cast x 
	Apron.Texpr1.Single Apron.Texpr1.Near
  | "data" ->
      Apron.Texpr1.unop Apron.Texpr1.Cast  x 
	Apron.Texpr1.Int Apron.Texpr1.Near
  | _ -> (* May be the struct_*_xM field introduced by Frama-C *)
    begin
      if framac_field fname then
	x
      else
	notimplemented "mkexpr_field field \"%s\" not known" fname
    end

(* Translates dereferencing of lvalue terms using the convention:
   x->data=expr    :    x=cast_d,0(expr)
   x->next=expr    :    x=cast_d,n(expr)
*)
let mkexpr_deref
    (e:Apron.Texpr1.t)
    (fname:string) 
    :
    Apron.Texpr1.t
    =
  match fname with
    "next" -> 
      Apron.Texpr1.unop Apron.Texpr1.Cast e 
	Apron.Texpr1.Double Apron.Texpr1.Near
  | "data" ->
      Apron.Texpr1.unop Apron.Texpr1.Cast e 
	Apron.Texpr1.Double Apron.Texpr1.Zero
  | "" -> e 
  | _ -> (* May be the struct_*_xM field introduced by Frama-C *)
    begin
      if framac_field fname then
	e
      else
	notimplemented "mkexpr_deref field \"%s\" not known" fname
    end

(** Translates the app terms used in the logic to denote data on lists, i.e.,
    \length(x)   :   cast_l,n(x)
    \sum(x)      :   cast_q,0(x)
    \mset(x)     :   cast_q,n(x)
*)
let mkexpr_fun
    (lname:string) 
    (x:Apron.Texpr1.t)
    :
    Apron.Texpr1.t
    = 
  match lname with
  | "\\length"  
  | "len" ->
    Apron.Texpr1.unop Apron.Texpr1.Cast x 
	Apron.Texpr1.Quad Apron.Texpr1.Up
  | "\\sum" -> 
    Apron.Texpr1.unop Apron.Texpr1.Cast x
	Apron.Texpr1.Quad Apron.Texpr1.Zero
  | "\\mset" -> 
    Apron.Texpr1.unop Apron.Texpr1.Cast x
	Apron.Texpr1.Quad Apron.Texpr1.Near
  | _ -> notimplemented "mkexpr_fun function \"%s\" not known" lname
  
let mkexpr_binop 
    (e1:Apron.Texpr1.t)
    (op:Cil_types.binop)
    (e2:Apron.Texpr1.t)
    :
    Apron.Texpr1.t
    =
  let ty = Apron.Texpr1.Int in
  let rnd = Apron.Texpr1.Near in
  let bop = match op with
    | MinusA -> Apron.Texpr1.Sub
    | PlusA -> Apron.Texpr1.Add
    | Mult -> Apron.Texpr1.Mul
    | Div -> Apron.Texpr1.Div
    | Mod -> Apron.Texpr1.Mod
    | _ -> notimplemented "mkexpr_binop operation not known" 
  in
  Apron.Texpr1.binop bop e1 e2 ty rnd

(* Translate free to x=sqrt_f,0(1) *)
let mkexpr_free env 
    :
    Apron.Texpr1.t
    =
  Apron.Texpr1.unop Apron.Texpr1.Sqrt (one_expr env) 
    Apron.Texpr1.Single Apron.Texpr1.Zero
  
(* Translate alloc to x=sqrt_f,n(1) *)
let mkexpr_alloc env 
    :
    Apron.Texpr1.t
    =
  Apron.Texpr1.unop Apron.Texpr1.Sqrt (one_expr env) 
    Apron.Texpr1.Single Apron.Texpr1.Near
  
let mkcons 
    (expr1:Apron.Texpr1.t)
    (rel:Cil_types.relation)
    (expr2:Apron.Texpr1.t)
    :
    Apron.Tcons1.t
    =
  let (ty,e) = 
    match rel with
      | Req ->
	(Apron.Tcons1.EQ,
	 Apron.Texpr1.binop Apron.Texpr1.Sub expr1 expr2 Apron.Texpr1.Real Apron.Texpr1.Rnd)
      | Rneq ->
	(Apron.Tcons1.DISEQ,
	 Apron.Texpr1.binop Apron.Texpr1.Sub expr1 expr2 Apron.Texpr1.Real Apron.Texpr1.Rnd)
      | Rge ->
	(Apron.Tcons1.SUPEQ,
	 Apron.Texpr1.binop Apron.Texpr1.Sub expr1 expr2 Apron.Texpr1.Real Apron.Texpr1.Rnd)
      | Rgt ->
	(Apron.Tcons1.SUP,
	 Apron.Texpr1.binop Apron.Texpr1.Sub expr1 expr2 Apron.Texpr1.Real Apron.Texpr1.Rnd)
      | Rle ->
	(Apron.Tcons1.SUPEQ,
	 Apron.Texpr1.binop Apron.Texpr1.Sub expr2 expr1 Apron.Texpr1.Real Apron.Texpr1.Rnd)
      | Rlt ->
	(Apron.Tcons1.SUP,
	 Apron.Texpr1.binop Apron.Texpr1.Sub expr2 expr1 Apron.Texpr1.Real Apron.Texpr1.Rnd)
  in
  Apron.Tcons1.make e ty

let negate_texpr (texpr:Apron.Texpr1.t) 
    : 
    Apron.Texpr1.t
    =
  let expr = Apron.Texpr1.to_expr texpr in
  let nexpr = match expr with
    | Apron.Texpr1.Unop(Apron.Texpr1.Neg,e,_typ,_round) ->
        e
    | _ ->
        Apron.Texpr1.Unop(
          Apron.Texpr1.Neg, expr,
          Apron.Texpr1.Real, Apron.Texpr1.Rnd
        )
  in
  let env = Apron.Texpr1.get_env texpr in
  Apron.Texpr1.of_expr env nexpr

let negate_tcons (tcons:Apron.Tcons1.t) 
    : 
    Apron.Tcons1.t
    =
  let texpr = Apron.Tcons1.get_texpr1 tcons in
  let (ntyp,ntexpr) = match Apron.Tcons1.get_typ tcons with
    | Apron.Tcons1.EQ -> (Apron.Tcons1.DISEQ,texpr)
    | Apron.Tcons1.DISEQ -> (Apron.Tcons1.EQ,texpr)
    | Apron.Tcons1.SUPEQ -> (Apron.Tcons1.SUP, negate_texpr texpr)
    | Apron.Tcons1.SUP -> (Apron.Tcons1.SUPEQ, negate_texpr texpr)
    | Apron.Tcons1.EQMOD _ -> failwith "EQMOD not supported now"
  in
  Apron.Tcons1.make ntexpr ntyp

let rec mkconjunct 
    (elist:Apron.Tcons1.t list)
    :
    Apron.Tcons1.t Boolexpr.t
    =
  match elist with
    | [] -> Boolexpr.make_cst true
    | [e] -> Boolexpr.make_condition e
    | e::el -> Boolexpr.make_and (Boolexpr.make_condition e) (mkconjunct el)

let rec mkdisjunct
    (elist:Apron.Tcons1.t list)
    :
    Apron.Tcons1.t Boolexpr.t
   =
  match elist with
    | [] -> Boolexpr.make_cst false
    | [e] -> Boolexpr.make_condition e
    | e::el -> Boolexpr.make_or (Boolexpr.make_condition e) (mkdisjunct el)

(* Build the list of atomic constraints in be which shall be a conjunct *)
let rec mkcons_list env 
    (be:Apron.Tcons1.t Boolexpr.t)
    :
    Apron.Tcons1.t list
    =
  match be with
    | Boolexpr.RANDOM -> [(true_cons env)]

    | Boolexpr.AND(be1,be2) ->
      let l1 = mkcons_list env be1 in
      let l2 = mkcons_list env be2 in
      List.append l1 l2

    | Boolexpr.CONDITION c -> [c]
    | Boolexpr.CST true -> [(true_cons env)]
    | Boolexpr.CST false -> [(false_cons env)]
    | _ -> 
      notimplemented "requires clause with disjuncts"


(*****************************************************************************)
(* Translation of Cil types                                                  *)
(*****************************************************************************)

(* Build a Clim type from a C type *)
let ctype ty =
  match unrollType ty with
    | TVoid _attr -> Clim_env.LCTnative Clim_env.Tunit

    | TInt(_ik,_attr) ->
        Clim_env.LCTnative Clim_env.Tinteger

    | TPtr(_elemty,_attr) ->
        if is_reference_type ty then
          (* Do not use [_elemty] directly but rather [pointed_type ty] in order
           * to get to the array element in references, i.e. pointers to arrays.
           *)
          begin match Cil.unrollType (pointed_type ty) with
            | TComp(compinfo,_,_) ->
	      (* TODO: size of reference not computed well because pb in common.ml:228 *)
		let refsize = 1 (* reference_size ty *) in
                let min_bound = Num.num_of_string "0" in
                let max_bound = 
                  Num.num_of_string (string_of_int (refsize - 1))
                in
		let rinfo = { Clim_env.lc_root_info_name = compinfo.cname;
			      Clim_env.lc_root_info_hroots = [];
			      Clim_env.lc_root_info_union_size_in_bytes = refsize; } in
                Clim_env.LCTpointer(Clim_env.LCroot rinfo,
				    Some min_bound,Some max_bound)
            | _ -> assert false
          end
        else
          begin match unrollType (pointed_type ty) with
            | TComp(compinfo,_,_) ->
	      (* TODO: see why is raising error in common.ml:228 *)
		let refsize = 1 (* reference_size ty *) in
		let rinfo = { Clim_env.lc_root_info_name = compinfo.cname;
			      Clim_env.lc_root_info_hroots = [];
			      Clim_env.lc_root_info_union_size_in_bytes = refsize; } in
                Clim_env.LCTpointer(Clim_env.LCroot rinfo,None,None)
            | _ -> assert false
          end

    | TArray _ -> assert false (* Removed by translation *)

    | TNamed _ -> assert false (* Removed by call to [unrollType] *)

    | TEnum(enuminfo,_) ->
	let einfo = { Clim_env.lc_enum_info_name = enuminfo.ename;
		      Clim_env.lc_enum_info_min = Num.num_of_int 0;
		      Clim_env.lc_enum_info_max = Num.num_of_int ((List.length enuminfo.eitems) - 1); } in
	Clim_env.LCTenum einfo

    | TFun _ ->
	unsupported "Function pointer type %a not allowed" !Ast_printer.d_type ty
    | TComp _ -> 
	unsupported "Type struct not allowed"
    | TFloat _ -> 
	unsupported "Type float not allowed"
    | TBuiltin_va_list _ -> 
	unsupported "Type builtin_va_list not allowed"

(* Return kind of type *)
let get_inout_type 
    (ty:Cil_types.typ) 
    : 
    bool * Clim_env.lc_type 
    =
  let lct = ctype ty in
  (* Do not use is_reference_type ty 
     because ctype eliminates all Ptr *)
  match ty with
    | TPtr _ -> false, lct
    | _ -> true, lct


(*****************************************************************************)
(* Translation of Cil constants.                                             *)
(*****************************************************************************)

let rec const ~in_code env = function
  | CInt64(i,_ik,_) ->
    Apron.Texpr1.cst env (Apron.Coeff.s_of_int (Int64.to_int i))

  | CStr _ | CWStr _ -> 
    assert false (* Should have been rewritten *)

  | CChr c -> 
    Apron.Texpr1.cst env (Apron.Coeff.s_of_int (Char.code c))

  | CReal _ ->
    notimplemented "Interp_eqn.cons Creal"

  | CEnum item ->
    (const_of_expr ~in_code env item.eival)

and const_of_expr ~in_code env 
    (e:Cil_types.exp)
    :
    Apron.Texpr1.t
    =
  match (stripInfo e).enode with
      Const c -> const ~in_code env c
    | _ -> assert false

and boolean_const (c:Cil_types.constant)
    :
    Apron.Tcons1.t Boolexpr.t
    = 
  match c with
  | CInt64(i,_ik,_text) ->
      if i = Int64.zero then 
	Boolexpr.make_cst false
      else Boolexpr.make_cst true

  | CStr _ | CWStr _ -> Boolexpr.make_cst true

  | CChr c ->
      if Char.code c = 0 then 
	Boolexpr.make_cst false
      else Boolexpr.make_cst true

  | CReal(f,_fk,_text) ->
      if f = 0.0 then Boolexpr.make_cst false else Boolexpr.make_cst true

  | CEnum {eival = e} -> boolean_const_of_expr e

and boolean_const_of_expr e =
  match (stripInfo e).enode with Const c -> boolean_const c | _ -> assert false


(*****************************************************************************)
(* Translation of Cil logic constructs                                       *)
(*****************************************************************************)

let term_lhost env = function
  | TVar v ->
      (try
         let li = Logic_env.find_logic_cons v in
         match
           li.l_labels with
             | [] -> mkexpr_var env v.lv_name
             | _ ->
                 Celia_options.fatal
                   "cannot handle logic constant %s with labels"
                   v.lv_name
       with Not_found ->
         mkexpr_var env v.lv_name)
  | TResult _ ->
      unsupported "cannot handle logic constant \\result"
  | TMem _ ->
      unsupported "this kind of memory access is not currently supported"


let isLogicConstant t = match t.term_node with
    TConst _ -> true
  | _ -> false

let product f t1 t2 =
  List.fold_right
    (fun x acc -> List.fold_right (fun y acc -> f x y :: acc) t2 acc) t1 []

let rec coerce_type env t =
  (*MS: for the moment signal floats and do nothing *)
  terms env t

and terms env 
    (t:Cil_types.term)
    :
    Apron.Texpr1.t list
    =
  match constFoldTermNodeAtTop t.term_node with
  | TConst c -> [const ~in_code:false env c]

  | TDataCons({ctor_type = {lt_name = name}},_args)
    when name = Utf8_logic.boolean ->
      [one_expr env]

  | TDataCons _
  | TUpdate _ ->
      notimplemented "Interp.terms TUpdate"

  | TLval lv ->
      (terms_lval env lv)

  | TSizeOf _ | TSizeOfE _ | TSizeOfStr _ | TAlignOf _ | TAlignOfE _ ->
      assert false (* Should be removed by constant folding *)

  | TUnOp(Neg,t) ->
      List.map 
	(fun x -> negate_texpr x)
	(coerce_type env t)

  | TUnOp _ ->
      unsupported "Interp_eqn.terms (LNot | BNot)"
 
  | TBinOp((Shiftrt | Shiftlt),_,_) ->
      unsupported "Interp_eqn.terms (Shiftrt | Shiftlt)"

  | TBinOp _ ->
      unsupported "Interp_eqn.terms relational BinOp"

  | TCastE _ ->
      unsupported "Interp_eqn.terms (TCastE) not allowed in logic"

  | TAddrOf _ -> assert false (* Should have been rewritten *)

  | TStartOf _ ->
      unsupported "Interp_eqn.terms (TStartOf) not allowed in logic"

  | Tapp(linfo,_,tlist) ->
      begin
	try
          let args = List.map (fun t -> (terms_ptr env [t])) tlist in
          let all_args = List.fold_right (product (fun x y -> x::y)) args [[]] in
          List.map
            (begin fun lp -> 
	      match lp with
		[x] -> mkexpr_fun linfo.l_var_info.lv_name x
	      | _ -> raise (CtePredicate false)
	    end) all_args
	with (CtePredicate _)->
	  [zero_expr env]
      end

  | Tif _ ->
      unsupported "Interp_eqn.terms (Tif) not allowed in logic"

  | Told _ ->
      unsupported "Interp_eqn.terms (Told) not allowed in logic"

  | Tbase_addr _ ->
      unsupported "Interp_eqn.terms (Tbase_adr) not allowed in logic"

  | Tat _
  | Tblock_length _ ->
      unsupported "Interp_eqn.terms (Tat | Tblock_length)"

  | Tnull -> [null_expr env]

  | Tlet _ ->
      (* TODO: no definition of new predicates yet, but allow 
         predefined predicates *)
      unsupported "Interp_eqn.terms Tlet"

  | TCoerce _ | TCoerceE _ ->
      (* TODO: see if useful *)
      unsupported "Interp_eqn.terms (TCoerce | TCoerceE)"

  | Tlambda _ ->
      unsupported "Interp_eqn.terms does not support lambda abstraction"

  | Ttypeof _ | Ttype _ -> assert false (* Should have been treated *)

  | Trange _ ->
      unsupported "Interp_eqn.terms Trange"

  | Tunion _ ->
      unsupported "Interp_eqn.terms Tunion"

  | Tcomprehension _ -> 
      unsupported "Interp_eqn.terms Tcomprehension"

  | Tinter _ -> 
      unsupported "Interp_eqn.terms Tinter"

  | Tempty_set -> []


and terms_ptr env 
    (lt:Cil_types.term list)
    : 
    Apron.Texpr1.t list
    =
  List.map (term_ptr env) lt

and term_ptr env t =
  if Logic_utils.isLogicPointerType t.term_type then
    begin
      match t.term_node with
      | TLval (TVar lv, TNoOffset) ->
          mkexpr_var env lv.lv_name
      | _ -> unsupported "Interp_eqn.term_ptr"
    end
  else
    raise (CtePredicate false)

and terms_lval env 
    (lv:Cil_types.term_lval)
    :
    Apron.Texpr1.t list
    =
  match lv with
    | lhost, TNoOffset -> [term_lhost env lhost]

    | (TVar _ | TResult _), _off ->
        assert false (* Should have been rewritten *)

    | TMem t, TField(fi,toff) ->
        assert (toff = TNoOffset); (* Others should have been rewritten *)
        let tl = terms env t in
        if not fi.fcomp.cstruct then (* field of union *)
	  notimplemented "Interp_eqn.terms_lval TMem field of union"
        else
	  List.map (fun x -> mkexpr_field fi.fname x) tl

    | TMem _e, TIndex _ ->
        assert false (* Should have been rewritten *)

and term env 
    (t:Cil_types.term)
    :
    Apron.Texpr1.t
    =
  match terms env t with [ tt ] -> tt
    | _ ->
	unsupported "Expecting a single term, not a set:@ %a@."
          !Ast_printer.d_term t

and term_lval env lv  =
  match terms_lval env lv with [ lv ] -> lv
    | _ ->
	unsupported "Expecting a single lval, not a set:@ %a@."
          !Ast_printer.d_term_lval lv

and pred env (p:Cil_types.predicate)
    : Apron.Tcons1.t Boolexpr.t
    =
  match p with
    | Pfalse -> Boolexpr.make_cst false

    | Ptrue -> Boolexpr.make_cst true

    | Papp(pinfo,_,tl) ->
      (* Predicates may be only the predefined ones *)
      (if Celia_options.debug_atleast 1 then
	Celia_options.feedback "Requires = Papp");
      begin
	try
          translated_pred env pinfo tl
	with 
	    (CtePredicate b) -> Boolexpr.make_cst b
      end

    | Prel((Rlt | Rgt | Rle | Rge),t1,_)
        when app_term_type isPointerType false t1.term_type ->
	unsupported "Interp_eqn.terms comparison between pointers"

    | Prel((Req | Rneq),t1,_) when isTypeTagType t1.term_type ->
        unsupported "Interp_eqn.terms comparison between tags"

    | Prel(rel,t1,t2) ->
        let res =
          product (fun t1 t2 -> mkcons t1 rel t2)
            (coerce_type env t1) (coerce_type env t2)
        in (mkconjunct res)

    | Pand(p1,p2) ->
        Boolexpr.make_and (pred env p1.content) (pred env p2.content)

    | Por(p1,p2) ->
        Boolexpr.make_or (pred env p1.content) (pred env p2.content)

    | Pxor(p1,p2) ->
        let bep1 = pred env p1.content in
	let bep2 = pred env p2.content in
	let notp1 = Boolexpr.make_not negate_tcons bep1 in
        let p2notp1 = Boolexpr.make_and notp1 bep2 in
	let notp2 = Boolexpr.make_not negate_tcons bep2 in
        let p1notp2 = Boolexpr.make_and bep1 notp2 in
        Boolexpr.make_or p1notp2 p2notp1

    | Pimplies(p1,p2) ->
        let bep1 = pred env p1.content in
	let bep2 = pred env p2.content in
	let notp1 = Boolexpr.make_not negate_tcons bep1 in
        Boolexpr.make_or notp1 bep2

    | Piff(p1,p2) ->
        let bep1 = pred env p1.content in
	let bep2 = pred env p2.content in
	let notp1 = Boolexpr.make_not negate_tcons bep1 in
        let notp1p2 = Boolexpr.make_or notp1 bep2 in
	let notp2 = Boolexpr.make_not negate_tcons bep2 in
        let notp2p1 = Boolexpr.make_or notp2 bep1 in
        Boolexpr.make_and notp1p2 notp2p1 

    | Pnot p -> 
	Boolexpr.make_not negate_tcons (pred env p.content)

    | Pif _ ->
	unsupported "Interp_eqn.pred Pif."

    | Plet _ ->
	unsupported "Interp_eqn.pred Plet."

    | Pforall([],p) -> 
	(pred env p.content)

    | Pforall _ ->
	unsupported "Interp_eqn.pred Pforall."

    | Pexists([],p) -> 
	(pred env p.content)

    | Pexists _ ->
	unsupported "Interp_eqn.pred Pexists."

    | Pold _ ->   
	unsupported "Interp_eqn.pred Pold." 

    | Pat _
    | Pvalid_index _
    | Pvalid_range _ 
    | Pvalid _  
    | Pfresh _
    | Psubtype _
    | Pseparated _ ->
      notimplemented "Interp_eqn.pred (Pat | Pvalid | Pfresh | Psubtype | Pseparated)"

(* Compute the constraint for a predefined predicate, i.e.,
	   - x > 0.0         for acyclic(x)
	   - cast_f,n(x) > x for cyclic(x)
	   - x - y = 0 mod 1 for disjoint(x,y)
	   - x - y = 0 mod 0 for iso(x,y)
	   - x > y           for reach(x,y)
	   - x == 0.0        for x is NULL
	   - x <> 0.0        for x is not NULL 
 *)
and translated_pred env 
    (li:Cil_types.logic_info)
    (tl:Cil_types.term list)
    :
    Apron.Tcons1.t Boolexpr.t
    =
  (if Celia_options.debug_atleast 1 then
      Celia_options.feedback "Translate predicate in requires %s" 
	li.l_var_info.lv_name);
  match li.l_var_info.lv_name with
  | "acyclic" ->
      begin
	match terms_ptr env tl with
	| [x] -> 
	    Boolexpr.make_condition (Apron.Tcons1.make x Apron.Tcons1.SUP)
	| _ -> raise (CtePredicate false)
      end

  | "cyclic" ->
      begin
	match terms_ptr env tl with 
	| [x] -> 
	    let xnext = mkexpr_field "next" x in
	    mkconjunct [mkcons xnext Rgt x]
	| _ -> raise (CtePredicate false)
      end

  | "disjoint" ->
      begin
	match terms_ptr env tl with
	| [x;y] -> 
	    let e = Apron.Texpr1.binop Apron.Texpr1.Sub x y 
		Apron.Texpr1.Real Apron.Texpr1.Rnd
	    in
	    Boolexpr.make_condition 
	      (Apron.Tcons1.make e 
		 (Apron.Tcons1.EQMOD (Apron.Scalar.of_int 1)))
	| _ -> raise (CtePredicate false)
      end

  | "iso" ->
      begin
	match terms_ptr env tl with
	| [x;y] -> 
	    let e = Apron.Texpr1.binop Apron.Texpr1.Sub x y 
		Apron.Texpr1.Real Apron.Texpr1.Rnd
	    in
	    Boolexpr.make_condition 
	      (Apron.Tcons1.make e 
		 (Apron.Tcons1.EQMOD (Apron.Scalar.of_int 0)))
	| _ -> raise (CtePredicate false)
      end

  | "reach" ->
      begin
	match terms_ptr env tl with
	| [x;y] -> 
	    mkconjunct [mkcons x Rgt y]
	| _ -> raise (CtePredicate false)
      end

  | _ ->
      unsupported "Predicate not implemented %s." li.l_var_info.lv_name


(*****************************************************************************)
(* Translation of Cil expressions                                            *)
(*****************************************************************************)

let set_curFundec, get_curFundec =
  let cf = ref None in
  ((fun f -> cf := Some f),
   (fun () ->
      match !cf with
          None ->
            let res = emptyFunction "@empty@" in cf := Some res; res
        | Some res -> res))

let rec expr env e =
  let e = stripInfo e in
  match e.enode with
  | Info _ -> assert false

  | Const c -> const ~in_code:true env c

  | Lval lv -> expr_lval env lv

  | SizeOf ty ->
      mkexpr_cst_int env (Cil.bitsSizeOf ty)

  | SizeOfE _ | SizeOfStr _ | AlignOf _ | AlignOfE _ ->
      assert false (* Should be removed by constant folding *)

  | UnOp(_op,_e,ty) when isIntegralType ty ->
      integral_expr env e

  | UnOp(_op,_e,_ty) ->
      assert false (* Should not appear after checking *)

  | BinOp(_op,_e1,_e2,ty) when isIntegralType ty ->
      integral_expr env e

  | BinOp(_op,_e1,_e2,_ty) ->
      assert false (* Should not appear after checking *)

  | CastE(ptrty,_e1) when isPointerType ptrty ->
      begin
        let e = stripCastsAndInfo e in
        match e.enode with
        | Const c
          when is_integral_const c
              && value_of_integral_const c = Int64.zero ->
		null_expr env
        | _ ->
	      unsupported "Casting from type %a to type %a not allowed"
                !Ast_printer.d_type (typeOf e) !Ast_printer.d_type ptrty
      end

  | CastE(ty,e') ->
    notimplemented  "Casting from type %a to type %a not allowed in %a with size %Ld and %Ld"
      !Ast_printer.d_type (typeOf e') !Ast_printer.d_type ty !Ast_printer.d_exp e
      ( bits_sizeof ty) ( bits_sizeof (typeOf e'))


  | AddrOf _lv -> assert false (* Should have been rewritten *)

  | StartOf lv -> expr_lval env lv


and expr_lval env
    (lv:Cil_types.lval)
    :
    Apron.Texpr1.t
    =
  match lv with
    | lhost, NoOffset -> expr_lhost env lhost

    | Var _, _off ->
        assert false (* Should have been rewritten *)

    | Mem e, Field(fi,toff) ->
        assert (toff = NoOffset); (* Others should have been rewritten *)
        let te = expr env e in
        if not fi.fcomp.cstruct then (* field of union *)
	  notimplemented "Interp_eqn.expr_lval Mem field of union"
        else
	  mkexpr_field fi.fname te

    | Mem _e, Index _ ->
        assert false (* Should have been rewritten *)


and expr_lhost env = function
  | Var v -> mkexpr_var env v.vname

  | Mem _ ->
      unsupported "this kind of memory access is not currently supported"

(* Translation of expressions that shall be boolean, i.e., combined
   with logic "and", "or".
*)
and boolean_expr env 
    (e:Cil_types.exp)
    :
    Apron.Tcons1.t Boolexpr.t
    =
  let boolean_node_from_expr (ty:Cil_types.typ) (e:Apron.Texpr1.t) =
    if isPointerType ty then mkcons e Rneq (null_expr env)
    else if isArithmeticType ty then mkcons e Rneq (zero_expr env)
    else assert false
  in
  match (stripInfo e).enode with
  | Info _ -> assert false

  | Const c -> boolean_const c

  | UnOp(LNot,e,_typ) -> Boolexpr.make_not negate_tcons (boolean_expr env e)

  | BinOp(LAnd,e1,e2,_typ) ->
      let be1 = boolean_expr env e1 in
      let be2 = boolean_expr env e2 in
      Boolexpr.make_and be1 be2

  | BinOp(LOr,e1,e2,_typ) ->
      let be1 = boolean_expr env e1 in
      let be2 = boolean_expr env e2 in
      Boolexpr.make_or  be1 be2

  | BinOp((Lt | Gt | Le |  Ge | Eq | Ne) as op,e1,e2,_typ) ->
      let te1 = expr env e1 in
      let te2 = expr env e2 in
      mkconjunct [(mkcons te1 (relation op) te2)]

  | _ -> mkconjunct [(boolean_node_from_expr (typeOf e) (expr env e))]


(* Translation of an expression that fit in a C integral type.
 *)
and integral_expr env 
    (e:Cil_types.exp)
    :
    Apron.Texpr1.t
    =
  match e.enode with
    | UnOp(Neg,_e,_ty) ->
      Apron.Texpr1.unop Apron.Texpr1.Neg (expr env _e) 
	Apron.Texpr1.Int Apron.Texpr1.Near
	
    | UnOp(_op,_e,_ty) ->
      unsupported  "Interp_eqn.integral_expr integer logic negation"

    | BinOp((LAnd | LOr),_e1,_e2,_ty) ->
      unsupported  "Interp_eqn.integral_expr integer logic op"

    | BinOp((Eq | Ne | Lt | Gt | Le | Ge),_,_,_) ->
        unsupported  "Interp_eqn.integral_expr integer comparison"

    | BinOp((Shiftrt | Shiftlt),_,_,_) ->
      unsupported "Interp_eqn.integral_expr (Shiftrt | Shiftlt)"

    | BinOp((PlusA | MinusA | Mult | Div | Mod) as op,e1,e2,_ty) ->
      mkexpr_binop (expr env e1) op (expr env e2)

    | CastE(_,e1) when isFloatingType (typeOf e1) ->
      unsupported  "Interp.integral_expr CastE from float"

    | CastE(_,e) when isIntegralType (typeOf e) ->
      integral_expr env e

    | _ -> expr env e

and lval 
    (lv:Cil_types.lval)
    :
    Apron.Var.t * string
    = 
  match lv with
    | Var v, NoOffset -> Apron.Var.of_string v.vname, ""

    | Var _v, _off -> assert false (* Should have been rewritten *)

    | Mem _e, NoOffset -> assert false (* Should have been rewritten *)

    | Mem {enode = Lval(Var v,NoOffset)}, Field(fi,off) ->
      assert (off = NoOffset); (* Others should have been rewritten *)
      if fi.fcomp.cstruct then
        Apron.Var.of_string v.vname, fi.fname
      else (* field of union *)
	unsupported "Interp_eqn.lval pos for unions"

    | _ ->
      unsupported "Unexpected lval %a" !Ast_printer.d_lval lv

and expr_as_param (* env *)
    (e:Cil_types.exp)
    :
    Apron.Var.t * Apron.Var.t (* Apron.Texpr1.t *)
    =
  match (stripInfo e).enode with
  | AddrOf lv  (* TODO: Normally rewritten *)
  | Lval lv -> 
    let v,s = lval lv in
    if s <> "" && not(framac_field s) then
      unsupported "Unexpected pointer dereference %a as function parameter"
	!Ast_printer.d_exp e
    else
      v, v (* mkexpr_var env (Apron.Var.to_string v) *)
  | Const _ (* c ->  (Apron.Var.of_string "dummy"), const false env c  *)
  | _ -> 
    unsupported "Unexpected expr as lval %a" !Ast_printer.d_exp e


(*****************************************************************************)
(* Forward equations *)
(*****************************************************************************)

(* Return input and output list of actual parameters.
   Actual input parameters may be variable or constant,
   Actual output parameters may be only left values.
*)
let keep_only_declared_nb_of_arguments (* env *) 
    (vi:Cil_types.varinfo)
    (l:Cil_types.exp list)
    :
    (* Apron.Texpr1.t list * Apron.Var.t list *)
    Apron.Var.t list * Apron.Var.t list
    =
  let _,args,is_variadic, _ = splitFunctionTypeVI vi in
  if args=None then
    (warning "skipping all arguments of implicit prototype %s" vi.vname;
     [],[])
  else if is_variadic then unsupported "unsupported variadic functions"
  else 
    (* split @code{l} wrt to the kind of attributes (in,out) *)
    let rpin, rpout =
      List.fold_left2
	(begin fun (pin,pout) arg formal ->
	  (* put actual param a in pin or pout depending on the kind of f *)
          let subarg = stripCasts arg in 
	  let v,e = expr_as_param (* env *) subarg in 
	  match formal with
	      (_name,_ty,_attrs) -> 
		let b,_ = get_inout_type _ty in
		if b then (* in *)
		  (e::pin,pout)
		else
		  (pin,v::pout)
	 end)
	([],[])
	l
	(match args with None -> [] | Some al -> al)
    in
    List.rev rpin, List.rev rpout

let rec instruction env 
    (procinfo:Cinv_equation.procinfo) 
    (bpoint:Cinv_equation.point)
    (sid:int)
    (i:Cil_types.instr)
    (graph:Cinv_equation.graph)
    :
    Cinv_equation.point
    = 
  match i with
  | Set(lv,e,pos) ->
      (** Supposed to be in normal form! *)
      let te = (expr env e) in
      let v,fi = (lval lv) in 
      let te' = mkexpr_deref te fi in
      let transfer = Cinv_equation.Tassign(v,te') in
      let dest = (point_of_instr pos sid) in
      begin
	Cinv_equation.add_equation graph [|bpoint|] transfer dest;
	dest
      end

  | Cil_types.Call(res,{enode = Lval(Var v,NoOffset)},eargs,pos) ->
      (** Function call *)
      let dest = (point_of_instr pos sid) in
      (try
      begin
	(if (is_assert_function v) && res=None then (* assert *)
	  let bte = boolean_expr env (as_singleton eargs) in
	  let transfer = Cinv_equation.Condition(bte) in (* TODO: correct assert *)
	  Cinv_equation.add_equation graph [|bpoint|] transfer dest

	else if (is_free_function v) && res=None then (* free *)
            let arg = as_singleton eargs in
            let subarg = stripCasts arg in
            let arg = if isPointerType (typeOf subarg) then subarg else arg in
	    let x,_ = expr_as_param (* env *) arg in
	    let te' = mkexpr_free env in
	    let transfer = Cinv_equation.Tassign(x,te') in
	    Cinv_equation.add_equation graph [|bpoint|] transfer dest

	else if (is_printf_function v) && res=None then (* printf *)
	  let transfer = Cinv_equation.Condition(mkconjunct []) in (* TODO: correct printf *)
	  (warning "skipping library function printf.";
	   Cinv_equation.add_equation graph [|bpoint|] transfer dest)
    
	else if (is_malloc_function v 
		 || is_realloc_function v 
		 || is_calloc_function v)	
	then (* alloc *)
	  (* arguments have been checked *)
	  let lv = (match res with Some lv -> lv | _ -> assert false) in
	  let x,_ = lval lv in (* TODO: fi shall be empty after rewriting *)
	  let te = mkexpr_alloc env in
	  let transfer = Cinv_equation.Tassign(x,te) in
	  Cinv_equation.add_equation graph [|bpoint|] transfer dest

        else (* general case *)
	  let info = PSHGraph.info graph in
	  let callee = Hashhe.find info.Cinv_equation.procinfo v.vname in
	  let pinl, poutl = keep_only_declared_nb_of_arguments (* env *) v eargs in
	  (* compute actual result *)
	  let x = (match res with None -> [] 
	  | Some lv -> (let r,_ = lval lv in [r]))
	  in
          let pin = Array.of_list pinl in
          let pout = Array.of_list (poutl@x) in
          let calltransfer = Cinv_equation.Call(procinfo,callee,pin,pout) in
          let rettransfer = Cinv_equation.Return(procinfo,callee,pin,pout) in
	  begin
            Cinv_equation.add_equation graph
	      [|bpoint|] calltransfer callee.Cinv_equation.pstart;
            Cinv_equation.add_equation graph
	      [|bpoint; callee.Cinv_equation.pexit|] rettransfer dest
	  end
	)
      end

       with Not_found -> (* function called not found, replace call by skip *)
	 begin
	   let transfer = Cinv_equation.Condition(mkconjunct []) in
	   Cinv_equation.add_equation graph [|bpoint|] transfer dest
	 end
      );
      dest

  | Cil_types.Call _ -> assert false

  | Asm _ -> assert false

  | Skip pos ->
      let dest = (point_of_instr pos sid) in
      let transfer = Cinv_equation.Condition(mkconjunct []) in
      Cinv_equation.add_equation graph [|bpoint|] transfer dest;
      dest

  | Code_annot _ -> (* TODO: verify *)
      bpoint

let loop_entry_points : Cinv_equation.point list ref = ref []
let loop_exit_points : Cinv_equation.point list ref = ref []
let push_loop bpoint dest =
  begin
    (if Celia_options.debug_atleast 2 then
	Celia_options.feedback "push loop!");
    loop_entry_points := bpoint::!loop_entry_points;
    loop_exit_points := dest::!loop_exit_points;
    (* 
       Format.printf "loop entry point ";
       List.iter (fun e -> Format.printf " (%d) " e.Cinv_equation.sid) !loop_entry_points;
       Format.printf "loop exit point ";
       List.iter (fun e -> Format.printf " (%d) " e.Cinv_equation.sid) !loop_exit_points;
    *)
    ()
  end
let pop_loop () =
  begin
    (if Celia_options.debug_atleast 2 then
	Celia_options.feedback "pop loop!");
    loop_entry_points := List.tl !loop_entry_points;
    loop_exit_points := List.tl !loop_exit_points
  end

let rec statement env  
    (procinfo:Cinv_equation.procinfo) 
    (bpoint:Cinv_equation.point)
    (s:Cil_types.stmt)
    (graph:Cinv_equation.graph)
    :
    Cinv_equation.point
    =
  (if Celia_options.debug_atleast 1 then
      Celia_options.feedback "statement %d" s.Cil_types.sid);
  match s.skind with
  | Instr i -> instruction env procinfo bpoint s.Cil_types.sid i graph

  | Cil_types.Return(Some e,pos) ->
      let dest = (point_of_instr pos s.Cil_types.sid) in
      (* e shall be a variable after normalization *)
      (* return is the last statement of the list *)
      let te = (expr env e) in
      let v = Apron.Var.of_string "\\return" in 
      let transfer = Cinv_equation.Tassign(v,te) in
      begin
	Cinv_equation.add_equation graph [|bpoint|] transfer dest;
	dest
      end

  | Cil_types.Return(None,pos) ->
      let dest = (point_of_instr pos s.Cil_types.sid) in
      let transfer = Cinv_equation.Condition(mkconjunct []) in
      begin
	Cinv_equation.add_equation graph [|bpoint|] transfer dest;
	dest
      end

  | Goto(sr,pos) ->
      (* TODO: check that Should not occur after normalize *)
      let dest = (point_of_instr pos s.Cil_types.sid) in
      let target = (point_of_stmt !sr) in
      let transfer = Cinv_equation.Condition(mkconjunct []) in
      begin
	Cinv_equation.add_equation graph [|bpoint|] transfer target;
	dest
      end

  | Break pos ->
    let dest = (point_of_instr pos s.Cil_types.sid) in
    begin match !loop_exit_points with
	[] -> assert false
      | _ -> begin
	let lexit = List.hd !loop_exit_points in
	let transfer = Cinv_equation.Condition(mkconjunct []) in
	begin
	  Cinv_equation.add_equation graph [|bpoint|] transfer lexit;
	  dest
	end
      end
    end
  (* Should not occur after [prepareCFG] *)

  | Continue pos ->
    let dest = (point_of_instr pos s.Cil_types.sid) in
    let lentry = List.hd !loop_entry_points in
    let transfer = Cinv_equation.Condition(mkconjunct []) in
    begin
      Cinv_equation.add_equation graph [|bpoint|] transfer lentry;
      dest
    end
  (* Should not occur after [prepareCFG] *)

  | If(e,bl1,bl2,pos) ->
      let dest = (point_of_instr pos s.Cil_types.sid) in
      let cond = (boolean_expr env e) in
      let condnot = Boolexpr.make_not negate_tcons cond in
      let condtransfer = Cinv_equation.Condition(cond) in
      let condnottransfer = Cinv_equation.Condition(condnot) in
      let truetransfer = Cinv_equation.Condition(mkconjunct []) in
      let bpoint1 = (point_of_block_if s) in
      let bpoint2 = (point_of_block_else s) in
      begin
	begin (* block if *)
	  if (List.length bl1.bstmts) = 0 then (* empty bl1 *)
	    Cinv_equation.add_equation graph [|bpoint|] condtransfer dest
	  else
	    let epoint1 = statement_list env procinfo bpoint1 bl1.bstmts graph in
	    begin
	      Cinv_equation.add_equation graph [|bpoint|] condtransfer bpoint1;
	      Cinv_equation.add_equation graph [|epoint1|] truetransfer dest
	    end
	end;
	begin (* block else *)
	  if (List.length bl2.bstmts) = 0 then
	    Cinv_equation.add_equation graph [|bpoint|] condnottransfer dest
	  else
	    let epoint2 = statement_list env procinfo bpoint2 bl2.bstmts graph in
	    begin
	      Cinv_equation.add_equation graph [|bpoint|] condnottransfer bpoint2;
	      Cinv_equation.add_equation graph [|epoint2|] truetransfer dest
	    end
	end;
	dest
      end

  | Switch _ ->
      (* Should not occur after [prepareCFG] *)
      unsupported "Interp_eqn.statement Switch"

  | Loop (_,bl,pos,_continue_stmt,_break_stmt) ->
      let dest = (point_of_instr pos s.Cil_types.sid) in
      let truetransfer = Cinv_equation.Condition(mkconjunct []) in
      begin
        (* push on loop_*_points, popped below *)
	(push_loop bpoint dest);
	if (List.length bl.bstmts) = 0 then (* infinite loop *)
	  Cinv_equation.add_equation graph [|bpoint|] truetransfer bpoint
	else
	  begin
	    (* do the block, link the exit point *)
	    let epoint = statement_list env procinfo bpoint bl.bstmts graph in
	    Cinv_equation.add_equation graph [|epoint|] truetransfer bpoint
	  end
	;
	(* MAJ loop_*_points *)
	pop_loop ();
	dest
      end

  | Block bl ->
      statement_list env procinfo bpoint bl.bstmts graph
	(* returns the exit point *)

  | UnspecifiedSequence seq ->
      (* (MS) Choose the order given by the list of statements *)
      let stmts = List.map (fun (s,_,_,_) -> s) seq in
      statement_list env procinfo bpoint stmts graph     

  | TryFinally _ | TryExcept _ -> assert false

and statement_list env
    (procinfo:Cinv_equation.procinfo) 
    (bpoint:Cinv_equation.point)
    (stmts:Cil_types.stmt list)
    (graph:Cinv_equation.graph)
    :
    Cinv_equation.point 
    =
  List.fold_left 
    (begin fun bp st ->
      statement env procinfo bp st graph
    end)
    bpoint
    stmts

(*****************************************************************************)
(* Collecting global infos for functions                                     *)
(*****************************************************************************)

let filter_fun (gdec:Cil_types.global) : bool 
  = 
  match gdec with
  | GFun _ -> true
  | _ -> false

let is_intlist_type ty = 
  match Cil.unrollType (pointed_type ty) with
  | TComp(compinfo,_,_) -> compinfo.cstruct (* TODO: be sure that only intlist is accepted *)
  | _ -> assert false

(** Compute the position of the first statement in a block. *)
(** Uses infos filled by !Cfg.computeCFG.*)
let rec start_of_block (loc:Cil_types.location) : Cinv_equation.point
  =
  { pos = (fst loc);
    sid = 0; 
  }

(** Compute the position of the last (return) statement in a block. *)
and exit_of_block  (pcode:Cil_types.stmt list) : Cinv_equation.point 
  =
  match pcode with
  | [] -> assert false
  | [s] -> let loc = get_stmtLoc s.skind in
    { pos = (fst loc);
      sid =  s.Cil_types.sid;
    }
  | _::ls -> exit_of_block ls 
(* TODO: what about return not beeing the last stmt? *)

(** Convert formal parameters in input/output parameter lists. *)
let rec convert_in_out 
    (outf:bool) 
    (fvars:Cil_types.varinfo list) 
    : 
    (Apron.Var.t * Clim_env.lc_type) list 
    * (Apron.Var.t * Clim_env.lc_type) list
  =
  match fvars with
  | [] -> [],[]
  | vi::vl -> 
      let in1,out1 = convert_in_out outf vl in
      let b,ty = get_inout_type vi.vtype in
      if b then (* in *)
	(Apron.Var.of_string vi.vname, ty)::in1, out1
      else if outf then (* out *)
	in1, (Apron.Var.of_string vi.vname, ty)::out1
      else
	(* Special case of local variables of type reference to intlist *)
	(Apron.Var.of_string vi.vname, ty)::in1, out1
	(* unsupported "Local reference variable %a " !Ast_printer.d_var vi *)

(** Convert return into a variable \return if not void type. *)
let rec convert_return 
    (ty:Cil_types.typ) 
    :
    (Apron.Var.t * Clim_env.lc_type) list
    =
  match ty with
  | TFun (rt,_,_,_) ->
      begin
	let lct = ctype rt in
	match lct with
	| Clim_env.LCTnative Clim_env.Tunit -> []
	| _ ->
	    if (is_reference_type ty) then
	      unsupported "Function with reference in return"
	    else
	      [(Apron.Var.of_string "\\return",lct)]
      end
  | _ -> assert false

(** Convert list of typed variables into arrays of Apron variables. *)
let convert_vars 
    (vlist:(Apron.Var.t * Clim_env.lc_type) list)
    : 
    Apron.Var.t array
    =
  Array.of_list (List.map (fun (v,_) -> v) vlist)
 
(** Add to an environment a list of typed variables. *)
let add_env
    (env:Apron.Environment.t)
    (lvartyp:(Apron.Var.t * Clim_env.lc_type) list)
    : 
    Apron.Environment.t
  =
  let (lint,lreal) =
    List.fold_left
      (begin fun (lint,lptr) (var,ty) ->
        match ty with
	| Clim_env.LCTnative _ | Clim_env.LCTenum _ -> (var::lint,lptr)
        | Clim_env.LCTpointer _ -> (lint,var::lptr)
	| _ -> assert false
      end)
      ([],[])
      lvartyp
  in
  Apron.Environment.add env
    (Array.of_list lint)
    (Array.of_list lreal)


(** Convert ACSL requirement expression into an array of tree constraints. *)
let rec convert_require 
    (vlist:(Apron.Var.t * Clim_env.lc_type) list)
    (iplist:Cil_types.identified_predicate list) 
    : Apron.Tcons1.earray
    =
  (if Celia_options.debug_atleast 1 then
      Celia_options.feedback "Collect prequires (%d predicates)" 
	(List.length iplist));  
  (** The environment is given by the formals *)
  let penv = Apron.Environment.make [||] [||] in
  let penv = add_env penv vlist in

  (** Build the list of conjuncts. *)
  let plist = List.map (fun ip -> ip.ip_content) iplist in
  try
    let bexpr = List.fold_left 
      (fun be p -> Boolexpr.make_and be (pred penv p)) 
      (Boolexpr.make_cst true)
      plist
    in
    let cl = mkcons_list penv bexpr in
    let size = List.length cl in
    let carray = Apron.Tcons1.array_make penv size in
    let rec set_tarray i l =
      begin
	match l with
	    [] -> ()
	  | c::cl -> Apron.Tcons1.array_set carray i c;
	    set_tarray (succ i) cl
      end
    in
    begin
      set_tarray 0 cl;
      carray
    end
  with (CtePredicate _) ->
    Apron.Tcons1.array_make penv 1



(** Build a [Cinv_equation.procinfo] object from [Cil_types.global]. *)
let make_procinfo (gdec:Cil_types.global) 
    : 
    Cinv_equation.procinfo * string
  =
  match gdec with
  | GFun(fd,loc) ->
    (if Celia_options.debug_atleast 1 then
	Celia_options.feedback "Collect infos procedure %s..." 
	  fd.svar.vname); 
    let funspec = Kernel_function.get_spec (Globals.Functions.get fd.svar) in
    let (pcode:Cil_types.stmt list) = fd.sallstmts in
      let pstart = start_of_block loc in
      let pexit = exit_of_block pcode in

      let vinput, voutput = convert_in_out true fd.sformals in
      let vreturn = convert_return fd.svar.vtype in
      let vlocal, _ = convert_in_out false fd.slocals in
      let vinput_old =
	List.map
	  (begin fun (v,ty) ->
            (Apron.Var.of_string (Format.sprintf "_%%old%s%%_" (Apron.Var.to_string v)), ty)
	  end)
	  vinput
      in
      let vinput_ini =
	List.fold_left
	  (begin fun l (v,ty) ->
            match ty with
	      | Clim_env.LCTpointer _ -> 
		(Apron.Var.of_string (Format.sprintf "_%%ini%s%%_" (Apron.Var.to_string v)), ty)::l
	      | _ -> l
	  end)
	  []
	  vinput
      in

      let pinput = convert_vars vinput in
      let pinput_old = convert_vars vinput_old in
      let pinput_ini = convert_vars (List.rev vinput_ini) in
      let poutput = convert_vars (voutput@vreturn) in
      let plocal = convert_vars (vlocal@vinput) in

      let penv = Apron.Environment.make [||] [||] in
      let penv = add_env penv vinput in
      let penv = add_env penv vinput_old in
      let penv = add_env penv vinput_ini in
      let penv = add_env penv voutput in
      let penv = add_env penv vreturn in
      let penv = add_env penv vlocal in

      let prequires = convert_require 
	  (*List.append vinput voutput*)
	  vinput (* limitation of cinv *)
	  funspec.spec_requires in

      let poutput_tmp =
	Array.mapi
	  (begin fun i _ ->
            Apron.Var.of_string (Format.sprintf "_%%out%i%%_" i)
	  end)
	  poutput
      in
      let pinput_tmp =
	Array.mapi
	  (begin fun i _ ->
            Apron.Var.of_string (Format.sprintf "_%%in%i%%_" i)
	  end)
	  pinput_ini
      in
      {
       Cinv_equation.pname = fd.svar.vname;
       Cinv_equation.pstart = pstart;
       Cinv_equation.pexit = pexit;
       Cinv_equation.pinput = pinput;
       Cinv_equation.pinput_old = pinput_old;
       Cinv_equation.pinput_ini = pinput_ini;
       Cinv_equation.poutput = poutput;
       Cinv_equation.plocal = plocal;
       Cinv_equation.penv = penv;
       Cinv_equation.pinput_tmp = pinput_tmp;
       Cinv_equation.poutput_tmp = poutput_tmp;
       Cinv_equation.prequires = prequires;
     },
      fd.svar.vname

  | _ -> assert false

(* Apply function f on each instruction *)
let rec iter_instr 
    (f:Cinv_equation.point * Cil_types.instr * int -> unit)
    (proc:Cil_types.global)
    :
    unit
    =
  let bpoint,stmts = 
    match proc with
	GFun(fd,loc) -> 
	  let start = start_of_block loc in
	  start,fd.sallstmts
      | _ -> assert false
  in
  let rec iter_instr_block f beforepoint slist = 
    ignore begin
      List.fold_left
	(begin fun bp s ->
	  begin match s.skind with
	      Instr i -> f (bp,i,s.Cil_types.sid)
	    | Cil_types.If(_e,b1,b2,_l) -> 
	      iter_instr_block f (point_of_block_if s) b1.bstmts;
	      iter_instr_block f (point_of_block_else s) b2.bstmts
	    | Loop (_,b,_,_,_) ->
	      iter_instr_block f bp b.bstmts
	    | Block b ->
	      iter_instr_block f bp b.bstmts
	    | UnspecifiedSequence l ->
	      iter_instr_block f bp (List.map (fun (s,_,_,_) -> s) l)
	    | Cil_types.Return _ | Break _ | Continue _ -> ()
	    | Goto _ -> () (* TODO: change point *)
	    | _ -> assert false
	  end;
	  (point_of_stmt s)
	 end)
	beforepoint
	slist
    end
  in
  (iter_instr_block f bpoint stmts) 
  ;
  ()
    
(* Apply function f on each statement *)
let rec iter_stmt 
    (f:Cil_types.stmt -> unit) 
    (s:Cil_types.stmt)
    :
    unit
    =
  ignore begin
    f s;
    begin match s.skind with
      Instr _ -> ()

    | If(_e,b1,b2,_l) -> 
	List.iter (iter_stmt f) b1.bstmts;
	List.iter (iter_stmt f) b2.bstmts

    | Loop (_,b,_,_,_) ->
      List.iter (iter_stmt f) b.bstmts

    | Block b ->
      List.iter (iter_stmt f) b.bstmts

    | UnspecifiedSequence l ->
      List.iter (iter_stmt f) (List.map (fun (s,_,_,_) -> s) l)

    | Cil_types.Return _ | Break _ | Continue _ | Goto _ -> ()
    | _ -> assert false
    end
  end

(** Build a [Cinv_equation.info] from [Cil_types.file]. *)
let rec make_info (procs:Cil_types.global list) : Cinv_equation.info =

  (** Collect procedure infos *)  
  let procinfo = Hashhe.create 3 in
  List.iter
    (begin fun gdec ->
      let info, fname = make_procinfo gdec in
      Hashhe.add procinfo fname info
    end)
    procs
    ;
  
  (** Collect call-return info *)
  let callret = DHashhe.create 3 in
  List.iter
    (begin fun proc ->
      iter_instr
      (begin fun (bpoint,i,sid) ->
        begin match i with
        | Cil_types.Call(_,_,_,loc) -> 
	    let p = (point_of_instr loc sid) in
	    DHashhe.add callret bpoint p
        | _ -> ()
        end
      end)
      proc
    end)
    procs
    ;
  
  (** Collect environment info *)
  let pointenv = Hashhe.create 3 in
  List.iter
    (begin fun proc -> 
      (* not an indenpendent function because need procinfo *)
      begin match proc with
	GFun(fd,loc) ->
	  begin 
	    let pinfo = Hashhe.find procinfo fd.svar.vname in
	    let env = pinfo.Cinv_equation.penv in
	    let pstart = start_of_block loc in
	    if not (Hashhe.mem pointenv pstart) then
	      Hashhe.add pointenv pstart env
	    ;
	    List.iter (iter_stmt 
	      (begin fun s ->
		begin
		  let p = (point_of_stmt s) in
		  if not (Hashhe.mem pointenv p) then
		    Hashhe.add pointenv p env;
		  match s.skind with
		  | If _ ->
		      let p1 = (point_of_block_if s) in
		      let p2 = (point_of_block_else s) in
		      Hashhe.add pointenv p1 env;
		      Hashhe.add pointenv p2 env
		  | _ -> ()			
		end
	      end))
	      fd.sallstmts
	  end
      | _ -> assert false
      end
    end)
    procs
    ;
  {
   Cinv_equation.procinfo = procinfo;
   Cinv_equation.callret = callret;
   Cinv_equation.pointenv = pointenv;
   Cinv_equation.counter = 0;
 }

    
(*****************************************************************************)
(* Adding block of functions to graph *)
(*****************************************************************************)

let make_block 
    (procinfo:Cinv_equation.procinfo)
    (fd:Cil_types.fundec)
    (graph:Cinv_equation.graph)
    :
    unit
    =
  (if Celia_options.debug_atleast 1 then
      Celia_options.feedback "Translate procedure %s " fd.svar.vname);
  let env = procinfo.Cinv_equation.penv in
  let bpoint = procinfo.Cinv_equation.pstart in
  let stmts = fd.sbody.bstmts in
  (* let stmts = fd.sallstmts in *)
  ignore begin
    List.fold_left 
      (begin fun bp st ->
	statement env procinfo bp st graph
       end)
      bpoint
      stmts
  end


(*****************************************************************************)
(* Main function *)
(*****************************************************************************)
let make f =
  (** Suppose that !FCFile.computeCFG has been called *)
  let procedures = (List.filter filter_fun f.globals) in

  (** Create global info on procedures *)
  let info = make_info procedures in

  (** Create control flow graph for the program using global info *)
  let graph = Cinv_equation.create 3 info in

  List.iter
    (begin fun gdec ->
      match gdec with
	  GFun (fd,_) ->
	    (try
	      let finfo = Hashhe.find info.Cinv_equation.procinfo fd.svar.vname in
	      make_block finfo fd graph
	    with Not_found ->
	      Celia_options.feedback "Procedure %s not found" fd.svar.vname)
	| _ -> assert false
    end)
    procedures;
  
  graph


(*
Local Variables:
compile-command: "LC_ALL=C make -C "
End:
*)
