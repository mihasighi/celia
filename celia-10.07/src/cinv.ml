(** Master module *)

(* This file is part of the Interproc analyzer, released under GPL license.
   Please read the COPYING file packaged in the distribution.

*)

open Format
open Cinv_options

let analyze_and_display
  (fmt:Format.formatter)
  (prog:Spl_syn.program)
  :
  unit
  =
  (* Build graphs from the program *)
  let (fgraph,bgraph) = Frontend.build_graphs prog in

  (* Choose the appropriate manager *)
  (* Compute the solution *)
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
  (* Parsing the program *)
  let input = open_in !Cinv_options.inputfilename in
  let lexbuf = Lexing.from_channel input in
  lexbuf.Lexing.lex_curr_p <- { lexbuf.Lexing.lex_curr_p with
    Lexing.pos_fname = "file "^(!Cinv_options.inputfilename);
  };
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

  (* Computing solution *)
  analyze_and_display Format.std_formatter prog;
  ()

let _ = main()
