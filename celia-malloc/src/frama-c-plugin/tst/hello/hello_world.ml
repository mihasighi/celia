(*
 * Test file: Writing a simple plugin in Frama-C (Sodium-20150201)
 *)

open Hello_options

let run () =
	if OptEnabled.get() then
		Hello_print.print_hello ()

let () = Db.Main.extend run

   
