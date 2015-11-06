(*
 * Test file: Writing a simple plugin in Frama-C (Sodium-20150201)
 *)

open Hello_options

let print_hello () = 
	let filename = OptOutFile.get () in
	let output_fun msg =
	if OptOutFile.is_default() then
		Self.result "%s" msg
	else
	  	let chan = open_out filename in
		Printf.fprintf chan "%s\n" msg;
		close_out chan
	in
		output_fun "Hello, world!"

