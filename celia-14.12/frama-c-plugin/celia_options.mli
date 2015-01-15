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


open Plugin
include Plugin.S

module ProjectName: Parameter_sig.String
module Analysis: Parameter_sig.Bool
module ForceAdHocNormalization: Parameter_sig.Bool
module CinvOpt: Parameter_sig.String
module UseSL3: Parameter_sig.Bool
module GenOnly: Parameter_sig.Bool
module ToSpl: Parameter_sig.Bool
module ToClim: Parameter_sig.Bool

(*
Local Variables:
compile-command: "LC_ALL=C make"
End:
*)
