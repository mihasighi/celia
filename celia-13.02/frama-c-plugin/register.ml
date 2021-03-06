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

(* Import from Cil and Frama-c *)
open Cil_types
open Cil
open Cilutil
module FCExtlib=Extlib
module FCAst=Ast
module FCFile=File
module FCProject=Project
module FCParameters=Parameters

(* Import from Clim *)
open Clim

(* Utility functions *)
open Common

let apply_if_dir_exist name f =
  try
    let d = Unix.opendir name in
    Unix.closedir d;
    f name
  with Unix.Unix_error (Unix.ENOENT, "opendir",_) -> ()

let sys_command cmd =
  if (Sys.command cmd) <> 0 then
    begin
      Celia_options.error "Celia subprocess failed: %s" cmd; 
      raise Exit
    end

let run () =
  Celia_options.feedback "Starting Celia...";
  (* Work in our own project, initialized by a copy of the main one. *)
  (* Calls Cfg.computeFileCFG *)
  let prj =
    FCFile.create_project_from_visitor "celia"
      (fun prj -> new Visitor.frama_c_copy prj)
  in
  FCProject.copy ~selection:(Plugin.get_selection ()) prj;
  FCProject.set_current prj;

  let file = FCAst.get () in
  try

    if file.globals = [] then
      Celia_options.abort 
	"Nothing to process. There was probably an error before.";

    (* 
     * Phase 0: create the "pan" directory storing the intermediary 
     *          and result files for this analysis.
     *)
    begin
      Celia_options.feedback "===== Create the `pan' directory =====";
      let cmd = "mkdir -p pan" in
      sys_command cmd
    end;

    (* 
     * Phase 1: various preprocessing steps before C to Clim translation 
     *)

    (* Enforce the prototype of malloc to exist before visiting anything.
     * It might be useful for allocation pointers from arrays
     *)
    ignore (Common.malloc_function ());
    ignore (Common.free_function ());

    (* Rewrite ranges in logic annotations by comprehesion *)
    !Db.Properties.Interp.from_range_to_comprehension
      (Cil.inplace_visit ()) (FCProject.current ()) file;
    if checking then check_types file;

    (*
     * Phase 2: C-level rewriting to facilitate analysis 
     *)
    Celia_options.feedback "===== Rewrite C code for analysis =====";
    Rewrite.rewrite file;
    if checking then check_types file;

    if Celia_options.debug_atleast 2 then 
      begin
	Celia_options.feedback "... Result in file pan/pan-rw.c.";
	let out_chan = open_out "pan/pan-rw.c" in
	let fmt = Format.formatter_of_out_channel out_chan in
	let printer = new Printer.print () in
	Format.fprintf fmt "%a" (Cil.d_file printer) file;
	flush out_chan;
	close_out out_chan
      end;

    (* 
     * Phase 3: Caduceus-like normalization that rewrites C-level AST into
     *          Clim-like AST, still in Cil (in order to use Cil visitors)
     *)

    Celia_options.feedback "===== Normalize C code for analysis =====";
    Norm.normalize file;
    Retype.retype file;
    if checking then check_types file;

    (* 
     * Phase 4: various postprocessing steps, still on Cil AST 
     *)

    (* Rewrite ranges back in logic annotations *)
    !Db.Properties.Interp.from_comprehension_to_range
      (Cil.inplace_visit ()) (FCProject.current ()) file;

    (* this file is used to interpret the analysis results *)
    begin
      Celia_options.feedback "... Result in file pan/pan-nm.c.";
      let gd = Parameters.GeneralDebug.get () in
      let _ = Parameters.GeneralDebug.set (gd+1) in
      let out_chan = open_out "pan/pan-nm.c" in
      let fmt = Format.formatter_of_out_channel out_chan in
      let printer = new Printer.print () in
      Format.fprintf fmt "%a" (Cil.d_file printer) file;
      flush out_chan;
      close_out out_chan;
      Parameters.GeneralDebug.set gd
    end; 

    (* Phase 5: Diagnosis phase
     *          - signal the unmanaged code 
     *          - register used types and generate the map of types
     *          - compute slice informations
     *)
    Celia_options.feedback "===== Diagnosis of C used =====";
    Celia_check.check file; 
    if checking then check_types file;

    (* Phase 6: Slice unmanaged code and specs (still Cil AST) *)
    (* Celia_options.feedback "===== Slice unmanaged code and specs ====="; *)
    (*TODO: call slicing plugin of Frama-C using generated infos *)

    (* if Celia_options.debug_atleast 1 then print_to_stdout file; *)

    (* 
     * Phase 7: Clim to Spl translation 
     *)
    if Celia_options.ToSpl.get() then
      begin
	Celia_options.feedback "===== Translate to SPL =====";
	Spl_rewrite.generate file; (* TODO: replace with pfile *)
	Celia_options.feedback "... Result in file pan/pan.spl.";
	Celia_options.feedback "===== Call the analysis on the input function %s =====" 
	  (FCParameters.MainFunction.get());
	let cmd = "cinv.opt -engine spl -config "^(Celia_options.CinvOpt.get())^" pan/pan.spl" in 
	sys_command cmd
      end;

    (* 
     * Phase 8: Clim (Cil AST) to ICFG translation
     *)
    Celia_options.feedback "===== Translate C code to an ICFG =====";
    let fgraph, bgraph = Interp_eqn.make file in
    begin
      Celia_options.feedback "... Result in files pan/pan.eq and pan/pan-b.eq.";
      let out_chan = open_out "pan/pan.eq" in
      let fmt = Format.formatter_of_out_channel out_chan in
      begin
	Cinv_equation.print_graph fmt fgraph;
	flush out_chan;
	close_out out_chan
      end;
      let out_chan = open_out "pan/pan-b.eq" in
      let fmt = Format.formatter_of_out_channel out_chan in
      begin
	Cinv_equation.print_graph fmt bgraph;
	flush out_chan;
	close_out out_chan
      end
    end;

    (* 
     * Phase 9: call AI analysis on the ICFG 
     *)
    Celia_options.feedback "===== Analyze the ICGF =====";
    let out_chan = open_out "pan/pan.abs" in
    let fmt = Format.formatter_of_out_channel out_chan in
    Cinv_frontend.analyze_and_display (Celia_options.CinvOpt.get()) fmt fgraph bgraph;
    flush out_chan;
    close_out out_chan;
    Celia_options.feedback "... Result in file pan/pan.abs.";

    flush_all ()

  with Exit -> ()

(* ************************************************************************* *)
(* Plug Celia to the Frama-C platform *)
(* ************************************************************************* *)

let run_and_catch_error () =
  try run ()
  with
    | Unsupported _ as e ->
	warn_general "Unsupported feature(s).@\nCelia plugin can not be used on your code." ;
	if Celia_options.debug_atleast 1 then raise e else ()
    | Assert_failure(file,a,b) ->
	fatal
	  "Unexpected failure.@\nPlease submit bug report (Ref. \"%s:%d:%d\")."
	  file a b
    | exn ->
	fatal
	  "Unexpected exception.@\nPlease submit bug report (Ref. \"%s\")."
	  (Printexc.to_string exn)

let run_and_catch_error =
  Dynamic.register
    "run_analysis"
    (Datatype.func Datatype.unit Datatype.unit)
    ~plugin:"Celia"
    ~journalize:true
    run_and_catch_error

let main () = if Celia_options.Analysis.get () then run_and_catch_error ()
let () = Db.Main.extend main

(*
Local Variables:
compile-command: "LC_ALL=C make"
End:
*)
