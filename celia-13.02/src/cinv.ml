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
open Cinv_options

(* ============================================================================ *)
(* Clim analysis *)
(* ============================================================================ *)

let analyze_and_display_clim
  (input:in_channel)
  :
  unit
  =
    begin
      printf "Clim analysis not yet supported. Bye!\n";
      close_in input
    end


(* ============================================================================ *)
(* Spl analysis *)
(* ============================================================================ *)

let analyze_and_display_spl
  (input:in_channel)
  :
  unit
  =
  let lexbuf = Lexing.from_channel input in
  lexbuf.Lexing.lex_curr_p <- { lexbuf.Lexing.lex_curr_p with
    Lexing.pos_fname = "file "^(!Cinv_options.inputfilename);
  };
  let fmt = Format.std_formatter in
  let prog = Frontend.parse_lexbuf Format.err_formatter lexbuf in
  close_in input;
  
  if !Cinv_options.debug>0 then
    printf "%sProgram with its control points:%s@.%a@."
      (!Cinv_options.displaytags).precolorB (!Cinv_options.displaytags).postcolor
      (PSpl_syn.print_program
	(begin fun fmt point ->
	  fprintf fmt "%s%a%s"
	  (!Cinv_options.displaytags).precolorR
	  PSpl_syn.print_point point
	  (!Cinv_options.displaytags).postcolor
	end))
      prog
  ;

  (* Build graphs from the program *)
  let (fgraph,bgraph) = Frontend.build_graphs prog in

  (* Choose the appropriate manager *)
  (* and Compute the solution *)
    begin match !Cinv_options.domain with
    | Box ->
        Frontend.compute_and_display fmt prog fgraph bgraph
        (Box.manager_alloc ()) 
    | Shape ->
        Frontend.compute_and_display fmt prog fgraph bgraph
        (Shape.manager_alloc ())  
    end
  


let main () =
  (* Parsing the command line *)
  Arg.parse
    Cinv_options.speclist
      (begin fun name -> Cinv_options.inputfilename := name end)
      "cinv <options> <inputfile>"
  ;
  (* Parsing the program depending on the format *)
  let input = open_in !Cinv_options.inputfilename in
  if !Cinv_options.engine == Spl then
    analyze_and_display_spl input
  else
    analyze_and_display_clim input
  

let _ = main()
