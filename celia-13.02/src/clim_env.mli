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



type native_type = 
    Tunit | Tboolean | Tinteger 

type abstract_domain = AbsNone | AbsBox | AbsShape

type lc_type =
  | LCTnative of native_type
  | LCTenum of enum_info
  | LCTpointer of pointer_class * Num.num option * Num.num option
  | LCTnull
  | LCTany (* used when typing (if ... then raise E else ...): raise E is any *)


and pointer_class =
  | LCtag of struct_info * lc_type list (* struct_info, type parameters *)
  | LCroot of root_info

and enum_info =
    { 
      lc_enum_info_name : string;
      lc_enum_info_min : Num.num;
      lc_enum_info_max : Num.num;
    }

and struct_info =
    { 
            (*  lc_struct_info_params : type_var_info list; *)
              lc_struct_info_name : string;
      mutable lc_struct_info_parent : (struct_info * lc_type list) option;
      mutable lc_struct_info_hroot : struct_info;
      mutable lc_struct_info_fields : field_info list;
      mutable lc_struct_info_root : root_info option;
        (* only valid for root structures *)
    }

and root_info =
    {
      lc_root_info_name : string;
(*      mutable lc_root_info_tags : struct_info list;*)
      mutable lc_root_info_hroots : struct_info list;
(*      lc_root_info_open : bool;*)
(*      lc_root_info_kind : root_kind; *)
      mutable lc_root_info_union_size_in_bytes: int;
    }

and field_info =
    {
      lc_field_info_tag : int;
      lc_field_info_name : string;
      lc_field_info_final_name : string;
      lc_field_info_type : lc_type;
      lc_field_info_struct: struct_info;
        (* The structure in which the field is defined *)
      lc_field_info_hroot : struct_info;
        (* The root of the structure in which the field is defined *)
      lc_field_info_rep : bool; (* "rep" flag *)
      lc_field_info_abstract : bool; (* "abstract" flag *)
      lc_field_info_bitsize : int option;
        (* Size of the field in bits, optional *)
    }

type region = 
    {
      mutable lc_reg_variable : bool;
      mutable lc_reg_bitwise : bool;
      lc_reg_id : int;
      lc_reg_name : string;
      lc_reg_final_name : string;
      lc_reg_type : lc_type;
    }

type var_info = {
    lc_var_info_tag : int;
    lc_var_info_name : string;
    mutable lc_var_info_final_name : string;
    mutable lc_var_info_type : lc_type;
    mutable lc_var_info_region : region;
    mutable lc_var_info_formal : bool;
    mutable lc_var_info_assigned : bool;
    lc_var_info_static : bool;
  }

type exception_info =
    {
      lc_exception_info_tag : int;
      lc_exception_info_name : string;
      lc_exception_info_type : lc_type option;
    }

type label_info =
    {
      label_info_name : string;
      label_info_final_name : string;
      mutable times_used : int;
    }

type label =
  | LabelName of label_info
  | LabelHere
  | LabelPost
  | LabelPre
  | LabelOld


(*
Local Variables: 
compile-command: "unset LANG ; make -C .. byte"
End: 
*)
