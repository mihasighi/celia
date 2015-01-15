#! /bin/sh

make -C interproc-dist all
make -C interproc-dist install
make -C apron-dist all
make -C apron-dist install
make -C smtlib2parser-dist all
make -C shapes all
make -C shapes sl3dp
make -C shapes ml
make -C src clim.cma
make -C frama-c-plugin frama-c-Celia.byte
mv frama-c-plugin/frama-c-Celia.byte bin
mv shapes/sl3dp bin
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CINV/shapes:$CINV/smtlib2parser-dist:$CINV/apron-dist/mlgmpidl/lib:$CINV/apron-dist/apron/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CINV/shapes:$CINV/smtlib2parser-dist:/usr/lib/ocaml/apron

