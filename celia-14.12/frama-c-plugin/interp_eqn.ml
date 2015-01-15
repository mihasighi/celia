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

(* Import from Cinv *)
open Clim
open Cinv_equation
open Cil2apron
(* open Cil2shad *)

(* Utility functions *)
open Common


(*****************************************************************************)
(* Types list encoding                                                       *)
(*****************************************************************************)

let rec filter_ltype (gdec:Cil_types.global) : unit = 
  match gdec with
  | GCompTag (ci,_) ->
    if not(ci.creferenced) then
      ()
    else if not(ci.cstruct) then
      warning "Type union ignored"
    else
      List.iter filter_field ci.cfields

  | _ -> ()

and filter_field (fi:Cil_types.fieldinfo) : unit =
  let pos = Cil2apron.get_field_number fi.fname in
  match Cil.unrollType fi.ftype with
    | TInt _ ->
      if pos <> 0 then
	warning "Integer field '%s' ignored" fi.fname

    | TPtr _ ->
      (* TODO: deal correctly with pointer fields *)
      let _ = Cil2apron.add_field fi.fname in
      ()

    | TNamed _ -> assert false (* Removed by call to [unrollType] *)
 
    | _ -> warning "Field '%s' ignored" fi.fname; ()


let name_of_printf = "printf"
let is_printf_function v = isFunctionType v.vtype && v.vname = name_of_printf


(*****************************************************************************)
(* Smart constructors.                                                       *)
(*****************************************************************************)

let point_of_stmt
    (s:Cil_types.stmt)
    :
    Cinv_equation.point
    =
  let loc = Cil_datatype.Stmt.loc s in
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
  { pos=(fst (Cil_datatype.Stmt.loc s)); sid=nsid; }

let point_of_block_else
    (s:Cil_types.stmt)
    : Cinv_equation.point
    =
  let nsid = (-2) in
  { pos=(fst (Cil_datatype.Stmt.loc s)); sid=nsid; }

(* Dummy FAIL control point for verification *)
let point_of_fail 
    (point:Cinv_equation.point)
    : Cinv_equation.point
    =
  let nsid = (-1) in
  { pos=point.pos; sid=nsid; }

let point_of_inv
    (s:Cil_types.stmt)
    : Cinv_equation.point
    =
  let nsid = (-3) in
  { pos=(fst (Cil_datatype.Stmt.loc s)); sid=nsid; }

let point_of_post
    (s:Cil_types.stmt)
    : Cinv_equation.point
    =
  let nsid = (-3) in
  { pos=(fst (Cil_datatype.Stmt.loc s)); sid=nsid; }

(*****************************************************************************)
(* Translation of Cil types                                                  *)
(*****************************************************************************)

(* Build a Clim type from a C type and return the number of ptr nested *)
let ctype ty =
  match unrollType ty with
    | TVoid _attr -> Clim_env.LCTnative Clim_env.Tunit , 0

    | TInt(_ik,_attr) ->
        Clim_env.LCTnative Clim_env.Tinteger , 0

    | TPtr(_elemty,_attr) ->
        if is_reference_type ty then
          (* Do not use [_elemty] directly but rather [pointed_type ty] in order
           * to get to the array element in references, i.e. pointers to arrays.
           *)
          begin
          let ty1 = unrollType (Ast_info.direct_pointed_type ty) in
          (* let ty1 = unrollType (pointed_type ty) in *)
          match ty1 with
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
				    Some min_bound,Some max_bound) , 1
            | TPtr(elemty1,_attr2) -> (*MS introduced because no more type arrays for out parameters *)
                let ety1 = unrollType (Ast_info.direct_pointed_type ty1) in
                match ety1 with
                | TComp(compinfo,_,_) ->
		let refsize = 1 (* reference_size ty *) in
		let rinfo = { Clim_env.lc_root_info_name = compinfo.cname;
			      Clim_env.lc_root_info_hroots = [];
			      Clim_env.lc_root_info_union_size_in_bytes = refsize; } in
                Clim_env.LCTpointer(Clim_env.LCroot rinfo,None,None) , 2
		(*MS normally is a pointer to pointer *)

                | _ -> 
		unsupported "Type (unrolled from %a) %a not allowed" 
			Printer.pp_typ ty 
			Printer.pp_typ ty1
          end
        else
          unsupported "Type %a not allowed" Printer.pp_typ ty

    | TArray _ -> assert false (* Removed by translation *)

    | TNamed _ -> assert false (* Removed by call to [unrollType] *)

    | TEnum(enuminfo,_) ->
	let einfo = { Clim_env.lc_enum_info_name = enuminfo.ename;
		      Clim_env.lc_enum_info_min = Num.num_of_int 0;
		      Clim_env.lc_enum_info_max = Num.num_of_int ((List.length enuminfo.eitems) - 1); } in
	Clim_env.LCTenum einfo, 0

    | TFun _ ->
	unsupported "Function pointer type %a not allowed" Printer.pp_typ ty
    | TComp _ -> 
	unsupported "Type struct not allowed"
    | TFloat _ -> 
	unsupported "Datatype.float not allowed"
    | TBuiltin_va_list _ -> 
	unsupported "Type builtin_va_list not allowed"

(* Return kind of type *)
let get_inout_type 
    (ty:Cil_types.typ) 
    : 
    bool * Clim_env.lc_type 
    =
  let lct, nptr = ctype ty in
  (* Do not use is_reference_type ty 
     because ctype eliminates all Ptr *)
  if nptr <= 1 then
    true, lct
  else
    false, lct 
  (* match ty with
    | TPtr _ -> false, lct
    | _ -> true, lct
  *)


(*****************************************************************************)
(* Translation of code annotations *)
(*****************************************************************************)

let generate_spec env
    (bpoint:Cinv_equation.point)
    (al:Cil_types.code_annotation list)
    : Apron.Tcons1.t Boolexpr.t
    =
  let filter_spec annot : Cil_types.identified_predicate list =
    (match annot.annot_content with
      | Cil_types.AAssert (_,p)
      | Cil_types.AInvariant(_,true,p) -> 
	[{Cil_types.ip_name=p.Cil_types.name; 
	  Cil_types.ip_loc=p.Cil_types.loc; 
	  Cil_types.ip_id=0; (* WARNING *)
	  Cil_types.ip_content=p.Cil_types.content}]
      | _ -> warning "unknown annotation (at %a)@." Cinv_equation.print_point bpoint; []
    )
  in
  let alf = (List.fold_left (fun l a -> l@(filter_spec a)) [] al) in
  match alf with
    | [] ->
      warning "Interp_eqn: annotation empty from %d (at %a)@." 
	(List.length al) Cinv_equation.print_point bpoint; 
      (Boolexpr.make_cst true)
    | _ ->  
      let conv_spec = 
	if Celia_options.UseSL3.get() then Cil2shad.convert_spec 
	else Cil2apron.convert_spec 
      in
      let spec_tc1_earray = conv_spec bpoint env alf in (* returns an array of length 1 *)
      if (Array.length spec_tc1_earray.Apron.Tcons1.tcons0_array) >= 1
      then begin
	let c0 = (Array.get spec_tc1_earray.Apron.Tcons1.tcons0_array 0) in
	Boolexpr.make_condition {Apron.Tcons1.tcons0=c0; Apron.Tcons1.env=env;}
      end
      else
	(Boolexpr.make_cst true)
      

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
	  let v,e = Cil2apron.expr_as_param (* env *) subarg in 
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

let instruction env 
    (procinfo:Cinv_equation.procinfo) 
    (bpoint:Cinv_equation.point)
    (sid:int)
    (i:Cil_types.instr)
    (fgraph:Cinv_equation.graph)
    (bgraph:Cinv_equation.graph)
    :
    Cinv_equation.point
    = 
  match i with
  | Set(lv,e,pos) ->
      (** Supposed to be in normal form! *)
      let te = (Cil2apron.expr env e) in
      let v,fi = (Cil2apron.lval lv) in 
      let te' = Cil2apron.mkexpr_deref env te fi in
      let transfer = Cinv_equation.Tassign(v,te') in
      let dest = (point_of_instr pos sid) in
      begin
	Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	Cinv_equation.add_equation bgraph [|dest|] transfer bpoint;
	dest
      end

  | Cil_types.Call(res,{enode = Lval(Var v,NoOffset)},eargs,pos) ->
      (** Function call *)
      let dest = (point_of_instr pos sid) in
      (try
      begin
	(if (is_assert_function v) && res=None then (* assert *)
	  let bte = Cil2apron.boolean_expr env (as_singleton eargs) in
	  let transfer = Cinv_equation.Condition(bte) in (* TODO: correct assert *)
	  begin
	    Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	    Cinv_equation.add_equation fgraph [|dest|] transfer bpoint
	  end
	else if (is_free_function v) && res=None then (* free *)
            let arg = as_singleton eargs in
            let subarg = stripCasts arg in
            let arg = if isPointerType (typeOf subarg) then subarg else arg in
	    let x,_ = Cil2apron.expr_as_param (* env *) arg in
	    let te' = Cil2apron.mkexpr_free env in
	    let transfer = Cinv_equation.Tassign(x,te') in
	    begin
	      Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	      Cinv_equation.add_equation bgraph [|dest|] transfer bpoint
	    end

	else if ((is_printf_function v)
		 && (res=None)) 
	then (* printf *)
	  let transfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in (* TODO: correct printf *)
	  begin
	    warning "skipping library function printf.";
	    Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	    Cinv_equation.add_equation bgraph [|dest|] transfer bpoint
	  end
    
	else if (is_malloc_function v 
		 || is_realloc_function v 
		 || is_calloc_function v)	
	then (* alloc *)
	  (* arguments have been checked *)
	  let lv = (match res with Some lv -> lv | _ -> assert false) in
	  let x,_ = Cil2apron.lval lv in (* TODO: fi shall be empty after rewriting *)
	  let te = Cil2apron.mkexpr_alloc env in
	  let transfer = Cinv_equation.Tassign(x,te) in
	  begin
	    Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	    Cinv_equation.add_equation bgraph [|dest|] transfer bpoint
	  end

        else (* general case *)
	  let info = PSHGraph.info fgraph in
	  let callee = Hashhe.find info.Cinv_equation.procinfo v.vname in
	  let pinl, poutl = keep_only_declared_nb_of_arguments (* env *) v eargs in
	  (* compute actual result *)
	  let x = (match res with None -> [] 
	  | Some lv -> (let r,_ = Cil2apron.lval lv in [r]))
	  in
          let pin = Array.of_list pinl in
          let pout = Array.of_list (poutl@x) in
          let calltransfer = Cinv_equation.Call(procinfo,callee,pin,pout) in
          let rettransfer = Cinv_equation.Return(procinfo,callee,pin,pout) in
	  begin
            Cinv_equation.add_equation fgraph
	      [|bpoint|] calltransfer callee.Cinv_equation.pstart;
            Cinv_equation.add_equation fgraph
	      [|bpoint; callee.Cinv_equation.pexit|] rettransfer dest;
            Cinv_equation.add_equation bgraph
	      [|callee.Cinv_equation.pstart|] calltransfer bpoint;
            Cinv_equation.add_equation bgraph
	      [|dest|] rettransfer callee.Cinv_equation.pexit
	  end
	)
      end
	
       with Not_found -> (* function called not found, replace call by skip *)
	 begin
	   let transfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in
	   begin
	     Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	     Cinv_equation.add_equation bgraph [|dest|] transfer bpoint
	   end
	 end
      );
      dest

  | Cil_types.Call _ -> assert false

  | Asm _ -> assert false

  | Skip pos ->
      let dest = (point_of_instr pos sid) in
      let transfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in
      begin
	Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	Cinv_equation.add_equation bgraph [|dest|] transfer bpoint;
	dest
      end

  | Code_annot(annot,pos) -> 
    let cond = generate_spec env bpoint [annot] in
    let pfail = (point_of_fail procinfo.pstart) in
    match cond with
      | Boolexpr.CST(true) -> (* nothing to do *)
	bpoint
      | Boolexpr.CST(false) -> (* goto fail *)
	pfail
      | _ -> (* generate an if (cond) then continue else fail *)
	let dest = (point_of_instr pos sid) in
	let condnot = Clim.Boolexpr.make_not Cil2apron.negate_tcons cond in
	let condtransfer = Cinv_equation.Condition(cond) in
	let condnottransfer = Cinv_equation.Condition(condnot) in
	begin
	  (Celia_options.debug 
	     "Code annotation within points (bpoint=%a,truepoint=%a,falsepoint=%a)\n"
	     (fun fmt p -> Cinv_equation.print_point fmt p) bpoint
	     (fun fmt p -> Cinv_equation.print_point fmt p) dest
	     (fun fmt p -> Cinv_equation.print_point fmt p) pfail);
	  (* put constraint computed between bpoint and dest *)
	  Cinv_equation.add_equation fgraph [|bpoint|] condtransfer dest;
	  Cinv_equation.add_equation bgraph [|dest|] condtransfer bpoint;
	  (* put negated constraint between bpoint and fail *)
	  Cinv_equation.add_equation fgraph [|bpoint|] condnottransfer pfail; (* *)
	  (* TODO: see for the backward graph *)
	  dest
	end

let loop_entry_points : Cinv_equation.point list ref = ref []
let loop_exit_points : Cinv_equation.point list ref = ref []
let push_loop bpoint dest =
  begin
    (Celia_options.debug "push loop!");
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
    (Celia_options.debug "pop loop!");
    loop_entry_points := List.tl !loop_entry_points;
    loop_exit_points := List.tl !loop_exit_points
  end

let rec statement env  
    (procinfo:Cinv_equation.procinfo) 
    (bpoint:Cinv_equation.point)
    (s:Cil_types.stmt)
    (fgraph:Cinv_equation.graph)
    (bgraph:Cinv_equation.graph)
    :
    Cinv_equation.point
    =
  (Celia_options.debug "statement %d" s.Cil_types.sid);
  match s.skind with
  | Instr i -> instruction env procinfo bpoint s.Cil_types.sid i fgraph bgraph

  | Cil_types.Return(Some e,pos) ->
      let dest = (point_of_instr pos s.Cil_types.sid) in
      (* e shall be a variable after normalization *)
      (* return is the last statement of the list *)
      let te = (Cil2apron.expr env e) in
      let v = Apron.Var.of_string "__return" in 
      let transfer = Cinv_equation.Tassign(v,te) in
      (* test post-condition *)
      let pfail = (point_of_fail procinfo.pstart) in
      let post_flag = (Array.length procinfo.pensures.Apron.Tcons1.tcons0_array) >= 1 in
      let post_point = if post_flag then (point_of_post s) else dest in
      begin
	Cinv_equation.add_equation fgraph [|bpoint|] transfer post_point;
	Cinv_equation.add_equation bgraph [|post_point|] transfer bpoint;
	if post_flag then (* test post-condition *)
	  begin
	    let c0 = (Array.get procinfo.pensures.Apron.Tcons1.tcons0_array 0) in
	    let postcond = Boolexpr.make_condition {Apron.Tcons1.tcons0=c0; Apron.Tcons1.env=env;} in
	    let notpostcond = Clim.Boolexpr.make_not Cil2apron.negate_tcons postcond in
	    let condtransfer = Cinv_equation.Condition(postcond) in
	    let condnottransfer = Cinv_equation.Condition(notpostcond) in
	    Cinv_equation.add_equation fgraph [|post_point|] condtransfer dest;
	    Cinv_equation.add_equation bgraph [|dest|] condtransfer post_point;
	    Cinv_equation.add_equation fgraph [|post_point|] condnottransfer pfail (* *)
	  (* TODO: and for backward graph? *)
	  end
	;
	dest
      end

  | Cil_types.Return(None,pos) ->
      let dest = (point_of_instr pos s.Cil_types.sid) in
      (* test post-condition *)
      let pfail = (point_of_fail procinfo.pstart) in
      let post_flag = (Array.length procinfo.pensures.Apron.Tcons1.tcons0_array) >= 1 in
      begin
	if post_flag then (* test post-condition *)
	  begin
	    let c0 = (Array.get procinfo.pensures.Apron.Tcons1.tcons0_array 0) in
	    let postcond = Boolexpr.make_condition {Apron.Tcons1.tcons0=c0; Apron.Tcons1.env=env;} in
	    let notpostcond = Clim.Boolexpr.make_not Cil2apron.negate_tcons postcond in
	    let condtransfer = Cinv_equation.Condition(postcond) in
	    let condnottransfer = Cinv_equation.Condition(notpostcond) in
	    Cinv_equation.add_equation fgraph [|bpoint|] condtransfer dest;
	    Cinv_equation.add_equation bgraph [|dest|] condtransfer bpoint;
	    Cinv_equation.add_equation fgraph [|bpoint|] condnottransfer pfail (* *)
	  (* TODO: and for backward graph? *)
	  end
	else
	  begin
	    let transfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in
	    Cinv_equation.add_equation fgraph [|bpoint|] transfer dest;
	    Cinv_equation.add_equation bgraph [|dest|] transfer bpoint
	  end
	;
	dest
      end

  | Goto(sr,pos) ->
      (* TODO: check that Should not occur after normalize *)
      let dest = (point_of_instr pos s.Cil_types.sid) in
      let target = (point_of_stmt !sr) in
      let transfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in
      begin
	Cinv_equation.add_equation fgraph [|bpoint|] transfer target;
	Cinv_equation.add_equation bgraph [|target|] transfer bpoint;
	dest
      end

  | Break pos ->
    let dest = (point_of_instr pos s.Cil_types.sid) in
    begin match !loop_exit_points with
	[] -> assert false
      | _ -> begin
	let lexit = List.hd !loop_exit_points in
	let transfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in
	begin
	  Cinv_equation.add_equation fgraph [|bpoint|] transfer lexit;
	  Cinv_equation.add_equation bgraph [|lexit|] transfer bpoint;
	  dest
	end
      end
    end
  (* Should not occur after [prepareCFG] *)

  | Continue pos ->
    let dest = (point_of_instr pos s.Cil_types.sid) in
    let lentry = List.hd !loop_entry_points in
    let transfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in
    begin
      Cinv_equation.add_equation fgraph [|bpoint|] transfer lentry;
      Cinv_equation.add_equation bgraph [|lentry|] transfer bpoint;
      dest
    end
  (* Should not occur after [prepareCFG] *)

  | If(e,bl1,bl2,pos) ->
      let dest = (point_of_instr pos s.Cil_types.sid) in
      let cond = (Cil2apron.boolean_expr env e) in
      let condnot = Clim.Boolexpr.make_not Cil2apron.negate_tcons cond in
      let condtransfer = Cinv_equation.Condition(cond) in
      let condnottransfer = Cinv_equation.Condition(condnot) in
      let truetransfer = Cinv_equation.Condition(Cil2apron.mkconjunct []) in
      let bpoint1 = (point_of_block_if s) in
      let bpoint2 = (point_of_block_else s) in
      begin
	begin (* block if *)
	  if (List.length bl1.bstmts) = 0 then (* empty bl1 *)
	    begin
	      Cinv_equation.add_equation fgraph [|bpoint|] condtransfer dest;
	      Cinv_equation.add_equation bgraph [|dest|] condtransfer bpoint
	    end
	  else
	    let epoint1 = statement_list env procinfo bpoint1 bl1.bstmts fgraph bgraph in
	    begin
	      Cinv_equation.add_equation fgraph [|bpoint|] condtransfer bpoint1;
	      Cinv_equation.add_equation fgraph [|epoint1|] truetransfer dest;
	      Cinv_equation.add_equation bgraph [|bpoint1|] condtransfer bpoint;
	      Cinv_equation.add_equation bgraph [|dest|] truetransfer epoint1
	    end
	end;
	begin (* block else *)
	  if (List.length bl2.bstmts) = 0 then
	    begin
	      Cinv_equation.add_equation fgraph [|bpoint|] condnottransfer dest;
	      Cinv_equation.add_equation bgraph [|dest|] condnottransfer bpoint
	    end
	  else
	    let epoint2 = statement_list env procinfo bpoint2 bl2.bstmts fgraph bgraph in
	    begin
	      Cinv_equation.add_equation fgraph [|bpoint|] condnottransfer bpoint2;
	      Cinv_equation.add_equation fgraph [|epoint2|] truetransfer dest;
	      Cinv_equation.add_equation bgraph [|bpoint2|] condnottransfer bpoint;
	      Cinv_equation.add_equation bgraph [|dest|] truetransfer epoint2
	    end
	end;
	dest
      end

  | Switch _ ->
      (* Should not occur after [prepareCFG] *)
      unsupported "Interp_eqn.statement Switch"

  | Loop (_,bl,pos,_continue_stmt,_break_stmt) ->
    let dest = (point_of_instr pos s.Cil_types.sid) in
    let pfail = (point_of_fail procinfo.pstart) in
    (* generate annotation: first argument above is empty! *)   
    let all_annot =
      List.sort
        (*MS for Neon Cil_datatype.Rooted_code_annotation_before_after.compare *)
        Cil_datatype.Code_annotation.compare
        (Annotations.code_annot s) (*MS changed for Neon *)
    in
    let loop_annot, _ = (*MS changed for Neon *)
      List.partition Logic_utils.is_loop_annot all_annot
    in
    let invcond = generate_spec env bpoint loop_annot in
    let notinvcond = Clim.Boolexpr.make_not Cil2apron.negate_tcons invcond in
    let inv_flag = (match invcond with
      | Boolexpr.CST(b) ->  
	if not(b) then warning "Interp_eqn: false invariant annotation"; 
	false
      | _ -> true
    ) in
    let spoint = if not(inv_flag) then bpoint else point_of_inv s in
    let truetransfer = Cinv_equation.Condition(mkconjunct []) in
    let condtransfer = 
      if not(inv_flag) then Cinv_equation.Condition(Cil2apron.mkconjunct [])
      else Cinv_equation.Condition(invcond) in
    let condnottransfer = 
      if not(inv_flag) then Cinv_equation.Condition(Cil2apron.mkconjunct []) 
      else Cinv_equation.Condition(notinvcond) in
    begin
      (push_loop bpoint dest);
      let epoint = (* exit point from the loop *)
	if (List.length bl.bstmts) = 0 then (* infinite loop: invariant tested at init *)
	  spoint
	else
	  (statement_list env procinfo spoint bl.bstmts fgraph bgraph)
      in
      begin
	(* return arc to bpoint *)
	Cinv_equation.add_equation fgraph [|epoint|] truetransfer bpoint;
	Cinv_equation.add_equation bgraph [|bpoint|] truetransfer epoint;
	if inv_flag then (* test invariant *)
	  begin
	    Cinv_equation.add_equation fgraph [|bpoint|] condtransfer spoint;
	    Cinv_equation.add_equation bgraph [|spoint|] condtransfer bpoint;
	    Cinv_equation.add_equation fgraph [|bpoint|] condnottransfer pfail (* *)
	  (* TODO: and for backward graph? *)
	  end
      end
      ;
      (* MAJ loop_*_points *)
      pop_loop ();
      dest	  
    end

  | Block bl ->
    statement_list env procinfo bpoint bl.bstmts fgraph bgraph
  (* returns the exit point *)
      
  | UnspecifiedSequence seq ->
    (* (MS) Choose the order given by the list of statements *)
    let stmts = List.map (fun (s,_,_,_,_) -> s) seq in
    statement_list env procinfo bpoint stmts fgraph bgraph    

  | TryFinally _ | TryExcept _ -> assert false

and statement_list env
    (procinfo:Cinv_equation.procinfo) 
    (bpoint:Cinv_equation.point)
    (stmts:Cil_types.stmt list)
    (fgraph:Cinv_equation.graph)
    (bgraph:Cinv_equation.graph)
    :
    Cinv_equation.point 
    =
  List.fold_left 
    (begin fun bp st ->
      statement env procinfo bp st fgraph bgraph
    end)
    bpoint
    stmts

(*****************************************************************************)
(* Collecting global infos for functions                                     *)
(*****************************************************************************)

let filter_fun (gdec:Cil_types.global) : bool 
  = 
  match gdec with
  | GFun (fi,_) -> 
        if Common.is_bswap_function fi.svar then false
        else true
  | _ -> false

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
  | [s] -> let loc = Cil_datatype.Stmt.loc s in
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
  (Celia_options.debug "Collect formal parameters...");
  match fvars with
  | [] -> [],[]
  | vi::vl -> 
      let in1,out1 = convert_in_out outf vl in
      let b,ty = get_inout_type vi.vtype in
      if b then (* in *)
      begin
        (Celia_options.debug "Variable %s is in" vi.vname);
	(Apron.Var.of_string vi.vname, ty)::in1, out1
      end
      else if outf then (* out *)
      begin
        (Celia_options.debug "Variable %s is out" vi.vname);
	in1, (Apron.Var.of_string vi.vname, ty)::out1
      end
      else
      begin
        (Celia_options.debug "Variable %s is inout" vi.vname);
	(* Special case of local variables of type reference to intlist *)
	(Apron.Var.of_string vi.vname, ty)::in1, out1
	(* unsupported "Local reference variable %a " Printer.pp_varinfo vi *)
      end

(** Convert return into a variable \return if not void type. *)
let convert_return 
    (ty:Cil_types.typ) 
    :
    (Apron.Var.t * Clim_env.lc_type) list
    =
  match ty with
  | TFun (rt,_,_,_) ->
      begin
	let lct, nptr = ctype rt in
	match lct with
	| Clim_env.LCTnative Clim_env.Tunit -> []
	| _ ->
	    if (* is_reference_type ty *) nptr >= 2 then
	      unsupported "Function with reference in return"
	    else
	      [(Apron.Var.of_string "__return",lct)]
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


(** Build a [Cinv_equation.procinfo] object from [Cil_types.global]. *)
let make_procinfo (gdec:Cil_types.global) 
    : 
    Cinv_equation.procinfo * string
  =
  match gdec with
  | GFun(fd,loc) ->
    (Celia_options.debug "Collect infos procedure %s with %d formals..." fd.svar.vname (List.length fd.sformals)); 
    let funspec = Annotations.funspec (Globals.Functions.get fd.svar) in
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

      let conv_spec = 
	if Celia_options.UseSL3.get() then Cil2shad.convert_spec 
	else Cil2apron.convert_spec in
      let prequires = 
	let lenv = add_env (Apron.Environment.make [||] [||]) vinput in
	conv_spec pstart
	  lenv (* limitation of cinv *)
	  (List.fold_left
	     (fun l b -> List.append l b.b_requires) (* WARNING: not correct because (pre,post) *)
	     []
	     funspec.spec_behavior) in

      let pensures = 
	conv_spec pexit
	  (* the current translation of post-condition tests it
           * in the full environment of the procedure *)
	  penv
	  (List.fold_left
	     (fun l b -> List.append l (List.map snd b.b_post_cond)) (* WARNING: not correct because (pre,post) *)
	     []
	     funspec.spec_behavior) in

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
       Cinv_equation.pensures = pensures;
     },
      fd.svar.vname

  | _ -> assert false

(* Apply function f on each instruction *)
let iter_instr 
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
	      iter_instr_block f bp (List.map (fun (s,_,_,_,_) -> s) l)
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
      List.iter (iter_stmt f) (List.map (fun (s,_,_,_,_) -> s) l)

    | Cil_types.Return _ | Break _ | Continue _ | Goto _ -> ()
    | _ -> assert false
    end
  end

(** Build a [Cinv_equation.info] from [Cil_types.file]. *)
let make_info (procs:Cil_types.global list) : Cinv_equation.info =

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
	    let pfail = point_of_fail pstart in
	    if not (Hashhe.mem pointenv pstart) then
	      Hashhe.add pointenv pstart env
	    ;
	    if not (Hashhe.mem pointenv pfail) then
	      Hashhe.add pointenv pfail env
	    ;
	    List.iter (iter_stmt 
	      (begin fun s ->
		begin
		  let p = (point_of_stmt s) in
		  if not (Hashhe.mem pointenv p) then
		    Hashhe.add pointenv p env;
		  match s.skind with
		  | Cil_types.Return(Some _,_) ->
		    let ppost = (point_of_post s) in
		    Hashhe.add pointenv ppost env

		  | If _ ->
		      let p1 = (point_of_block_if s) in
		      let p2 = (point_of_block_else s) in
		      Hashhe.add pointenv p1 env;
		      Hashhe.add pointenv p2 env

		  | Loop _ ->
		    let pinv = (point_of_inv s) in
		    Hashhe.add pointenv pinv env		    
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
    (fgraph:Cinv_equation.graph)
    (bgraph:Cinv_equation.graph)
    :
    unit
    =
  (Celia_options.debug "Translate procedure %s " fd.svar.vname);
  let env = procinfo.Cinv_equation.penv in
  let bpoint = procinfo.Cinv_equation.pstart in
  let stmts = fd.sbody.bstmts in
  (* let stmts = fd.sallstmts in *)
  ignore begin
    List.fold_left 
      (begin fun bp st ->
	statement env procinfo bp st fgraph bgraph
       end)
      bpoint
      stmts
  end


(*****************************************************************************)
(* Main function *)
(*****************************************************************************)
let make f =
  (** Colect informations on types and export them *)
  let _ = (List.iter filter_ltype f.globals) in

  (** Suppose that !FCFile.computeCFG has been called *)
  let procedures = (List.filter filter_fun f.globals) in

  (** Create global info on procedures *)
  let info = make_info procedures in

  (** Initialize control flow graph for the program using global info *)
  let fgraph = Cinv_equation.create 3 info in
  let bgraph = Cinv_equation.create 3 info in

  (** Create file exporting the fields used by list-like types *)
  Cil2apron.export_ltype "pan/pan.fld";

  (** Fill the CFG for each procedure *)
  List.iter
    (begin fun gdec ->
      match gdec with
	  GFun (fd,_) ->
	    (try
	      let finfo = Hashhe.find info.Cinv_equation.procinfo fd.svar.vname in
	      make_block finfo fd fgraph bgraph
	    with Not_found ->
	      Celia_options.warning "Procedure %s not found" fd.svar.vname)
	| _ -> assert false
    end)
    procedures;
  
  (fgraph, bgraph)


(*
Local Variables:
compile-command: "LC_ALL=C make -C "
End:
*)
