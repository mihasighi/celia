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
module FCPrinter=Ast_printer
module FCParameters=Parameters

open Db_types
open Visitor

open Common

(*****************************************************************************)
(* Rewrite to SPL                                                            *)
(*****************************************************************************)

(* Output channel *)
let spl_out = ref stdout

(* Stores generated during the visit *)
(*    - input parameters and local variables *)
let predef_vars_real = ref ["_data"; "_free"; "_len"; 
			   "_new"; "_next"; "_null"];;

let predef_vars_int  = ref ["_l"; "_k"; "S"];;

let local_vars_real = ref [];;

let local_vars_int  = ref [];;

let decl_vars_real lv =
  let decl_var v = Printf.fprintf !spl_out ", %s:real " v in
    List.iter decl_var lv

let decl_vars_int lv =
  let decl_var v = Printf.fprintf !spl_out ", %s:int " v in
    List.iter decl_var lv

let init_ptr_code lv =
  let init_ptr v = 
    Printf.fprintf !spl_out "%s = _null; " v
  in
  List.iter init_ptr lv

(*****************************************************************************)
(* Rewrite variables                                                         *)
(*****************************************************************************)

(* Module for visitor *)
class rewrite_vars =
  object(_self)
    inherit Visitor.generic_frama_c_visitor (Project.current())
	(Cil.inplace_visit())
	

  method vglob_aux = function
    | GFun (fundec,_) ->
      begin
	if (String.compare fundec.svar.vname (FCParameters.MainFunction.get())) == 0 then
	  DoChildren
	else
	  SkipChildren
      end

    | GType _ | GAnnot _ | GVar _ | GVarDecl _
    | GCompTag _ | GCompTagDecl _ | GEnumTagDecl _
    | GEnumTag _ | GAsm _ | GPragma _ | GText _ -> SkipChildren 


  method vvdec vinfo = 
    if vinfo.vglob then
      SkipChildren
    else (* only variables from the input function are visited here *)
      match vinfo.vtype with
	| TInt _ -> 
	    local_vars_int := List.append
	      !local_vars_int [vinfo.vname];
	    SkipChildren
	    
	| TNamed (tinfo,_) ->
	    if (String.compare tinfo.tname "intlist") == 0 then
	      local_vars_real := List.append
		!local_vars_real [vinfo.vname];
	    SkipChildren
	    
	| TFun _ -> (* to access the main function and its variables *)
	    DoChildren

	| TFloat _ | TVoid _ | TBuiltin_va_list _
	| TPtr _ | TArray _ | TComp _
	| TEnum _ -> 
	    SkipChildren
    end

(* Function encapsulating the visitor *)
let rewrite_vars f =
  let vis = new rewrite_vars in Visitor.visitFramacFile vis f;
  Globals.Functions.iter (fun kf -> ignore (Kernel_function.get_spec kf))


(*****************************************************************************)
(* Rewrite statements                                                        *)
(*****************************************************************************)

(*    - local expressions *)
let local_exp = ref [];;
let local_stmt = ref [];;
let top_exp () =
  match !local_exp with
    [] -> ""
  | e::_ -> e

let top_exp_stmt () =
  match !local_exp, !local_stmt with
    [], [] -> ""
  | [], s::_ -> s
  | e::[], [] -> e
  | e::[], s::_ -> e^s
  | e::_, _ -> e
  
let pop_exp () =
  match !local_exp with
    [] -> ()
  | _::l -> local_exp:=l

let pop_stmt () =
  match !local_stmt with
    [] -> ()
  | _::l -> local_stmt:=l

let pop_stmt_all () =
  List.iter (Printf.fprintf !spl_out "%s") !local_stmt

let pop_exp_stmt () = 
  match !local_exp,!local_stmt with
    [], [] -> ()
  | [], _::l -> local_stmt:=l
  | _::[], [] -> local_exp:=[]
  | _::[], _::l -> local_exp:=[]; local_stmt:=l
  | _::l, _ -> local_exp:=l

let push_exp e = 
  local_exp:=e::(!local_exp)

let push_stmt s = 
  local_stmt:=s::(!local_stmt)

let add_exp e = 
  match !local_exp with
      [] -> local_exp:=[e]
    | o::l -> local_exp:=(e^o)::l 

let local_cast = ref false;;
let local_loop = ref false;;


(* Module for visitor *)
class rewrite_main =
  object(_self)
    inherit Visitor.generic_frama_c_visitor (Project.current())
	(Cil.inplace_visit())
	
  method vglob_aux = function
    | GFun (fundec,_) ->
      begin
	if (String.compare fundec.svar.vname (FCParameters.MainFunction.get())) == 0 then
	  DoChildren
	else
	  SkipChildren
      end

    | GType _ | GAnnot _ | GVar _ | GVarDecl _
    | GCompTag _ | GCompTagDecl _ | GEnumTagDecl _
    | GEnumTag _ | GAsm _ | GPragma _ | GText _ -> SkipChildren 

  (**** BLOCK is only visit children *****)
      
  (**** STMT *****)
  method vinst i =
    match i with 
     | Set _ -> 
	 (push_exp "= ");
	 DoChildren (* TODO *)
     | _ -> 
	 Printf.fprintf !spl_out " /* instr */ ";
	 SkipChildren

  method vstmt s = 
    match s.skind with 
     | Instr _ ->
	 (push_stmt " ;\n"); 
	 DoChildren
     | If _ -> 
	 if not(!local_loop) then
	   begin
	     Printf.fprintf !spl_out "\nif ";
	     (push_stmt " endif;\n");
	     (push_stmt " \n else \n");
	     (push_stmt " then\n");
	     DoChildren
	   end
	 else
	   begin
	     local_loop:=false;
	     Printf.fprintf !spl_out "\nwhile ";
	     (push_stmt " done;\n");
	     (push_stmt " \n /* else */ \n");
	     (push_stmt " do\n");
	     DoChildren
	   end

     | Loop _ ->
	 local_loop:=true;
	 DoChildren 
    
     | Block _ ->
	 Printf.fprintf !spl_out "\nbegin ";
	 (push_stmt " end;\n");
	 DoChildren

     | _ ->
	 Printf.fprintf !spl_out "\n/* stmt */\n";
	 SkipChildren (* TODO *)

  
  (**** LVAL ****)
  method vlval lv =
    match lv with
      | (Var(vi), NoOffset) -> 
	  Printf.fprintf !spl_out "%s %s" vi.vname (top_exp ());
	  (pop_exp ());
	  SkipChildren

      | (Var(vi), Field(fi,NoOffset)) ->
	  Printf.fprintf !spl_out "%s %s" vi.vname (top_exp ());
	  (pop_exp ());
	  (add_exp (" / _"^fi.fname));
	  SkipChildren

      | (Mem(e),Field(fi,NoOffset)) ->
	   begin
	     match e.enode with
	       | Lval(Var vi,NoOffset) ->
		   Printf.fprintf !spl_out "%s %s" vi.vname (top_exp ());
		   (pop_exp ());
		   (add_exp (" / _"^fi.fname));
		   SkipChildren
	       | _ -> 
		   Printf.fprintf !spl_out "/* error */ %s " (top_exp ());
		   (pop_exp ());
		   SkipChildren
	   end
    
      | _ ->
	  Printf.fprintf !spl_out "/* error */";
	  (pop_exp ());
	  SkipChildren

  (**** EXPR ****)
  method vexpr e = 
    match e.enode with
      | Const(CInt64(v,_,_)) -> 
	  if !local_cast then 
	    begin
	      local_cast:=false;
	      Printf.fprintf !spl_out "_null %s " (top_exp_stmt ())
	    end
	  else 
	    Printf.fprintf !spl_out "%s %s" (Int64.to_string v) (top_exp_stmt ())
	      ;
	  (pop_exp_stmt ());
	  SkipChildren

      | Lval(Var vinfo, NoOffset) ->  
	  Printf.fprintf !spl_out "%s %s" vinfo.vname (top_exp_stmt ());
	  (pop_exp_stmt ());
	  SkipChildren

      | Lval(Mem(e), Field (finfo,NoOffset)) -> 
	   begin
	     match e.enode with
	       | Lval(Var vi,NoOffset) ->
		   Printf.fprintf !spl_out "(%s * _%s) %s" vi.vname 
		     finfo.fname (top_exp_stmt ());
		   (pop_exp_stmt ());
		   SkipChildren
	       | _ -> 
		   Printf.fprintf !spl_out "/* error */ %s " (top_exp_stmt ());
		   (pop_exp_stmt ());
		   SkipChildren
	   end


      | UnOp(Neg, _, _) ->  
	  Printf.fprintf !spl_out " ( - ( ";
	  (add_exp ") )");
	  DoChildren

      | BinOp(PlusA, _, _, _) ->  
	  (push_exp " ");   
	  (push_exp " + ");
	  DoChildren
      | BinOp(MinusA, _, _, _) ->  
	  (push_exp " ");   
	  (push_exp " - ");
	  DoChildren
      | BinOp(Mult, _, _, _) ->  
	  (push_exp " * ");
	  DoChildren
      | BinOp(Div, _, _, _) ->  
	  (push_exp " ");   
	  (push_exp " / ");
	  DoChildren

      | BinOp(Lt, _, _, _) ->   
	  (push_exp " ");  
	  (push_exp " < ");
	  DoChildren
      | BinOp(Gt, _, _, _) ->  
	  (push_exp " ");   
	  (push_exp " > ");
	  DoChildren
      | BinOp(Le, _, _, _) ->  
	  (push_exp " ");   
	  (push_exp " <= ");
	  DoChildren
      | BinOp(Ge, _, _, _) ->   
	  (push_exp " ");  
	  (push_exp " >= ");
	  DoChildren
      | BinOp(Ne, _, _, _) ->  
	  (push_exp " "); 
	  (push_exp " != ");
	  DoChildren

      | BinOp(LAnd, _, _, _) ->  
	  (push_exp " ");   
	  (push_exp " and ");
	  DoChildren
      | BinOp(LOr, _, _, _) ->  
	  (push_exp " ");   
	  (push_exp " or ");
	  DoChildren
	    

      | CastE(TPtr _,_) -> 
	  local_cast:=true;
	  DoChildren

      | CastE(TInt _, _) | CastE(TFloat _,_) -> 
	  DoChildren
	    
      | _ ->  
	  Printf.fprintf !spl_out "/* expr */ %s"  (top_exp_stmt ());
	  (pop_exp_stmt ());
	  SkipChildren
	    
  end

(* Function encapsulating the visitor *)
let rewrite_main f =
  let vis = new rewrite_main in Visitor.visitFramacFile vis f;
  Globals.Functions.iter (fun kf -> ignore (Kernel_function.get_spec kf));
  (pop_stmt_all ())


(*****************************************************************************)
(* Rewrite to SPL extended for ptr                                           *)
(*****************************************************************************)

(* Main *)
let generate file = 
  spl_out := open_out "pan.spl";
  rewrite_vars file;
  Printf.fprintf !spl_out "var %s:real" (List.hd !predef_vars_real);
  (decl_vars_real (List.tl !predef_vars_real));
  (decl_vars_real !local_vars_real);
  (decl_vars_int !predef_vars_int);
  (decl_vars_int !local_vars_int);
  Printf.fprintf !spl_out "; \nbegin \n ";
  (init_ptr_code !local_vars_real);
  Printf.fprintf !spl_out "\n ";
  rewrite_main file;
  Printf.fprintf !spl_out "\nend ";
  (* Close the channel *)
  close_out !spl_out

(*
Local Variables:
compile-command: "LC_ALL=C make -C ../.. -j bin/toplevel.byte"
End:
*)


