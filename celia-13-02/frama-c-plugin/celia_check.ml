(**************************************************************************)
(*                                                                        *)
(*  This file is part of CINV.                                            *)
(*                                                                        *)
(*  Copyright (C) 2009-2012                                               *)
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


(*****************************************************************************)
(* Diagnosis of the C used                                                   *)
(*****************************************************************************)

(* Import from Cil *)
open Cil_types
open Cil


(* Utility functions *)
open Common
open Clim

(*****************************************************************************)
(* Utility functions                                                         *)
(*****************************************************************************)

(* Builds a name from a type description following the rules:
 * - basic types have given names
 * - append a star ("*" string) for a TPtr
 * - append an array ("[]" string) for a TArray
 * - prefix with "struct " or "union " for TComp
 *)
let rec type_name_of_typ
    (ty: Cil_types.typ)
    : string
    =
  match ty with
    | TVoid _ -> "void"
    | TInt (k,_) -> type_name_of_typ_int k
    | TFloat (k,_) -> type_name_of_typ_float k
    | TPtr (ty1,_) -> (type_name_of_typ ty1)^"*"
    | TArray (ty1,_,_,_) -> (type_name_of_typ ty1)^"[]" 
    (** TODO: deal with length *)
    | TNamed (ti, _) -> (type_name_of_typ ti.ttype)
    | TComp (ci,_,_) -> (type_name_of_comp ci)
    | TEnum (ei,_) -> ("enum "^ei.ename) (*TODO*)
    | TFun _ -> "unknownFun"
    | TBuiltin_va_list _ -> "unknownBuiltin"
      
and type_name_of_comp
    (ci: Cil_types.compinfo)
    : string
    =
  ("struct "^(ci.cname)) (** TODO: union *)

and type_name_of_typ_int
    (k: Cil_types.ikind)
    : string
    =
  match k with
    | IChar -> "char"
    | IBool -> "_Bool"
    | ISChar -> "signed char"
    | IUChar -> "unsigned char"
    | IInt -> "int"
    | IUInt -> "unsigned int"
    | IShort -> "short"
    | IUShort -> "unsigned short"
    | ILong -> "long"
    | IULong -> "unsigned long"
    | ILongLong -> "long long"
    | IULongLong -> "unsigned long long"
                 
and type_name_of_typ_float
    (k: Cil_types.fkind)
    : string
    =
  match k with
      FFloat -> "float"
    | FDouble ->  "double"
    | FLongDouble ->  "long double"


(* Register the type and the types it use *)
let rec register_typ
    (ty: Cil_types.typ)
    (loc: Cil_types.location)
    : Cinv_environment.typeid
    =
  begin
    (* The refered types used by ty are visited in the register_types, *)
    (* Register now the type ty. *)
    (let tyname = type_name_of_typ ty in
     let id = Cinv_environment.get_typeid tyname in
     if id < 0 then (* type not in the environment *)
       begin
	 let tloc = Loc.extract loc in
	 let tyinfo = { Cinv_environment.torig_name = tyname;
			Cinv_environment.tname = tyname;
			Cinv_environment.ttype = Cinv_environment.TVoid; (* information to be completed *)
			Cinv_environment.tloc = tloc;
			Cinv_environment.tid = -1;
		      } in
	 begin
	   (Celia_options.feedback "... type '%s' added." tyname);
	   Cinv_environment.add_type tyname tyinfo
	 end
       end
     else
       id
    )
  end

let register_comp
    (ci: Cil_types.compinfo)
    (loc: Cil_types.location)
    : Cinv_environment.typeid
    =  
  (* Get the name of this type from its kind. *)
  let tyname = type_name_of_comp ci in
  let tid = Cinv_environment.get_typeid tyname in
  if tid < 0 then (* type not in the environment *)
    begin
      let tloc = Loc.extract loc in
      let tyinfo = { Cinv_environment.torig_name = ci.corig_name;
		     Cinv_environment.tname = tyname;
		     Cinv_environment.ttype = Cinv_environment.TVoid; (* information to be completed *)
		     Cinv_environment.tloc = tloc;
		     Cinv_environment.tid = -1;
		   } in
      let ntid = Cinv_environment.add_type tyname tyinfo in
      begin
	(Celia_options.feedback "... struct %s added with id %d." tyname ntid);
	ntid
      end
    end
  else
    tid

let register_enum
    (ei: Cil_types.enuminfo)
    (loc: Cil_types.location)
    : Cinv_environment.typeid
    =  
  (* Get the name of this type from its kind. *)
  let tyname = "enum "^ei.ename in
  let tid = Cinv_environment.get_typeid tyname in
  if tid < 0 then (* type not in the environment *)
    begin
      let tloc = Loc.extract loc in
      let tyinfo = { Cinv_environment.torig_name = ei.eorig_name;
		     Cinv_environment.tname = tyname;
		     Cinv_environment.ttype = Cinv_environment.TInt Cinv_environment.ISChar;
		     Cinv_environment.tloc = tloc;
		     Cinv_environment.tid = -1;
		   } in
      let ntid = Cinv_environment.add_type tyname tyinfo in
      begin
	(Celia_options.feedback "... type `%s' added with id %d." tyname ntid);
	ntid
      end
    end
  else
    tid


(**
 * Fill the type information in the environement.
 *)
(* Type information *)
let rec extract_typ 
    (ty: Cil_types.typ)
    : Cinv_environment.typ
    =
  match ty with
    | TVoid _ -> Cinv_environment.TVoid

    | TInt (k,_) -> Cinv_environment.TInt (extract_ikind k)

    | TFloat (k,_) -> Cinv_environment.TFloat (extract_fkind k)

    | TPtr (ty1,_) -> 
      let tid = Cinv_environment.get_typeid (type_name_of_typ ty1) in
      Cinv_environment.TPtr tid

    | TArray (ty1,None,_,_) -> 
      let tid = Cinv_environment.get_typeid (type_name_of_typ ty1) in
      Cinv_environment.TArray (tid, None)

    | TArray (ty1,Some e,_,_) -> 
      let tid = Cinv_environment.get_typeid (type_name_of_typ ty1) in
      let sz = extract_int e in
      if sz >= 0 then 
	Cinv_environment.TArray (tid, Some sz)
      else
	Cinv_environment.TArray (tid, None)

    | TNamed (ti, _) -> (extract_typ ti.ttype)

    | TComp (ci,_,_) -> 
      let newci = (extract_compinfo ci) in
      Cinv_environment.TComp newci

    | _ -> assert false

and extract_ikind 
    (k : Cil_types.ikind)
    : Cinv_environment.ikind
    =
  match k with
      IBool -> Cinv_environment.IBool
    | IChar -> Cinv_environment.IChar
    | ISChar -> Cinv_environment.ISChar
    | IUChar -> Cinv_environment.IUChar
    | IInt -> Cinv_environment.IInt
    | IUInt -> Cinv_environment.IUInt
    | IShort -> Cinv_environment.IShort
    | IUShort -> Cinv_environment.IUShort
    | ILong -> Cinv_environment.ILong
    | IULong -> Cinv_environment.IULong
    | ILongLong -> Cinv_environment.ILongLong
    | IULongLong -> Cinv_environment.IULongLong

and extract_fkind
    (k : Cil_types.fkind)
    : Cinv_environment.fkind
    =
  match k with 
      FFloat -> Cinv_environment.FFloat
    | FDouble -> Cinv_environment.FDouble
    | FLongDouble -> Cinv_environment.FLongDouble

and extract_int 
    (e: Cil_types.exp)
    : int
    =
  match e.enode with
    | Const (CInt64(i,_,_)) ->
      (Int64.to_int i)
    | _ -> -1 (* error code *)

and extract_compinfo
    (ci: Cil_types.compinfo)
    : Cinv_environment.compinfo
    =
  let tyname = (type_name_of_comp ci) in
  let ti = Cinv_environment.get_typeinfo tyname in
  let fld,recfld = 
    List.fold_left 
      (fun p fi -> filter_rec_field ci.cname p fi) 
      ([],[]) 
      ci.cfields 
  in
  {
    Cinv_environment.cstruct = true;
    Cinv_environment.corig_name = ci.corig_name; (* TODO: copy string? *)
    Cinv_environment.cname = ci.cname; (* TODO: copy string? *)
    Cinv_environment.ckey = ci.ckey;
    Cinv_environment.cloc = ti.Cinv_environment.tloc;
    Cinv_environment.cfields = fld; 
    Cinv_environment.crecfields = recfld;
  }

(* Separate recursive and non-recursive types. *)
and filter_rec_field 
    (cname: string)
    (p: Cinv_environment.fieldid list * Cinv_environment.fieldid list)
    (finfo: Cil_types.fieldinfo)
    : Cinv_environment.fieldid list * Cinv_environment.fieldid list
    =
  let fid = Cinv_environment.get_fieldid finfo.fname in
  let ftypname = (type_name_of_typ finfo.ftype) in
  (* remove 'struct ' and ending star *)
  let ptrcname = "struct "^cname^"*" in
  begin
    (*MS DEBUG 
    Celia_options.feedback "Field %s ftypname=%s ptrcname=%s" finfo.fname 
      ftypname ptrcname; *)
    if (String.compare ftypname ptrcname) = 0 then
      ((fst p),(snd p)@[fid])
    else
      ((fst p)@[fid],(snd p))
  end

(* *)
let complete_fieldinfo
    (finfo: Cil_types.fieldinfo)
    : Cinv_environment.fieldinfo
    =
  let ftypname = (type_name_of_typ finfo.ftype) in
  let ftypeid = Cinv_environment.get_typeid  ftypname in 
  let fi = {
    Cinv_environment.fcomp = -1 (* TODO *);
    Cinv_environment.forig_name = finfo.forig_name; (* TODO: copy string? *)
    Cinv_environment.fname = finfo.fname; (* TODO: copy string? *)
    Cinv_environment.ftype = ftypeid;
    Cinv_environment.fattr = 0; (* TODO *)
    Cinv_environment.fid = 0; (* Undefined *)
    Cinv_environment.floc = (Loc.extract finfo.floc);
    Cinv_environment.fsize_in_bits = finfo.fsize_in_bits;
    Cinv_environment.foffset_in_bits = finfo.foffset_in_bits;
    Cinv_environment.fpadding_in_bits = finfo.fpadding_in_bits;
  } in
  fi

(*****************************************************************************)
(* Types diagnosis                                                           *)
(*****************************************************************************)

(* Types registering                                                         *)
(**
 * Visitor collecting all the names of the types 
 * - defined globally
 * - used in declarations
 * The typeinfo stored in the HashTable of the Cinv_environment
 * is incomplete; it will be completed by the complete_types visitor
 *)
class register_types = object (_self)
  inherit Visitor.generic_frama_c_visitor (Project.current())
    (Cil.inplace_visit())

  (* Register types defined and used in globals types *)
  method vglob_aux = function
    | GType (ti,loc) -> 
      (** A typedef *)
      begin
	(Celia_options.feedback "Type definition '%s' seen: " ti.tname);
	(if not (ti.treferenced) then
  	  (Celia_options.feedback "... not used, ignored!")
	 else
	    (* Get the named type. *)
	    let ty = Cil.unrollType ti.ttype in
	    let tid = register_typ ty loc in
  	    (Celia_options.feedback "... type registered with id %d." tid) 
	);
	DoChildren
      end

    | GCompTag (ci,loc) ->
      (** A struct/union type *)
      begin
	(Celia_options.feedback "Type composite '%s' seen:" ci.corig_name);
	(if not(ci.creferenced) then
	    begin
	      (Celia_options.feedback "... not used, ignored!");
	      SkipChildren
	    end
	 else if not(ci.cstruct) then
	   begin
	     (Celia_options.feedback "... union type, ignored!");
	     SkipChildren
	   end
	 else
	   begin
	    let tid = register_comp ci loc in
  	    (Celia_options.feedback "... type registered with id %d." tid);
	    DoChildren
	   end
	)
      end

    | GCompTagDecl (ci,loc) ->
      (** A forward use of a struct/union type *)
      let tid = register_comp ci loc in
      begin
	(Celia_options.feedback 
	   "Forward definition of struct '%s' seen:\n"  ci.corig_name);
  	(Celia_options.feedback "... type struct '%s' registered with id %d." 
	   ci.cname tid);
	SkipChildren
      end

    | GEnumTag (ei,loc) ->
      (** An enum type *)
      begin
	(Celia_options.feedback "Type enum '%s' seen:" ei.eorig_name);
	(if not(ei.ereferenced) then
	    begin
	      (Celia_options.feedback "... not used, ignored!");
	      SkipChildren
	    end
	 else
	   begin
	    let tid = register_enum ei loc in
  	    (Celia_options.feedback "... type registered with id %d." tid);
	    SkipChildren
	   end
	)
      end
      (**MS: commented out for nagios
      begin
	  Celia_options.feedback 
	   "Type definition '%s' seen:\n... enum type, ignored!" ei.eorig_name);
       	  SkipChildren
      end
      *)

    | GEnumTagDecl (ei,_) ->
      begin
	(Celia_options.feedback 
	   "Forward type definition '%s' seen:\n... enum type, ignored!" 
	   ei.eorig_name);
	SkipChildren
      end

    | _ -> SkipChildren


  (* Register types used in field definitions. *)
  method vfieldinfo fi =
    begin
      (Celia_options.feedback "Field '%s' seen." fi.fname);
      DoChildren
    end


  (* Register types used anywhere else (var/fun declaration, etc.) *)
  method vtype ty =
    let bty = Cil.unrollType ty in
    let tid = register_typ bty (Lexing.dummy_pos, Lexing.dummy_pos) in
    begin
      (Celia_options.feedback "Type with id %d registered." tid);
      DoChildren
    end
end

(* Types information completing                                              *)
(**
 * Visitor completing the informations about the registered types.
 * All the valid informations about types are stored in the 
 * HashTable of the Cinv_environmant
 *)
class complete_types = object (_self)
  inherit Visitor.generic_frama_c_visitor (Project.current())
    (Cil.inplace_visit())

  (* Visit type definitions. *)
  method vglob_aux = function
    | GType (ti,_) ->
      begin
	(if ti.treferenced then
	    begin
  	      (Celia_options.feedback "Complete type '%s':\n" ti.torig_name);
	      (* Get the named type. *)
	      let ty = Cil.unrollType ti.ttype in
	      (* Get the name of this type from its kind. *)
	      let tyname = type_name_of_typ ty in
	      (* Get the incomplete information for this type. *)
	      let oldti = Cinv_environment.get_typeinfo tyname in
	      (match oldti.Cinv_environment.ttype with
		|  Cinv_environment.TVoid -> (* TODO: if name not void *)
		  (* Build the complete information for this type. *)
		  let newti = {
		    Cinv_environment.torig_name = oldti.Cinv_environment.torig_name;
		    Cinv_environment.tname = oldti.Cinv_environment.tname;
		    Cinv_environment.ttype = (extract_typ ty);
		    Cinv_environment.tloc = oldti.Cinv_environment.tloc;
		    Cinv_environment.tid = oldti.Cinv_environment.tid;
		  } in
		  let ntid = Cinv_environment.add_type tyname newti in
  		  (Celia_options.feedback "... type '%s' updated (from %d to %d).\n" 
		     tyname oldti.Cinv_environment.tid ntid)
		| _ ->
		  (* Information already updated *)
  		  (Celia_options.feedback " ... type '%s' seen (id %d).\n" 
		     tyname oldti.Cinv_environment.tid)
	      )
	    end    
	);
	DoChildren
      end

    | GCompTag (ci,_) ->
      begin
	(if ci.creferenced && ci.cstruct then
	    begin
  	      (Celia_options.feedback "Complete struct '%s':\n" ci.corig_name);
	      let tyname = type_name_of_comp ci in
	      let oldti = Cinv_environment.get_typeinfo tyname in
	      (match oldti.Cinv_environment.ttype with
		|  Cinv_environment.TVoid ->
		  (* Build the complete information for this type. *)
		  let newti = { 
		    Cinv_environment.torig_name = oldti.Cinv_environment.torig_name;
		    Cinv_environment.tname = oldti.Cinv_environment.tname;
		    Cinv_environment.ttype = Cinv_environment.TComp (extract_compinfo ci);
		    Cinv_environment.tloc = oldti.Cinv_environment.tloc;
		    Cinv_environment.tid = oldti.Cinv_environment.tid;
		  } in
		  let ntid = Cinv_environment.add_type tyname newti in
  		  (Celia_options.feedback 
		     "... type '%s' updated (from %d to %d).\n" 
		     tyname oldti.Cinv_environment.tid ntid)
		| _ ->
		  (* Information already updated *)
  		  (Celia_options.feedback " ... type '%s' seen (id %d).\n" 
		     tyname oldti.Cinv_environment.tid)
	      )
	    end    
	);
	DoChildren
      end
	
    | _ -> SkipChildren


  (* Visit type uses. *)
  method vtype ty =
      (* Get the named type *)
      let bty = Cil.unrollType ty in
      (* Get the name of this type from its kind. *)
      let tyname = type_name_of_typ bty in
      (* Get the incomplete information for this type. *)
      let oldti = Cinv_environment.get_typeinfo tyname in
      begin
	(match oldti.Cinv_environment.ttype with
	  |  Cinv_environment.TVoid -> (* TODO: if name not void *)
	  (* Build the complete information for this type. *)
	    let newti = { 
	      Cinv_environment.torig_name = oldti.Cinv_environment.torig_name;
	      Cinv_environment.tname = oldti.Cinv_environment.tname;
	      Cinv_environment.ttype = (extract_typ ty);
	      Cinv_environment.tloc = oldti.Cinv_environment.tloc;
	      Cinv_environment.tid = oldti.Cinv_environment.tid;
	    } in
	    let ntid = Cinv_environment.add_type tyname newti in
  	    (Celia_options.feedback "... type '%s' updated (from %d to %d).\n" 
	       tyname oldti.Cinv_environment.tid ntid)
	  | _ ->
	    (* Information already updated *)
  	    (Celia_options.feedback " ... type '%s' seen (id %d).\n" 
	       tyname oldti.Cinv_environment.tid)
	);
	SkipChildren
      end

end


(* Fields registering                                                        *)
(**
 * Visitor collecting all the names of fields 
 * The fieldinfo stored in the HashTable of the Cinv_environment
 * is complete.
 *)
class register_fields = object (_self)
  inherit Visitor.generic_frama_c_visitor (Project.current())
    (Cil.inplace_visit())

  method vfieldinfo fi = 
    if fi.fcomp.creferenced then
      let cname = (type_name_of_comp fi.fcomp) in
      let cid = Cinv_environment.get_typeid cname in
      let tname = (type_name_of_typ fi.ftype) in
      let tid = Cinv_environment.get_typeid tname in
      let nfi = { Cinv_environment.fcomp = cid;
		  Cinv_environment.forig_name = fi.forig_name;
		  Cinv_environment.fname = fi.fname;
		  Cinv_environment.ftype = tid;
		  Cinv_environment.fattr = (String.compare "next" 
					      (* if several next fields, next__ used *)
					      (String.sub fi.forig_name 0 4));
		  Cinv_environment.fid = -1;
		  Cinv_environment.floc = (Loc.extract fi.floc);
		  Cinv_environment.fsize_in_bits = fi.fsize_in_bits;
		  Cinv_environment.foffset_in_bits = fi.foffset_in_bits;
		  Cinv_environment.fpadding_in_bits = fi.fpadding_in_bits;
		} in
      let nid = Cinv_environment.add_field nfi in
      begin
	(Celia_options.feedback "Field '%s' registered with id %d.\n" 
	   fi.fname nid);
	SkipChildren
      end
    else
      SkipChildren
 end


(* Function encapsulating the visitor *)
let diag_types f =
  let vregty = new register_types in (* names of types are collected *)
  let vregfld = new register_fields in (* names of fields are collected *)
  let vcomty = new complete_types in  (* infos on types are completed *)
  (* TODO: complete fields *)
  begin
    Cinv_environment.init_types;
    Visitor.visitFramacFile vregty f;
    Visitor.visitFramacFile vregfld f;
    Visitor.visitFramacFile vcomty f;
    ()
  end


(*****************************************************************************)
(* Check entry function definition                                           *)
(*****************************************************************************)
(* Module for visitor *)
class diag_funs = (* TODO *)
  object(_self)
    inherit Visitor.generic_frama_c_visitor (Project.current())
      (Cil.inplace_visit())

  method vglob_aux = function
    | GFun (fundec,_) ->
      begin
	Celia_options.feedback "See function %s " fundec.svar.vname;
	DoChildren
      end

    | GType _ | GAnnot _ | GVar _ | GVarDecl _
    | GCompTag _ | GCompTagDecl _ | GEnumTagDecl _
    | GEnumTag _ | GAsm _ | GPragma _ | GText _ -> SkipChildren 

  method vvdec vinfo = 
    if vinfo.vglob then
      SkipChildren
    else (* only variables from the input function are visited then *)
    begin
      Celia_options.debug "Variable %s" vinfo.vname;
      match vinfo.vtype with
	| TInt _ -> SkipChildren
	| TNamed (tinfo,_) ->
	    begin
	      if (String.compare tinfo.tname "intlist") != 0 then
		Celia_options.feedback "Unsupported type %s" tinfo.tname
	      else
		Celia_options.feedback "Allowed type %s" tinfo.tname
	      ;
	      SkipChildren
	    end 
	| TFun _ -> DoChildren
	| TVoid _ | TFloat _ | TBuiltin_va_list _
	| TPtr _ | TArray _ | TComp _
	| TEnum _ -> 
	  (Celia_options.feedback "Unsupported type for variable %s" 
	     vinfo.vname);
	  SkipChildren
    end
  end

(* Function encapsulating the visitor *)
let diag_funs f =
  let vis = new diag_funs in 
  Visitor.visitFramacFile vis f

(*****************************************************************************)
(* Check restrictions on the statements                                      *)
(*****************************************************************************)
(* Module for visitor *)
class diag_stmts = (* TODO *)
  object(_self)
    inherit Visitor.generic_frama_c_visitor (Project.current())
      (Cil.inplace_visit())

    method vfile _f =
      DoChildren
  end

(* Function encapsulating the visitor *)
let diag_stmts f =
  let vis = new diag_stmts in 
  Visitor.visitFramacFile vis f


(*****************************************************************************)
(* Main                                                                      *)
(*****************************************************************************)

(** Colect informations through visitors
 *  @return report in file pan/pan.dia
 *)
let check file =
  (* Diagnosis for types *)
  Celia_options.feedback "Diagnosis of types: ";
  diag_types file;
  Celia_options.feedback "... printed in file diag_types.dot";
  Cinv_environment.fprint_types ();

  (* Diagnosis for functions *)
  Celia_options.feedback "Diagnosis of functions: ";
  diag_funs file;

  (* Diagnosis for statements *)
  Celia_options.feedback "Diagnosis of statements: ";
  diag_stmts file;
  
  (* Print diagnosis results *)
  (** Open file for diagnosis *)
  let dia_chan = open_out "pan/pan.dia" in
  begin
    (* Types, funs, stmts, vars: used/unused *)
    Printf.fprintf dia_chan "TODO\n";
    flush dia_chan;
    close_out dia_chan
  end
