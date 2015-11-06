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


open Clim_stdlib
open Clim_env

class type positioned =
object
  method pos: Loc.position
end

class type typed =
object
  method typ: lc_type
end

class type labeled =
object
  method label: label option
  method set_label: label option -> unit
end

class type marked =
object
  method mark: string
end

type const =
  | LCCvoid
  | LCCnull
  | LCCboolean of bool
  | LCCinteger of string

class type identifier = 
object
  inherit positioned
  method name: string
end

class type ['a] node_positioned = 
object
  inherit positioned
  method node: 'a
end

(***************)
(* parse trees *)
(***************)
(*MS: Needed only if independent parser is done. *)
(*    Imported from JC only useful definitions *)

type ptype_node = 
  | LCPTnative of native_type
  | LCPTidentifier of string * ptype list
  | LCPTpointer of string * ptype list * Num.num option * Num.num option

and ptype = ptype_node node_positioned

type comparison_op = [ `Blt | `Bgt | `Ble | `Bge | `Beq | `Bneq ]
type arithmetic_op = [ `Badd | `Bsub | `Bmul | `Bdiv | `Bmod ]
type logical_op = [ `Bland | `Blor | `Bimplies | `Biff ]

type basic_op = [ comparison_op | arithmetic_op ]
type operational_op = [ basic_op | `Bland | `Blor ]
type bin_op = [ operational_op | logical_op ]
      
type pre_unary_op = [ `Uprefix_inc | `Uprefix_dec ]
type post_unary_op = [ `Upostfix_inc | `Upostfix_dec ]
type pm_unary_op = [ pre_unary_op | post_unary_op | `Uplus ]
type unary_op = [ `Uminus | `Unot ]

type pexpr_unary_op = [ unary_op | `Uplus ]

type native_operator_type = [ `Unit | `Boolean | `Integer ]
type operator_type = [ native_operator_type | `Pointer ]

type pred_bin_op = [comparison_op | logical_op] * operator_type
type expr_unary_op = unary_op * native_operator_type
type term_unary_op = expr_unary_op
type expr_bin_op = operational_op * operator_type
type term_bin_op = bin_op * operator_type
type pred_rel_op = comparison_op * operator_type

type address_kind = Addr_absolute | Addr_pointer

type quantifier = Forall | Exists

type asrt_kind = 
  | Aassert (* Assertion to prove *)
  | Ahint   (* Assertion to help in proofs, 
	       can be either discarded or both used and proved *)
  | Aassume (* Assertion that can be relied on without proof *)
  | Acheck  (* Assertion to prove but which is not used after *)


class type ['expr_node] c_pexpr =
object
  inherit labeled
  inherit ['expr_node] node_positioned
end

type 'expr pbehavior = 
    Loc.position * string * identifier option * 'expr option 
    * 'expr option * (Loc.position * 'expr list) option * 'expr
      (*r loc, name, throws, assumes,requires,assigns,ensures *)
 
and 'expr loopbehavior =
    identifier list 
    * 'expr option * (Loc.position * 'expr list) option 
      (*r idents, invariant, assigns *)
    
and pexpr_node =
  | LCPEconst of const
  | LCPElabel of string * pexpr
  | LCPEvar of string
  | LCPEderef of pexpr * string
  | LCPEbinary of pexpr * bin_op * pexpr
  | LCPEunary of pexpr_unary_op * pexpr
  | LCPEapp of string * label list * pexpr list
  | LCPEassign of pexpr * pexpr
  | LCPEcast of pexpr * ptype (* MS: only for pointers and enum *)
  | LCPEold of pexpr
  | LCPEaddress of address_kind * pexpr 
      (* expression is of integer type for an absolute address, and of
	 pointer type for a pointer address *)
  | LCPEbase_block of pexpr 
  | LCPEif of pexpr * pexpr * pexpr
  | LCPEdecl of ptype * string * pexpr option
  | LCPEalloc of pexpr * string
  | LCPEfree of pexpr
  | LCPEblock of pexpr list
  | LCPEassert of identifier list * asrt_kind * pexpr
  | LCPEcontract of 
      pexpr option * (pexpr * identifier option) option * 
	pexpr pbehavior list * pexpr 
	(* requires, decreases, behaviors, expression *)
  | LCPEwhile of 
      pexpr * pexpr loopbehavior list * 
	(pexpr * identifier option) option * pexpr
	(*r condition, behaviors, variant, body *)
  | LCPEfor of 
      pexpr list * pexpr * pexpr list * pexpr loopbehavior list 
      * (pexpr * identifier option) option * pexpr
	(*r inits, condition, updates, behaviors, variant, body *)
  | LCPEreturn of pexpr
  | LCPEbreak of string
  | LCPEcontinue of string
  | LCPEswitch of pexpr * (pexpr option list * pexpr) list
(*
  | LCPEtry of pexpr * (identifier * string * pexpr) list * pexpr
  | LCPEthrow of identifier * pexpr
*)

and pexpr = pexpr_node node_positioned

and 'a ptag_node =
  | LCPTtag of identifier
  | LCPTbottom
  | LCPTtypeof of 'a

and 'a ptag = 'a ptag_node node_positioned

type 'expr clause =
  | LCCrequires of 'expr
  | LCCbehavior of 'expr pbehavior

type field_modifiers = bool * bool

type 'expr decl_node =
  | LCDvar of ptype * string * 'expr option
  | LCDfun of ptype * identifier * (bool * ptype * string) list * 'expr clause list
      * 'expr option
  | LCDtag of
      string (* name of the tag *)
      * string list (* type parameters *)
      * (string * ptype list) option (* parent tag, applied type parameters *)
      * (field_modifiers * ptype * string * int option) list (* fields *)
      * (identifier * string * 'expr) list (* invariants *)
  | LCDenum_type of string * Num.num * Num.num


and 'expr decl = 'expr decl_node node_positioned

type pdecl = pexpr decl

(* Normalized expressions. Not yet typed by simpler and labeled. *)

class type ['expr_node] c_nexpr =
object
  inherit labeled
  inherit ['expr_node] node_positioned
end

type nexpr_node =
  | LCNEconst of const
  | LCNElabel of string * nexpr
  | LCNEvar of string
  | LCNEderef of nexpr * string
  | LCNEbinary of nexpr * bin_op * nexpr
  | LCNEunary of unary_op * nexpr
  | LCNEapp of string * label list * nexpr list
  | LCNEassign of nexpr * nexpr
  | LCNEcast of nexpr * ptype
  | LCNEold of nexpr
  | LCNEbase_block of nexpr
  | LCNEif of nexpr * nexpr * nexpr
  | LCNEalloc of nexpr * string
  | LCNEfree of nexpr
  | LCNEassert of identifier list * asrt_kind * nexpr
  | LCNEcontract of
      nexpr option * (nexpr * identifier option) option *
        nexpr pbehavior list * nexpr
        (* requires, decreases, behaviors, expression *)
  | LCNEblock of nexpr list
  | LCNEwhile of nexpr * nexpr
        (* cond, body *)
  | LCNEreturn of nexpr option

and nexpr = nexpr_node c_nexpr


(*************)
(* typed ast *)
(*************)

class type ['node] c_term =
object
  inherit typed
  inherit marked
  inherit labeled
  inherit ['node] node_positioned
end

type 'li app = 
    {
      lc_app_fun : 'li;
      lc_app_args : 'li term list;
      lc_app_label_assoc : (label * label) list;
    }

and 'li term_node =
  | LCTconst of const
  | LCTvar of var_info
  | LCTderef of 'li term * label * field_info
  | LCTbinary of 'li term * term_bin_op * 'li term
  | LCTunary of term_unary_op * 'li term
  | LCTapp of 'li app
  | LCTold of 'li term
  | LCTif of 'li term * 'li term * 'li term

and 'li term = 'li term_node c_term

type 'li tag = 'li tag_node node_positioned

and 'li tag_node =
  | LCTtag of struct_info
  | LCTbottom
  | LCTtypeof of 'li term * struct_info

class type ['assertion_node] c_assertion =
object
  inherit marked
  inherit labeled
  inherit ['assertion_node] node_positioned
end

type 'li assertion_node =
  | LCAtrue
  | LCAfalse
  | LCArelation of 'li term * pred_rel_op * 'li term
  | LCAand of 'li assertion list
  | LCAor of 'li assertion list
  | LCAimplies of 'li assertion * 'li assertion
  | LCAiff of 'li assertion * 'li assertion
  | LCAnot of 'li assertion
  | LCAapp of 'li app
  | LCAold of 'li assertion
  | LCAbool_term of 'li term
  | LCAif of 'li term * 'li assertion * 'li assertion

and 'li assertion = 'li assertion_node c_assertion

type 'li term_or_assertion =
  | LCAssertion of 'li assertion
  | LCNone


type 'li behavior =
    { 
      lc_behavior_assumes : 'li assertion option ;
      mutable lc_behavior_ensures : 'li assertion;
    }

type 'li fun_spec =
    {
      mutable lc_fun_requires : 'li assertion;
      (* "free" precondition, proved by static analysis. It can be used
	 to prove the function correctness without being checked at 
	 calls, if static analysis is trusted (option [-trust-ai]) *)
      mutable lc_fun_free_requires : 'li assertion; 
      mutable lc_fun_behavior : (Loc.position * string * 'li behavior) list;
    }


(******************)
(*    typed ast   *)
(******************)

class type ['node] c_expr =
object
  inherit typed
  inherit marked
  inherit ['node] node_positioned
  method original_type: lc_type
end

(* application, increment and assignment are statements.
   special assignment with operation disappears.
 *)
type ('li,'fi) expr_node =
  | LCEconst of const
  | LCEvar of var_info
  | LCEderef of ('li,'fi) expr * field_info
  | LCEbinary of ('li,'fi) expr * expr_bin_op * ('li,'fi) expr
  | LCEunary of expr_unary_op * ('li,'fi) expr
  | LCEapp of ('li,'fi) call
  | LCEassign_var of var_info * ('li,'fi) expr
  | LCEassign_heap of ('li,'fi) expr * field_info * ('li,'fi) expr
  | LCEcast of ('li,'fi) expr * struct_info
  | LCEif of ('li,'fi) expr * ('li,'fi) expr * ('li,'fi) expr
  | LCEbase_block of ('li,'fi) expr
  | LCEalloc of ('li,'fi) expr * struct_info
  | LCEfree of ('li,'fi) expr
  | LCEassert of identifier list * asrt_kind * 'li assertion
  | LCEcontract of 'li assertion option * 
      ('li term * identifier option) option * var_info * 
      (Loc.position * string * 'li behavior) list * ('li,'fi) expr
      (* requires, decreases, vi of \result, behaviors, expression *)
  | LCEblock of ('li,'fi) expr list
  | LCEwhile of 
      ('li, 'fi) expr * ('li, 'fi) expr
	(*r condition, body *)
  | LCEreturn_void 
  | LCEreturn of lc_type * ('li,'fi) expr (*r expected return type *) 
      
and ('li,'fi) expr = ('li,'fi) expr_node c_expr

and ('li,'fi) callee = 
    LClogic_fun of 'li | LCfun of 'fi

and ('li,'fi) call = 
    {
      lc_call_fun : ('li,'fi) callee;
      lc_call_args : ('li,'fi) expr list;
      lc_call_label_assoc : (label * label) list;
    }


(*
Local Variables: 
compile-command: "LC_ALL=C make -j -C .. bin/jessie.byte"
End: 
*)
