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

(** Define types and functions to deal with global declarations *)

(**************************************************************************)
(* Types                                                                  *)
(**************************************************************************)
(** Clim types follows the Cil types, with some simplifications *)
type typeinfo = {
  torig_name: string; (** The original name as found in C file. *)
  tname: string; (** The name. Used to order the types. *)
  ttype: typ; (** The type description. *)
  tloc: Loc.floc; (** A location where it has been defined. *)
  tid: typeid; (** The unique identifier. *)
}

(** Type description *)
and typ =
    TVoid 
  (** Void type. *)

  | TInt of ikind 
  (** An integer type. The kind specifies the sign and width. *)

  | TFloat of fkind
  (** A floating-point type. The kind specifies the precision. *)

  | TPtr of typeid
  (** Pointer type. Reference to a type into the table. *)
  
  | TArray of typeid * int option
  (** Array type. It indicates the base type and the array length. *)

  | TComp of compinfo
  (** A reference to a struct or a union type. *)

(** Various kinds of integers *)
and ikind =
    IBool       (** [_Bool] *)
  | IChar       (** [char] *)
  | ISChar      (** [signed char] *)
  | IUChar      (** [unsigned char] *)
  | IInt        (** [int] *)
  | IUInt       (** [unsigned int] *)
  | IShort      (** [short] *)
  | IUShort     (** [unsigned short] *)
  | ILong       (** [long] *)
  | IULong      (** [unsigned long] *)
  | ILongLong   (** [long long] (or [_int64] on Microsoft Visual C) *)
  | IULongLong  (** [unsigned long long] (or [unsigned _int64] on Microsoft
                    Visual C) *)

(** Various kinds of floating-point numbers *)
and fkind =
    FFloat      (** [float] *)
  | FDouble     (** [double] *)
  | FLongDouble (** [long double] *)

(** Type identifier is an index in the {typ_table} *)
and typeid = int

(** The definition of a structure or union type. *)
and compinfo = {
  cstruct: bool;
    (** True if struct, False if union *)
  corig_name: string;
    (** Original name as found in C file. *)
  cname: string;
    (** The name. Always non-empty. Do not include "struct" or "union" *)
  ckey: int;
    (** A unique integer. *)
  cloc: Loc.floc;
    (** The location where it has been defined *)
  cfields: fieldid list;
    (** Information about the fields. Each fieldinfo has a
      * pointer back to the host compinfo. *)
  crecfields: fieldid list;
    (** The recursive fields, i.e., fields of type pointer to this compinfo *)
  }

(** Information about a struct/union field. *)
and fieldinfo = {
  fcomp: typeid;
    (** The host structure that contains this field; it is unique. *)
  forig_name: string;
    (** original name as found in C file. *)
  fname: string;
    (** The name of the field, desambiguated. *)
  ftype: typeid;
    (** The type *)
  fattr: int;
    (** The attributes for this field: next/prev *)
  fid: fieldid;
    (** The unique identifier *)
  floc: Loc.floc;
    (** The location where this field is defined *)
  fsize_in_bits: int option;
    (** Recall the size of the original field. *)
  foffset_in_bits: int option;
    (** Store the offset at which the field starts in the structure. *)
  fpadding_in_bits: int option;
    (** Store the size of the padding that follows the field, if any. *)
}

and fieldid = int

(** Information about variables. *)
and varinfo = {
  vname: string;
    (** Variable name. *)
  vtyp: typeid;
    (** Variable declaration type. *)
  vid: int;
    (** Variable unique identifier. *)
}


(**************************************************************************)
(* Stores                                                                 *)
(**************************************************************************)
    
(* The store of types declared and used.
 *
 * The hash table is used to find easily the type name.
 * After the interpretation into equations, it is translated into an
 * array.
 *)
let typ_table : (string, typeinfo) Hashtbl.t = Hashtbl.create 10
(* Counter used to provide unique identifiers typeid to types. *)
let typ_cnt : int ref = ref 0
(* Table used to map type identifiers to their name *)
let typ_array : (typeid, string) Hashtbl.t = Hashtbl.create 10


(** Dummy value for typeinfo *)
let dummy_typeinfo = {
  torig_name = "undef";
  tname = "undef";
  ttype = TVoid;
  tloc = Loc.dummy_floc;
  tid = -1;
}

(** Initialization of the store with void type *)
let init_types
    : unit = 
    if (Hashtbl.length typ_table) = 0 then
      let tyVoid = { torig_name = "void"; 
		     tname = "void"; 
		     ttype = TVoid; 
		     tloc = Loc.dummy_floc;
		     tid = !typ_cnt; } in
      begin 
	Hashtbl.add typ_table "void" tyVoid;
	Hashtbl.add typ_array tyVoid.tid "void";
	typ_cnt := !typ_cnt+1
      end
    else 
      (* This shall be avoided *)
      assert false

(** Add a type to the store if not present. *)
let add_type
    (tname: string) (* The type given by Cil, not the original type *)
    (tinfo: typeinfo) (* May be incompletely specified, e.g., tid, ttype *)
    : typeid (* Or -1 if not added for some reason ... *)
    =
  try 
    begin
      let ti = Hashtbl.find typ_table tname in
      (* type in the table, return its identifier *)
      (* TODO: update informations *)
      begin
	Hashtbl.replace typ_table tname tinfo;
	ti.tid
      end
    end
  with Not_found ->
    begin
      (* type not in the table, add it with a new number *)
      let ti = { torig_name = tinfo.torig_name;
		 tname = tinfo.tname;
		 ttype = tinfo.ttype;
		 tloc = tinfo.tloc;
		 tid = !typ_cnt;
	       } in
      begin
	Hashtbl.add typ_table tname ti;
	Hashtbl.add typ_array ti.tid tname;
	typ_cnt := !typ_cnt + 1;
	ti.tid
      end
    end

(** Get the information for type. *)
let get_typeinfo
    (tname: string) (* The type given by Cil, not the original type *)
    : typeinfo (* Or dummy if not found for some reason ... *)
    =
  try 
    begin
      let ti = Hashtbl.find typ_table tname in
      (* type in the table, return its identifier *)
      (* TODO: update informations *)
      ti
    end
  with Not_found ->
    dummy_typeinfo

(** Get the unique id for type. *)
let get_typeid
    (tname: string) (* The type given by Cil, not the original type *)
    : typeid (* Or -1 if not found for some reason ... *)
    =
  try 
    begin
      let ti = Hashtbl.find typ_table tname in
      (* type in the table, return its identifier *)
      (* TODO: update informations *)
      ti.tid
    end
  with Not_found -> -1

(* Get the type info from the identifier. *)
let get_type
    (tid: typeid)
    : typeinfo
    = 
  try
    let tname = Hashtbl.find typ_array tid in
    get_typeinfo tname
  with Not_found -> 
    dummy_typeinfo
  

(* The store of fields declared and used.
 *
 * The hash table is used to find easily the field name.
 * After the interpretation into equations, it is translated into an
 * array.
 *)
let fld_table : (string, fieldinfo) Hashtbl.t = Hashtbl.create 10
(* Counter used to provide unique identifiers typeid to types. *)
let fld_cnt : int ref = ref 0
(* Table used to map field identifiers to their name *)
let fld_array : (fieldid, string) Hashtbl.t = Hashtbl.create 10



(** Dummy value for fieldinfo *)
let dummy_fieldinfo = {
  fcomp = 0; (* TVoid *)
  forig_name = "undef";
  fname = "undef";
  ftype = 0; (* TVoid *)
  fattr = 0;
  fid = -1;
  floc = Loc.dummy_floc;
  fsize_in_bits = None;
  foffset_in_bits = None;
  fpadding_in_bits = None;
}

(** Add a field to the store if not present. *)
let add_field
    (finfo: fieldinfo) (* May be incompletely specified, e.g., fid *)
    : fieldid (* Or -1 if not added for some reason ... *)
    =
  try 
    begin
      let fi = Hashtbl.find fld_table finfo.fname in
      (* field in the table, return its identifier *)
      begin
	Hashtbl.replace fld_table finfo.fname finfo;
	fi.fid
      end
    end
  with Not_found ->
    begin
      (* field not in the table, add it with a new number *)
      let fi = { fcomp = finfo.fcomp;
		 forig_name = finfo.forig_name;
		 fname = finfo.fname;
		 ftype = finfo.ftype;
		 fattr = finfo.fattr;
		 fid = !fld_cnt;
		 floc = finfo.floc;
		 fsize_in_bits = finfo.fsize_in_bits;
		 foffset_in_bits = finfo.foffset_in_bits;
		 fpadding_in_bits = finfo.fpadding_in_bits;
	       } in
      begin
	fld_cnt := !fld_cnt + 1;
	Hashtbl.add fld_table finfo.fname fi;
	Hashtbl.add fld_array fi.fid finfo.fname;
	fi.fid
      end
    end

(** Get the unique id for field. *)
let get_fieldid
    (fname: string) (* The name given by Cil, not the original type *)
    : fieldid (* Or -1 if not found for some reason ... *)
    =
  try 
    begin
      let fi = Hashtbl.find fld_table fname in
      (* field info in the table, return its identifier *)
      fi.fid
    end
  with Not_found -> -1
    
let get_fieldinfo
    (fid: fieldid)
    : fieldinfo
    =
  try
    let fname = Hashtbl.find fld_array fid in
    let finfo = Hashtbl.find fld_table fname in
    finfo
  with Not_found -> 
    dummy_fieldinfo

(* Export in a file the environment of types. 
 * The export is done in a dot format for lisibility.
 *)
let rec fprint_type
    (fmt:Format.formatter)
    (tname:string)
    (tinfo:typeinfo)
    : unit
    =
  begin
    (* Print type information: *)
    (* node name *)
    Format.fprintf fmt "\n \"%s\" " tname;
    (* node label: first the name and type id *)
    Format.fprintf fmt "[label=\"%s (aka %s, tid %d) " 
      tname tinfo.torig_name tinfo.tid;
    (* node definition *)
    (match tinfo.ttype with
	TVoid | TInt _ | TFloat _ -> ()
      | TPtr _ -> () (* ptr already in the name *)
      | TArray _ -> () (* array already in the name *)
      | TComp ci -> (* print all fields *)
	begin
	  List.iter (fprint_field_record fmt) ci.cfields;
	  List.iter (fprint_field_record fmt) ci.crecfields;
	  ()
	end
    );    
    (* end of node definition *)
    Format.fprintf fmt " \"];\n";
    (* edges = pointer fields to other types *) 
    (match tinfo.ttype with
	TVoid | TInt _ | TFloat _ -> ()
      | TPtr btid -> 
	(* edge labeled by * to the base type *)
	let bty = get_type btid in 
	begin
	  Format.fprintf fmt "\n\t\"%s\" -> \"%s\" [label=\"*\"];"
	    tname bty.tname;
	  ()
	end

      | TArray _ -> () (* array already in the name *)
      | TComp ci -> (* print all fields *)
	begin
	  Format.fprintf fmt "\n\t/* edges for type %s */" tname;
	  List.iter (fprint_field_edge fmt ci) ci.cfields;
	  Format.fprintf fmt "\n\t/* recedges for type %s */" tname;
	  List.iter (fprint_field_edge fmt ci) ci.crecfields;
	  ()
	end
    );
    ()
  end

and fprint_field_record
    (fmt:Format.formatter)
    (fid:fieldid)
    : unit
    = 
  let finfo = get_fieldinfo fid in
  let fty = get_type finfo.ftype in 
  Format.fprintf fmt "| %s %s; " fty.tname finfo.fname


and fprint_field_edge
    (fmt:Format.formatter)
    (ci:compinfo)
    (fid:fieldid)
    : unit
    = 
  let finfo = get_fieldinfo fid in
  let fti = get_type finfo.ftype in 
  match fti.ttype with
      TPtr btid ->
	let btyinfo = get_type btid in
	Format.fprintf fmt "\n\t\"struct %s\" -> \"%s\" [label=\"*%s\"];"
	  ci.cname btyinfo.tname finfo.fname
    | _ -> 
      (*MS: DEBUG *)
	Format.fprintf fmt "\n\t/* field `%s' of type `%s' in struct `%s' */"
	  finfo.fname fti.tname ci.cname
  

let fprint_types
    unit
    : unit 
    = 
  (** Build the name of the file used *)
  let fn_diag = "diag_types.dot" in
  let ch_diag = open_out fn_diag in
  let fmt = Format.formatter_of_out_channel ch_diag in
  begin
    (* header of the dot file *)
    Format.fprintf fmt "digraph types { /* %d types (%d registerd) */ \n" (Hashtbl.length typ_table) !typ_cnt;
    Format.fprintf fmt "\tgraph [rankdir=\"LR\"];\n"; (* direction of printing *)
    Format.fprintf fmt "\tnode [shape=Mrecord];\n"; (* nodes are records *)

    (* nodes and edges corresponding to each used type *)
    Hashtbl.iter (fprint_type fmt) typ_table;

    (* end of the dot file *)
    Format.fprintf fmt "\n}\n";
    flush ch_diag;
    close_out ch_diag
  end

    
(* The environment of global variables.
 *
 * It maps variables names to variable info (including its type).
 *)
let var_table : (string, varinfo) Hashtbl.t = Hashtbl.create 10
(* Counter used to provide unique identifiers typeid to types. *)
let var_cnt : int ref = ref 0

(** Add a variable to the table. *)
let add_var
    (name: string)
    (tid: typeid)
    (vinit: bool)
    : int
    =
  try 
    begin
      let vi = Hashtbl.find var_table name in
      (* var in the table, return its identifier *)
      vi.vid
    end
  with Not_found ->
    begin
      (* var not in the table, add it with a new number *)
      let vi = { vname = name;
		 vtyp = tid;
		 vid = !var_cnt;
	       } in
      begin
	var_cnt := !var_cnt + 1;
	Hashtbl.add var_table name vi;
	vi.vid
      end
    end
