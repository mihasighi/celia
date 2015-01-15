#! /bin/sh

make -C interproc-dist all
make -C interproc-dist install
make -C apron-dist all
make -C apron-dist install
make -C shapes all
make -C shapes ml
make src/clim.cma
make -C frama-c-plugin frama-c-Celia.byte
mv frama-c-plugin/frama-c-Celia.byte bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CINV/shapes:$CINV/apron-dist/mlgmpidl/lib:$CINV/apron-dist/apron/lib

