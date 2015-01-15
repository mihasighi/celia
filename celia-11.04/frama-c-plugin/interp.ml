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
open Clim_constructors
open Clim_ast
open Clim_env
(* open Clim_fenv *)
open Clim_pervasives


(* Utility functions *)
open Common
open Integer
open Format


(*****************************************************************************)
(* Smart constructors.                                                       *)
(*****************************************************************************)

let mktype tnode = new ptype tnode

let mkexpr enode pos = new pexpr ~pos enode

let void_expr = mkexpr (LCPEconst LCCvoid) Loc.dummy_position
let null_expr = mkexpr (LCPEconst LCCnull) Loc.dummy_position
let true_expr = mkexpr (LCPEconst(LCCboolean true)) Loc.dummy_position
let false_expr = mkexpr (LCPEconst(LCCboolean false)) Loc.dummy_position
let zero_expr = mkexpr (LCPEconst(LCCinteger "0")) Loc.dummy_position
let one_expr = mkexpr (LCPEconst(LCCinteger "1")) Loc.dummy_position

let mktag tag_node pos = new ptag ~pos tag_node

let mkidentifier name pos = new identifier ~pos name

let rec mkconjunct elist pos =
  match elist with
    | [] -> true_expr
    | [e] -> e
    | e::el -> mkexpr (LCPEbinary(e,`Bland,mkconjunct el pos)) pos

let rec mkdisjunct elist pos =
  match elist with
    | [] -> false_expr
    | [e] -> e
    | e::el -> mkexpr (LCPEbinary(e,`Blor,mkdisjunct el pos)) pos

let mkdecl dnode pos = new decl ~pos dnode


(*****************************************************************************)
(* Locate Clim expressions on source program.                                *)
(*****************************************************************************)

let reg_pos ?id ?kind ?name pos = Output.reg_pos "C" ?id ?kind ?name pos

(* [locate] should be called on every Jessie expression which we would like to
 * locate in the original source program.
 *)
let locate ?alarm ?pos e =
  (* Recursively label conjuncts so that splitting conjuncts in Why still
   * allows to locate the resulting VC.
   *)
  let rec dopos ~toplevel e =
    (* Generate (and store) a label associated to this source location *)
    let pos = match pos with
      | None -> e#pos
      | Some pos ->
          if is_unknown_location e#pos then pos else e#pos
    in
    let lab = match alarm with
      | None ->
          reg_pos pos
      | Some Alarms.Division_alarm ->
          reg_pos ~kind:Output.DivByZero pos
      | Some Alarms.Memory_alarm | Some Alarms.Index_alarm ->
          reg_pos ~kind:Output.PointerDeref pos
      | Some (Alarms.Shift_alarm|Alarms.Signed_overflow_alarm) ->
          reg_pos ~kind:Output.ArithOverflow pos
      | Some Alarms.Pointer_compare_alarm
      | Some Alarms.Using_nan_or_infinite_alarm
      | Some Alarms.Result_is_nan_or_infinite_alarm ->
          reg_pos pos
      | Some Alarms.Separation_alarm -> reg_pos pos
      | Some Alarms.Other_alarm -> reg_pos pos
    in
    let e = match e#node with
      | LCPEbinary(e1,`Bland,e2) ->
          begin match e1#node,e2#node with
            | LCPElabel _,LCPElabel _ -> e (* already labelled *)
            | LCPElabel _,_ -> (* [e1] already labelled *)
                let e2 = dopos ~toplevel:false e2 in
                mkexpr (LCPEbinary(e1,`Bland,e2)) pos
            | _,LCPElabel _ -> (* [e2] already labelled *)
                let e1 = dopos ~toplevel:false e1 in
                mkexpr (LCPEbinary(e1,`Bland,e2)) pos
            | _,_ -> (* none already labelled *)
                let e1 = dopos ~toplevel:false e1 in
                let e2 = dopos ~toplevel:false e2 in
                mkexpr (LCPEbinary(e1,`Bland,e2)) pos
          end
      | _ -> e
    in
    (* Do not generate a label for every intermediate conjunct *)
    match e#node with
      | LCPEbinary(_e1,`Bland,_e2) when not toplevel -> e
      | _ ->
          (* Label the expression accordingly *)
          mkexpr (LCPElabel(lab,e)) pos
  in
  dopos ~toplevel:true e


(*****************************************************************************)
(* Cil to Clim translation of operators                                      *)
(*****************************************************************************)

let unop = function
  | Neg -> `Uminus
  | BNot -> assert false
  | LNot -> `Unot

let binop = function
  | PlusA -> `Badd
  | PlusPI -> assert false
  | IndexPI -> `Badd (*TODO: MS extract index correctly *)
  | MinusA -> `Bsub
  | MinusPI
  | MinusPP  -> assert false
  | Mult -> `Bmul
  | Div -> `Bdiv
  | Mod -> `Bmod
  | Shiftlt 
  | Shiftrt -> assert false
  | Lt -> `Blt
  | Gt -> `Bgt
  | Le -> `Ble
  | Ge -> `Bge
  | Eq -> `Beq
  | Ne -> `Bneq
  | BAnd 
  | BXor 
  | BOr -> assert false
  | LAnd -> `Bland
  | LOr -> `Blor

let relation = function
  | Rlt -> `Blt
  | Rgt -> `Bgt
  | Rle -> `Ble
  | Rge -> `Bge
  | Req -> `Beq
  | Rneq -> `Bneq


let rec name_with_profile s prof =
  match prof with
    | [] ->
(*
	Format.eprintf "producing new translated name ``%s''@." s;
*)
	s
    | v::rem ->
	let n = Common.logic_type_name v.lv_type in
(*
	Format.eprintf "type name ``%s''@." n;
*)
	name_with_profile (s^"_"^n) rem

let translated_names_table = Hashtbl.create 257

let translated_name linfo largs =
(*
  Format.eprintf "Jessie.interp: linfo = %s(%a)(%d)@."
    linfo.l_name
    (fprintfList ~sep:",@ " d_logic_type)
    (List.map (fun v -> v.lv_type) linfo.l_profile)
    (Obj.magic linfo);
*)
  try
    let n = Hashtbl.find translated_names_table linfo.l_var_info.lv_id in
(*
    Format.eprintf "Jessie.interp: translated(%s) = %s" linfo.l_var_info.lv_name n;
*)
    n
  with Not_found ->
    let name =
      match linfo.l_var_info.lv_name with
	| "\\abs" ->
	    begin
	      match linfo.l_type with
		| Some Lreal -> "\\real_abs"
		| Some Linteger -> "\\integer_abs"
		| _ -> assert false
	    end
	| "\\min" ->
	    begin
	      match linfo.l_type with
		| Some Lreal -> "\\real_min"
		| Some Linteger -> "\\integer_min"
		| _ -> assert false
	    end
	| "\\max" ->
	    begin
	      match linfo.l_type with
		| Some Lreal -> "\\real_max"
		| Some Linteger -> "\\integer_max"
		| _ -> assert false
	    end
	| "\\exact" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\double_exact"
		| Ctype x when x == floatType -> "\\single_exact"
		| _ -> assert false
	    end
	| "\\model" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\double_model"
		| Ctype x when x == floatType -> "\\single_model"
		| _ -> assert false
	    end
	| "\\round_float" ->
	    begin
	      match (List.hd largs).term_node with
                | TDataCons(ctor,_args) ->
                    begin
                      match ctor.ctor_name with
		        | "\\Double" -> "\\round_double"
		        | "\\Single" -> "\\round_single"
		        | s ->
                            warning "first arg '%s' of \\round_float unsupported (should be \\Double or \\Single)" s;
                            assert false
                    end
                | _ ->    assert false
	    end
	| "\\round_error" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\double_round_error"
		| Ctype x when x == floatType -> "\\single_round_error"
		| _ -> assert false
	    end
	| "\\total_error" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\double_total_error"
		| Ctype x when x == floatType -> "\\single_total_error"
		| _ -> assert false
	    end
	| "\\relative_error" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\double_relative_error"
		| Ctype x when x == floatType -> "\\single_relative_error"
		| _ -> assert false
	    end
	| "\\pow" ->
	    begin
	      match linfo.l_type with
		| Some Lreal -> "\\real_pow"
		| _ -> assert false
	    end
	| "\\sqrt" ->
	    begin
	      match linfo.l_type with
		| Some Lreal -> "\\real_sqrt"
		| _ -> assert false
	    end
	| "\\sign" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\double_sign"
		| Ctype x when x == floatType -> "\\single_sign"
		| _ -> assert false
	    end
(*
	| "\\is_finite" ->
            full_model_function linfo "is_finite" true
	| "\\is_infinite" ->
            full_model_function linfo "is_infinite" false
	| "\\is_NaN" ->
            full_model_function linfo "is_NaN" false
	| "\\is_minus_infinity" ->
            full_model_function linfo "is_minus_infinity" false
	| "\\is_plus_infinity" ->
            full_model_function linfo "is_plus_infinity" false
*)
	| "\\le_float" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\le_double"
		| Ctype x when x == floatType -> "\\le_single"
		| _ -> assert false
	    end
	| "\\lt_float" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\lt_double"
		| Ctype x when x == floatType -> "\\lt_single"
		| _ -> assert false
	    end
	| "\\ge_float" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\ge_double"
		| Ctype x when x == floatType -> "\\ge_single"
		| _ -> assert false
	    end
	| "\\gt_float" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\gt_double"
		| Ctype x when x == floatType -> "\\gt_single"
		| _ -> assert false
	    end
	| "\\eq_float" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\eq_double"
		| Ctype x when x == floatType -> "\\eq_single"
		| _ -> assert false
	    end
	| "\\ne_float" ->
	    begin
	      match (List.hd linfo.l_profile).lv_type with
		| Ctype x when x == doubleType -> "\\ne_double"
		| Ctype x when x == floatType -> "\\ne_single"
		| _ -> assert false
	    end
	| s ->
	    try
(*
	      Format.eprintf "Jessie.interp: Checking if %s overloaded" s;
*)
	      let x = Hashtbl.find Rewrite.logic_names_overloading s in
	      if !x then
		let ns = name_with_profile s linfo.l_profile in
(*
		Format.eprintf "yes! -> %s@." ns;
*)
		ns
	      else
		begin
(*
		  Format.eprintf "no@.";
*)
		  s
		end
	    with Not_found ->
	      (* this happens with Jessie-generated predicates like valid_* etc *)
(*
	      Format.eprintf "Jessie.Interp: warning, logic id `%s' not present in overloading table@." s;
*)
	      s
    in
(*
    Format.eprintf "Jessie.interp: translated(%s) = %s" linfo.l_var_info.lv_name name;
*)
    Hashtbl.add translated_names_table linfo.l_var_info.lv_id name;
    name

(*****************************************************************************)
(* Cil to Clim translation of types                                          *)
(*****************************************************************************)

let type_conversion_table = Hashtbl.create 0

exception CtePredicate of bool

let type_conversion ty1 ty2 =
  let ty1 = typeRemoveAttributes ["const";"volatile"] (unrollType ty1) in
  let ty2 = typeRemoveAttributes ["const";"volatile"] (unrollType ty2) in
  let sig1 = typeSig ty1 and sig2 = typeSig ty2 in
  try
    let _,_,ty1_to_ty2,ty2_to_ty1 =
      Hashtbl.find type_conversion_table (sig1,sig2)
    in
    ty1_to_ty2,ty2_to_ty1
  with Not_found ->
    try
      let _,_,ty2_to_ty1,ty1_to_ty2 =
        Hashtbl.find type_conversion_table (sig2,sig1)
      in
      ty1_to_ty2,ty2_to_ty1
    with Not_found ->
      let n1 = type_name ty1 and n2 = type_name ty2 in
      let ty1_to_ty2 = unique_logic_name (n1 ^ "_to_" ^ n2) in
      let ty2_to_ty1 = unique_logic_name (n2 ^ "_to_" ^ n1) in
      Hashtbl.add
        type_conversion_table (sig1,sig2) (ty1,ty2,ty1_to_ty2,ty2_to_ty1);
      ty1_to_ty2,ty2_to_ty1

(*
type float_rounding_mode = [ `Downward | `Nearest | `Upward | `Towardzero | `Towardawayzero ]

let float_rounding_mode : float_rounding_mode ref = ref `Nearest
*)

let ctype (* ?bitsize *) ty =
  let tnode = match unrollType ty with
    | TVoid _attr -> LCPTnative Tunit

    | TInt(_ik,_attr) ->
        LCPTnative Tinteger

    | TPtr(_elemty,_attr) ->
        if is_reference_type ty then
          (* Do not use [_elemty] directly but rather [pointed_type ty] in order
           * to get to the array element in references, i.e. pointers to arrays.
           *)
          begin match Cil.unrollType (pointed_type ty) with
            | TComp(compinfo,_,_) ->
                let min_bound = Num.num_of_string "0" in
                let max_bound =
                  Num.num_of_string (Int64.to_string (reference_size ty - 1L))
                in
                LCPTpointer(compinfo.cname,[],Some min_bound,Some max_bound)
            | _ -> assert false
          end
        else
          begin match unrollType (pointed_type ty) with
            | TComp(compinfo,_,_) ->
                LCPTpointer(compinfo.cname,[],None,None)
            | _ -> assert false
          end

    | TArray _ -> assert false (* Removed by translation *)

    | TFun _ ->
	unsupported "Function pointer type %a not allowed"
	  !Ast_printer.d_type ty

    | TNamed _ -> assert false (* Removed by call to [unrollType] *)

    | TComp(compinfo,_,_) -> LCPTidentifier (compinfo.cname,[])

    | TEnum(enuminfo,_) -> LCPTidentifier (enuminfo.ename,[])

    | TFloat _ -> unsupported "Type float not allowed"
    | TBuiltin_va_list _ -> unsupported "Type builtin_va_list not allowed"
  in
  mktype tnode

let ltype t =
  (* Expand synonym types before translation. *)
  match Logic_utils.unroll_type t with
  | Ctype ty -> ctype ty
  | Ltype (s,[]) when s.lt_name = Utf8_logic.boolean ->
      mktype (LCPTidentifier ("boolean",[]))
  | Ltype (s,[]) -> mktype (LCPTidentifier (s.lt_name,[]))
  | Linteger -> mktype (LCPTnative Tinteger)
  | Ltype(_,_) | Lvar _ | Larrow _ | Lreal ->
      (* TODO *)
      Extlib.not_yet_implemented  "Interp.ltype (Lreal | Ltype |Lvar | Larrow)"


(*****************************************************************************)
(* Cil to Clim translation of constants                                      *)
(*****************************************************************************)

let rec const ~in_code pos = function
  | CInt64(_,_,Some s) ->
      (* Use the textual representation if available *)
      LCPEconst(LCCinteger s)

  | CInt64(i,_ik,None) ->
      LCPEconst(LCCinteger(Int64.to_string i))

  | CStr _ | CWStr _ -> assert false (* Should have been rewritten *)

  | CChr c -> LCPEconst(LCCinteger(string_of_int (Char.code c)))

  | CReal _ ->
      (* TODO *)
      Extlib.not_yet_implemented  "Interp.cons Creal"

  | CEnum item ->
      let e = mkexpr (const_of_expr ~in_code pos item.eival) pos in
	LCPEcast(e,ctype (TEnum(item.eihost,[])))

and const_of_expr ~in_code pos e =
  match (stripInfo e).enode with
      Const c -> const ~in_code pos c
    | _ -> assert false

and boolean_const = function
  | CInt64(i,_ik,_text) ->
      if i = Int64.zero then LCCboolean false else LCCboolean true

  | CStr _ | CWStr _ -> LCCboolean true

  | CChr c ->
      if Char.code c = 0 then LCCboolean false else LCCboolean true

  | CReal(f,_fk,_text) ->
      if f = 0.0 then LCCboolean false else LCCboolean true

  | CEnum {eival = e} -> boolean_const_of_expr e

and boolean_const_of_expr e =
  match (stripInfo e).enode with Const c -> boolean_const c | _ -> assert false


(*****************************************************************************)
(* Cil to Clim translation of logic constructs                               *)
(*****************************************************************************)

let label = function
  | Label(lab,_,_) -> lab
  | Case _ | Default _ -> assert false

let logic_label lab =
  let label_name s =
    LabelName {
      label_info_name = s;
      label_info_final_name = s;
      times_used = 0;
    }
  in
  match lab with
    | LogicLabel s -> label_name s
    | StmtLabel sref ->
        let labels = filter_out is_case_label !sref.labels in
        assert (not (labels = []));
        label_name (label (List.hd labels))

let logic_labels = List.map logic_label

let logic_labels_assoc =
  List.map (fun (_,l) -> logic_label l)

let term_lhost pos = function
  | TVar v ->
      (try
         let li = Logic_env.find_logic_cons v in
         match
           li.l_labels with
             | [] -> mkexpr(LCPEvar v.lv_name) pos
             | [_] ->  mkexpr (LCPEapp (v.lv_name,[],[])) pos
             | _ ->
                 Celia_options.fatal
                   "cannot handle logic constant %s with several labels"
                   v.lv_name
       with Not_found ->
         mkexpr (LCPEvar v.lv_name) pos)
  | TResult _ -> mkexpr (LCPEvar "\\result") pos
  | TMem _ ->
      unsupported "this kind of memory access is not currently supported"
(*
         Loc.report_position pos
*)

let isLogicConstant t = match t.term_node with
    TConst _ -> true
  | _ -> false

let product f t1 t2 =
  List.fold_right
    (fun x acc -> List.fold_right (fun y acc -> f x y :: acc) t2 acc) t1 []

let rec coerce_type t =
  (*MS: for the moment signal floats and do nothing *)
  terms t

and terms t =
  Cil.CurrentLoc.set t.term_loc;
  let enode = match constFoldTermNodeAtTop t.term_node with
    | TConst c -> [const ~in_code:false t.term_loc c]

    | TDataCons({ctor_type = {lt_name = name}} as d,_args)
        when name = Utf8_logic.boolean ->
        [LCPEconst (LCCboolean (d.ctor_name = "\\true"))]

    | TDataCons _
    | TUpdate _ ->
	(* TODO *)
        notimplemented "Interp.terms TUpdate"

    | TLval lv ->
        List.map (fun x -> x#node) (terms_lval t.term_loc lv)

    | TSizeOf _ | TSizeOfE _ | TSizeOfStr _ | TAlignOf _ | TAlignOfE _ ->
        assert false (* Should be removed by constant folding *)

    | TUnOp(op,t) ->
        List.map (fun x -> LCPEunary(unop op,x)) (coerce_type t)

    | TBinOp((Lt | Gt | Le | Ge as op),t1,t2)
        when app_term_type isPointerType false t1.term_type ->
        (* Pointer comparison is translated as subtraction *)
        let t1 = terms t1 in
        let t2 = terms t2 in
        let expr x y =
          let sube = mkexpr (LCPEbinary(x,`Bsub,y)) t.term_loc in
          LCPEbinary(sube,binop op,zero_expr)
        in product expr t1 t2

    | TBinOp((Shiftrt | Shiftlt),_,_) ->
        unsupported "Interp.terms (Shiftrt | Shiftlt)"

    | TBinOp((Lt | Gt | Le | Ge) as op,t1,t2) ->
        product (fun x y -> LCPEbinary(x,binop op,y)) 
	  (coerce_type t1) 
	  (coerce_type t2)

    | TBinOp(op,t1,t2) ->
        product
          (fun x y -> LCPEbinary(x,binop op,y))
             (coerce_type t1)
             (coerce_type t2)

    | TCastE _ ->
        unsupported "Interp.terms (TCastE) not allowed in logic"

    | TAddrOf _ -> assert false (* Should have been rewritten *)

    | TStartOf tlv ->
        List.map (fun x -> x#node) (terms_lval t.term_loc tlv)

    | Tapp(linfo,labels,tlist) ->
	begin
	  try
            let name = translated_name linfo tlist in
            let tlist =
              if linfo.l_var_info.lv_name = "\\round_float" then
                List.tl tlist
              else
                tlist
            in
            let args =
	      List.map
		(fun t -> (terms t))
		tlist
	    in
            let all_args = List.fold_right (product (fun x y -> x::y)) args [[]] in
            List.map
              (fun x -> LCPEapp(name,logic_labels_assoc labels,x)) all_args
	  with (CtePredicate b)->
	   [LCPEconst (LCCboolean b)]
	end

    | Tif(t1,t2,t3) ->
        let t1 = terms t1 in let t2 = terms t2 in let t3 = terms t3 in
        product (fun f x -> f x)
          (product (fun x y z -> LCPEif(x,y,z)) t1 t2) t3

    | Told t -> List.map (fun x -> LCPEold x) (terms t)

    | Tbase_addr t -> List.map (fun x -> LCPEbase_block x) (terms t)

    | Tat _
    | Tblock_length _ ->
	(* TODO: memory model for Jessie *)
	unsupported "Interp.terms (Tat | Tblock_length)"

    | Tnull -> [LCPEconst LCCnull]

    | Tlet _ ->
	(* TODO: no definition of new predicates yet, but allow 
                 predefined predicates *)
	unsupported "Interp.terms Tlet"

    | TCoerce _ | TCoerceE _ ->
	(* TODO: see if useful *)
	unsupported "Interp.terms (TCoerce | TCoerceE)"

    | Tlambda _ ->
	unsupported "Jessie plugin does not support lambda abstraction"

    | Ttypeof _ | Ttype _ -> assert false (* Should have been treated *)

    | Trange _ ->
	unsupported "Interp.terms Trange"

    | Tunion _ ->
	unsupported "Interp.terms Tunion"

    | Tcomprehension _ -> 
	unsupported "Interp.terms Tcomprehension" (* TODO: does not exist in Jessie *)

    | Tinter _ -> 
	unsupported "Interp.terms Tinter" (* TODO: does not exist in Jessie *)

    | Tempty_set -> []
  in
  List.map (swap mkexpr t.term_loc) enode

and tag t =
  let tag_node = match t.term_node with
    | Ttypeof t -> LCPTtypeof (term t)
    | Ttype ty ->
        let id = mkidentifier (get_struct_name (pointed_type ty)) t.term_loc in
        LCPTtag id
    | _ -> assert false (* Not a tag *)
  in
  mktag tag_node t.term_loc

and terms_lval pos lv =
  match lv with
    | lhost, TNoOffset -> [term_lhost pos lhost]

    | (TVar _ | TResult _), _off ->
        assert false (* Should have been rewritten *)

    | TMem t, TField(fi,toff) ->
        assert (toff = TNoOffset); (* Others should have been rewritten *)
        let e = terms t in
        if not fi.fcomp.cstruct then (* field of union *)
	  Extlib.not_yet_implemented "Interp.terms_lval TMem fielf of union"
        else
          List.map (fun e -> mkexpr (LCPEderef(e,fi.fname)) pos) e

    | TMem t, TIndex(it,TField(fi,toff)) ->
        assert (toff = TNoOffset); (* Others should have been rewritten *)
        (* Normalization made it equivalent to simple add *)
        let e = product
          (fun t it -> mkexpr (LCPEbinary(t,`Badd,it)) pos)
          (terms t) (terms it)
        in
        if not fi.fcomp.cstruct then (* field of union *)
	  unsupported "Interp.terms_lval TMem fielf of union"
        else
          List.map (fun e -> mkexpr (LCPEderef(e,fi.fname)) pos) e

    | TMem _e, TIndex _ ->
        assert false (* Should have been rewritten *)

and term t =
  match terms t with [ t ] -> t
    | _ ->
	unsupported "Expecting a single term, not a set:@ %a@."
          !Ast_printer.d_term t

and term_lval pos lv  =
  match terms_lval pos lv with [ lv ] -> lv
    | _ ->
	unsupported "Expecting a single lval, not a set:@ %a@."
          !Ast_printer.d_term_lval lv

and pred p =
  CurrentLoc.set p.loc;
  let enode = match p.content with
    | Pfalse -> LCPEconst(LCCboolean false)

    | Ptrue -> LCPEconst(LCCboolean true)

    | Papp(pinfo,labels,tl) ->
	begin
	  try
            let name = translated_name pinfo tl in
	(*
          LCPEapp(name,logic_labels_assoc labels,List.map term tl)
	*)
        let args =
	  List.map
	    (fun t -> term t)
	    tl
	in
	LCPEapp(name,logic_labels_assoc labels, args)
	  with (CtePredicate b) -> LCPEconst(LCCboolean b)

	end

    | Prel((Rlt | Rgt | Rle | Rge),t1,_)
        when app_term_type isPointerType false t1.term_type ->
        (* Pointer comparison is translated as subtraction *)
	unsupported "Interp.terms comparison between pointers"

    | Prel((Req | Rneq),t1,_) when isTypeTagType t1.term_type ->
        unsupported "Interp.terms comparison between tags"

    | Prel(rel,t1,t2) ->
        let res =
          product (fun t1 t2 -> mkexpr (LCPEbinary(t1,relation rel,t2)) p.loc)
            (coerce_type t1) (coerce_type t2)
        in (mkconjunct res p.loc)#node

    | Pand(p1,p2) ->
        LCPEbinary(pred p1,`Bland,pred p2)

    | Por(p1,p2) ->
        LCPEbinary(pred p1,`Blor,pred p2)

    | Pxor(p1,p2) ->
        let notp2 = { p2 with content = Pnot p2; } in
        let p1notp2 = { p with content = Pand(p1,notp2); } in
        let notp1 = { p1 with content = Pnot p1; } in
        let p2notp1 = { p with content = Pand(p2,notp1); } in
        LCPEbinary(pred p1notp2,`Blor,pred p2notp1)

    | Pimplies(p1,p2) ->
        LCPEbinary(pred p1,`Bimplies,pred p2)

    | Piff(p1,p2) ->
        LCPEbinary(pred p1,`Biff,pred p2)

    | Pnot p -> LCPEunary(`Unot,pred p)

    | Pif(t,p1,p2) -> LCPEif(term t,pred p1,pred p2)

    | Plet _ ->
	unsupported "Interp.pred Plet."

    | Pforall([],p) -> 
	(pred p)#node

    | Pforall _ ->
	unsupported "Interp.pred Pforall."

    | Pexists([],p) -> 
	(pred p)#node

    | Pexists _ ->
	unsupported "Interp.pred Pexists."

    | Pold p ->        
	LCPEold(pred p)

    | Pat _ -> 
	unsupported "Interp.pred Pat."

    | Pvalid_index _
    | Pvalid_range _ 
    | Pvalid _  
    | Pfresh _
    | Psubtype _ ->
	(* TODO *)
	Extlib.not_yet_implemented "Interp.pred (Pvalid | Pfresh | Psubtype)"

    | Pseparated(_seps) ->
	(* TODO *)
	Extlib.not_yet_implemented "Interp.pred Pseparated"

  in
  mkexpr enode p.loc

(* Keep names associated to predicate *)
let named_pred p =
  List.fold_right
    (fun lab p -> mkexpr (LCPElabel(lab,p)) p#pos) p.name (pred p)

let pred_has_name p n =
  List.exists (fun n2 -> n = n2) p.name

let remove_pred_name p n =
  { p with name = List.filter (fun n2 -> not (n = n2)) p.name }

let conjunct pos pl =
  mkconjunct (List.map (pred $ Logic_const.pred_of_id_pred) pl) pos

let zone = function
  | Location tset,_from -> terms tset.it_content
  | Nothing,_from -> []

(* Distinguish between:
 * - no assign, which is the empty list in Cil and None in Jessie
 * - assigns nothing, which is [Nothing] in Cil and the Some[] in Jessie
 *)
let assigns = function
  | [] -> None
  | assign_list ->
      let assign_list =
        List.filter
          (function
               Location out,_ ->
                 not (Logic_utils.is_result out.it_content)
             | Nothing, _ -> false)
          assign_list
      in
      let assign_list = List.flatten (List.map zone assign_list) in
      Some(Loc.dummy_position,assign_list)

let spec funspec =
  let require p =
    LCCrequires(locate (pred (Logic_const.pred_of_id_pred p)))
  in
  let requires = List.map require funspec.spec_requires in
  let is_normal_postcond =
    function (Normal,_) -> true
      | (Exits | Returns | Breaks | Continues),_ -> false
  in
  let behavior b =
    if List.exists (not $ is_normal_postcond) b.b_post_cond then
      warn_once "abrupt clause(s) ignored";
(*
    Format.eprintf "producing behavior '%s'@." b.b_name;
*)
    LCCbehavior(
      Loc.dummy_position,
      b.b_name,
      None, (* throws *)
      Some(conjunct Loc.dummy_position b.b_assumes),
      None, (* requires *)
      assigns b.b_assigns,
      locate
        (conjunct Loc.dummy_position
           (Extlib.filter_map
              (function (Normal,_) -> true
                 | (Exits | Returns | Breaks | Continues),_ -> false)
              snd b.b_post_cond)))
  in
  let behaviors = List.map behavior funspec.spec_behavior in

  let complete_behaviors_assertions : Clim_ast.pexpr list =
    List.map
      (fun bnames ->
         (* inutile, car dans le contexte de la precondition
            let r = mkconjunct
            (List.map (function
            | LCCrequires p -> p
            | _ -> assert false)
            requires)
            Loc.dummy_position
            in
         *)
         let a = mkdisjunct
           (List.fold_left
              (fun acc b ->
                 match b with
                   | LCCbehavior(_,name,_,Some a,_,_,_) ->
                       if (bnames = [] && name <> "default")
                         || List.mem name bnames
                       then
                         a :: acc
                       else acc
                   | _ -> assert false)
              [] behaviors)
           Loc.dummy_position
         in
         (*
           Some(mkexpr (LCPEbinary(r,`Bimplies,a)) Loc.dummy_position)
         *)
         a)
      funspec.spec_complete_behaviors
  in
  let disjoint_behaviors_assertions  : Clim_ast.pexpr list =
    List.fold_left
      (fun acc bnames ->
         let all_assumes =
           List.fold_left
             (fun acc b ->
                match b with
                  | LCCbehavior(_,name,_,Some a,_,_,_) ->
(*
                      Format.eprintf "name = %s, len bnames = %d@."
                        name (List.length bnames);
*)
                      if (bnames = [] && name <> "default") ||
                        List.mem name bnames
                      then
                        a :: acc
                      else acc
                  | _ -> assert false)
             [] behaviors
         in
         let rec aux assumes prevs acc =
           match assumes with
             | [] -> acc
             | b::rem ->
                 let acc =
                   List.fold_left
                     (fun acc a ->
                        (mkexpr (LCPEunary(`Unot,
                                           mkconjunct [b;a] Loc.dummy_position))
                           Loc.dummy_position)
                        :: acc)
                     acc prevs
                 in
                 aux rem (b::prevs) acc
         in
         aux all_assumes [] acc)
      [] funspec.spec_disjoint_behaviors
  in

  if funspec.spec_terminates <> None then
    warn_once "Termination condition(s) ignored" ;

  (* TODO: translate terminates clauses *)
  (requires @ behaviors),
  complete_behaviors_assertions,
  disjoint_behaviors_assertions

(* Depending on the argument status, an assertion with this status may
   not generate any PO but still be used as an hypothesis. *)
let assertion = function
  | { status = Checked { valid = True } } -> Aassume
  | _ -> Aassert

let built_behavior_ids l =
  let l =
    if l = [] then [ name_of_default_behavior ] else l
  in
  List.map (fun i -> new identifier i) l

let code_annot pos ((acc_assert_before,acc_assert_after,contract) as acc) a =
  let a, is_after =
    match a with
      | Before ca -> ca,false
      | After ca -> ca,true
  in
  let push s =
    if is_after
    then (acc_assert_before,s::acc_assert_after,contract)
    else (s::acc_assert_before,acc_assert_after,contract)
  in
  match a with
     | User annot ->
         begin
           match annot.annot_content with
             | AAssert (behav,p,status) ->
                 let behav = built_behavior_ids behav in
                 let asrt = assertion status in
                 push
                   (mkexpr
                      (LCPEassert (behav,asrt,locate ~pos (named_pred p))) pos)
             | AInvariant(_behav,is_loop_inv,_p) ->
                 if is_loop_inv then acc (* should be handled elsewhere *)
                 else unsupported "general code invariant"
(*
                let behav = built_behavior_ids behav in
                push
                  (mkexpr
                     (LCPEassert
                        (behav,Aassert,locate ~pos (named_pred p))) pos)
*)
            | APragma _ -> acc (* just ignored *)
            | AAssigns (_, _) -> acc (* should be handled elsewhere *)
            | AVariant _ -> acc (* should be handled elsewhere *)
            | AStmtSpec s ->
                begin
                  match contract with
                    | None -> (acc_assert_before,acc_assert_after,Some s)
                    | Some _ -> assert false
                end
        end
    | AI(alarm,annot) ->
        begin match annot.annot_content with
          | AAssert (behav,p,status) ->
              let asrt =
                if pred_has_name p name_of_hint_assertion then Ahint
                else assertion status
              in
              let p = remove_pred_name p name_of_hint_assertion in
              let behav = built_behavior_ids behav in
              push
                (mkexpr
                   (LCPEassert
                      (behav,asrt,locate ~alarm ~pos (named_pred p))) pos)
          | AInvariant(_behav,is_loop_inv,_p) ->
              if is_loop_inv then acc (* should be handled elsewhere *)
              else unsupported "general code invariant"
                (*
              let behav = built_behavior_ids behav in
              push
                (mkexpr
                   (LCPEassert
                      (behav,Aassert,locate ~alarm ~pos (named_pred p))) pos)
                *)
          | APragma _ -> assert false
          | AAssigns (_, _) -> assert false
          | AVariant _ -> assert false
          | AStmtSpec _ -> assert false
        end


(*****************************************************************************)
(* Cil to Clim translation of coding constructs                            *)
(*****************************************************************************)

let set_curFundec, get_curFundec =
  let cf = ref None in
  ((fun f -> cf := Some f),
   (fun () ->
      match !cf with
          None ->
            let res = emptyFunction "@empty@" in cf := Some res; res
        | Some res -> res))

let rec expr pos e =
  (* Precise the location if possible *)
  let pos = match e.enode with Info(_,einfo) -> einfo.exp_loc | _ -> pos in

  let expr = expr pos in
  let integral_expr = integral_expr pos in

  let enode =
    let e = stripInfo e in
    match e.enode with
    | Info _ -> assert false

    | Const c -> const ~in_code:true pos c

    | Lval lv -> (lval pos lv)#node

    | SizeOf ty ->
	(mkexpr (LCPEconst(LCCinteger(Int64.to_string (Int64.of_int (Cil.bitsSizeOf ty))))) pos)#node

    | SizeOfE _ | SizeOfStr _ | AlignOf _ | AlignOfE _ ->
        assert false (* Should be removed by constant folding *)

    | UnOp(_op,_e,ty) when isIntegralType ty ->
        (integral_expr e)#node

    | UnOp(op,e,_ty) ->
        let e =
          locate (mkexpr (LCPEunary(unop op,expr e)) pos)
        in
        e#node

    | BinOp(_op,_e1,_e2,ty) when isIntegralType ty ->
        (integral_expr e)#node

    | BinOp(op,e1,e2,_ty) ->
        let e =
          locate (mkexpr (LCPEbinary(expr e1,binop op,expr e2)) pos)
        in
        e#node

    | CastE(ptrty,_e1) when isPointerType ptrty ->
        begin
          let e = stripCastsAndInfo e in
          match e.enode with
          | Const c
              when is_integral_const c
                && value_of_integral_const c = Int64.zero ->
              LCPEconst LCCnull
          | _ ->
              let ety = typeOf e in
              if isIntegralType ety(*  && bits_sizeof ety = bits_sizeof ptrty *) then
(*                 let _,int_to_ptr = type_conversion ptrty ety in *)
(*                 LCPEapp(int_to_ptr,[],[integral_expr e]) *)
		unsupported "Casting from type %a to type %a not allowed"
                  !Ast_printer.d_type (typeOf e) !Ast_printer.d_type ptrty
              else if isPointerType ety then
(*                 let destty = pointed_type ptrty in *)
(*                 let srcty = pointed_type ety in *)
(*                 if Retype.subtype srcty destty then *)
(*                   (expr e)#node *)
(*                 else if Retype.TypeUnion.same_class destty srcty then *)
(*                   let enode = LCPEcast(expr e,ctype ptrty) in *)
(*                   let e = locate (mkexpr enode pos) in *)
(*                   e#node *)
(*                 else *)
                  (* bitwise cast *)
                  let enode = LCPEcast(expr e,ctype ptrty) in
                  let e = locate (mkexpr enode pos) in
                  e#node
(*                   let _,ptr_to_ptr = type_conversion ptrty ety in *)
(*                   LCPEapp(ptr_to_ptr,[],[expr e]) *)
              else
                (* Only hierarchical types are available in Jessie. It
                 * should have been encoded as the use of pointer types
                 * on structure type.
                 *)
(*               match unrollType ty with *)
(*                 | TComp(compinfo,_) -> *)
(*                     LCPEcast(expr (stripCasts e),compinfo.cname) *)
(*                 | _ -> assert false *)
                unsupported "Casting from type %a to type %a not allowed"
                  !Ast_printer.d_type (typeOf e) !Ast_printer.d_type ptrty
        end

    | CastE(ty,e') ->
        (* TODO: support other casts in Jessie as well, through low-level
         * memory model
         *)
        notimplemented  "Casting from type %a to type %a not allowed in %a with size %Ld and %Ld"
          !Ast_printer.d_type (typeOf e') !Ast_printer.d_type ty !Ast_printer.d_exp e
      ( bits_sizeof ty) ( bits_sizeof (typeOf e'))


    | AddrOf _lv -> assert false (* Should have been rewritten *)

    | StartOf lv -> (lval pos lv)#node
  in
  mkexpr enode pos

(* Function called when expecting a boolean in Clim, i.e. when translating
   a test or a sub-expression of an "or" or "and".
*)
and boolean_expr pos e =
  (* Precise the posation if possible *)
  let pos = match e.enode with Info(_,einfo) -> einfo.exp_loc | _ -> pos in

  let expr = expr pos in
  let boolean_expr = boolean_expr pos in
  let boolean_node_from_expr ty e =
    if isPointerType ty then LCPEbinary(e,`Bneq,null_expr)
    else if isArithmeticType ty then LCPEbinary (e,`Bneq,zero_expr)
    else assert false
  in

  let enode = match (stripInfo e).enode with
    | Info _ -> assert false

    | Const c -> LCPEconst(boolean_const c)

    | UnOp(LNot,e,_typ) -> LCPEunary(unop LNot,boolean_expr e)

    | BinOp((LAnd | LOr) as op,e1,e2,_typ) ->
        LCPEbinary(boolean_expr e1,binop op,boolean_expr e2)

    | BinOp((Eq | Ne) as op,e1,e2,_typ) ->
        LCPEbinary(expr e1,binop op,expr e2)

    | BinOp((Lt | Gt | Le | Ge) as op,e1,e2,_typ) ->
        let ty = typeOf e1 in
        if isArithmeticType ty then
          LCPEbinary(expr e1,binop op,expr e2)
        else
          (* Pointer comparison is translated as subtraction *)
          let sube = mkexpr (LCPEbinary(expr e1,`Bsub,expr e2)) pos in
          LCPEbinary(sube,binop op,zero_expr)

    | _ -> boolean_node_from_expr (typeOf e) (expr e)
  in
  mkexpr enode pos

(* Function called instead of plain [expr] when the evaluation result should
 * fit in a C integral type.
 *)
and integral_expr pos e =

  let rec int_expr pos e =
    let expr = expr pos in
    let int_expr = int_expr pos in
    let boolean_expr = boolean_expr pos in
    let node_from_boolean_expr e = LCPEif(e,one_expr,zero_expr) in

    let enode = match e.enode with
      | UnOp(LNot,e,_ty) ->
          let e = mkexpr (LCPEunary(unop LNot,boolean_expr e)) pos in
          node_from_boolean_expr e

      | UnOp(op,e,_ty) ->
          let e =
            locate (mkexpr (LCPEunary(unop op,expr e)) pos)
          in
          e#node

      | BinOp((LAnd | LOr) as op,e1,e2,_ty) ->
          let e =
            mkexpr (LCPEbinary(boolean_expr e1,binop op,boolean_expr e2)) pos
          in
          node_from_boolean_expr e

      | BinOp((Lt | Gt | Le | Ge),e1,_,_)
          when isPointerType (typeOf e1) ->
          unsupported  "Interp.integral_expr pointer comparison"

(*       | BinOp((Eq | Ne as op),e1,e2,_ty) *)
(*           when isPointerType (typeOf e1) && *)
(*             not (is_null_expr e2 || is_null_expr e1) -> *)
(*           (\* Pointer (dis-)equality is translated as subtraction *\) *)
(*           let sube = mkexpr (LCPEbinary(expr e1,`Bsub,expr e2)) pos in *)
(*           let e = mkexpr (LCPEbinary(sube,binop op,zero_expr)) pos in *)
(*           node_from_boolean_expr e *)

      | BinOp((Eq | Ne) as op,e1,e2,_ty) ->
          let e = mkexpr (LCPEbinary(expr e1,binop op,expr e2)) pos in
          node_from_boolean_expr e

      | BinOp((Lt | Gt | Le | Ge) as op,e1,e2,_ty) ->
          let e = mkexpr (LCPEbinary(expr e1,binop op,expr e2)) pos in
          node_from_boolean_expr e

      | BinOp((Shiftrt | Shiftlt),_,_,_) ->
          unsupported "Interp.integral_expr (Shiftrt | Shiftlt)"

      | BinOp(op,e1,e2,_ty) ->
          let e =
            locate (mkexpr (LCPEbinary(expr e1,binop op,expr e2)) pos)
          in
          e#node

      | CastE(_,e1) when isFloatingType (typeOf e1) ->
          unsupported  "Interp.integral_expr CastE from float"

      | CastE(_,e) when isIntegralType (typeOf e) ->
            (int_expr e)#node

      | _ -> (expr e)#node
    in
    mkexpr enode pos
  in

  match e.enode with
    | CastE _ -> int_expr pos e
    | _ -> int_expr pos (new_exp(CastE(typeOf e,e)))

and lval pos = function
  | Var v, NoOffset -> mkexpr (LCPEvar v.vname) pos

  | Var _v, _off -> assert false (* Should have been rewritten *)

  | Mem _e, NoOffset -> assert false (* Should have been rewritten *)

  | Mem e, Field(fi,off) ->
      assert (off = NoOffset); (* Others should have been rewritten *)
      let e = expr pos e in
      if fi.fcomp.cstruct then
        locate (mkexpr (LCPEcast(e,ctype fi.ftype)) pos)
      else (* field of union *)
	unsupported "Interp.lval pos for unions"

  | Mem _e, Index _ as lv ->
      Celia_options.fatal ~current:true "Unexpected lval %a" !Ast_printer.d_lval lv

let keep_only_declared_nb_of_arguments vi l =
  let _,args,is_variadic, _ = splitFunctionTypeVI vi in
  if args=None then
    (warning "skipping all arguments of implicit prototype %s" vi.vname;
     [])
  else if vi.vname="printf" then
    (warning "skipping all arguments of library function printf.";
     [])
  else if is_variadic then unsupported "unsupported variadic functions"
  else l

let rec instruction = function
  | Set(lv,e,pos) ->
      let enode = LCPEassign(lval pos lv,expr pos e) in
      (locate (mkexpr enode pos))#node

  | Call(None,{enode = Lval(Var v,NoOffset)},eargs,pos) ->
      if is_assert_function v then
        LCPEassert([new identifier name_of_default_behavior],
                   Aassert,locate (boolean_expr pos (as_singleton eargs)))
      else
        let enode =
          if is_free_function v then
            let arg = as_singleton eargs in
            let subarg = stripCasts arg in
            let arg = if isPointerType (typeOf subarg) then subarg else arg in
            LCPEfree(expr pos arg)
          else
            LCPEapp(v.vname,[],
                    keep_only_declared_nb_of_arguments
                      v
                      (List.map (expr pos) eargs))
        in
        (locate (mkexpr enode pos))#node

  | Call(Some lv,{enode = Lval(Var v,NoOffset)},eargs,pos) ->
      let enode =
        if is_malloc_function v || is_realloc_function v then
          let lvtyp = pointed_type (typeOfLval lv) in
          let lvsiz = (bits_sizeof lvtyp) lsr 3 in
          let arg =
            if is_malloc_function v then as_singleton eargs
            else (* realloc *)
              match eargs with [ _; arg ] -> arg | _ -> assert false
          in
          let arg = stripInfo arg in
          let ty,arg = match arg.enode with
            | Info _ -> assert false
            | Const c when is_integral_const c ->
                let allocsiz = (value_of_integral_expr arg) / lvsiz in
                let siznode = LCPEconst(LCCinteger(Int64.to_string allocsiz)) in
                lvtyp, mkexpr siznode pos
            | BinOp(Mult,({enode = Const c} as arg),nelem,_ty)
            | BinOp(Mult,nelem,({enode = Const c} as arg),_ty)
                when is_integral_const c ->
                let factor = (value_of_integral_expr arg) / lvsiz in
                let siz =
                  if factor = Int64.one then
                    expr pos nelem
                  else
                    let factor = constant_expr factor in
                    expr pos (new_exp(BinOp(Mult,nelem,factor,typeOf arg)))
                in
                lvtyp, siz
            | _ ->
                if lvsiz = Int64.one then
                  lvtyp, expr pos arg
                else
                  let esiz = constant_expr lvsiz in
                  lvtyp, expr pos (new_exp (BinOp(Div,arg,esiz,typeOf arg)))
          in
          let name_of_type = match unrollType ty with
            | TComp(compinfo,_,_) -> compinfo.cname
            | _ -> assert false
          in
          LCPEalloc(arg,name_of_type)
        else if is_calloc_function v then
          let nelem,elsize = match eargs with
            | [nelem;elsize] -> nelem,elsize
            | _ -> assert false
          in
          let arg = stripInfo elsize in
          let ty,arg = match arg.enode with
            | Info _ -> assert false
            | Const c when is_integral_const c ->
                let lvtyp = pointed_type (typeOfLval lv) in
                let lvsiz = (bits_sizeof lvtyp) lsr 3 in
                let factor = (value_of_integral_expr arg) / lvsiz in
                let siz =
                  if factor = Int64.one then
                    expr pos nelem
                  else
                    let factor = constant_expr factor in
                    expr pos (new_exp (BinOp(Mult,nelem,factor,typeOf arg)))
                in
                lvtyp, siz
            | _ ->
                let lvtyp = pointed_type (typeOfLval lv) in
                let lvsiz = (bits_sizeof lvtyp) lsr 3 in
                let esiz = constant_expr lvsiz in
                lvtyp,
                expr pos
                  (new_exp
                     (BinOp(Div,
                            new_exp (BinOp(Mult,nelem,elsize,typeOf arg)),
                            esiz,
                            typeOf arg)))
          in
          let name_of_type = match unrollType ty with
            | TComp(compinfo,_,_) -> compinfo.cname
            | _ -> assert false
          in
          LCPEalloc(arg,name_of_type)
        else
          LCPEapp(v.vname,[],
                  keep_only_declared_nb_of_arguments
                    v
                    (List.map (expr pos) eargs))
      in
      let lvty = typeOfLval lv in
      let call = locate (mkexpr enode pos) in
      let enode =
        if TypeComparable.equal lvty (getReturnType v.vtype)
          || is_malloc_function v
          || is_realloc_function v
          || is_calloc_function v
        then
          LCPEassign(lval pos lv,call)
        else
          Extlib.not_yet_implemented  "Interp.instruction Call"
      in
      (locate (mkexpr enode pos))#node

  | Call _ -> assert false

  | Asm _ -> assert false

  | Skip _pos -> LCPEconst LCCvoid

  | Code_annot _ -> assert false

let rec statement s =
  let pos = get_stmtLoc s.skind in
  CurrentLoc.set pos;
(*
  let assert_list =
    Annotations.get_filter Logic_utils.is_assert s
    @ Annotations.get_filter Logic_utils.is_stmt_invariant s
  in
  let assert_before,assert_after =
    List.partition (function Before _ -> true | After _ -> false) assert_list
  in
  let assert_before =
    List.flatten (List.map ((assert_ pos) $ before_after_content) assert_before)
  in
  let assert_after =
    List.flatten (List.map ((assert_ pos) $ before_after_content) assert_after)
  in
*)

  let assert_before, assert_after, _ =
    List.fold_left (code_annot pos)
      ([],[],None)
      (Annotations.get_filter (fun _ -> true) s)
  in
  let snode = match s.skind with
    | Instr i -> instruction i

    | Return(Some e,pos) ->
        LCPEreturn(expr pos e)

    | Return(None,_pos) ->
        LCPEreturn(mkexpr (LCPEconst LCCvoid) pos)

    | Goto _ ->
	assert false (* Should not occur after normalize *)

    | Break l ->
        LCPEbreak (Loc.string l)
	  (* Should not occur after [prepareCFG] *)

    | Continue l ->
        LCPEcontinue (Loc.string l)
	  (* Should not occur after [prepareCFG] *)

    | If(e,bl1,bl2,pos) ->
        LCPEif(boolean_expr pos e,block bl1,block bl2)

    | Switch(e,bl,slist,pos) ->
        let case_blocks stat_list case_list =
          let rec next_case curr_list final_list stat_list case_list =
            match stat_list,case_list with
              | curr_stat :: rest_stat, curr_case :: rest_case ->
                  if curr_case.sid = curr_stat.sid then
                    (* Beginning of a new case. Add previous case to list
                       if not empty. *)
                    let add_to_list cond e l = if cond e then e::l else l in
                    let not_empty_list = function [] -> false | _ -> true in
                    let final_list =
                      add_to_list not_empty_list (List.rev curr_list) final_list
                    in
                    let curr_list = [curr_stat] in
                    next_case curr_list final_list rest_stat rest_case
                  else
                    let curr_list = curr_stat :: curr_list in
                    next_case curr_list final_list rest_stat case_list
              | [], [] ->
                  if List.length curr_list <> 0 then
                    List.rev curr_list :: final_list
                  else
                    final_list
              | [], _ ->
                  (* More cases than available. *)
                  assert false
              | stat_list, [] ->
                  (List.rev_append curr_list stat_list) :: final_list
          in
          List.rev (next_case [] [] stat_list case_list)
        in
        let switch_label = function
          | Label _ -> assert false
          | Case(e,pos) -> Some(expr pos e)
          | Default _ -> None
        in
        let case = function
          | [] -> assert false
          | case_stmt :: _ as slist ->
              let switch_labels = List.filter is_case_label case_stmt.labels in
              let labs = List.map switch_label switch_labels in
              let slist = mkexpr (LCPEblock(statement_list slist)) pos in
              labs, slist
        in
        let case_list = List.map case (case_blocks bl.bstmts slist) in
        LCPEswitch(expr pos e,case_list)

    | Loop (_,bl,_pos,_continue_stmt,_break_stmt) ->
	let loop_annot =
	  Annotations.get_filter Logic_utils.is_loop_annot s
	in
	let loop_annot =
	  lift_annot_list_func (List.map (fun x -> x.annot_content)) loop_annot
	in
        let behs,variant =
	  List.fold_right (* to keep the same order *)
	    (fun annot (beh,var) ->
	       match annot with
		 | AVariant(v,rel) ->
		     begin
		       match var with
			 | None ->
			     begin
			       match rel with
				 | Some r ->
				     (beh, Some (locate (term v),
						 Some (new identifier r)) )
				 | None ->
				     (beh,Some (locate (term v) ,None ))
			     end
			 | Some _ -> assert false (* At most one variant *)
		     end
(*
		 | ALoopBehavior(ids,invs,assigns) ->
		     ((ids,List.map (fun p -> locate (pred p)) invs,
		       assigns)::beh,var)
*)
		 | AInvariant(ids,true,inv) ->
		     ((ids,[locate (pred inv)],[])::beh,var)
		 | AAssigns(ids,assign) ->
		     ((ids,[],[assign])::beh,var)
		 | APragma _ -> (* ignored *) (beh,var)
		 | _ -> assert false
		     (* others should not occur in loop annot *))
	    loop_annot ([],None)
	in
        (* Locate the beginning of the loop, to serve as location for generated
         * invariants and variants.
         *)
	(*         let lab = reg_pos pos in *)
        (* TODO: add loop-assigns to Jessie *)
        (* PARTIALLY DONE: added them as new jessie loop behaviors *)
        let behs = List.map
          (fun (beh_names,invs,ass) ->
             let beh_names = built_behavior_ids beh_names in
	     let inv =
	       match invs with
		 | [] -> None
		 | _ -> Some (mkconjunct invs pos)
	     in
	     let ass = assigns ass in
             (beh_names,inv,ass))
          behs
        in
        LCPEwhile(true_expr,behs,variant,block bl)

    | Block bl ->
        LCPEblock(statement_list bl.bstmts)

    | UnspecifiedSequence seq ->
        (* [VP] TODO: take into account undefined behavior tied to the
          effects of the statements... *)
        LCPEblock(statement_list (List.map (fun (x,_,_,_) -> x) seq))

    | TryFinally _ | TryExcept _ -> assert false
  in
  (* Prefix statement by all non-case labels *)
  let labels = filter_out is_case_label s.labels in
  let s = mkexpr snode pos in
  let s = match assert_before @ s :: assert_after with
    | [s] -> s
    | slist -> mkexpr (LCPEblock slist) pos
  in
  List.fold_left (fun s lab -> mkexpr (LCPElabel(label lab,s)) pos) s labels

and statement_list slist = List.rev (List.rev_map statement slist)

and block bl =
  match bl.bstmts with
    | [] -> mkexpr (LCPEconst LCCvoid) Loc.dummy_position
    | [s] -> statement s
    | slist -> mkexpr (LCPEblock(statement_list slist)) Loc.dummy_position


(*****************************************************************************)
(* Cil to Clim translation of global declarations                          *)
(*****************************************************************************)

let drop_on_unsupported_feature = false

let logic_variable v =
  let name = opt_app (fun v -> v.vname) v.lv_name v.lv_origin in
  ltype v.lv_type, name

let rec annotation is_axiomatic annot pos = match annot with
  | Dfun_or_pred info ->
      (* TODO: test that is predefined predicate on lists *)
      (if is_axiomatic &&
	 info.l_var_info.lv_name = "reach" then
	  warning "Declaration of predicate reach at line %s@." (Loc.string pos)
      else
	warning "Dropping declaration of predicate %s@." info.l_var_info.lv_name
      );
      []

  | Dlemma _
  | Dinvariant _ ->
      warning "Interp.annotation dropping (Dlemma | Dinvariant_property)";
      []

  | Dtype_annot _ 
  | Dtype _ ->
      (* TODO: store the information about list types *)
      warning "Interp.annotation dropping (Dtype_annot | Dtype)";
      []

  | Daxiomatic _ ->
      warning "Interp.annotation Daxiomatic";
      []

let default_field_modifiers = (false,false)

let global vardefs g =
  let pos = get_globalLoc g in
  CurrentLoc.set pos;
  let dnodes = match g with
    | GType _ -> [] (* No typedef in Jessie *)

    | GCompTag(compinfo,_) when compinfo.cstruct -> (* struct type *)
        let field fi =
          let this =
            default_field_modifiers,
            ctype (* ?bitsize:fi.fsize_in_bits *) fi.ftype,
            fi.fname, fi.fsize_in_bits
          in
          [this]
          (* TODO: MS deal correctly with padding *)
        in
        let fields =
          List.fold_right (fun fi acc ->
                             let repfi = Retype.FieldUnion.repr fi in
                             if FieldinfoComparable.equal fi repfi then
                               field fi @ acc
                             else acc
                          ) compinfo.cfields []
        in
        let _parent = None in
(*           find_or_none (Hashtbl.find Norm.type_to_parent_type) compinfo.cname *)
(*         in *)
        let ty = TComp(compinfo, empty_size_cache (), []) in
        begin try
          let parentty = TypeHashtbl.find Retype.type_to_parent_type ty in
          let parent = get_struct_name parentty in
          [
            LCDtag(compinfo.cname,[],Some (parent,[]),fields,[])
          ]
        with Not_found ->
          try
            ignore(TypeHashtbl.find Norm.generated_union_types ty);
            [LCDtag(compinfo.cname,[],None,fields,[])]
          with Not_found ->
            (* let id = mkidentifier compinfo.cname pos in *)
            [
              LCDtag(compinfo.cname,[],None,fields,[])
	      (* TODO: what are variants? ;
              LCDvariant_type(compinfo.cname,[id]) *)
            ]
        end

    | GCompTag _ -> 
	notimplemented  "Interp.global union type GCompTag"

    | GCompTagDecl _ -> [] (* No struct/union declaration in Jessie *)

    | GEnumTag(enuminfo,_pos) ->
        assert (not (enuminfo.eitems = []));
        let enums =
          List.map
            (fun {eival = enum} -> value_of_integral_expr enum) enuminfo.eitems
        in
        let emin =
          List.fold_left (fun acc enum ->
                            if acc < enum then acc else enum) (List.hd enums) enums
        in
        let min = Num.num_of_string (Int64.to_string emin) in
        let emax =
          List.fold_left (fun acc enum ->
                            if acc > enum then acc else enum) (List.hd enums) enums
        in
        let max = Num.num_of_string (Int64.to_string emax) in
        [LCDenum_type(enuminfo.ename,min,max)]

    | GEnumTagDecl _ -> [] (* No enumeration declaration in Jessie *)

    | GVarDecl(_,v,pos) -> 
        (* Keep only declarations for which there is no definition *)
        if List.mem v vardefs
          || (isFunctionType v.vtype &&
                (v.vname = name_of_assert
                    || v.vname = name_of_free
                    || v.vname = name_of_malloc))
        then []
        else if isFunctionType v.vtype then
          let rtyp = match unrollType v.vtype with
            | TFun(rt,_,_,_) -> rt
            | _ -> assert false
          in
          let id = mkidentifier v.vname pos in
          let kf = Globals.Functions.get v in
          let funspec = Kernel_function.get_spec kf in
          let params = Globals.Functions.get_params kf in
          let formal v = true, ctype v.vtype, unique_name_if_empty v.vname in
          let formals = List.map formal params in
          let s,_cba,_dba = spec funspec in
          [LCDfun(ctype rtyp,id,formals,s,None)]
        else
          [LCDvar(ctype v.vtype,v.vname,None)]

    | GVar(v,{init=None},_pos) ->
        [LCDvar(ctype v.vtype,v.vname,None)]

    | GVar(_v,_iinfo,_pos) ->
        (* Initialization should have been rewritten as code in an
         * initialization function, that is called by the main function in
         * global analyses and ignored otherwise.
         *)
        assert false

    | GFun(f,pos) ->
        set_curFundec f;
        if f.svar.vname = name_of_assert
          || f.svar.vname = name_of_free then []
        else
          let rty = match unrollType f.svar.vtype with
            | TFun(ty,_,_,_) -> ty
            | _ -> assert false
          in
          let formal v = true, ctype v.vtype, v.vname in
          let formals = List.map formal f.sformals in
          let id = mkidentifier f.svar.vname f.svar.vdecl in
          let funspec =
            Kernel_function.get_spec (Globals.Functions.get f.svar)
          in
          begin try
            let local v =
              mkexpr (LCPEdecl(ctype v.vtype,v.vname,None)) v.vdecl
            in
            let locals = List.rev (List.rev_map local f.slocals) in
            let body = mkexpr (LCPEblock(statement_list f.sbody.bstmts)) pos in
            let s,cba,dba = spec funspec in
            let body =
              List.fold_left
                (fun acc a ->
                   (mkexpr
                      (LCPEassert([],
                                  Acheck,
                                  mkexpr
                                    (LCPElabel("complete_behaviors",a))
                                    a#pos))
                      a#pos)
                   :: acc)
                (locals @ [body]) cba
            in
            let body =
              List.fold_left
                (fun acc a ->
                   (mkexpr
                      (LCPEassert([],
                                  Acheck,
                                  mkexpr
                                    (LCPElabel("disjoint_behaviors",a))
                                    a#pos))
                      a#pos)
                   :: acc)
                body dba
            in
            let body = mkexpr (LCPEblock body) pos in
            ignore
              (reg_pos ~id:f.svar.vname
                 ~name:("Function " ^ f.svar.vname) f.svar.vdecl);
            [LCDfun(ctype rty,id,formals,s,Some body)]
          with (Unsupported _ | NotImplemented _) when drop_on_unsupported_feature ->
            warning "Dropping definition of function %s@." f.svar.vname ;
            let s,_cba,_dba = spec funspec in
            [LCDfun(ctype rty,id,formals,s,None)]
          end

    | GAsm _ ->
	unsupported "assembly code"

    | GPragma _ -> [] (* Pragmas treated separately *)

    | GText _ -> [] (* Ignore text in Jessie *)

    | GAnnot(la,pos) -> annotation false la pos

  in
  List.map (fun dnode -> mkdecl dnode pos) dnodes

let integral_type name ty bitsize =
  let min = Num.num_of_big_int (min_value_of_integral_type ~bitsize ty) in
  let max = Num.num_of_big_int (max_value_of_integral_type ~bitsize ty) in
  mkdecl (LCDenum_type(name,min,max)) Loc.dummy_position

let integral_types () = []

let file f =
  let filter_defined = function GFun _ | GVar _ -> true | _ -> false in
  let defined_var =
    function GFun(f,_) -> f.svar | GVar(vi,_,_) -> vi | _ -> assert false
  in
  let globals =
(* AVOID CHECKING THE GLOBAL INITIALIZATION FUNCTION, WHICH IS GUARANTEED *)
(*     if Globals.has_entry_point () then *)
(*       let gif = *)
(*         Kernel_function.get_definition (Globals.Functions.get_glob_init f) *)
(*       in *)
(*       f.globals @ [GFun(gif,Loc.dummy_position)] *)
(*     else  *)f.globals
  in
  let vardefs =
    List.rev (List.rev_map defined_var (List.filter filter_defined globals))
  in
  (* Compute translation of [globals] *)
  let globals' =
    List.flatten (List.rev (List.rev_map (global vardefs) globals))
  in
    globals'


(*
Local Variables:
compile-command: "LC_ALL=C make -C .. -j jessie_plugin.byte"
End:
*)
