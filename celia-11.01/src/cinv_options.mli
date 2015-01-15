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
   Changed by Mihaela Sighireanu for Cinv in 2009.
*)
(**  Argument, Options and Parsing of command line *)

open Format 

val inputfilename : string ref
  (** input filename *)
val debug : int ref
  (** debug level *)
val feedback : int -> ('a, formatter, unit) format -> 'a
  (** messages *)

(*  ---------------------------------------------------------------------- *)
(** {3 Display style} *)
(*  ---------------------------------------------------------------------- *)

type displaytags = {
  precolorB : string;
  precolorR : string;
  precolorG : string;
  postcolor : string;
}
val texttags : displaytags
val colortags : displaytags
val htmltags : displaytags

val displaytags : displaytags ref

(*  ---------------------------------------------------------------------- *)
(** {3 Choice of abstract domain} *)
(*  ---------------------------------------------------------------------- *)

type domain =
    Box
  | Shape
val assocnamedomain : (string * domain) list
val domain : domain ref

type dwdomain =
    LSum
  | MSet
  | UCons
val assocnamedwdomain : (string * dwdomain) list
val dwdomain : dwdomain list ref

val max_anon : int ref
val max_anon_segm : int ref

type fun_spec_ty = { fanon: int; fsegm: int; fpatt: int; }
val fun_spec : (string * fun_spec_ty) list ref
val file_fun_spec : string ref
val print_fun_spec : Format.formatter -> (string * fun_spec_ty) list -> unit
val get_fun_spec : string -> int

(*  ---------------------------------------------------------------------- *)
(** {3 Choice of the input format and engine} *)
(*  ---------------------------------------------------------------------- *)

type encoding = Spl | Clim
val engine : encoding ref

(*  ---------------------------------------------------------------------- *)
(** {4 Choice of analysis type} *)
(*  ---------------------------------------------------------------------- *)

type analysis =
	| Intraproc
	| Interproc
val assocnameanalysis : (string * analysis) list
val analysis : analysis ref

(*  ---------------------------------------------------------------------- *)
(** {6 Solving tuning} *)
(*  ---------------------------------------------------------------------- *)

val iteration_depth : int ref
  (** Depth of recursion in iteration. If the depth is deeper, one tries to
    stabilize inner loops first before propagating to enclosing loops.*)

val iteration_guided : bool ref
  (** Guided iteration technique *)

val widening_start : int ref
  (** Number of steps without widening *)

val widening_descend : int ref
  (** Number of descending iterations *)

val main_entry : string ref
  (** Name of the entry function in the graph *)

(*  ---------------------------------------------------------------------- *)
(** {7 Speclist} *)
(*  ---------------------------------------------------------------------- *)

val speclist : (Arg.key * Arg.spec * Arg.doc) list
