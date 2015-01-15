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
(**  Argument, Options and Parsing of command line} *)

open Format

(** input filename *)
let inputfilename = ref ""

(** debug level *)
let debug = ref 0

let feedback level =
  (if level <= !debug then
      (fprintf std_formatter "[cinv] "; fprintf std_formatter)
   else
      (* (eprintf err_formatter "[cinv] "; eprintf) *)
      (ifprintf err_formatter "[cinv] "; ifprintf err_formatter) 
  )

(*  ---------------------------------------------------------------------- *)
(** {3 Display style} *)
(*  ---------------------------------------------------------------------- *)

type displaytags = {
  precolorB : string;
  precolorR : string;
  precolorG : string;
  postcolor : string;
}
let (texttags:displaytags) = {
  precolorB=""; 
  precolorR=""; 
  precolorG=""; 
  postcolor="";
}
let (colortags:displaytags) = {
  precolorB ="\027[32m";
  precolorR = "\027[31m"; 
  precolorG="\027[34m";
  postcolor="\027[m";
}
let (htmltags:displaytags) = {
  precolorB ="<span style=\"color:blue;\">";
  precolorR ="<span style=\"color:red;\">";
  precolorG="<span style=\"color:green;\">";
  postcolor="</span>";
}

let (assocnamedisplaytags : (string * displaytags) list) =
  [
    ("text",texttags);
    ("color",colortags);
    ("html",htmltags);
  ]
let (lnamedisplaytags : string list) =
  List.map
    (fun (name,_) -> name)
    assocnamedisplaytags

let displaytags = ref colortags

(*  ---------------------------------------------------------------------- *)
(** {3 Choice of abstract domain} *)
(*  ---------------------------------------------------------------------- *)
type domain =
| Box
| Shape
let (assocnamedomain : (string * domain) list) =
  [
    ("box",Box);
    ("shape",Shape)
  ]
let (lnamedomain : string list) =
  List.map
    (fun (name,_) -> name)
    assocnamedomain
let domain = ref Shape (** abstract domain to use *)

type dwdomain =
| LSum
| MSet
| UCons
let (assocnamedwdomain : (string * dwdomain) list) =
  [
    ("lsum",LSum);
    ("mset",MSet);
    ("ucons",UCons)
  ]
let (lnamedwdomain : string list) =
  List.map
    (fun (name,_) -> name)
    assocnamedwdomain
let dwdomain = ref [] (** abstract domain to use *)

let max_anon = ref 0
let max_anon_segm = ref 1
type fun_spec_ty = { fanon: int; fsegm: int; fpatt: int; }
let fun_spec = ref []
let file_fun_spec = ref "cinv_fun_spec.txt"

let print_fun_spec fmt fspec =
  List.iter 
    (fun e -> Format.fprintf fmt "%s: %d,%d,%d\n" 
      (fst e) (snd e).fanon (snd e).fsegm (snd e).fpatt) fspec

let get_fun_spec fname =
  let fa,fs,fp =
    try
      let fspec = List.assoc fname !fun_spec in
      fspec.fanon, fspec.fsegm, fspec.fpatt
    with Not_found ->
      !max_anon, !max_anon_segm, 1
  in
  feedback 1 "Get fun spec %s (%d,%d,%d)\n" fname fa fs fp;
  (fa land (int_of_string "0xf")) lor 
    ((fs lsl 4) land (int_of_string "0xf0")) lor 
    (fp lsl 8)


(*  ---------------------------------------------------------------------- *)
(** {3 Choice of engine} *)
(*  ---------------------------------------------------------------------- *)
type encoding =
  | Spl
  | Clim
let (assocnameengine : (string * encoding) list) =
  [
    ("spl",Spl);
    ("c",Clim)
  ]
let (lnameengine : string list) =
  List.map
    (fun (name,_) -> name)
    assocnameengine
let engine = ref Spl

(*  ---------------------------------------------------------------------- *)
(** {4 Choice of analysis type} *)
(*  ---------------------------------------------------------------------- *)
type analysis =
  | Intraproc
  | Interproc
let (assocnameanalysis : (string * analysis) list) =
  [
    ("intra",Intraproc);
    ("inter",Interproc)
  ]
let (lnameanalysis : string list) =
  List.map
    (fun (name,_) -> name)
    assocnameanalysis
let analysis = ref Intraproc

(*  ---------------------------------------------------------------------- *)
(** {6 Solving tuning} *)
(*  ---------------------------------------------------------------------- *)

let iteration_depth = ref 2
let iteration_guided = ref false
let widening_start = ref 1
let widening_descend = ref 2

let main_entry = ref "main"

(*  ---------------------------------------------------------------------- *)
(** {7 Main specification list} *)
(*  ---------------------------------------------------------------------- *)

let (speclist:(Arg.key * Arg.spec * Arg.doc) list) =
  Arg.align
    [
      (
	"-debug",
	Arg.Set_int(debug),
	"<int> : debug level, from 0 to 4 (default: 0)"
      );
      (
	"-domain",
	Arg.Symbol(
	  lnamedomain,
	  begin fun name -> domain := List.assoc name assocnamedomain end
	),
	" : abstract domain to use (default: shape)"
      );
      (
	"-dwdomain",
	Arg.Symbol(
	  lnamedwdomain,
	  begin fun name -> 
	    dwdomain := List.append !dwdomain [(List.assoc name assocnamedwdomain)] end
	),
	" : abstract domain used for list segments (default: lsum)"
      );
      (
	"-maxanon",
	Arg.Int(begin fun n ->
	  if n < 0 then
	    raise (Arg.Bad ("Wrong argument `"^(string_of_int n)^"'; option `-maxanon' expects an integer >=0"))
	  else
	    max_anon := n
	end),
	"<int> : default maximum number of anonymous nodes per segment (default 0, may only be more)"
      );
      (
	"-maxasegm",
	Arg.Int(begin fun n ->
	  if n < 1 then
	    raise (Arg.Bad ("Wrong argument `"^(string_of_int n)^"'; option `-maxasegm' expects an integer >=1"))
	  else
	    max_anon_segm := n
	end),
	"<int> : default maximum number of segments with anonymous nodes (default 1, may only be more)"
      );
      (
	"-funspec",
	Arg.Set_string(file_fun_spec),
	"<file> : file containing parameters for functions analysis"  
      );
      (
	"-depth",
	Arg.Int(begin fun n ->
	  if n<2 then
	    raise (Arg.Bad ("Wrong argument `"^(string_of_int n)^"'; option `-depth' expects an integer >= 2"))
	  else
	    iteration_depth := n
	end),
	"<int> : depth of recursive iterations (default 2, may only be more)"
      );
      (
	"-guided",
	Arg.Bool(begin fun b -> iteration_guided := b end),
	"<bool> : if true, guided analysis of Gopand and Reps (default: false)"
      );
      (
	"-wdelay",
	Arg.Int(begin fun n ->
	    if n<0 then
	      raise (Arg.Bad ("Wrong argument `"^(string_of_int n)^"'; option `-wdelay' expects a positive integer for its `widening start' argument"))
	    else
	      widening_start := n
        end),
	" : specifies usage of widening delay steps (default: 1)"
      );
      (
	"-wdesc",
	  Arg.Int(begin fun n ->
	    if n<0 then
	      raise (Arg.Bad ("Wrong argument `"^(string_of_int n)^"'; option `-wdesc' expects a positive integer for its `descending' argument"))
	    else
	      widening_descend := n
	  end),
	" : specifies usage of widening number of descending steps (default: 2)"
      );
      (
	"-display",
	Arg.Symbol(
	  lnamedisplaytags,
	  begin fun name -> displaytags := List.assoc name assocnamedisplaytags end
	),
	" : display style (default: color)"
      );
      (
	"-margin",
	Arg.Int(fun n -> Format.set_margin n),
	" : nb of columns for text display"
      );
      (
	"-engine",
	Arg.Symbol(
	  lnameengine,
	  begin fun name -> engine := List.assoc name assocnameengine end
	),
	" : engine to use (default: spl)"
      );
      (
	"-analysis",
	Arg.Symbol(
	  lnameanalysis,
	  begin fun name -> analysis := List.assoc name assocnameanalysis end
	),
	" : kind of analysis inter- or intra-procedural (default: intra)"
      );
      (
	"-main",
	Arg.Set_string(main_entry),
	"<fun> : entry point function for the analysis (default: main)"
      )
    ]
