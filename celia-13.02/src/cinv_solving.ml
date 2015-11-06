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


(* This file is part of the Interproc analyzer, released under GPL license.
   Please read the COPYING file packaged in the distribution.

   Copyright (C) Mathias Argoud, Gaël Lalire, Bertrand Jeannet 2007.
   Copyright (C) Mihaela Sighireanu 2010.
*)
(** Solving the equations *)

open Format
open Cinv_options
open Cinv_equation


(*  ********************************************************************* *)
(** {1 Instantiated module and options} *)
(*  ********************************************************************* *)

(*  ===================================================================== *)
(** {1 Options} *)
(*  ===================================================================== *)

(* See @module{cinv_options} *)

(*  ===================================================================== *)
(** {2 Functions} *)
(*  ===================================================================== *)

(** Build a fixpoint manager (for module [Fixpoint]) given:
  - an equation graph (forward or backward)
  - optionally, the result of a previous, dual analysis
  - a function [apply graph output manager hyperedge tabstract]
  - a function [abstract_init]
  - an APRON manager;
  - a debug level
*)
let make_fpmanager
  (graph: Cinv_equation.graph)
  ~(output : (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output option)
  (apply :
     Cinv_equation.graph ->
   output:(Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output option ->
   'a Apron.Manager.t -> int -> 'a Apron.Abstract1.t array ->
   unit * 'a Apron.Abstract1.t)
  (abstract_init : point -> 'a Apron.Abstract1.t)
  (man:'abstract Apron.Manager.t)
  ~(debug:int)
  :
  (point, int, 'a Apron.Abstract1.t, unit) Fixpoint.manager
  =
  let info = PSHGraph.info graph in
  {
    (* Lattice operation *)
    Fixpoint.bottom = begin fun vtx ->
      (try
	 Apron.Abstract1.bottom man (Hashhe.find info.Cinv_equation.pointenv vtx)
       with Not_found ->
	 begin
	   Cinv_options.feedback 0 "Vertex not found (%a)\n" print_point vtx;
	   failwith "Vertex not found (Fixpoint.bottom)\n"
	 end
      )
    end;
    Fixpoint.canonical = begin fun vtx abs -> ()
      (* Apron.Abstract1.canonicalize man abs *)
    end;
    Fixpoint.is_bottom = begin fun vtx abs ->
      Apron.Abstract1.is_bottom man abs
    end;
    Fixpoint.is_leq = begin fun vtx abs1 abs2 ->
      Apron.Abstract1.is_leq man abs1 abs2
    end;
    Fixpoint.join = begin fun vtx abs1 abs2 ->
      Apron.Abstract1.join man abs1 abs2
    end;
    Fixpoint.join_list = begin fun vtx labs ->
      Apron.Abstract1.join_array man (Array.of_list labs)
    end;
    Fixpoint.widening = begin fun vtx abs1 abs2 ->
      Apron.Abstract1.widening man abs1 abs2
    end;
    (* Initialisation of equations *)
    Fixpoint.abstract_init = abstract_init;
    Fixpoint.arc_init = begin fun hedge -> () end;
    (* Interpreting hyperedges *)
    Fixpoint.apply = begin fun hedge tx ->
      apply graph ~output man hedge tx
    end;
    (* Printing functions *)
    Fixpoint.print_vertex=print_point;
    Fixpoint.print_hedge=pp_print_int;
    Fixpoint.print_abstract = Apron.Abstract1.print;
    Fixpoint.print_arc = begin fun fmt () -> pp_print_string fmt "()" end;
    (* Fixpoint Options *)
    Fixpoint.accumulate = true;
    (* Widening Options *)
    Fixpoint.widening_start = !widening_start;
    Fixpoint.widening_descend = !widening_descend;
    (* Printing Options *)
    Fixpoint.print_fmt = Format.std_formatter;
    Fixpoint.print_analysis=debug>=1;
    Fixpoint.print_component=debug>=2;
    Fixpoint.print_step=debug>=3;
    Fixpoint.print_state=debug>=4;
    Fixpoint.print_postpre=debug>=5;
    Fixpoint.print_workingsets=debug>=6;
    (* DOT Options *)
    Fixpoint.dot_fmt = Some Format.std_formatter;
    Fixpoint.dot_vertex=print_point;
    Fixpoint.dot_hedge=pp_print_int;
    Fixpoint.dot_attrvertex=print_point;
    Fixpoint.dot_attrhedge=(fun fmt hedge -> ());
  }

(** Make an output graph filled with bottom abstract avlues *)
let make_emptyoutput
  (graph : (point,int,'a,'b,'c) PSHGraph.t)
  (manager : 'abstract Apron.Manager.t)
  :
  (point, int, 'abstract Apron.Abstract1.t,unit) Fixpoint.output
  =
  let info = PSHGraph.info graph in
  PSHGraph.map graph
    (begin fun vertex attr ->
      begin
	try
	  Apron.Abstract1.bottom manager (Hashhe.find info.Cinv_equation.pointenv vertex)
	with Not_found ->
	  begin
	    Cinv_options.feedback 0 "Vertex not found";
	    failwith ""
	  end
      end
    end)
    (begin fun hedge arc -> () end)
    (begin fun info ->
      {
	Fixpoint.time = 0.0;
	Fixpoint.iterations = 0;
	Fixpoint.descendings = 0;
	Fixpoint.stable = false;
      }
    end)

(** *)

let environment_of_tvar
  (typ_of_var : Apron.Var.t -> Apron.Environment.typvar)
  (tvar : Apron.Var.t array)
  :
  Apron.Environment.t
  =
  let (lint,lreal) =
    Array.fold_right
      (begin fun var (lint,lreal) ->
	begin 
	Cinv_options.feedback 2  
	  "environment_of_var: var %a\n" 
	  Apron.Var.print var;
	match typ_of_var var with
	  | Apron.Environment.INT -> (var::lint,lreal)
	  | Apron.Environment.REAL -> (lint,var::lreal)
	end
       end)
      tvar
      ([],[])
  in
  let tint = Array.of_list lint and treal = Array.of_list lreal in
  Apron.Environment.make tint treal

let environment_of_texp
  (typ_of_var : Apron.Var.t -> Apron.Environment.typvar)
  (texp : Apron.Texpr1.t array)
  :
  Apron.Environment.t
  =
  let (lint,lreal) =
    Array.fold_right
      (begin fun exp (lint,lreal) ->
	begin match (Apron.Texpr1.to_expr exp) with
	  | Apron.Texpr1.Var var ->
	    begin match typ_of_var var with
	      | Apron.Environment.INT -> (var::lint,lreal)
	      | Apron.Environment.REAL -> (lint,var::lreal)
	    end
	  | _ -> (lint,lreal)
	end
      end)
      texp
      ([],[])
  in
  let tint = Array.of_list lint and treal = Array.of_list lreal in
  Apron.Environment.make tint treal

(*  ********************************************************************** *)
(** {2 Forward semantics} *)
(*  ********************************************************************** *)

module Forward = struct

  (*  ===================================================================== *)
  (** {3 Transfer function} *)
  (*  ===================================================================== *)

  let apply_tassign
    (manager:'a Apron.Manager.t)
    (abstract:'a Apron.Abstract1.t)
    (var: Apron.Var.t)
    (expr:Apron.Texpr1.t)
    (dest:'a Apron.Abstract1.t option)
    =
    let res =
      Apron.Abstract1.assign_texpr
	manager abstract
	var expr dest
    in
    res

  let apply_condition
    (manager:'a Apron.Manager.t)
    (abstract:'a Apron.Abstract1.t)
    (expr:Apron.Tcons1.t Boolexpr.t)
    (dest:'a Apron.Abstract1.t option)
    :
    'a Apron.Abstract1.t
    =
    let rec apply_condition
      (abstract:'a Apron.Abstract1.t)
      (expr:Apron.Tcons1.t Boolexpr.t)
      :
      'a Apron.Abstract1.t
      =
      let res =
	match expr with
	| Boolexpr.CST x ->
	    if x then
	      abstract
	    else
	      Apron.Abstract1.bottom manager (Apron.Abstract1.env abstract)
	| Boolexpr.RANDOM ->
	    abstract
	| Boolexpr.CONDITION tcons ->
	    let env = Apron.Tcons1.get_env tcons in
	    let array = Apron.Tcons1.array_make env 1 in
	    Apron.Tcons1.array_set array 0 tcons;
	    Apron.Abstract1.meet_tcons_array manager abstract array
	| Boolexpr.OR(e1,e2) ->
	    Apron.Abstract1.join manager
	    (apply_condition abstract e1)
	    (apply_condition abstract e2)
	| Boolexpr.AND(e1,e2) ->
	    Apron.Abstract1.meet manager
	    (apply_condition abstract e1)
	    (apply_condition abstract e2)
      in
	res
    in
    let abstract =
      match dest with
      | None -> abstract
      | Some dest ->
	  Apron.Abstract1.meet manager abstract dest
    in
    apply_condition abstract expr

(** We compute the relation corresponding to the input heap. **)
  let apply_call
    (manager:'a Apron.Manager.t)
    (abstract:'a Apron.Abstract1.t)
    (calleeinfo:Cinv_equation.procinfo)
    (inargs:Apron.Var.t array)
    (dest:'a Apron.Abstract1.t option)
    =
    (* current environment *)
    let env = Apron.Abstract1.env abstract in
    (* 1. Remove all non-argument variables from the current abstract value
          and change the parameters according to the function specification
    *)
    let aenv = 
	environment_of_tvar (Apron.Environment.typ_of_var env) inargs
    in
    let falgo = Cinv_options.get_fun_spec calleeinfo.pname in
    let abstract1 =
      begin
	Cinv_options.feedback 2  
	  "apply_call: remove non-argument (on %a)\n" 
	  Apron.Abstract1.print abstract;
	Apron.Abstract1.approximate manager abstract falgo; 
	Apron.Abstract1.change_environment manager 
	  abstract aenv false (* No DUP *)
      end
    in
    (* 2. Rename actuals by formal input old variables *)
    let abstract2 =
      begin
	Cinv_options.feedback 2 
	  "apply_call: rename actuals by formals (on %a)\n"
	  Apron.Abstract1.print abstract1;
	Apron.Abstract1.rename_array 
	  manager abstract1
	  inargs calleeinfo.Cinv_equation.pinput_old
      end
    in 
    (* 3. Duplicate input args *)
    let oenv = 
	environment_of_tvar 
	  (Apron.Environment.typ_of_var calleeinfo.Cinv_equation.penv) 
	  calleeinfo.Cinv_equation.pinput_old
    in
    let ienv = 
      environment_of_tvar 
	(Apron.Environment.typ_of_var calleeinfo.Cinv_equation.penv) 
	calleeinfo.Cinv_equation.pinput
    in
    let ioenv =
      Apron.Environment.lce oenv ienv
    in
    let abstract3 =
      begin
	Cinv_options.feedback 2 
	  "apply_call: duplicate formals (on %a)\n"
	  Apron.Abstract1.print abstract2;
      Apron.Abstract1.change_environment manager 
	abstract2 ioenv true (* DUP *)
      end
    in
    (* 4. Embed in the callee environment *)
    let abstract4 = 
      begin
	Cinv_options.feedback 2 
	  "apply_call: embed in callee (on %a)\n"
	  Apron.Abstract1.print abstract3;
      Apron.Abstract1.change_environment 
	manager abstract3 calleeinfo.Cinv_equation.penv false (* No DUP *)
      end
    in
    (* 5. Assign pinput_ini to respect the passage by value semantics *)
    let var_to_texpr inivars =
      Array.map 
	(fun v -> 
	  let vini_string = (Apron.Var.to_string v) in
	  let len_in = (String.length vini_string) - 7 (* size of _%ini...%_ *) in
	  let vin_string = (String.sub vini_string 5 len_in) in
	  (* DEBUG let _ = printf "\nvin = %s from %s" vin_string vini_string in *)
	  let vin = (Apron.Var.of_string vin_string) in
	  (Apron.Texpr1.var calleeinfo.Cinv_equation.penv vin)
	)
	inivars
    in
    let abstract5 = 
      begin
	Cinv_options.feedback 2 "apply_call: assign pinput_ini (on %a)\n"
	  Apron.Abstract1.print abstract4;
	Apron.Abstract1.assign_texpr_array 
	  manager abstract4	    
	  calleeinfo.Cinv_equation.pinput_ini 
	  (var_to_texpr calleeinfo.Cinv_equation.pinput_ini) 
	  None
      end
    in
    (* From now on work with side effects *)
    (* 8. We possibly intersect with the result of a previous analysis *)
    begin match dest with
    | None -> ()
    | Some dest ->
        Apron.Abstract1.meet_with manager abstract5 dest
    end;
    abstract5

  (* Computes the relation for the entry point *)
  let apply_call_main
    (manager:'a Apron.Manager.t)
    (maininfo:Cinv_equation.procinfo)
    :
    'a Apron.Abstract1.t
    =
    (* 1. Computes the value (over pinput) corresponding to prequires *)
    let env_in =
      environment_of_tvar 
	(Apron.Environment.typ_of_var maininfo.Cinv_equation.penv) 
	maininfo.Cinv_equation.pinput 
    in
    let falgo = Cinv_options.get_fun_spec maininfo.pname in
    let abstract1 =
      begin
	Cinv_options.feedback 2 
	  "apply_call_main: build object from requires (fspec=%d)\n" falgo;
	Apron.Abstract1.approximate manager 
	  (Apron.Abstract1.top manager env_in) falgo;
	Apron.Abstract1.of_tcons_array manager env_in 
	  maininfo.Cinv_equation.prequires
      end
    in
    (* 2. Duplicate pinput into pinput_old *)
    let env_oin =
      begin
	Cinv_options.feedback 2 
	  "apply_call_main: duplicate input params over the environment %a\n" 
	  (fun fmt e -> Apron.Environment.print fmt e) maininfo.Cinv_equation.penv;
	environment_of_tvar 
	  (Apron.Environment.typ_of_var maininfo.Cinv_equation.penv) 
	  maininfo.Cinv_equation.pinput_old
      end
    in
    let env_in_oin = 
      Apron.Environment.lce env_oin env_in in
    let abstract2 =
      begin
	Cinv_options.feedback 2 
	  "apply_call_main: duplicate input vars (on %a)\n"
	  Apron.Abstract1.print abstract1;
      Apron.Abstract1.change_environment manager 
	abstract1 env_in_oin true (* DUP *)
      end 
    in 
    (* 3. add to the environment the other variables (without dup) *)
    let abstract3 = 
      begin
	Cinv_options.feedback 2 
	  "apply_call_main: embed in callee (on %a)\n"
	  Apron.Abstract1.print abstract2;
	Apron.Abstract1.change_environment 
	  manager abstract2 maininfo.Cinv_equation.penv false (* No DUP *)
      end
    in
    (* 4. assign pinput_ini to respect the passage by value semantics *)
    let var_to_texpr inivars =
      Array.map 
	(fun v -> 
	  let vini_string = (Apron.Var.to_string v) in
	  let len_in = (String.length vini_string) - 7 (* size of _%ini...%_ *) in
	  let vin_string = (String.sub vini_string 5 len_in) in
	  (* DEBUG let _ = printf "\nvin = %s from %s" vin_string vini_string in *)
	  let vin = (Apron.Var.of_string vin_string) in
	  (Apron.Texpr1.var maininfo.Cinv_equation.penv vin)
	)
	inivars
    in
    begin
      Cinv_options.feedback 2 "apply_call_main: assign pinput_ini (on %a)\n"
	Apron.Abstract1.print abstract3;
      Apron.Abstract1.assign_texpr_array 
	manager abstract3	    
	maininfo.Cinv_equation.pinput_ini 
	(var_to_texpr maininfo.Cinv_equation.pinput_ini) 
	None
    end

  let apply_return
    (manager:'a Apron.Manager.t)
    (abscaller:'a Apron.Abstract1.t) (abscallee:'a Apron.Abstract1.t)
    (callerinfo:Cinv_equation.procinfo)
    (calleeinfo:Cinv_equation.procinfo)
    (inargs: Apron.Var.t array) (outargs:Apron.Var.t array)
    (dest:'a Apron.Abstract1.t option)
      =
    (* 1. We assign 0 to local vars and pinput *)
    let env = calleeinfo.Cinv_equation.penv in
    let vl,texprs =
      Array.fold_left 
	(fun (vv,ee) v ->
	    begin match (Apron.Environment.typ_of_var env v) with
	      | Apron.Environment.REAL ->
		(v::vv, (Apron.Texpr1.cst env (Apron.Coeff.s_of_float 0.0))::ee)
	      | _ -> (vv,ee)
	    end
	)
	([],[])
	calleeinfo.Cinv_equation.plocal
    in
    let abstract1 = 
      begin
	Cinv_options.feedback 2 
	  "apply_return: assign NULL to local ptr (on %a)\n"
	  Apron.Abstract1.print abscallee; 
	Apron.Abstract1.assign_texpr_array
	  manager abscallee
	  (Array.of_list vl) (Array.of_list texprs) None
      end
    in
    (* 2. We forget local (including pinput) variables in abscallee *)
    let env =
      Apron.Environment.remove (Apron.Abstract1.env abscallee)
	(calleeinfo.Cinv_equation.plocal)
    in
    let abstract2 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: forget locals (on %a)\n"
	  Apron.Abstract1.print abstract1;
	Apron.Abstract1.change_environment manager
	  abstract1 env false (* No DUP *)
      end
    in
    (* 3. We rename in modified abscallee
       - formal vars of pinput_old parameters by actual inparameters
       - formal out and in parameters by special names (to avoid name conflicts)
    *)
    let abstract3 = 
      begin
	Cinv_options.feedback 2 
	  "apply_return: rename input_old by actuals (on %a)\n"
	  Apron.Abstract1.print abstract2;
	Apron.Abstract1.rename_array
	  manager abstract2
	  (Array.append 
	     calleeinfo.Cinv_equation.pinput_old 
	     (Array.append 
		calleeinfo.Cinv_equation.pinput_ini
		calleeinfo.Cinv_equation.poutput))
	  (Array.append 
	     inargs 
	     (Array.append
		calleeinfo.Cinv_equation.pinput_tmp
		calleeinfo.Cinv_equation.poutput_tmp))
      end
    in
    (* 4. We unify the renamed callee value and the caller value 
     *    on vars caller,callee.pinput_ini_tmp,callee.poutput_tmp *)
    (*    Since unification is done by meet, signal to meet that
     *    approximate meet is needed. Flag reset by meet. *)
    let abstract4 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: unify callee with caller (on caller %a, callee %a)\n"
	  Apron.Abstract1.print abscaller
	  Apron.Abstract1.print abstract3;  
	Apron.Abstract1.approximate manager abstract3 (-1);
	Apron.Abstract1.unify manager abstract3 abscaller
      end
    in
    (* 5. We substitute ptr(inargs) with corresponding pinput_tmp.
          The substitution function in the Apron.Abstract1 interface is
          used wtih the flag -1 (reset by substitute_texpr_array).
     *)
    let env4 = Apron.Abstract1.env abstract4 in
    let ptr_inargs = 
      (Array.fold_right
	 (fun v l ->
	   match Apron.Environment.typ_of_var env4 v with
	       Apron.Environment.INT -> l
	     | Apron.Environment.REAL -> v::l
	 )
	 inargs [])
    in
    let texpr_ini = (* may be other values than pointers *)
      (Array.map
	 (fun v -> Apron.Texpr1.var env4 v)
	 calleeinfo.Cinv_equation.pinput_tmp)
    in
    let abstract5 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: substitute actuals by pinput_ini (on %a)\n"
	  Apron.Abstract1.print abstract4;
	Apron.Abstract1.approximate manager abstract4 (-1); 
	Apron.Abstract1.substitute_texpr_array manager abstract4
	  (Array.of_list ptr_inargs) texpr_ini None
      end
    in 
    (* 6. We remove pinput_tmp *)
    let env6 = 
      Apron.Environment.remove env4
	calleeinfo.Cinv_equation.pinput_tmp
    in
    let abstract6 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: remove input_tmp (on %a)\n"
	  Apron.Abstract1.print abstract5;
	Apron.Abstract1.change_environment manager 
	  abstract5 env6 false (* No DUP *)
      end
    in
    (* 7. We assign the actual out parameters.
     *    This operation fails if an actual pointer parameter is not NULL.
     *)
    let tlinexpr =
      Array.map
	(begin fun var ->
	  let e = Apron.Linexpr1.make ~sparse:true env6 in
	  Apron.Linexpr1.set_coeff e var (Apron.Coeff.s_of_int 1);
	  e
	end)
	calleeinfo.Cinv_equation.poutput_tmp
    in
    let abstract7 = 
      begin
	Cinv_options.feedback 2 
	  "apply_return: assign outputs (on %a)\n"
	  Apron.Abstract1.print abstract6;
	(if tlinexpr<>[||] then
	    (Apron.Abstract1.assign_linexpr_array
	       manager abstract6
	       outargs tlinexpr None)
	 else
	    abstract6)
      end
    in
    (* 8. We remove the introduced temporary variables *)
    let env8 = 
      Apron.Environment.remove env6
	(calleeinfo.Cinv_equation.poutput_tmp)
    in
    let falgo = Cinv_options.get_fun_spec callerinfo.pname in
    begin
      Cinv_options.feedback 2 
	"apply_return: remove temporary outputs (on %a)\n"
	Apron.Abstract1.print abstract7;
      Apron.Abstract1.approximate manager abstract7 falgo;
      Apron.Abstract1.change_environment_with
	manager abstract7 env8 false (* No DUP *)
      ;
      (* 10. We possibly intersect with the result of a previous analysis *)
      begin match dest with
	| None -> ()
	| Some dest ->
	  Apron.Abstract1.meet_with manager abstract7 dest
      end;
      abstract7
    end

  (** Main transfer function *)
  let apply
    (graph:Cinv_equation.graph)
    ~(output : (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output option)
    (manager:'a Apron.Manager.t)
    (hedge:int)
    (tabs:'a Apron.Abstract1.t array)
    :
    unit * 'a Apron.Abstract1.t
    =
    let transfer = PSHGraph.attrhedge graph hedge in
    let abs = tabs.(0) in
    let dest = match output with
      | None -> None
      | Some(output) ->
          let tdest = PSHGraph.succvertex graph hedge in
          assert(Array.length tdest = 1);
          let dest = PSHGraph.attrvertex output tdest.(0) in
          Some dest
    in
    let res =
      match transfer with
      | Cinv_equation.Tassign(var,expr) ->
	begin
	  Cinv_options.feedback 1 "(h=%d) apply_tassign %a := %a (on %a)\n"
	    hedge
	    Apron.Var.print var Apron.Texpr1.print expr
	    Apron.Abstract1.print abs; 
	  apply_tassign manager abs var expr dest
	end
      | Cinv_equation.Lassign _ ->
	  failwith ""
      | Cinv_equation.Condition cond ->
	begin
	  Cinv_options.feedback 1 "(h=%d) apply_tcond (%a) /\ (on %a)\n"
	    hedge 
	    (Boolexpr.print Apron.Tcons1.print) cond
	    Apron.Abstract1.print abs;
	  apply_condition manager abs cond dest
	end
      | Cinv_equation.Call(callerinfo,calleeinfo,tin,tout) ->
	begin
	  Cinv_options.feedback 1 "(h=%d) apply_call %s (on %a)\n"	    
	    hedge 
	    calleeinfo.pname
	    Apron.Abstract1.print abs;
	  apply_call manager abs calleeinfo tin dest
	end
      | Cinv_equation.SCall(callerinfo,calleeinfo,tin,tout) ->
	  failwith ""
      | Cinv_equation.SReturn(callerinfo,calleeinfo,tin,tout) ->
	  failwith ""
      | Cinv_equation.Return(callerinfo,calleeinfo,tin,tout) ->
	begin
	  Cinv_options.feedback 1 
	    "(h=%d) apply_return to %s from %s (on %a) ...\n"
	    hedge
	    callerinfo.pname
	    calleeinfo.pname
	    Apron.Abstract1.print tabs.(1)
	  ;
	  (* DEBUG *)
	  begin
	    let hout = open_out "hedge.txt" in
	    output_byte hout hedge;
	    close_out hout
	  end;
	  apply_return manager abs tabs.(1) callerinfo calleeinfo tin tout dest
	end
    in
    ((),res)

  (*  ===================================================================== *)
  (** {3 Compute (post)fixpoint} *)
  (*  ===================================================================== *)

  let compute
    (graph:Cinv_equation.graph)
    ~(output : (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output option)
    (manager:'a Apron.Manager.t)
    ~(debug:int)
    :
    (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output
    =
    let info = PSHGraph.info graph in
    let maininfo = 
      (try
	 Hashhe.find info.Cinv_equation.procinfo !main_entry
       with Not_found ->
         begin
	   Cinv_options.feedback 0 "Function %s not found, analyze main function." !main_entry;
	   try
	     Hashhe.find info.Cinv_equation.procinfo "main"
	   with Not_found ->
	     begin
	       Cinv_options.feedback 0 "Function `main' not found, stop.";
	       failwith ""
	     end
	 end
      )
    in
    let start = maininfo.Cinv_equation.pstart in
    let sstart =
      begin match output with
	| None ->
	  begin
	    if 0 <> (Apron.Tcons1.array_length maininfo.Cinv_equation.prequires) 
	    then
              PSette.singleton Cinv_equation.compare_point start 
	    else
	      PSette.empty Cinv_equation.compare_point
	  end
	| Some output ->
          let abstract = PSHGraph.attrvertex output start in
          if Apron.Abstract1.is_bottom manager abstract then
            PSette.empty Cinv_equation.compare_point
          else
            (PSette.singleton Cinv_equation.compare_point start)
      end
    in
    if PSette.is_empty sstart then begin
      make_emptyoutput graph manager
    end
    else begin
      let abstract_init = begin fun vertex ->
	if Cinv_equation.compare_point vertex start = 0 then
	  apply_call_main manager maininfo
	else
	  begin
	    try
	      Apron.Abstract1.top manager (Hashhe.find info.Cinv_equation.pointenv vertex)
	    with Not_found ->
	      begin
		Cinv_options.feedback 0 "Vertex not found (top)";
		failwith ""
	      end
	  end
      end
      in
      let fpmanager =
	make_fpmanager graph ~output 
	  apply abstract_init
	  manager ~debug
      in
      let fp =
	if !iteration_guided then
	  Fixpoint.analysis_guided
	    fpmanager graph sstart	
	    (fun filter  ->
	      Fixpoint.make_strategy_default
		~vertex_dummy:Cinv_equation.vertex_dummy
		~hedge_dummy:Cinv_equation.hedge_dummy
		~priority:(PSHGraph.Filter filter)
		graph sstart)
	else
	  Fixpoint.analysis_std
	    fpmanager graph sstart
	    (Fixpoint.make_strategy_default
	       ~vertex_dummy:Cinv_equation.vertex_dummy
	       ~hedge_dummy:Cinv_equation.hedge_dummy
	       graph sstart)
      in
      Cinv_options.feedback 0 "Analysis stats: ";
      Fixpoint.print_stat Format.std_formatter (PSHGraph.info fp);
      Format.fprintf Format.std_formatter "@.";
      fp
    end
end


(*  ********************************************************************** *)
(** {2 Backward semantics} *)
(*  ********************************************************************** *)

module Backward = struct

  (*  ===================================================================== *)
  (** {3 Transfer function} *)
  (*  ===================================================================== *)

  let apply_tassign
    (manager:'a Apron.Manager.t)
    (abstract:'a Apron.Abstract1.t)
    (var: Apron.Var.t)
    (expr:Apron.Texpr1.t)
    (dest:'a Apron.Abstract1.t option)
    =
    let res =
      Apron.Abstract1.substitute_texpr
	manager abstract
	var expr dest
    in
    res

  let apply_condition = Forward.apply_condition

  (** We compute the relation corresponding to the input heap. **)
  let apply_call
    (manager:'a Apron.Manager.t)
    (abstract:'a Apron.Abstract1.t)
    (calleeinfo:Cinv_equation.procinfo)
    (inargs:Apron.Var.t array)
    (dest:'a Apron.Abstract1.t option)
    =
    (* current environment *)
    let env = Apron.Abstract1.env abstract in
    (* 1. Remove all non-argument variables from the current abstract value
          and change the parameters according to the function specification
    *)
    let aenv = 
	environment_of_tvar (Apron.Environment.typ_of_var env) inargs
    in
    let falgo = Cinv_options.get_fun_spec calleeinfo.pname in
    let abstract1 =
      begin
	Cinv_options.feedback 2  
	  "apply_call: remove non-argument (on %a)\n" 
	  Apron.Abstract1.print abstract;
	Apron.Abstract1.approximate manager abstract falgo; 
	Apron.Abstract1.change_environment manager 
	  abstract aenv false (* No DUP *)
      end
    in
    (* 2. Rename actuals by formal input old variables *)
    let abstract2 =
      begin
	Cinv_options.feedback 2 
	  "apply_call: rename actuals by formals (on %a)\n"
	  Apron.Abstract1.print abstract1;
	Apron.Abstract1.rename_array 
	  manager abstract1
	  inargs calleeinfo.Cinv_equation.pinput_old
      end
    in 
    (* 3. Duplicate input args *)
    let oenv = 
	environment_of_tvar 
	  (Apron.Environment.typ_of_var calleeinfo.Cinv_equation.penv) 
	  calleeinfo.Cinv_equation.pinput_old
    in
    let ienv = 
      environment_of_tvar 
	(Apron.Environment.typ_of_var calleeinfo.Cinv_equation.penv) 
	calleeinfo.Cinv_equation.pinput
    in
    let ioenv =
      Apron.Environment.lce oenv ienv
    in
    let abstract3 =
      begin
	Cinv_options.feedback 2 
	  "apply_call: duplicate formals (on %a)\n"
	  Apron.Abstract1.print abstract2;
      Apron.Abstract1.change_environment manager 
	abstract2 ioenv true (* DUP *)
      end
    in
    (* 4. Embed in the callee environment *)
    let abstract4 = 
      begin
	Cinv_options.feedback 2 
	  "apply_call: embed in callee (on %a)\n"
	  Apron.Abstract1.print abstract3;
      Apron.Abstract1.change_environment 
	manager abstract3 calleeinfo.Cinv_equation.penv false (* No DUP *)
      end
    in
    (* 5. Assign pinput_ini to respect the passage by value semantics *)
    let var_to_texpr inivars =
      Array.map 
	(fun v -> 
	  let vini_string = (Apron.Var.to_string v) in
	  let len_in = (String.length vini_string) - 7 (* size of _%ini...%_ *) in
	  let vin_string = (String.sub vini_string 5 len_in) in
	  (* DEBUG let _ = printf "\nvin = %s from %s" vin_string vini_string in *)
	  let vin = (Apron.Var.of_string vin_string) in
	  (Apron.Texpr1.var calleeinfo.Cinv_equation.penv vin)
	)
	inivars
    in
    let abstract5 = 
      begin
	Cinv_options.feedback 2 "apply_call: assign pinput_ini (on %a)\n"
	  Apron.Abstract1.print abstract4;
	Apron.Abstract1.assign_texpr_array 
	  manager abstract4	    
	  calleeinfo.Cinv_equation.pinput_ini 
	  (var_to_texpr calleeinfo.Cinv_equation.pinput_ini) 
	  None
      end
    in
    (* From now on work with side effects *)
    (* 8. We possibly intersect with the result of a previous analysis *)
    begin match dest with
    | None -> ()
    | Some dest ->
        Apron.Abstract1.meet_with manager abstract5 dest
    end;
    abstract5

  (* Computes the relation for the entry point *)
  let apply_call_main
    (manager:'a Apron.Manager.t)
    (maininfo:Cinv_equation.procinfo)
    :
    'a Apron.Abstract1.t
    =
    (* 1. Computes the value (over pinput+poutput) corresponding to pensures *)
    let env_inout =
      environment_of_tvar 
	(Apron.Environment.typ_of_var maininfo.Cinv_equation.penv) 
	(Array.append maininfo.Cinv_equation.pinput
	   maininfo.Cinv_equation.poutput) 
    in
    let falgo = Cinv_options.get_fun_spec maininfo.pname in
    let abstract1 =
      begin
	Cinv_options.feedback 2 
	  "apply_call_main: build object from ensures (fspec=%d)\n" falgo;
	Apron.Abstract1.approximate manager 
	  (Apron.Abstract1.top manager env_inout) falgo;
	Apron.Abstract1.of_tcons_array manager env_inout 
	  maininfo.Cinv_equation.pensures
      end
    in
    (* 2. Add to the environment the other variables (without dup) *)
    begin
      Cinv_options.feedback 2 
	"apply_call_main: embed in callee (on %a)\n"
	Apron.Abstract1.print abstract1;
      Apron.Abstract1.change_environment 
	manager abstract1 maininfo.Cinv_equation.penv false (* No DUP *)
    end

  let apply_return
    (manager:'a Apron.Manager.t)
    (abscaller:'a Apron.Abstract1.t) (abscallee:'a Apron.Abstract1.t)
    (callerinfo:Cinv_equation.procinfo)
    (calleeinfo:Cinv_equation.procinfo)
    (inargs: Apron.Var.t array) (outargs:Apron.Var.t array)
    (dest:'a Apron.Abstract1.t option)
      =
    (* 1. We assign 0 to local vars and pinput *)
    let env = calleeinfo.Cinv_equation.penv in
    let vl,texprs =
      Array.fold_left 
	(fun (vv,ee) v ->
	    begin match (Apron.Environment.typ_of_var env v) with
	      | Apron.Environment.REAL ->
		(v::vv, (Apron.Texpr1.cst env (Apron.Coeff.s_of_float 0.0))::ee)
	      | _ -> (vv,ee)
	    end
	)
	([],[])
	calleeinfo.Cinv_equation.plocal
    in
    let abstract1 = 
      begin
	Cinv_options.feedback 2 
	  "apply_return: assign NULL to local ptr (on %a)\n"
	  Apron.Abstract1.print abscallee; 
	Apron.Abstract1.assign_texpr_array
	  manager abscallee
	  (Array.of_list vl) (Array.of_list texprs) None
      end
    in
    (* 2. We forget local (including pinput) variables in abscallee *)
    let env =
      Apron.Environment.remove (Apron.Abstract1.env abscallee)
	(calleeinfo.Cinv_equation.plocal)
    in
    let abstract2 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: forget locals (on %a)\n"
	  Apron.Abstract1.print abstract1;
	Apron.Abstract1.change_environment manager
	  abstract1 env false (* No DUP *)
      end
    in
    (* 3. We rename in modified abscallee
       - formal vars of pinput_old parameters by actual inparameters
       - formal out and in parameters by special names (to avoid name conflicts)
    *)
    let abstract3 = 
      begin
	Cinv_options.feedback 2 
	  "apply_return: rename input_old by actuals (on %a)\n"
	  Apron.Abstract1.print abstract2;
	Apron.Abstract1.rename_array
	  manager abstract2
	  (Array.append 
	     calleeinfo.Cinv_equation.pinput_old 
	     (Array.append 
		calleeinfo.Cinv_equation.pinput_ini
		calleeinfo.Cinv_equation.poutput))
	  (Array.append 
	     inargs 
	     (Array.append
		calleeinfo.Cinv_equation.pinput_tmp
		calleeinfo.Cinv_equation.poutput_tmp))
      end
    in
    (* 4. We unify the renamed callee value and the caller value 
     *    on vars caller,callee.pinput_ini_tmp,callee.poutput_tmp *)
    (*    Since unification is done by meet, signal to meet that
     *    approximate meet is needed. Flag reset by meet. *)
    let abstract4 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: unify callee with caller (on caller %a, callee %a)\n"
	  Apron.Abstract1.print abscaller
	  Apron.Abstract1.print abstract3;  
	Apron.Abstract1.approximate manager abstract3 (-1);
	Apron.Abstract1.unify manager abstract3 abscaller
      end
    in
    (* 5. We substitute ptr(inargs) with corresponding pinput_tmp.
          The substitution function in the Apron.Abstract1 interface is
          used wtih the flag -1 (reset by substitute_texpr_array).
     *)
    let env4 = Apron.Abstract1.env abstract4 in
    let ptr_inargs = 
      (Array.fold_right
	 (fun v l ->
	   match Apron.Environment.typ_of_var env4 v with
	       Apron.Environment.INT -> l
	     | Apron.Environment.REAL -> v::l
	 )
	 inargs [])
    in
    let texpr_ini = (* may be other values than pointers *)
      (Array.map
	 (fun v -> Apron.Texpr1.var env4 v)
	 calleeinfo.Cinv_equation.pinput_tmp)
    in
    let abstract5 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: substitute actuals by pinput_ini (on %a)\n"
	  Apron.Abstract1.print abstract4;
	Apron.Abstract1.approximate manager abstract4 (-1); 
	Apron.Abstract1.substitute_texpr_array manager abstract4
	  (Array.of_list ptr_inargs) texpr_ini None
      end
    in 
    (* 6. We remove pinput_tmp *)
    let env6 = 
      Apron.Environment.remove env4
	calleeinfo.Cinv_equation.pinput_tmp
    in
    let abstract6 =
      begin
	Cinv_options.feedback 2 
	  "apply_return: remove input_tmp (on %a)\n"
	  Apron.Abstract1.print abstract5;
	Apron.Abstract1.change_environment manager 
	  abstract5 env6 false (* No DUP *)
      end
    in
    (* 7. We assign the actual out parameters.
     *    This operation fails if an actual pointer parameter is not NULL.
     *)
    let tlinexpr =
      Array.map
	(begin fun var ->
	  let e = Apron.Linexpr1.make ~sparse:true env6 in
	  Apron.Linexpr1.set_coeff e var (Apron.Coeff.s_of_int 1);
	  e
	end)
	calleeinfo.Cinv_equation.poutput_tmp
    in
    let abstract7 = 
      begin
	Cinv_options.feedback 2 
	  "apply_return: assign outputs (on %a)\n"
	  Apron.Abstract1.print abstract6;
	(if tlinexpr<>[||] then
	    (Apron.Abstract1.assign_linexpr_array
	       manager abstract6
	       outargs tlinexpr None)
	 else
	    abstract6)
      end
    in
    (* 8. We remove the introduced temporary variables *)
    let env8 = 
      Apron.Environment.remove env6
	(calleeinfo.Cinv_equation.poutput_tmp)
    in
    let falgo = Cinv_options.get_fun_spec callerinfo.pname in
    begin
      Cinv_options.feedback 2 
	"apply_return: remove temporary outputs (on %a)\n"
	Apron.Abstract1.print abstract7;
      Apron.Abstract1.approximate manager abstract7 falgo;
      Apron.Abstract1.change_environment_with
	manager abstract7 env8 false (* No DUP *)
      ;
      (* 10. We possibly intersect with the result of a previous analysis *)
      begin match dest with
	| None -> ()
	| Some dest ->
	  Apron.Abstract1.meet_with manager abstract7 dest
      end;
      abstract7
    end

  (** Main transfer function *)
  let apply
    (graph:Cinv_equation.graph)
    ~(output : (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output option)
    (manager:'a Apron.Manager.t)
    (hedge:int)
    (tabs:'a Apron.Abstract1.t array)
    :
    unit * 'a Apron.Abstract1.t
    =
    let transfer = PSHGraph.attrhedge graph hedge in
    let abs = tabs.(0) in
    let dest = match output with
      | None -> None
      | Some(output) ->
          let tdest = PSHGraph.succvertex graph hedge in
          assert(Array.length tdest = 1);
          let dest = PSHGraph.attrvertex output tdest.(0) in
          Some dest
    in
    let res =
      match transfer with
      | Cinv_equation.Tassign(var,expr) ->
	begin
	  Cinv_options.feedback 1 "(h=%d) apply_tassign %a := %a (on %a)\n"
	    hedge
	    Apron.Var.print var Apron.Texpr1.print expr
	    Apron.Abstract1.print abs; 
	  apply_tassign manager abs var expr dest
	end
      | Cinv_equation.Lassign _ ->
	  failwith ""
      | Cinv_equation.Condition cond ->
	begin
	  Cinv_options.feedback 1 "(h=%d) apply_tcond (%a) /\ (on %a)\n"
	    hedge 
	    (Boolexpr.print Apron.Tcons1.print) cond
	    Apron.Abstract1.print abs;
	  apply_condition manager abs cond dest
	end
      | Cinv_equation.Call(callerinfo,calleeinfo,tin,tout) ->
	begin
	  Cinv_options.feedback 1 "(h=%d) apply_call %s (on %a)\n"	    
	    hedge 
	    calleeinfo.pname
	    Apron.Abstract1.print abs;
	  apply_call manager abs calleeinfo tin dest
	end
      | Cinv_equation.SCall(callerinfo,calleeinfo,tin,tout) ->
	  failwith ""
      | Cinv_equation.SReturn(callerinfo,calleeinfo,tin,tout) ->
	  failwith ""
      | Cinv_equation.Return(callerinfo,calleeinfo,tin,tout) ->
	begin
	  Cinv_options.feedback 1 
	    "(h=%d) apply_return to %s from %s (on %a) ...\n"
	    hedge
	    callerinfo.pname
	    calleeinfo.pname
	    Apron.Abstract1.print tabs.(1)
	  ;
	  (* DEBUG *)
	  begin
	    let hout = open_out "hedge.txt" in
	    output_byte hout hedge;
	    close_out hout
	  end;
	  apply_return manager abs tabs.(1) callerinfo calleeinfo tin tout dest
	end
    in
    ((),res)

  (*  ===================================================================== *)
  (** {3 Compute (post)fixpoint} *)
  (*  ===================================================================== *)

  let compute
    (graph:Cinv_equation.graph)
    ~(output : (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output option)
    (manager:'a Apron.Manager.t)
    ~(debug:int)
    :
    (Cinv_equation.point, int, 'a Apron.Abstract1.t, unit) Fixpoint.output
    =
    let info = PSHGraph.info graph in
    let maininfo = 
      (try
	 Hashhe.find info.Cinv_equation.procinfo !main_entry
       with Not_found ->
         begin
	   Cinv_options.feedback 0 "Function %s not found, analyze main function." !main_entry;
	   try
	     Hashhe.find info.Cinv_equation.procinfo "main"
	   with Not_found ->
	     begin
	       Cinv_options.feedback 0 "Function `main' not found, stop.";
	       failwith ""
	     end
	 end
      )
    in
    let pexit = maininfo.Cinv_equation.pexit in
    let sstart = 
      begin match output with
	| None ->
	  begin
	    if 0 <> (Apron.Tcons1.array_length maininfo.Cinv_equation.pensures) 
	    then
              PSette.singleton Cinv_equation.compare_point pexit 
	    else
	      PSette.empty Cinv_equation.compare_point
	  end

	| Some output ->
          let abstract = PSHGraph.attrvertex output pexit in
          if Apron.Abstract1.is_bottom manager abstract then
            PSette.empty Cinv_equation.compare_point
          else
            (PSette.singleton Cinv_equation.compare_point pexit)
      end
    in
    if PSette.is_empty sstart then begin
      make_emptyoutput graph manager
    end
    else begin
      let abstract_init = begin fun vertex ->
	  begin
	    try
              begin match output with
		| None ->
		  if Cinv_equation.compare_point vertex pexit = 0 then
		    apply_call_main manager maininfo
		  else
		    Apron.Abstract1.top manager 
		      (Hashhe.find info.Cinv_equation.pointenv vertex)
		| Some(output) ->
		  PSHGraph.attrvertex output vertex
              end
	    with Not_found ->
	      begin
		Cinv_options.feedback 0 "Vertex not found (top)";
		failwith ""
	      end
	  end
      end
      in
      let fpmanager =
	make_fpmanager graph ~output
	  apply abstract_init
	  manager ~debug
      in
      let fp =
	if !iteration_guided then
	  Fixpoint.analysis_guided
	    fpmanager graph sstart	
	    (fun filter  ->
	      Fixpoint.make_strategy_default
		~vertex_dummy:Cinv_equation.vertex_dummy
		~hedge_dummy:Cinv_equation.hedge_dummy
		~priority:(PSHGraph.Filter filter)
		graph sstart)
	else
	  Fixpoint.analysis_std
	    fpmanager graph sstart
	    (Fixpoint.make_strategy_default
	       ~vertex_dummy:Cinv_equation.vertex_dummy
	       ~hedge_dummy:Cinv_equation.hedge_dummy
	       graph sstart)
      in
      Cinv_options.feedback 0 "Analysis stats: ";
      Fixpoint.print_stat Format.std_formatter (PSHGraph.info fp);
      Format.fprintf Format.std_formatter "@.";
      fp
    end
end

