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
open Cinv_equation
open Cinv_solving

(*  ********************************************************************** *)
(** {Parsing input file} *)
(*  ********************************************************************** *)

let parse_lexbuf (** TODO: for the moment we use Frama-c **)
  (fmt:Format.formatter)
  (lexbuf:Lexing.lexbuf)
  : 
  unit 
  = ()

(*  ********************************************************************** *)
(** {Reading options} *)
(*  ********************************************************************** *)

let option_read = ref []
let add_option 
    (kv:string list)
    :
    unit
    =
  match (List.length kv) with
    |  2 ->
	let lc_key = String.lowercase (List.nth kv 0) in
	let lc_val = List.nth kv 1 in
	begin
	  (* printf "\noption read %s=%s\n" lc_key lc_val; *)
	  option_read := ("-"^lc_key)::lc_val::!option_read;
	  ()
	end
    | 0 -> ()
    | _ ->  Cinv_options.feedback 0 "Bad option in configuration file.\n"

let read_config 
    (configfile:string)
    :
    unit
    =
  let rec read_option ch =
    try
      let lstr = input_line ch in
      let kv = Str.split (Str.regexp "[ =]+") lstr in
      (* printf "Read line: %s\n" lstr; *)
      add_option kv;
      read_option ch
    with End_of_file -> ()
  in
  begin
    let ch = open_in configfile in
    begin
      Cinv_options.feedback 1 "Read options from file %s.\n" configfile;
      read_option ch;
      close_in ch;
      Cinv_options.feedback 1 "Options read: %a\n"
	(fun fmt -> List.iter (fun s -> Format.fprintf fmt " %s " s)) 
	!option_read;
	(* Arg.parse_argv ignore the first element of the array of arguments *)
      let argv = Array.of_list ("cinv"::!option_read) in
      Arg.parse_argv argv 
	Cinv_options.speclist 
	(fun s -> ()) "";
      option_read := []
    end
  end

let read_fun_spec ch =
  let add_fun_spec kv = 
    (* The string read are of the form: fun_name int int pXX pXX ... *)
    if List.length kv < 3 then
      Cinv_options.feedback 0 "Wrong function specification (line %d)\n" 
	(List.length !fun_spec)
    else
      begin
	let fn = List.nth kv 0 in
	let fa = int_of_string (List.nth kv 1) in
	let fs = int_of_string (List.nth kv 2) in
	let fp = 
	  (if (List.length kv >= 4) then 
	      (int_of_string (List.nth kv 3))
	   else
	      1)
	in
	fun_spec := (fn, {fanon=fa; fsegm=fs; fpatt=fp;})::!fun_spec
      end
  in
  let rexp = Str.regexp "[ :,]+" in
  let rec read_spec ch =
    (try
      let lstr = input_line ch in
      let kv = Str.split rexp lstr in
      add_fun_spec kv;
      read_spec ch
    with End_of_file -> ())
  in
  begin
    read_spec ch;
    Cinv_options.feedback 1 "Read specifications for functions...%a\n" Cinv_options.print_fun_spec !fun_spec
  end

let write_config 
    unit
    : 
    unit
    =
  begin
  (* Produce file cinv.txt read by the shape domain *)
    (try
      let ch = open_out "cinv.txt" in
      Printf.fprintf ch "%d %d " !Cinv_options.max_anon !Cinv_options.max_anon_segm ;
      List.iter (fun dwd ->
	begin match dwd with
	  | LSum -> Printf.fprintf ch " LSUM "
	  | MSet -> Printf.fprintf ch " MSET "
	  | UCons -> Printf.fprintf ch " UCONS "
	end)
	!Cinv_options.dwdomain;
      close_out ch;
    with _ -> 
      Cinv_options.feedback 0 
	"Error while writing the Shape configuration file (cinv.txt)"
    );
      (* Read parameters of analysis for each function *)
    try
      let ch = open_in !file_fun_spec in
      read_fun_spec ch;
      close_in ch
    with _ ->
      Cinv_options.feedback 0 
	"Function specification file `%s' not found, by default parameters are used!\n" !file_fun_spec
  end

(*  ********************************************************************** *)
(** {Analyzing and displaying the solution} *)
(*  ********************************************************************** *)

let compute_and_display
  (fmt:Format.formatter)
  (fgraph:Cinv_equation.graph) 
  (manager:'a Apron.Manager.t)
  :
  unit
  =  
  let fp_out = 
    Cinv_solving.Forward.compute fgraph manager ~debug:!Cinv_options.debug
  in
    PSHGraph.iter_vertex fp_out
      (begin fun vtx abs ~pred ~succ ->
	fprintf fmt "%a: %a@."
          Cinv_equation.print_point vtx 
	  Apron.Abstract1.print abs
       end)


(* Entry for computing solution *)
let analyze_and_display
  (configfile:string)
  (fmt:Format.formatter)
  (fgraph:Cinv_equation.graph)
  :
  unit
  =
  begin
    read_config configfile;
    write_config ();
    match !Cinv_options.domain with
      | Box ->
	compute_and_display fmt fgraph
	  (Box.manager_alloc ())
      | Shape ->
	compute_and_display fmt fgraph
	  (Shape.manager_alloc ()) 
  end


