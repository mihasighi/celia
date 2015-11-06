(*
 * Test file: Writing a simple plugin in Frama-C (Sodium-20150201)
 *)

open Cil_types

(**
 * State of the plugin
 *)

module Ast2dot_state = State_builder.Hashtbl
	(Cil_datatype.Fundec.Hashtbl)
	(Datatype.String)
	(struct
	let name = "Ast2dot.state"
	let dependencies = [ Ast.self; Db.Value.self ]
	let size = 17
	end)
;;

(**
 * Printing of statement nodes
 *)
let print_stmt out = function
| Instr i -> Printer.pp_instr out i
| Return _ -> Format.pp_print_string out "<return>"
| Goto _ -> Format.pp_print_string out "<goto>"
| Break _ -> Format.pp_print_string out "<break>"
| Continue _ -> Format.pp_print_string out "<continue>"
| If (e,_,_,_) -> Format.fprintf out "if %a" Printer.pp_exp e
| Switch(e,_,_,_) -> Format.fprintf out "switch %a" Printer.pp_exp e
| Loop _ -> Format.fprintf out "<loop>"
| Block _ -> Format.fprintf out "<block>"
| UnspecifiedSequence _ -> Format.fprintf out "<unspecified sequence>"
| TryFinally _ | TryExcept _ | TryCatch _ -> Format.fprintf out "<try>"
| Throw _ -> Format.fprintf out "<throw>"


(**
 * Visitor of the AST
 *)
class print_cfg out = object
  inherit Visitor.frama_c_inplace
  
  method! vfile _ =
    Format.fprintf out "@[<hov 2>digraph cfg {@ ";
    Cil.DoChildrenPost (fun f -> Format.fprintf out "}@]@."; f)

  method! vglob_aux g =
    match g with
    | GFun(f,_) ->
        Format.fprintf out "@[<hov 2>subgraph cluster_%a {@ \
		@[<hv 2>graph@ [label=\"%a\"];@]@ "
	Printer.pp_varinfo f.svar
	Printer.pp_varinfo f.svar;
	Cil.DoChildrenPost(fun g -> Format.fprintf out "}@]@ "; g)
    | _ -> Cil.SkipChildren

  method! vstmt_aux s =
    let color =
      if Db.Value.is_computed () then
        let state = Db.Value.get_stmt_state s in
        let reachable = Db.Value.is_reachable state in
        if reachable then "fillcolor=\"#ccffcc\" style=filled"
        else "fillcolor=pink style=filled"
        else ""
    in
    Format.fprintf out "@[s%d@ [label=%S %s]@];@ "
      s.sid (Pretty_utils.to_string print_stmt s.skind) color;
    (** MS: changed the general code to add colors on edges: red for false *)
    (* List.iter
      (fun succ -> Format.fprintf out "@[s%d -> s%d;@]@ " s.sid succ.sid)
      s.succs; *)
    (match s.succs with
     | [] -> ()
     | sc::sl -> 
       begin
         Format.fprintf out "@[s%d -> s%d;@]@ " s.sid sc.sid;
	 List.iter
           (fun succ -> Format.fprintf out "@[s%d -> s%d [color=red];@]@ " s.sid succ.sid)
           sl
        end
    );
    Format.fprintf out "@]";
    Cil.DoChildren
end

(**
 * Print one function
 *)
let tostring_fun fundec =
	Ast2dot_options.Self.feedback "Computing CFG for function %s" (fundec.svar.vorig_name);
	ignore
	(Visitor.visitFramacFunction (new print_cfg Format.str_formatter) fundec);
	Format.flush_str_formatter ()

let tostring_fun_memoized = Ast2dot_state.memo tostring_fun

let print_fun fundec fmt =
	Format.fprintf fmt "@[<hov 2>digraph cfg {%s}@]@\n"
		(tostring_fun_memoized fundec)


(**
 * Print the full graph
 *)
let print_all fmt = 
	Visitor.visitFramacFileSameGlobals (new print_cfg fmt) (Ast.get ())

