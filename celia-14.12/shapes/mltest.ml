(* mltest.ml
 * 
 * A very simple example
 *
 * CINV Library / Shape Domain 
 *
 * Copyright (C) Antoine Mine' 2007
 * Modified by Mihaela Sighireanu 2009
 *)


(* compile with:

ocamlc -I ${INSTALL_DIR}/lib \
   gmp.cma apron.cma shape.cma mltest.ml \
   -cclib -lshape -cclib -lapron

you can change:
- INSTALL_DIR to where APRON is installed,
- ocamlc to ocamlopt, and replace each .cma to the corresponding .cmxa

*)

open Apron

let m = Shape.manager_alloc ()
let _ = 
  Printf.printf "name: %s\nversion: %s\n" 
    (Manager.get_library m) (Manager.get_version m)

(* TO BE CONTINUED *)
