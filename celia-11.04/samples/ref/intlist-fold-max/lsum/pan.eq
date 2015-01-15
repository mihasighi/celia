[ { v = ../../../c/intlist-fold-max.c:(15,-2), attrv = , predh = [6],
    succh = [5] }
  { v = ../../../c/intlist-fold-max.c:(16,11), attrv = , predh = [10 11],
    succh = [12] }
  { v = ../../../c/intlist-fold-max.c:(17,12), attrv = , predh = [8],
    succh = [10] }
  { v = ../../../c/intlist-fold-max.c:(20,16), attrv = , predh = [14],
    succh = [15] }
  { v = ../../../c/intlist-fold-max.c:(16,-1), attrv = , predh = [9],
    succh = [8] }
  { v = ../../../c/intlist-fold-max.c:(15,6), attrv = , predh = [5],
    succh = [17] }
  { v = ../../../c/intlist-fold-max.c:(19,15), attrv = , predh = [13],
    succh = [14] }
  { v = ../../../c/intlist-fold-max.c:(23,18), attrv = , predh = [17],
    succh = [] }
  { v = ../../../c/intlist-fold-max.c:(21,17), attrv = , predh = [15],
    succh = [16] }
  { v = ../../../c/intlist-fold-max.c:(15,8), attrv = , predh = [4 7],
    succh = [9 11] }
  { v = ../../../c/intlist-fold-max.c:(15,9), attrv = , predh = [],
    succh = [7] }
  { v = ../../../c/intlist-fold-max.c:(14,5), attrv = , predh = [3 16],
    succh = [4 6] }
  { v = ../../../c/intlist-fold-max.c:(9,0), attrv = , predh = [],
    succh = [0] }
  { v = ../../../c/intlist-fold-max.c:(13,4), attrv = , predh = [2],
    succh = [3] }
  { v = ../../../c/intlist-fold-max.c:(12,2), attrv = , predh = [0],
    succh = [1] }
  { v = ../../../c/intlist-fold-max.c:(18,14), attrv = , predh = [12],
    succh = [13] }
  { v = ../../../c/intlist-fold-max.c:(12,3), attrv = , predh = [1],
    succh = [2] }
  { h = 0, attrh = tmp = 0., predv = [|../../../c/intlist-fold-max.c:(9,0)|],
    succv = [|../../../c/intlist-fold-max.c:(12,2)|] }
  { h = 1, attrh = xi = tmp,
    predv = [|../../../c/intlist-fold-max.c:(12,2)|],
    succv = [|../../../c/intlist-fold-max.c:(12,3)|] }
  { h = 2, attrh = xi = x, predv = [|../../../c/intlist-fold-max.c:(12,3)|],
    succv = [|../../../c/intlist-fold-max.c:(13,4)|] }
  { h = 3, attrh = m = 0, predv = [|../../../c/intlist-fold-max.c:(13,4)|],
    succv = [|../../../c/intlist-fold-max.c:(14,5)|] }
  { h = 4, attrh = IF xi - 0. <> 0,
    predv = [|../../../c/intlist-fold-max.c:(14,5)|],
    succv = [|../../../c/intlist-fold-max.c:(15,8)|] }
  { h = 5, attrh = IF true,
    predv = [|../../../c/intlist-fold-max.c:(15,-2)|],
    succv = [|../../../c/intlist-fold-max.c:(15,6)|] }
  { h = 6, attrh = IF xi - 0. = 0,
    predv = [|../../../c/intlist-fold-max.c:(14,5)|],
    succv = [|../../../c/intlist-fold-max.c:(15,-2)|] }
  { h = 7, attrh = IF true, predv = [|../../../c/intlist-fold-max.c:(15,9)|],
    succv = [|../../../c/intlist-fold-max.c:(15,8)|] }
  { h = 8, attrh = m = cast_i,n xi,
    predv = [|../../../c/intlist-fold-max.c:(16,-1)|],
    succv = [|../../../c/intlist-fold-max.c:(17,12)|] }
  { h = 9, attrh = IF cast_i,n xi - m > 0,
    predv = [|../../../c/intlist-fold-max.c:(15,8)|],
    succv = [|../../../c/intlist-fold-max.c:(16,-1)|] }
  { h = 10, attrh = IF true,
    predv = [|../../../c/intlist-fold-max.c:(17,12)|],
    succv = [|../../../c/intlist-fold-max.c:(16,11)|] }
  { h = 11, attrh = IF -(cast_i,n xi - m) >= 0,
    predv = [|../../../c/intlist-fold-max.c:(15,8)|],
    succv = [|../../../c/intlist-fold-max.c:(16,11)|] }
  { h = 12, attrh = tmp = cast_f,n xi,
    predv = [|../../../c/intlist-fold-max.c:(16,11)|],
    succv = [|../../../c/intlist-fold-max.c:(18,14)|] }
  { h = 13, attrh = xi = 0.,
    predv = [|../../../c/intlist-fold-max.c:(18,14)|],
    succv = [|../../../c/intlist-fold-max.c:(19,15)|] }
  { h = 14, attrh = xi = tmp,
    predv = [|../../../c/intlist-fold-max.c:(19,15)|],
    succv = [|../../../c/intlist-fold-max.c:(20,16)|] }
  { h = 15, attrh = tmp = 0.,
    predv = [|../../../c/intlist-fold-max.c:(20,16)|],
    succv = [|../../../c/intlist-fold-max.c:(21,17)|] }
  { h = 16, attrh = IF true,
    predv = [|../../../c/intlist-fold-max.c:(21,17)|],
    succv = [|../../../c/intlist-fold-max.c:(14,5)|] }
  { h = 17, attrh = \return = m,
    predv = [|../../../c/intlist-fold-max.c:(15,6)|],
    succv = [|../../../c/intlist-fold-max.c:(23,18)|] }
  info = { procinfo = [fold_max => { pstart = ../../../c/intlist-fold-max.c:(9,0);
                                     pexit = ../../../c/intlist-fold-max.c:(23,18);
                                     pinput = [|x|];
                                     pinput_old = [|_%oldx%_|]; pinput_ini = [|
                                     _%inix%_|];
                                     poutput = [|\return|];
                                     plocal = [|m; xi; tmp; x|];
                                     penv = [|0> \return:int; 1> m:int;
                                              2> _%inix%_:real;
                                              3> _%oldx%_:real; 4> tmp:real;
                                              5> x:real; 6> xi:real|];
                                     pinput_tmp = [|_%in0%_|];
                                     poutput_tmp = [|_%out0%_|];
                                     prequires = [|x > 0|] }];
           callret = [];
           pointenv = [../../../c/intlist-fold-max.c:(15,-1) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(19,15) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(15,-1) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(18,14) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(12,3) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(20,16) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(14,5) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(16,11) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(15,-2) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(15,-2) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(16,-2) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(16,-1) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(16,-2) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(16,-1) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(15,9) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(12,2) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(17,12) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(21,17) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(13,4) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(23,18) => [|0> \return:int;
                                                                  1> m:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-max.c:(12,1) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(15,8) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(15,6) => [|0> \return:int;
                                                                 1> m:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-max.c:(9,0) => [|0> \return:int;
                                                                1> m:int;
                                                                2> _%inix%_:real;
                                                                3> _%oldx%_:real;
                                                                4> tmp:real;
                                                                5> x:real;
                                                                6> xi:real|]];
           counter = 18; }
   ]