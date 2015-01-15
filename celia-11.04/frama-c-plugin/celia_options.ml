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
    let descr = "analysis using Celia"
   end)

module ProjectName =
  EmptyString
    (struct
      let option_name = "-celia-project-name"
      let arg_name = ""
      let descr = "specify project name for Celia analysis"
     end)

module Analysis =
  False(struct
    let option_name = "-celia"
    let descr = "perform C analysis using Celia"
  end)

module ForceAdHocNormalization =
  False(struct
    let option_name = "-celia-adhoc-normalization"
    let descr =
      "enforce code normalization in a mode suitable for Celia plugin"
  end)

module CinvOpt =
  EmptyString(struct
    let option_name = "-celia-cinv-opt"
    let arg_name = ""
    let descr = "file containing Cinv options"
  end)
    
module GenOnly =
  False(struct
    let option_name = "-celia-gen-only"
    let descr = "only generates intermediate code (for developer use)"
  end)

module ToSpl =
  False(struct
    let option_name = "-celia-spl"
    let descr = "call Cinv on the SPL code (for developer use)"
  end)

module ToClim =
  False(struct
    let option_name = "-celia-clim"
    let descr = "call Cinv on the Clim code (for developer use)"
  end)

(*
Local Variables:
compile-command: "LC_ALL=C make"
End:
*)
