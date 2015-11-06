(*
 * Test file: Writing a simple plugin in Frama-C (Sodium-20150201)
 *)

let ast2dot_msg = "output a dot file with the CFG of the program"

module Self = Plugin.Register
	(struct
	let name = "Ast to Dot translator"
	let shortname = "ast2dot"
	let help = ast2dot_msg
	end)

module OptEnabled = Self.False
	(struct
	let option_name = "-ast2dot"
	let help = "when on (off by default), " ^ ast2dot_msg
	end)

module OptOutFile = Self.String
	(struct
	let option_name = "-ast2dot-output"
	let default = "-"
	let arg_name = "output-file"
	let help =
	"file where the CFG in dot format is output (default: output to the console)"
	end)

