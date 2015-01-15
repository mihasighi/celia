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

(*****************************************************************************)
(* Types list encoding                                                       *)
(*****************************************************************************)
let fieldNames = ref [("data",0); ("next",1); ("prev",2)]
let fieldMaxNumber = 10

let get_field_number f = 
  try
    List.assoc f !fieldNames 
  with Not_found -> -1

let add_field f =
  let pos = get_field_number f in
  if pos = -1 && 
    (List.length !fieldNames) < fieldMaxNumber then
    begin
      fieldNames := List.append !fieldNames [(f, (List.length !fieldNames))];
      (List.length !fieldNames)-1
    end
  else
    pos


let export_ltype filename =
  let out_chan = open_out filename in
  (* Warning: Format is not working!! *)
  begin
    output_string out_chan ((string_of_int (List.length !fieldNames))^"\n");
    List.iter (fun (f,_) -> begin
      (Celia_options.feedback "Field '%s' present" f);
      output_string out_chan (f^"\n")
      end) !fieldNames;
    flush out_chan;
    close_out out_chan
  end

let framac_field 
    (s:string)
    : 
    bool
    =
  if (String.length s) < 8 then false
  else  let n = (String.compare (String.sub s 0 7) "struct_") in
	n = 0

(*****************************************************************************)
(* Cil to Apron translation of operators                                     *)
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

(*****************************************************************************)
(* Cil to Apron translation of expressions                                   *)
(*****************************************************************************)

let mkexpr_var env 
    (vname:string)
    :
    Apron.Texpr1.t
    =
  (Celia_options.debug "mkexpr_var %s" vname);
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

(* Translates lvalue terms using the convention:
   new:
   x->data  :   cast_i,0(x *_r,0 cst (get_field data))
   x->f     :   cast_r,0(x *_r,0 cst (get_field f)) with f pointer field
   x[0]     :   cast_i,u(x *_r,u cst 0)
   x[i]     :   cast_i,u(x *_r,u dim i)

   old:
   x->data  :   cast_i,0(x)
   x->next  :   cast_f,u(x)
   x->prev  :   cast_f,d(x)
*)
let mkexpr_field env
    (fname:string) 
    (x:Apron.Texpr1.t)
    :
    Apron.Texpr1.t
    =
  let pos = get_field_number fname in
  if pos = -1 then
    begin
      if (String.length fname) == 0 || framac_field fname then
	x
      else
	notimplemented "mkexpr_field field \"%s\" not known" fname
    end
  else 
    let ty = if pos = 0 then Apron.Texpr1.Int else Apron.Texpr1.Real in
    let ft = Apron.Texpr1.cst env (Apron.Coeff.s_of_int pos) in
    Apron.Texpr1.unop Apron.Texpr1.Cast 
      (Apron.Texpr1.binop Apron.Texpr1.Mul x ft
	 Apron.Texpr1.Real Apron.Texpr1.Zero
      )
      ty Apron.Texpr1.Zero

let mkexpr_select env
    (v:Cil_types.varinfo) 
    (e:Apron.Texpr1.t)
    :
    Apron.Texpr1.t
    =
  let x = mkexpr_var env v.vname in
    Apron.Texpr1.unop Apron.Texpr1.Cast 
      (Apron.Texpr1.binop Apron.Texpr1.Mul x e
	 Apron.Texpr1.Real Apron.Texpr1.Up
      )
      Apron.Texpr1.Int Apron.Texpr1.Up


(* Translates dereferencing of lvalue terms using the convention:
   new:
   x->data=expr    :    x=cast_r,0(expr /_i,0 cst (data pos))
   x->f=expr       :    x=cast_r,0(expr /_r,0 cst (f pos))
   x[0]=expr       :    x=cast_r,u(expr /_i,u cst 0)
   x[i]=expr       :    x=cast_r,u(expr /_i,u dim i)

   old:
   x->data=expr    :    x=cast_d,0(expr)
   x->next=expr    :    x=cast_d,u(expr)
   x->prev=expr    :    x=cast_d,d(expr)
*)
let mkexpr_deref env
    (e:Apron.Texpr1.t)
    (fname:string) 
    :
    Apron.Texpr1.t
    =
  let pos = get_field_number fname in
  if pos = -1 then
    begin
      if (String.length fname) == 0 || framac_field fname then
	e
      else
	notimplemented "mkexpr_deref field \"%s\" not known" fname
    end
  else (* TODO: deal with array index *)
    let ty = Apron.Texpr1.Real in
    let ft = Apron.Texpr1.cst env (Apron.Coeff.s_of_int pos) in
    Apron.Texpr1.unop Apron.Texpr1.Cast 
      (Apron.Texpr1.binop Apron.Texpr1.Div e ft
	 Apron.Texpr1.Real Apron.Texpr1.Zero
      )
      ty Apron.Texpr1.Zero


(** Translates the app terms used in the logic to denote data on lists, i.e.,
    data(n,0)    :   cast_i,u(x *_r,u cst 0)
    data(n,y)    :   cast_i,u(x *_r,u dim y)

    \length(x)   :   cast_q,u(x)
    len(n)

    \sum(x)      :   cast_q,0(x)
    sum(n)

    \mset(x)     :   cast_q,n(x)
    mset(n)

    \ucons(x)    :   cast_q,d(x)
  
*)
let mkexpr_fun
    (lname:string) 
    (lp:Apron.Texpr1.t list)
    :
    Apron.Texpr1.t
    = 
  match lname, lp with
  | "data", x::[y] ->
    Apron.Texpr1.unop Apron.Texpr1.Cast 
      (Apron.Texpr1.binop Apron.Texpr1.Mul x y
	 Apron.Texpr1.Real Apron.Texpr1.Up 
      )
      Apron.Texpr1.Int Apron.Texpr1.Up

  | "\\length", [x]  
  | "len", [x] ->
    Apron.Texpr1.unop Apron.Texpr1.Cast x 
	Apron.Texpr1.Quad Apron.Texpr1.Up

  | "\\sum", [x]
  | "sum", [x] -> 
    Apron.Texpr1.unop Apron.Texpr1.Cast x
	Apron.Texpr1.Quad Apron.Texpr1.Zero

  | "\\mset", [x] 
  | "mset", [x] -> 
    Apron.Texpr1.unop Apron.Texpr1.Cast x
	Apron.Texpr1.Quad Apron.Texpr1.Near

  | "\\ucons", [x] -> 
    Apron.Texpr1.unop Apron.Texpr1.Cast x
	Apron.Texpr1.Quad Apron.Texpr1.Down

  | "sloc", [x] ->
    x

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

(*****************************************************************************)
(* Cil to Apron translation of constraints                                   *)
(*****************************************************************************)

let true_cons env = Apron.Tcons1.make (zero_expr env) Apron.Tcons1.EQ 
let false_cons env = Apron.Tcons1.make (one_expr env) Apron.Tcons1.EQ 
  
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
(* Cil to Apron translation of constants.                                    *)
(*****************************************************************************)

let rec logic_const ~in_code env = function
  | Integer(i,_ik) ->
    Apron.Texpr1.cst env (Apron.Coeff.s_of_int (Integer.to_int i))

  | LStr _ | LWStr _ -> 
    assert false (* Should have been rewritten *)

  | LChr c -> 
    Apron.Texpr1.cst env (Apron.Coeff.s_of_int (Char.code c))

  | LReal _ ->
    notimplemented "Cil2apron.cons Creal"

  | LEnum item ->
    (const_of_expr ~in_code env item.eival)

and const ~in_code env = function
  | CInt64(i,_ik,_) ->
    Apron.Texpr1.cst env (Apron.Coeff.s_of_int (Integer.to_int i))

  | CStr _ | CWStr _ -> 
    assert false (* Should have been rewritten *)

  | CChr c -> 
    Apron.Texpr1.cst env (Apron.Coeff.s_of_int (Char.code c))

  | CReal _ ->
    notimplemented "Cil2apron.cons Creal"

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
      if i = Integer.zero then 
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
(* Cil to Apron translation of logic constructs                              *)
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
  | TConst c -> [logic_const ~in_code:false env c]

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

  | TUnOp(Neg,t1) when isLogicIntegralType t1.term_type ->
      List.map 
	(fun x -> negate_texpr x)
	(coerce_type env t1)

  | TUnOp _ ->
      unsupported "Cil2apron.terms (LNot | BNot)"
 
  | TBinOp((Shiftrt | Shiftlt),_,_) ->
      unsupported "Cil2apron.terms (Shiftrt | Shiftlt)"

  | TBinOp((PlusA | MinusA | Mult | Div | Mod) as op,t1,t2) when isLogicIntegralType t.term_type ->
    let e1l = (coerce_type env t1) in
    let e2l = (coerce_type env t2) in
    let e1 = (if (List.length e1l) >= 1 then (List.hd e1l) else (zero_expr env)) in
    let e2 = (if (List.length e2l) >= 1 then (List.hd e2l) else (zero_expr env)) in
      [(mkexpr_binop e1 op e2)]

  | TBinOp _ ->
      unsupported "Cil2apron.terms: BinOp (at line %d:%d)@." (fst t.term_loc).Lexing.pos_lnum (snd t.term_loc).Lexing.pos_cnum

  | TCastE(TInt _,  t1) -> (* Support only casts for integer constants *)
    (match t1.term_node with
	TConst c -> [logic_const ~in_code:false env c]
      | _ -> 
	unsupported "Cil2apron.terms (TCastE to TInt) not allowed for non-constant terms"
    )

  | TCastE _ ->
      unsupported "Cil2apron.terms (TCastE) not allowed in logic"

  | TAddrOf _ -> assert false (* Should have been rewritten *)

  | TStartOf _ ->
      unsupported "Cil2apron.terms (TStartOf) not allowed in logic"

  | Tapp(linfo,_,tlist) ->
      begin
	try
          let args = List.map (fun t -> (terms_ptr env [t])) tlist in
          let all_args = List.fold_right (product (fun x y -> x::y)) args [[]] in
          List.map
            (begin fun lp -> 
	      match lp with
		  [_] | _::[_] -> mkexpr_fun linfo.l_var_info.lv_name lp
		| _ -> raise (CtePredicate false)
	     end) all_args
	with (CtePredicate _)->
	  [zero_expr env]
      end

  | Tat(t1,_) -> (* TODO: check correctly the label *)
      (coerce_type env t1)

  | Tif _ ->
      unsupported "Cil2apron.terms (Tif) not allowed in logic"

  | Tbase_addr _ ->
      unsupported "Cil2apron.terms (Tbase_adr) not allowed in logic"

  | Tblock_length _ ->
      unsupported "Cil2apron.terms (Tat | Tblock_length)"

  | Tnull -> [null_expr env]

  | Tlet _ ->
      (* TODO: no definition of new predicates yet, but allow 
         predefined predicates *)
      unsupported "Cil2apron.terms Tlet"

  | TCoerce _ | TCoerceE _ ->
      (* TODO: see if useful *)
      unsupported "Cil2apron.terms (TCoerce | TCoerceE)"

  | Tlambda _ ->
      unsupported "Cil2apron.terms does not support lambda abstraction"

  | Ttypeof _ | Ttype _ -> assert false (* Should have been treated *)

  | Trange _ ->
      unsupported "Cil2apron.terms Trange"

  | Tunion _ ->
      unsupported "Cil2apron.terms Tunion"

  | Tcomprehension _ -> 
      unsupported "Cil2apron.terms Tcomprehension"

  | Tinter _ -> 
      unsupported "Cil2apron.terms Tinter"

  | Toffset _ -> 
      unsupported "Cil2apron.terms TMem"

  | TLogic_coerce (_lty,t1) -> 
      (coerce_type env t1)
      (*MS do it correctly unsupported "Cil2apron.terms TLogic_coerce" *)

  | Tempty_set -> []


and terms_ptr env 
    (lt:Cil_types.term list)
    : 
    Apron.Texpr1.t list
    =
  List.map (term_ptr env) lt

and term_ptr env t =
  if (Logic_utils.isLogicPointerType t.term_type) ||
    (Cil.isLogicIntegralType t.term_type)
  then
    begin
      match t.term_node with
      | TLval (TVar lv, TNoOffset) ->
          mkexpr_var env lv.lv_name
      | TConst c -> (logic_const ~in_code:false env c)
      | TCastE(TInt _, t1) -> (term_ptr env t1)
      | _ -> 
        Printer.pp_term Format.std_formatter t;
        (* Format.print_flush; *)
	unsupported "Cil2apron.term_ptr: Expecting a constant, lval or cast not a@ %a@."
        Printer.pp_term t
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
        Celia_options.debug "terms_lval Tvar, TResult";
        assert false (* Should have been rewritten *)

    | TMem t, TField(fi,toff) ->
        assert (toff = TNoOffset); (* Others should have been rewritten *)
        let tl = terms env t in
        if not fi.fcomp.cstruct then (* field of union *)
	  notimplemented "Cil2apron.terms_lval TMem field of union"
        else
	  List.map (fun x -> mkexpr_field env fi.fname x) tl

    | TMem _e, _ ->
        Celia_options.debug "terms_lval TMem";
	unsupported "Cil2apron.terms_lval"

and term env 
    (t:Cil_types.term)
    :
    Apron.Texpr1.t
    =
  match terms env t with [ tt ] -> tt
    | _ ->
	unsupported "Expecting a single term, not a set:@ %a@."
          Printer.pp_term t

and term_lval env lv  =
  match terms_lval env lv with [ lv ] -> lv
    | _ ->
        Printer.pp_term_lval Format.std_formatter lv;
        (* Format.print_flush; *)
	unsupported "Expecting a single lval, not a set:@ %a@."
        Printer.pp_term_lval lv

and pred env (p:Cil_types.predicate)
    : Apron.Tcons1.t Boolexpr.t
    =
  match p with
    | Pfalse -> Boolexpr.make_cst false

    | Ptrue -> Boolexpr.make_cst true

    | Papp(pinfo,_,tl) ->
      (* Predicates may be only the predefined ones *)
      (Celia_options.debug "Requires = Papp");
      begin
	try
          translated_pred env pinfo tl
	with 
	    (CtePredicate b) -> Boolexpr.make_cst b
      end

    | Prel((Rlt | Rgt | Rle | Rge),t1,_)
        when app_term_type isPointerType false t1.term_type ->
	unsupported "Cil2apron.terms comparison between pointers"

    | Prel((Req | Rneq),t1,_) when isTypeTagType t1.term_type ->
        unsupported "Cil2apron.terms comparison between tags"

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
	unsupported "Cil2apron.pred Pif."

    | Plet _ ->
	unsupported "Cil2apron.pred Plet."

    | Pforall([],p) -> 
	(pred env p.content)

    | Pforall _ ->
	unsupported "Cil2apron.pred Pforall."

    | Pexists([],p) -> 
	(pred env p.content)

    | Pexists _ ->
	unsupported "Cil2apron.pred Pexists."

    | Pat _
    | Pvalid _  
    | Pfresh _
    | Psubtype _
    | Pseparated _
    | Pvalid_read _
    | Pinitialized _
    | Pallocable _
    | Pfreeable _ ->
      notimplemented "Cil2apron.pred (Pat | Pvalid | Pfresh | Psubtype | Pseparated)"

(* Compute the constraint for a predefined predicate, i.e.,
	   - x > 0.0         for acyclic(x)
	   - cast_f,n(x) > x for cyclic(x)
	   - x - y = 0 mod 1 for disjoint(x,y)
	   - x - y = 0 mod 0 for iso(x,y)
	   - x > y           for reach(x,y)
	   - x == 0.0        for x is NULL
	   - x <> 0.0        for x is not NULL 
	   - cast_q,d(x) + 20 = 0 for sorted(x)
 *)
and translated_pred env 
    (li:Cil_types.logic_info)
    (tl:Cil_types.term list)
    :
    Apron.Tcons1.t Boolexpr.t
    =
  (Celia_options.debug "Translate predicate in requires %s" 
     li.l_var_info.lv_name);
  match li.l_var_info.lv_name with
  | "acyclic" ->
      begin
	match terms_ptr env tl with
	| [x] -> 
	    (* Celia_options.debug "term_ptr"; *)
	    Boolexpr.make_condition (Apron.Tcons1.make x Apron.Tcons1.SUP)
	| _ -> raise (CtePredicate false)
      end

  | "cyclic" ->
      begin
	match terms_ptr env tl with 
	| [x] -> 
	    let xnext = mkexpr_field env "next" x in
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

  | "sorted" ->
      begin
	match terms_ptr env tl with 
	| [x] -> 
	    let ucons_x = mkexpr_fun "\\ucons" [x] in
	    let e = Apron.Texpr1.binop Apron.Texpr1.Add ucons_x (Apron.Texpr1.cst env (Apron.Coeff.s_of_int 20))
		Apron.Texpr1.Int Apron.Texpr1.Near
	    in
	    mkconjunct [mkcons e Req (zero_expr env)]
	| _ -> raise (CtePredicate false)
      end

  | _ ->
      unsupported "Predicate not implemented %s." li.l_var_info.lv_name


(*****************************************************************************)
(* Cil to Apron translation of expressions                                   *)
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
              && value_of_integral_const c = Integer.zero ->
		null_expr env
        | _ ->
	      unsupported "Casting from type %a to type %a not allowed"
                Printer.pp_typ (typeOf e) Printer.pp_typ ptrty
      end

  | CastE(ty,e') ->
    let sz1 = bits_sizeof ty in
    let sz2 = bits_sizeof (typeOf e') in
    if sz1 <> sz2 then
      notimplemented  "Casting from type %a to type %a not allowed in %a with size %Ld and %Ld"
        Printer.pp_typ (typeOf e') Printer.pp_typ ty Printer.pp_exp e
        (bits_sizeof ty) (bits_sizeof (typeOf e'))
    else
      expr env e'  


   |AddrOf _lv -> assert false (* Should have been rewritten *)

  | StartOf lv -> expr_lval env lv


and expr_lval env
    (lv:Cil_types.lval)
    :
    Apron.Texpr1.t
    =
  match lv with
    | lhost, NoOffset -> expr_lhost env lhost

    | Var _,_ ->
        assert false (* Rewritten *)

    | Mem e, Field(fi,toff) ->
        assert (toff = NoOffset); (* Others should have been rewritten *)
        let te = expr env e in
        if not fi.fcomp.cstruct then (* field of union *)
	  notimplemented "Cil2apron.expr_lval Mem field of union"
        else
	  mkexpr_field env fi.fname te

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
      unsupported  "Cil2apron.integral_expr integer logic negation"

    | BinOp((LAnd | LOr),_e1,_e2,_ty) ->
      unsupported  "Cil2apron.integral_expr integer logic op"

    | BinOp((Eq | Ne | Lt | Gt | Le | Ge),_,_,_) ->
        unsupported  "Cil2apron.integral_expr integer comparison"

    | BinOp((Shiftrt | Shiftlt),_,_,_) ->
      unsupported "Cil2apron.integral_expr (Shiftrt | Shiftlt)"

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

    | Mem _e, NoOffset -> (* assert false * Should have been rewritten *)
	unsupported "Cil2apron.lval Mem %a " Printer.pp_lval lv

    | Mem {enode = Lval(Var v,NoOffset)}, Field(fi,off) ->
      assert (off = NoOffset); (* Others should have been rewritten *)
      if fi.fcomp.cstruct then
        Apron.Var.of_string v.vname, fi.fname
      else (* field of union *)
	unsupported "Cil2apron.lval pos for unions"

    | _ ->
      unsupported "Unexpected lval %a" Printer.pp_lval lv

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
	Printer.pp_exp e
    else
      v, v (* mkexpr_var env (Apron.Var.to_string v) *)
  | Const _ (* c ->  (Apron.Var.of_string "dummy"), const false env c  *)
  | _ -> 
    unsupported "Unexpected expr as lval %a" Printer.pp_exp e


(** Convert ACSL pre/post condition into an array of tree constraints. *)
let rec convert_spec  
    (ploc:Cinv_equation.point)
    (penv:Apron.Environment.t)
    (iplist:Cil_types.identified_predicate list) 
    : Apron.Tcons1.earray
    =
  let spec_file = ploc.Cinv_equation.pos.Lexing.pos_fname in
  let spec_line = ploc.Cinv_equation.pos.Lexing.pos_lnum in
  let spec_sid = ploc.Cinv_equation.sid in
  (Celia_options.feedback "Collect constraint at %s:%d, sid:%d (of %d predicates)" 
     spec_file spec_line spec_sid (List.length iplist));
  (** The environment is given by the formals *)

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

