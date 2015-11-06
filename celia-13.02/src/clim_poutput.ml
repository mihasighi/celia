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

open Format
open Pp
open Clim_env
open Clim_ast
open Clim_output_misc


let is_not_true (p : Clim_ast.pexpr) =
  match p#node with
    | LCPEconst (LCCboolean true) -> false
    | _ -> true

let bin_op = function
  | `Blt -> "<"
  | `Bgt -> ">"
  | `Ble -> "<="
  | `Bge -> ">="
  | `Beq -> "=="
  | `Bneq -> "!="
  | `Badd -> "+"
  | `Bsub -> "-"
  | `Bmul -> "*"
  | `Bdiv -> "/"
  | `Bmod -> "%"
  | `Bland -> "&&"
  | `Blor -> "||"
  | `Bimplies -> "==>"
  | `Biff -> "<==>"

let unary_op = function
  | `Uplus -> "+"
  | `Uminus -> "-"
  | `Unot -> "!"

let identifier fmt id =
  fprintf fmt "%s" id#name

let rec pexpr fmt e =
  match e#node with
    | LCPElabel(lab,e) ->
	assert (lab <> "");
	fprintf fmt "@[(%s : %a)@]" lab pexpr e
    | LCPEconst c -> const fmt c
    | LCPEvar vi -> 
	fprintf fmt "%s" vi
    | LCPEbinary (e1, op, e2) ->
	fprintf fmt "@[<hv 2>(%a %s@ %a)@]" pexpr e1 (bin_op op) pexpr e2
    | LCPEunary(op,e1) ->
	fprintf fmt "@[(%s %a)@]" (unary_op op) pexpr e1 
    | LCPEif (e1,e2,e3) -> 
	fprintf fmt "@[(if %a then %a else %a)@]" pexpr e1 pexpr e2 pexpr e3
    | LCPEassign (v, e) -> 
	fprintf fmt "(%a = %a)" pexpr v pexpr e
    | LCPEcast (e, si) ->
	fprintf fmt "(%a :> %a)" pexpr e ptype si
    | LCPEalloc (e, si) ->
	fprintf fmt "(new %s[%a])" si pexpr e 
    | LCPEfree (e) ->
	fprintf fmt "(free(%a))" pexpr e 
    | LCPEaddress(absolute,e) ->
	fprintf fmt "\\%aaddress(%a)" address_kind absolute pexpr e 
    | LCPEbase_block(e) ->
	fprintf fmt "\\base_block(%a)" pexpr e
    | LCPEderef (e, fi) -> 
	fprintf fmt "%a.%s" pexpr e fi
    | LCPEold t -> fprintf fmt "@[\\old(%a)@]" pexpr t
    | LCPEapp(f,labs,args) ->
	fprintf fmt "%s%a(@[%a@])" f
	  (fun fmt labs -> if List.length labs = 0 then () else
	    fprintf fmt "%a" (print_list_delim lbrace rbrace comma label) labs) labs
	  (print_list comma pexpr) args 
    | LCPEreturn (e) ->
	fprintf fmt "@\n(return %a)" pexpr e
    | LCPEcontinue lab -> 
	fprintf fmt "@\n(continue %s)" lab
    | LCPEbreak lab -> 
	fprintf fmt "@\n(break %s)" lab
    | LCPEwhile (e, behaviors, variant, s)-> 
	fprintf fmt "@\n@[loop %a%a@\nwhile (%a)%a@]"
	  (print_list nothing loop_behavior) behaviors
	  (print_option 
             (fun fmt (t,r) -> 
                match r with
                  | None ->
                      fprintf fmt "@\nvariant %a;" pexpr t
                  | Some r ->
                      fprintf fmt "@\nvariant %a for %a;" pexpr t identifier r
             ))
	  variant
	  pexpr e block [s]
    | LCPEfor (inits, cond, updates, behaviors, variant, body)-> 
	fprintf fmt "@\n@[loop %a@\n%a@\nfor (%a ; %a ; %a)%a@]"
	  (print_list nothing loop_behavior) behaviors
	  (print_option 
             (fun fmt (t,r) -> 
                match r with
                  | None ->
                      fprintf fmt "@\nvariant %a;" pexpr t
                  | Some r ->
                      fprintf fmt "@\nvariant %a for %a;" pexpr t identifier r
             ))
	  variant
	  (print_list comma pexpr) inits 
	  pexpr cond (print_list comma pexpr) updates
	  block [body]
    | LCPEdecl (ty,vi, None)-> 
	fprintf fmt "@\n(var %a %s)" ptype ty vi
    | LCPEdecl (ty,vi, Some e)-> 
	fprintf fmt "@\n(var %a %s = %a)" 
	  ptype ty vi pexpr e
    | LCPEassert(behav,asrt,a)-> 
	fprintf fmt "@\n(%a %a%a)" 
	  asrt_kind asrt
	  (print_list_delim 
	     (constant_string "for ") (constant_string ": ") 
	     comma identifier)
	  behav
	  pexpr a
    | LCPEcontract(req,dec,behs,e) -> 
	fprintf fmt "@\n@[<v 2>( ";	
	Option_misc.iter 
	  (fun e -> if is_not_true e then
	     fprintf fmt "requires %a;@\n" pexpr e) req;
	Option_misc.iter 
	  (fun (t,r) -> match r with
	     | None -> fprintf fmt "decreases %a;@\n" pexpr t
	     | Some r -> fprintf fmt "decreases %a for %a;@\n" 
		 pexpr t identifier r) 
	  dec;
	List.iter (behavior fmt) behs;
	fprintf fmt "@\n{ %a@ })@]" pexpr e 	
    | LCPEblock l -> block fmt l
    | LCPEswitch (e, csl) ->
	fprintf fmt "@\n@[<v 2>switch (%a) {%a@]@\n}"
	  pexpr e (print_list nothing case) csl

and triggers fmt trigs = 
  print_list_delim lsquare rsquare alt (print_list comma pexpr) fmt trigs

and ptag fmt tag =
  match tag#node with
    | LCPTtag id -> string fmt id#name
    | LCPTbottom -> string fmt "\\bottom"
    | LCPTtypeof e -> fprintf fmt "\\typeof(%a)" pexpr e

and handler fmt (ei,vio,s) =
  fprintf fmt "@\n@[<v 2>catch %s %s %a@]"
    ei#name vio
    pexpr s

and pexprs fmt l = print_list semi pexpr fmt l

and block fmt b =
  match b with
    | [] -> fprintf fmt "{()}"
    | _::_ ->
        fprintf fmt "@\n@[<v 0>{@[<v 2>  ";
        pexprs fmt b;
        fprintf fmt "@]@\n}@]"

and case fmt (c,sl) =
  let onecase fmt = function
    | Some c ->
	fprintf fmt "@\ncase %a:" pexpr c
    | None ->
	fprintf fmt "@\ndefault:"
  in
  fprintf fmt "%a%a" (print_list nothing onecase) c pexpr sl

and behavior fmt (_loc,id,throws,assumes,requires,assigns,ensures) =
  fprintf fmt "@\n@[<v 2>behavior %s:" id;
  Option_misc.iter 
    (fun a ->
(*       Format.eprintf "Jc_poutput: assumes %a@." pexpr a;*)
       if is_not_true a then
         fprintf fmt "@\nassumes %a;" pexpr a) assumes;
  Option_misc.iter (fprintf fmt "@\nrequires %a;" pexpr) requires;
  Option_misc.iter 
    (fun id -> fprintf fmt "@\nthrows %s;" id#name) throws;
  Option_misc.iter 
    (fun (_,locs) -> fprintf fmt "@\nassigns %a;" 
       (print_list_or_default "\\nothing" comma pexpr) locs)
    assigns;
  fprintf fmt "@\nensures %a;@]" pexpr ensures

and loop_behavior fmt (ids,inv,ass) =        
  fprintf fmt "@\n@[<v 2>behavior %a:@\n"
    (print_list comma (fun fmt id -> fprintf fmt "%s" id#name)) ids;
  Option_misc.iter
    (fun i -> fprintf fmt "invariant %a;" pexpr i) inv;
  Option_misc.iter 
    (fun (_,locs) -> fprintf fmt "@\nassigns %a;" 
       (print_list_or_default "\\nothing" comma pexpr) locs)
    ass;
  fprintf fmt "@]"



let pclause fmt = function
  | LCCrequires e -> 
      if is_not_true e then
	fprintf fmt "@\n@[<v 2>  requires @[%a@];@]" pexpr e
  | LCCbehavior b -> behavior fmt b

let param fmt (ty,vi) =
  fprintf fmt "%a %s" ptype ty vi

let fun_param fmt (valid,ty,vi) =
  fprintf fmt "%s%a %s" (if valid then "" else "! ") ptype ty vi

let field fmt (modifier,ty,fi,bitsize) =
  let (rep,abstract) = modifier in
  fprintf fmt "@\n";
  if abstract then
    fprintf fmt "abstract "
  else
    if rep then
      fprintf fmt "rep ";
  fprintf fmt "%a %s" 
    ptype ty fi;
  match bitsize with
    | Some bitsize ->
	fprintf fmt ": %d;" bitsize
    | None -> 
	fprintf fmt ";"

let invariant fmt (id, vi, a) =
  fprintf fmt "@\n@[<hv 2>invariant %s(%s) =@ %a;@]"
    id#name vi pexpr a

let type_params_decl fmt l = print_list_delim lchevron rchevron comma Pp.string fmt l

let type_params fmt = function
  | [] -> ()
  | l -> fprintf fmt "<%a>" (print_list comma ptype) l

let super_option fmt = function
  | None -> ()
  | Some(s, p) -> fprintf fmt "%s%a with " s type_params p

let rec pdecl fmt d =
  match d#node with
    | LCDfun(ty,id,params,clauses,body) ->
	fprintf fmt "@\n@[%a %s(@[%a@])%a%a@]@\n" ptype ty id#name
	  (print_list comma fun_param) params 
	  (print_list nothing pclause) clauses
	  (print_option_or_default "\n;" pexpr) body
    | LCDenum_type(id,min,max) ->
	fprintf fmt "@\n@[type %s = %s..%s@]@\n"
	  id (Num.string_of_num min) (Num.string_of_num max)
    | LCDtag(id, params, super, fields, invs) ->
	fprintf fmt "@\n@[<v 2>tag %s%a = %a{%a%a@]@\n}@\n"
          id
          type_params_decl params
          super_option super
          (print_list space field) fields 
	  (print_list space invariant) invs
    | LCDvar(ty,id,init) ->
	fprintf fmt "@\n@[%a %s%a;@]@\n" ptype ty id
	  (print_option (fun fmt e -> fprintf fmt " = %a" pexpr e)) init

and pdecls fmt (l : pexpr decl list) =
  match l with
    | [] -> ()
    | d::r -> pdecl fmt d; pdecls fmt r

(*
Local Variables: 
compile-command: "LC_ALL=C make -j -C .. byte"
End: 
*)
