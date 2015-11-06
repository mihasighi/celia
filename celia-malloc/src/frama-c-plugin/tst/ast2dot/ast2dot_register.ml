(*
 * Test file: Writing a simple plugin in Frama-C (Sodium-20150201)
 *)

let run () =
	if Ast2dot_options.OptEnabled.get() then
        let filename = if Ast2dot_options.OptOutFile.is_default() 
		       then "cfg.dot"
		       else Ast2dot_options.OptOutFile.get ()
        in
        let chan = open_out filename in
        let fmt = Format.formatter_of_out_channel chan in
	  	Ast2dot_core.print_all fmt;
		close_out chan

let () = Db.Main.extend run

   
