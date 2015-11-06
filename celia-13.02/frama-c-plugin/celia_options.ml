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



include
  Plugin.Register
  (struct
    let name = "celia"
    let shortname = "celia"
    let help = "analysis using Celia"
   end)

module ProjectName =
  EmptyString
    (struct
      let option_name = "-celia-project-name"
      let help = "specify project name for Celia analysis"
      let kind = `Correctness
      let arg_name = ""
     end)

module Analysis =
  False
    (struct
      let option_name = "-celia"
      let help = "perform C analysis using Celia"
      let kind = `Correctness
     end)

module ForceAdHocNormalization =
  False
    (struct
      let option_name = "-celia-adhoc-normalization"
      let help =
	"enforce code normalization in a mode suitable for Celia plugin"
      let kind = `Tuning
     end)

module CinvOpt =
  EmptyString
    (struct
      let option_name = "-celia-cinv-opt"
      let help = "file containing Cinv options"
      let kind = `Tuning
      let arg_name = ""
     end)
    
module GenOnly =
  False
    (struct
      let option_name = "-celia-gen-only"
      let help = "only generates intermediate code (for developer use)"
      let kind = `Tuning
     end)
    
module UseSL3 =
  False
    (struct
      let option_name = "-celia-sl3"
      let help = "use sl3 logic for annotations"
      let kind = `Tuning
     end)

module ToSpl =
  False
    (struct
      let option_name = "-celia-spl"
      let help = "call Cinv on the SPL code (for developer use)"
      let kind = `Tuning
     end)

module ToClim =
  False
    (struct
      let option_name = "-celia-clim"
      let help = "call Cinv on the Clim code (for developer use)"
      let kind = `Tuning
     end)

(*
Local Variables:
compile-command: "LC_ALL=C make"
End:
*)
