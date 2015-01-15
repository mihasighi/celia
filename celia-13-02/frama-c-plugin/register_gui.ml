(**************************************************************************)
(*                                                                        *)
(*  This file is part of CINV.                                            *)
(*                                                                        *)
(*  Copyright (C) 2012                                                    *)
(*    LIAFA (University of Paris Diderot and CNRS)                        *)
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
(*  See the GNU Lesser General Public License version 3                   *)
(*  for more details (enclosed in the file licenses/LICENSE).             *)
(*                                                                        *)
(**************************************************************************)

(* Import from Cil and Frama-c *)
open Cil_types
open Cil
open CilE
open Format
open Db
open Db_types
open Pretty_source
open Gtk_helper


let select_kf tree_view kf =
  let vi = Kernel_function.get_vi kf in
  tree_view#select_global vi

let pretty_offsetmap lv fmt offsetmap =
  begin match offsetmap with
  | None ->  Format.fprintf fmt "<BOTTOM>"
  | Some off ->
      Format.fprintf fmt "%a%a"
	!Ast_printer.d_lval lv
	(Cvalue_type.V_Offsetmap.pretty_typ
	    (Some (typeOfLval lv)))
	off
  end

let gui_annot_action (main_ui:Design.main_window_extension_points) txt =
  let tag_style_italic = Gtk_helper.make_tag main_ui#annot_window#buffer ~name:"slicing:style italic" [`STYLE `ITALIC] in
  main_ui#annot_window#buffer#insert ~tags:[tag_style_italic] ((txt ()) ^ "\n")


let gui_compute_values  (main_ui:Design.main_window_extension_points) =
  if not (Db.Value.is_computed ())
  then main_ui#launcher ()

(*MS: table to be filled with the results of the analysis *)
module Shape_States =
  State_builder.Hashtbl(Cil_datatype.Stmt.Hashtbl)
    (Relations_type.Model) (*MS: TODO Put a file number *)
    (struct
       let name = "Register_gui.Shape_states"
       let dependencies = [Db.Value.self] (*MS: TODO Celia *)
       let kind = `Correctness
       let size = 257
     end)

(*MS: TODO fill the shape states computed *)
(* We unconditionnally monitor the states after a statement to
   correctly display the "After statement" information of the gui *)
let () = 
  Db.Value.Record_Value_After_Callbacks.extend
    (fun (_stack, h) ->
       Cil_datatype.Stmt.Hashtbl.iter
         (fun stmt state ->
            try
              let prev = After_States.find stmt in
              After_States.replace stmt (Relations_type.Model.join prev state)
            with Not_found ->
              After_States.add stmt state
         ) h
    )
              


let rec to_do_on_select
    (popup_factory:GMenu.menu GMenu.factory)
    (main_ui:Design.main_window_extension_points) button_nb selected
    =
  let inset_utf8 = Unicode.inset_string () in

  let annot = main_ui#annot_window#buffer in
  if button_nb = 1 then
    begin
      if Db.Value.is_computed ()
      then begin
	  match selected with
	  | PStmt (_kf,ki) -> begin
		(* Find kinstr and kf *)
		  (* Is it an accessible statement ? *)
		  if Db.Value.is_accessible (Kstmt ki) then begin
		      (* Out for this statement *)
		      let outs = Db.Outputs.kinstr (Kstmt ki) in
		      let n = ( match outs with
                      | Some outs ->
                          Pretty_utils.sfprintf
			    "Modifies @[<hov>%a@]@\n" Db.Outputs.pretty outs
                      | _ -> "\n");
		      in annot#insert n
		    end else annot#insert "This code is dead\n";
            end
	  | PLval (_kf, ki,lv) ->
              if not (isFunctionType (typeOfLval lv))
              then begin
		  (try
		      let offsetmap =
			!Db.Value.lval_to_offsetmap ~with_alarms:CilE.warn_none_mode ki lv
		      in
		      annot#insert (Pretty_utils.sfprintf
				       "Before statement:@\n%a@\n"
				       (pretty_offsetmap lv ) offsetmap);
		    with Lmap.Cannot_copy ->
		      let value = !Db.Value.access ki lv in
		      annot#insert (Pretty_utils.sfprintf "Before statement:@\n%a %s %a@\n"
				       !Ast_printer.d_lval lv inset_utf8 Db.Value.pretty value));
		  ((* Ugly hack: we use this condition to determine
                      if the after states have been computed *)
                    if After_States.length () > 0 then
                      match ki with
                        | Kstmt ({ skind = Instr _} as stmt) ->
                            let state =
                              try After_States.find stmt
                              with Not_found -> Relations_type.Model.bottom
                            in
		            let offsetmap_after =
                              !Db.Value.lval_to_offsetmap_state state lv in
		            annot#insert (Pretty_utils.sfprintf
                                            "After statement:\n%a\n"
                                            (pretty_offsetmap lv)
                                            offsetmap_after);
                        | Kglobal | Kstmt _ -> ()
                    else
                      try
                        let offsetmap_after =
                          !Db.Value.lval_to_offsetmap_after ki lv in
                        annot#insert "At next statement:\n";
                        annot#insert (Pretty_utils.sfprintf "%a\n"
                                        (pretty_offsetmap lv) offsetmap_after);
                      with Not_found -> ()
                  );
		end
	  | PTermLval _ -> () (* JS: TODO (?) *)
	  | PVDecl (_kf,_vi) -> ()
	  | PGlobal _  | PIP _ -> ()
	end
    end
  else if button_nb = 3
  then begin
      match selected with
      | PVDecl (_,vi) ->
	  begin
	    try
	      let kfun = Globals.Functions.get vi in
	      if Db.Value.is_computed ()
	      then
		let callers = !Value.callers kfun in
		(* popup a menu to jump to the definitions of the callers *)
		let do_menu l =
		  try
		    List.iter
		      (fun (v,call_sites) ->
			let v = Kernel_function.get_vi v in
			let nb_sites = List.length call_sites in
			let label = "Go to caller " ^
			  (Pretty_utils.escape_underscores
			      (Pretty_utils.sfprintf "%a"
				  Ast_info.pretty_vname v))
			in
			let label =
			  if nb_sites > 1
			  then
			    label ^ " (" ^ (string_of_int nb_sites) ^" call sites)"
			  else label
			in
			ignore
			  (popup_factory#add_item
			      label
			      ~callback:
			      (fun () -> main_ui#file_tree#select_global v)))
		      l;
		  with Not_found -> ()
		in
		do_menu callers
	      else
		ignore
		  (popup_factory#add_item
		      "Callers ..."
		      ~callback:
		      (fun () -> (gui_compute_values main_ui)))

	    with Not_found ->
	      ()
          end

      | PStmt (kf,ki) ->
	  if Db.Value.is_computed ()
	  then begin
              let eval_expr () =
		let txt =
		  GToolbox.input_string
                    ~title:"Evaluate"
                    "  Enter an ACSL expression to evaluate  "
		    (* the spaces at beginning and end should not be necessary
		       but are the quickest fix for an aesthetic GTK problem *)
		in
		match txt with
		| None -> ()
		| Some txt -> try
	              let exp =
			!Db.Properties.Interp.term_to_exp ~result:None
			  (!Db.Properties.Interp.expr kf ki txt)
	              in
                      begin match exp.enode with
                      | Lval lv | StartOf lv ->
			  to_do_on_select popup_factory main_ui 1 (PLval((Some kf),Kstmt ki,lv))
                      | _ ->
			  let loc =
			    !Db.Value.access_expr
                              (Kstmt ki)
                              exp
			  in
			  let txt =
			    Format.sprintf
                              "Before the selected statement, all the values taken by the expression %s are contained in %s@\n"
		              (Pretty_utils.sfprintf "%a" !Ast_printer.d_exp exp)
                              (Pretty_utils.sfprintf "%a" Cvalue_type.V.pretty loc)
			  in
			  annot#insert txt
                      end
		  with e ->
	            main_ui#error "Invalid expression: %s" (Cmdline.protect e)
              in
              begin
		try
		  ignore
		    (popup_factory#add_item "_Evaluate expression"
			~callback:eval_expr)
		with Not_found -> ()
              end
	    end
	  else
	    ignore
	      (popup_factory#add_item
		  "_Evaluate expression ..."
		  ~callback:
		  (fun () -> (gui_compute_values main_ui)))
      | PLval (_kf, ki, lv) ->
          let ty = typeOfLval lv in
          (* Do special actions for functions *)
          begin
	    (* popup a menu to jump the definitions of the given varinfos *)
	    let do_menu l =
	      match l with
	      | [] -> ()
	      | _ ->
                  try
		    List.iter
		      (fun v ->
                        ignore
                          (popup_factory#add_item
			      ("Go to definition of " ^
                                  (Pretty_utils.escape_underscores
				      (Pretty_utils.sfprintf "%a"
					  Ast_info.pretty_vname v))
				^ " (indirect)")
			      ~callback:
			      (fun () ->
                                main_ui#file_tree#select_global v)))
		      l ;
                  with Not_found -> ()
	    in
	    (match lv with
	    | Var _,NoOffset when isFunctionType ty ->
                (* simple literal calls are done by [Design]. *)
                ()
	    | Mem ({ enode = Lval lv}), NoOffset  ->
                if isFunctionType ty then
                  (* Function pointers *)
	          begin try
		      (* get the list of exact bases in the values *)
		      let value,_exact =
			Cvalue_type.V.find_exact_base_without_offset
                          (!Db.Value.access ki lv)
		      in
		      let functions =
			List.fold_left
                          (fun acc ->
			    (function
			      | Base.Var (vi,_) -> vi::acc
			      | _ -> acc))
                          []
                          value
		      in
		      do_menu functions

		    with Not_found -> ()
                  end
            | _ -> ()
            )
          end
      | PTermLval _ -> () (* No C function calls in logic *)
      | PGlobal _ -> ()
      | PIP _ -> ()
    end

module UsedVarState =
  Cil_state_builder.Varinfo_hashtbl
    (Datatype.Bool)
    (struct
       let size = 17
       let name = "Value.Gui.UsedVarState"
       let dependencies = [ !Db.Inputs.self_external;
                            !Db.Outputs.self_external; ]
       let kind = `Internal
     end)

let used_var var =
  try
    UsedVarState.find var
  with Not_found ->
    let return v = UsedVarState.add var v; v in
    try
      let f = fst (Globals.entry_point ()) in
      let inputs = !Db.Inputs.get_external f
      and outputs = !Db.Outputs.get_external f in
      let b = Base.create_varinfo var in
      return (Locations.Zone.mem_base b inputs ||
              Locations.Zone.mem_base b outputs)
    with e ->
      Gui_parameters.error ~once:true
        "Exception during usability analysis of var %s: %s"
        var.vname (Printexc.to_string e);
      return true (* No really sane value, so in doubt... *)


let reset (main_ui:Design.main_window_extension_points) =
  Globals.Functions.iter
      (fun kf ->
         try
           let vi = Kernel_function.get_vi kf in

           main_ui#file_tree#set_global_attribute
             ~strikethrough:(Value.is_computed () && not (!Value.is_called kf))
             vi
         with Not_found -> ());
  Globals.Vars.iter
    (fun vi _ ->
       if vi.vlogic = false then
         main_ui#file_tree#set_global_attribute
           ~strikethrough:(Value.is_computed () && not (used_var vi))
           vi
    );
  List.iter
    (fun file ->
       (* the display name removes the path *)
       let name, _globals = Globals.FileIndex.find file in
       let globals_state = main_ui#file_tree#get_file_globals name in
       main_ui#file_tree#set_file_attribute
         ~strikethrough:(Value.is_computed () &&
                         List.for_all snd globals_state)
         name
    )
    (Globals.FileIndex.get_files ())

module DegeneratedHighlighted =
  State_builder.Option_ref
    (Pretty_source.Localizable)
    (struct
       let name = "Value_gui.DegeneratedHighlightedState"
       let dependencies = [ Ast.self ]
       let kind = `Internal
     end)

let main (main_ui:Design.main_window_extension_points) =

  (* reset attributes for the list of source files *)
  reset main_ui;

  let value_selector
      menu (main_ui:Design.main_window_extension_points) ~button localizable =
    to_do_on_select
      menu
      main_ui
      button
      localizable
  in
  main_ui#register_source_selector value_selector;

  let highlighter (buffer:GSourceView2.source_buffer) localizable ~start ~stop =
    (* highlight the degeneration point *)
    Extlib.may
      (fun loc ->
         if Cilutil.equals localizable loc then
           let orange_area = make_tag
             buffer
             ~name:"degeneration"
             [`BACKGROUND "orange" ]
           in
           apply_tag buffer orange_area start stop)
      (DegeneratedHighlighted.get_option ());

    (* highlight dead code areas if values were computed.*)
    if Db.Value.is_computed () then
      let ki = ki_of_localizable localizable in
      if not (Value.is_accessible ki) then
        let dead_code_area =
          make_tag
            buffer
            "deadcode"
            [`BACKGROUND "tomato";
             `STRIKETHROUGH true;
             `STYLE `ITALIC;]
        in
        apply_tag buffer dead_code_area start stop
  in
  main_ui#register_source_highlighter highlighter

let () =
  Design.register_extension main;
  Design.register_reset_extension reset

(*
Local Variables:
compile-command: "make -C ../.."
End:
*)
