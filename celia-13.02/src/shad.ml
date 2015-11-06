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

(* Utility functions *)
open Format
open Apron

(** Types 
**)
module Typ = struct 
  type t = Tvoid 
end

(** Variables 
**)
module Var = struct 
  type t = Apron.Var.t
  let of_string = Apron.Var.of_string
  let to_string = Apron.Var.to_string
end

(** Dimensions
**)
module Dim = struct
  type t = Apron.Dim.t
  type dimension = Apron.Dim.dimension
end

(** Environment
**)
module Environment = struct
  type t = Apron.Environment.t
  let make = Apron.Environment.make
  let add = Apron.Environment.add
end

(** Manager
**)
module Manager = struct
  type 'a t = 'a Apron.Manager.t
end


(** Constants
**)
module Coeff = struct
  type t = Apron.Coeff.t
  let s_of_int = Apron.Coeff.s_of_int
  let s_of_float = Apron.Coeff.s_of_float
  let print = Apron.Coeff.print
end

(** Expressions 
**)
module Texpr1 = struct

  type t = Apron.Texpr1.t
  type unop = Apron.Texpr1.unop =
	      | Neg
	      | Cast
	      | Sqrt
		  
  type binop = Apron.Texpr1.binop =
	       | Add
	       | Sub
	       | Mul
	       | Div
	       | Mod

  type typ = Apron.Texpr1.typ =
	     | Real
	     | Int
	     | Single
	     | Double
	     | Extended
	     | Quad

  type round = Apron.Texpr1.round =
	       | Near
	       | Zero
	       | Up
	       | Down
	       | Rnd

  type expr = Apron.Texpr1.expr =
	      | Cst of Coeff.t
	      | Var of Var.t
	      | Unop of unop * expr * typ * round
	      | Binop of binop * expr * expr * typ * round

  let cst = Apron.Texpr1.cst
  let var = Apron.Texpr1.var
  let unop = Apron.Texpr1.unop
  let binop = Apron.Texpr1.binop
end

module Texpr0 = struct
  type t = Apron.Texpr0.t
  type unop = Apron.Texpr0.unop =
	      | Neg
	      | Cast
	      | Sqrt
  type binop = Apron.Texpr0.binop =
	       | Add
	       | Sub
	       | Mul
	       | Div
	       | Mod
  type typ = Apron.Texpr0.typ =
	     | Real
	     | Int
	     | Single
	     | Double
	     | Extended
	     | Quad 
  type round = Apron.Texpr0.round =
	       | Near
	       | Zero
	       | Up
	       | Down
	       | Rnd
  type expr = Apron.Texpr0.expr =
	      | Cst of Coeff.t
	      | Dim of Dim.t
	      | Unop of unop * expr * typ * round
	      | Binop of binop * expr * expr * typ * round

  let of_expr = Apron.Texpr0.of_expr
  let to_expr = Apron.Texpr0.to_expr
end

(** Constraints
**)
module Tcons1 = struct
  type t = Apron.Tcons1.t
  type earray = Apron.Tcons1.earray
  type typ = 
    | EQ
    | SUPEQ
    | SUP
    | DISEQ
    | EQMOD of Apron.Scalar.t

  let make env c0 = {Apron.Tcons1.tcons0=c0; Apron.Tcons1.env=env;}
  let get_env = Apron.Tcons1.get_env 
  let get_texpr1 = Apron.Tcons1.get_texpr1
  let get_tcons0 = Apron.Tcons1.get_tcons0 

  let array_make = Apron.Tcons1.array_make
  let array_get = Apron.Tcons1.array_get 
  let array_set = Apron.Tcons1.array_set
  let array_length = Apron.Tcons1.array_length
  let array_print = Apron.Tcons1.array_print
end

module Tcons0 = struct
  type t = Apron.Tcons0.t
  type typ = 
    | EQ
    | SUPEQ
    | SUP
    | DISEQ
    | EQMOD of Apron.Scalar.t
end

(** Formulas
**)
module Tform1 = struct
(** Shad formulas are SL3 formulas.
 *  At the top most level (Ftop_or), the formulas is a non empty disjunction
 *  of simple Shad formulas.
 *)
type formula = 
    { env: Apron.Environment.t; (** Declared ptr and data vars *)
      form: shadform list; (** Disjunction list *)
    }
    
(** A simple shad formula is a list of nodes and a conjunction of
* edge, pointer and sequence formulas.
*)
and shadform = 
    { nodes: Apron.Var.t array; (** The declared nodes, including null *)
      eform: edgeform list; (** Non empty list of separated edges *)
      pform: labform list; (** Labeling formulas *)
      dform: Apron.Tcons0.t list; 
      (** Data constraints over the environment of scalars and nodes! *)
      uform: univform list; (** Universally quantified formulas *)
    }

(** Edge formula links two nodes
*)
and edgeform =
    Esegm of Apron.Var.t * Apron.Var.t
  | Enull of Apron.Var.t

(** Label formula links a ptr variable to anode or NULL
*)
and labform =
    Lnode of Apron.Var.t * Apron.Var.t
  | Lnull of Apron.Var.t

(** Universally quantified formuals have the special form
 *  forall y:Int. G(y,n) ==> U(y,n)
*)
and univform =
    { qvars: Apron.Var.t array; (** Array of quantified variables *)
      guard: guardform; (** Guard formula over qvars and nodes *)
      data: Apron.Tcons0.t list; (** Data constraints over scalars, nodes, and qvars *)
    }

and guardform =
    Gall of Apron.Var.t * Apron.Var.t (** Gall(y,n) *)
  | Gle2 of Apron.Var.t * Apron.Var.t * Apron.Var.t (** Gle2(y1,y2,n) *)
  | Gsucc2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t (** Gsucc2(y1,y2,n) *)
  | Gfst of Apron.Var.t * Apron.Var.t (** Gfst(y,n) *)
  | Glst of Apron.Var.t * Apron.Var.t (** Glst(y,n) *)
  | Geq2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Var.t (** Geq2(y1,n1,y2,n2) *)
  | Gsl2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Texpr0.t (** Gsl2(y1,n1,y2,n2,c) *)
  | Gsr2 of  Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Var.t * Apron.Texpr0.t (** Gsr2(y1,n1,y2,n2,c) *)
  | Gother of Apron.Tcons0.t list (** other guards over qvars, nodes and prg vars *)


(*****************************************************************************)
(* Building constants *)
(*****************************************************************************)

let shadform_top
    (env:Apron.Environment.t)
    : shadform
    = 
  { nodes=[||]; (* Array.create 1 (Apron.Var.of_string "nilNode"); *)
    eform=[];
    pform=[]; (* TODO: all ptrvar to nilNode? *)
    dform=[];
    uform=[];
  }
    
let check_shadform
    (env:Apron.Environment.t)
    (shf:shadform)
    : shadform
    = 
  (* TODO: the following checks
   * - the graph is completely specified
   *)
  if true then shf 
  else shadform_top env


(*****************************************************************************)
(* Priting functions *)
(*****************************************************************************)

let rec print_smtlib_spec
    (fmt:Format.formatter)
    (spec:formula)
    : unit
    = 
  begin
    print_smtlib_prelude fmt spec.env;
    print_smtlib_formula fmt spec
  end

and print_smtlib_prelude
    (fmt:Format.formatter)
    (penv:Apron.Environment.t)
    : unit
    =
  Format.fprintf fmt "(set-logic SL3)\n";
  (* Print sorts used *)
  Format.fprintf fmt "(declare-sort Ptr 0)\n";
  Format.fprintf fmt "(declare-sort Node 0)\n";
  (* Print predefined predicates and functions for logic sl3 *)
  Format.fprintf fmt "(declare-fun nilNode () Node)\n";
  Format.fprintf fmt "(declare-fun len (Node) Int)\n";
  Format.fprintf fmt "(declare-fun data (Node Int) Int)\n";
  Format.fprintf fmt "(declare-fun sum (Node) Int)\n";
  Format.fprintf fmt "(declare-fun mset (Node) Int)\n";
  Format.fprintf fmt "(declare-fun sep (Bool Bool) Bool)\n";
  Format.fprintf fmt "(declare-fun ls (Node Node) Bool)\n";
  Format.fprintf fmt "(declare-fun label (Ptr Node) Bool)\n";
  Format.fprintf fmt "(declare-fun Gall (Int Node) Bool)\n";
  Format.fprintf fmt "(declare-fun Gle2 (Int Int Node) Bool)\n";
  Format.fprintf fmt "(declare-fun Gsucc2 (Int Int Node) Bool)\n";
  Format.fprintf fmt "(declare-fun Gfst (Int Node) Bool)\n";
  Format.fprintf fmt "(declare-fun Glst (Int Node) Bool)\n";
  Format.fprintf fmt "(declare-fun Geq2 (Int Node Int Node) Bool)\n";
  Format.fprintf fmt "(declare-fun Gsl2 (Int Node Int Node Int) Bool)\n";
  Format.fprintf fmt "(declare-fun Gsr2 (Int Node Int Node Int) Bool)\n";

  (* Print environment *)
  let iarr, parr = Apron.Environment.vars penv in
  Array.iter (fun v -> print_smtlib_prelude_int fmt v) iarr;
  Array.iter (fun v -> print_smtlib_prelude_ptr fmt v) parr

and print_smtlib_prelude_int 
    (fmt:Format.formatter)
    (v:Apron.Var.t)
    : unit
    =
  Format.fprintf fmt "(declare-fun %s () Int)\n" (Apron.Var.to_string v)

and print_smtlib_prelude_ptr 
    (fmt:Format.formatter)
    (v:Apron.Var.t)
    : unit
    =
  Format.fprintf fmt "(declare-fun %s () Ptr)\n" (Apron.Var.to_string v)

and print_smtlib_formula
    (fmt:Format.formatter)
    (f:formula)
    : unit
    = 
  begin
    Format.fprintf fmt "(assert ";
    if (List.length f.form) >= 2 then
      Format.fprintf fmt "\n    (or "
    ;
    List.iter (fun sf ->  print_smtlib_shadform fmt f.env sf) f.form;
    if (List.length f.form) >= 2 then
      Format.fprintf fmt "\n    )" (* end or *)
    ;
    Format.fprintf fmt "\n)\n " (* end assert *)
  end

and print_smtlib_shadform 
    (fmt:Format.formatter)
    (env:Apron.Environment.t)
    (sf:shadform)
    : unit
    =
  let nenv = Apron.Environment.add env [||] sf.nodes in
  begin
    (* Print node declaration *)
    Format.fprintf fmt "\n\t(exists (";
    (Array.iter (fun n -> (Format.fprintf fmt "(?%s Node) " (Apron.Var.to_string n)) )
      sf.nodes);
    Format.fprintf fmt ")\n\t\t(and ";
    (* Print edge formula *)
    (List.iter (fun ef -> print_smtlib_edgeform fmt ef) sf.eform);
    (* Print labeling formula *)
    (List.iter (fun lf -> print_smtlib_labform fmt lf) sf.pform);
    (* Print data part *)
    (List.iter (fun tc0 -> print_smtlib_tcons0 fmt nenv tc0) sf.dform);    
    (* Print universal part *)
    (List.iter (fun uf -> print_smtlib_univform fmt nenv uf) sf.uform);
    (* Print end *)
    Format.fprintf fmt "\n\t\t)\n\t) "; (* end and and exists *)
    ()

  end

and print_smtlib_edgeform
    (fmt:Format.formatter)
    (ef:edgeform)
    : unit
    =
  match ef with
      Esegm(nsrc, ndst) ->
	Format.fprintf fmt "\n\t\t(ls ?%s ?%s) " 
	  (Apron.Var.to_string nsrc)
	  (Apron.Var.to_string ndst)
    | Enull n -> 
	Format.fprintf fmt "\n\t\t(ls ?%s nilNode) " 
	  (Apron.Var.to_string n)

and print_smtlib_labform
    (fmt:Format.formatter)
    (lf:labform)
    : unit
    =
  match lf with
      Lnode(v,n) ->
	Format.fprintf fmt "\n\t\t(label %s ?%s) " 
	  (Apron.Var.to_string v)
	  (Apron.Var.to_string n)
    | Lnull v -> 
	Format.fprintf fmt "\n\t\t(label %s nilNode) " 
	  (Apron.Var.to_string v)

and print_smtlib_tcons0 
    (fmt:Format.formatter)
    (env:Apron.Environment.t)
    (c:Apron.Tcons0.t)
    : unit
    =
  begin
    Format.fprintf fmt "\n\t\t(";
    (match c.Apron.Tcons0.typ with
	Apron.Tcons0.EQ -> Format.fprintf fmt "= 0 "
      | Apron.Tcons0.SUPEQ ->  Format.fprintf fmt "<= 0 "
      | Apron.Tcons0.SUP -> Format.fprintf fmt "< 0 "
      | Apron.Tcons0.DISEQ 
      | Apron.Tcons0.EQMOD _ -> Format.fprintf fmt "?? 0 "
    );
    print_smtlib_texpr0 fmt env (Apron.Texpr0.to_expr c.Apron.Tcons0.texpr0);
    Format.fprintf fmt ") "
  end
 
and print_smtlib_texpr0
    (fmt:Format.formatter)
    (env:Apron.Environment.t)
    (e:Apron.Texpr0.expr)
    : unit
    =
  begin
    (match e with
      | Apron.Texpr0.Cst c -> Apron.Coeff.print fmt c; Format.fprintf fmt " "

      | Apron.Texpr0.Dim d -> 
	let dim = (Apron.Environment.dimension env) in
	let intdim = dim.Apron.Dim.intd in
	let v = Apron.Environment.var_of_dim env d in
	let v_name = Apron.Var.to_string v in
	begin
	  if (d >= intdim) || 
	     (v_name.[0] == 'y') 
          then (* real type vars are always quantified at this point *)
	    (* TODO: the integer ones are quantified only if they are defined in forall *)
	    Format.fprintf fmt "?";
	  Format.fprintf fmt "%s " v_name
	end

      | Apron.Texpr0.Unop(Apron.Texpr0.Neg, e1, Apron.Texpr0.Int, _) ->
	begin
	  Format.fprintf fmt "(- ";
	  print_smtlib_texpr0 fmt env e1;
	  Format.fprintf fmt ") "
	end

      | Apron.Texpr0.Unop(Apron.Texpr0.Cast, e1, ty, rnd) ->
	(* decode Shad expressions *)
	begin
	  match ty,rnd with
	      Apron.Texpr1.Int, Apron.Texpr1.Up -> (* (data x 0) or (data x y) iff e1 is *_r,u *)
		begin
		  Format.fprintf fmt "(data ";
		  (match e1 with 
		      Apron.Texpr0.Binop(Apron.Texpr0.Mul, e1p, e2p, Apron.Texpr1.Real, Apron.Texpr1.Up)
		      -> print_smtlib_texpr0 fmt env e1p;
			print_smtlib_texpr0 fmt env e2p
		    | _ -> Format.fprintf fmt " ?? ??"
		  );
		  Format.fprintf fmt ") "
		end

	    | Apron.Texpr1.Quad, Apron.Texpr1.Up -> 
	      begin
		Format.fprintf fmt "(len ";
		print_smtlib_texpr0 fmt env e1;
		Format.fprintf fmt ") "
	      end

	    | Apron.Texpr1.Quad, Apron.Texpr1.Zero -> 
	      begin
		Format.fprintf fmt "(sum ";
		print_smtlib_texpr0 fmt env e1;
		Format.fprintf fmt ") "
	      end

	    | Apron.Texpr1.Quad, Apron.Texpr1.Near -> 
	      begin
		Format.fprintf fmt "(mset ";
		print_smtlib_texpr0 fmt env e1;
		Format.fprintf fmt ") "
	      end

	    | Apron.Texpr1.Quad, Apron.Texpr1.Down -> 
	      begin
		Format.fprintf fmt "(ucons ";
		print_smtlib_texpr0 fmt env e1;
		Format.fprintf fmt ") "
	      end

	    | _ -> Format.fprintf fmt "cast?? "
	end
	  
      | Apron.Texpr0.Unop(_, _, _, _) -> Format.fprintf fmt "unop?? "

      | Apron.Texpr0.Binop(op, e1, e2, _ , _) ->
	let sop = (match op with 
	  | Apron.Texpr0.Add -> "+"
	  | Apron.Texpr0.Sub -> "-"
	  | Apron.Texpr0.Mul -> "*"
	  | Apron.Texpr0.Div -> "/"
	  | Apron.Texpr0.Mod -> "binopInt??"
	) in
	Format.fprintf fmt "(%s " sop;
	(print_smtlib_texpr0 fmt env e1);
	(print_smtlib_texpr0 fmt env e2);
	Format.fprintf fmt ") "
    )
  end

and print_smtlib_univform 
    (fmt:Format.formatter)
    (env:Apron.Environment.t)
    (uf:univform)
    : unit
    =
  let nenv = Apron.Environment.add env uf.qvars [||] in
    begin
      Format.fprintf fmt "\n\t\t(forall (";
      (Array.iter (fun v -> Format.fprintf fmt "(?%s Int) " (Apron.Var.to_string v)) 
	 uf.qvars);
      Format.fprintf fmt ")\n\t\t    (=> ";
      (* TODO: Print guard *)
      print_smtlib_guardform fmt nenv uf.guard;
      (* TODO: Print data *)
      Format.fprintf fmt "\n\t\t    (and ";
      (List.iter (fun c -> print_smtlib_tcons0 fmt nenv c) uf.data);
      Format.fprintf fmt "\n\t\t    )";
      (* end *)
      Format.fprintf fmt "\n\t\t    )\n\t\t) "; (* end =>, end forall *)
  end

and print_smtlib_guardform
    (fmt:Format.formatter)
    (env:Apron.Environment.t)
    (gf:guardform)
    : unit
    =
  match gf with
    | Gall(y,n) -> 
      Format.fprintf fmt "\n\t\t        (Gall ?%s ?%s) " 
      (Apron.Var.to_string y) (Apron.Var.to_string n)
    | Gfst(y,n) -> 
      Format.fprintf fmt "\n\t\t        (Gfst ?%s ?%s) " 
	(Apron.Var.to_string y) (Apron.Var.to_string n)

    | Glst(y,n) -> 
      Format.fprintf fmt "\n\t\t        (Glst ?%s ?%s) " 
	(Apron.Var.to_string y) (Apron.Var.to_string n)

    | Gle2(y1,y2,n) -> 
      Format.fprintf fmt "\n\t\t        (Gle2 ?%s ?%s ?%s) " 
	(Apron.Var.to_string y1) (Apron.Var.to_string y2) (Apron.Var.to_string n)

    | Gsucc2(y1,y2,n) -> 
      Format.fprintf fmt "\n\t\t        (Gsucc2 ?%s ?%s ?%s) " 
	(Apron.Var.to_string y1) (Apron.Var.to_string y2) (Apron.Var.to_string n)

    | Geq2(y1,n1,y2,n2) ->
      Format.fprintf fmt "\n\t\t        (Geq2 ?%s ?%s ?%s ?%s)"
	(Apron.Var.to_string y1) (Apron.Var.to_string n1) 
	(Apron.Var.to_string y2) (Apron.Var.to_string n2)

    | Gsl2(y1,n1,y2,n2,c) ->
      Format.fprintf fmt "\n\t\t        (Gsl2 ?%s ?%s ?%s ?%s"
	(Apron.Var.to_string y1) (Apron.Var.to_string n1) 
	(Apron.Var.to_string y2) (Apron.Var.to_string n2);
      print_smtlib_texpr0 fmt env (Apron.Texpr0.to_expr c);
      Format.fprintf fmt ") "

    | Gsr2(y1,n1,y2,n2,c) ->
      Format.fprintf fmt "\n\t\t        (Gsr2 ?%s ?%s ?%s ?%s"
	(Apron.Var.to_string y1) (Apron.Var.to_string n1) 
	(Apron.Var.to_string y2) (Apron.Var.to_string n2);
      print_smtlib_texpr0 fmt env (Apron.Texpr0.to_expr c);
      Format.fprintf fmt ") "

    | Gother cl ->
      let cl_length = List.length cl in
      if (cl_length == 0) then
	Format.fprintf fmt "\n\t\ttrue "
      else if (cl_length == 1) then
	print_smtlib_tcons0 fmt env (List.hd cl)
      else
	begin
	  Format.fprintf fmt "\n\t\t(and ";
	  List.iter (fun c -> (print_smtlib_tcons0 fmt env c)) cl;
	  Format.fprintf fmt "\n\t\t) "
	end
      
end

(** Abstact values 
**)
module Abstract1 = struct

  type 'a t = 'a Apron.Abstract1.t

end
