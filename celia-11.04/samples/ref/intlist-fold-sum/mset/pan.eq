[ { v = ../../../c/intlist-fold-sum.c:(11,1), attrv = , predh = [0],
    succh = [1] }
  { v = ../../../c/intlist-fold-sum.c:(14,6), attrv = , predh = [5],
    succh = [14] }
  { v = ../../../c/intlist-fold-sum.c:(14,8), attrv = , predh = [4 7],
    succh = [8] }
  { v = ../../../c/intlist-fold-sum.c:(13,5), attrv = , predh = [3 13],
    succh = [4 6] }
  { v = ../../../c/intlist-fold-sum.c:(12,3), attrv = , predh = [1],
    succh = [2] }
  { v = ../../../c/intlist-fold-sum.c:(18,13), attrv = , predh = [11],
    succh = [12] }
  { v = ../../../c/intlist-fold-sum.c:(14,9), attrv = , predh = [],
    succh = [7] }
  { v = ../../../c/intlist-fold-sum.c:(12,4), attrv = , predh = [2],
    succh = [3] }
  { v = ../../../c/intlist-fold-sum.c:(17,12), attrv = , predh = [10],
    succh = [11] }
  { v = ../../../c/intlist-fold-sum.c:(21,15), attrv = , predh = [14],
    succh = [] }
  { v = ../../../c/intlist-fold-sum.c:(19,14), attrv = , predh = [12],
    succh = [13] }
  { v = ../../../c/intlist-fold-sum.c:(14,-2), attrv = , predh = [6],
    succh = [5] }
  { v = ../../../c/intlist-fold-sum.c:(15,10), attrv = , predh = [8],
    succh = [9] }
  { v = ../../../c/intlist-fold-sum.c:(9,0), attrv = , predh = [],
    succh = [0] }
  { v = ../../../c/intlist-fold-sum.c:(16,11), attrv = , predh = [9],
    succh = [10] }
  { h = 0, attrh = sum = 0, predv = [|../../../c/intlist-fold-sum.c:(9,0)|],
    succv = [|../../../c/intlist-fold-sum.c:(11,1)|] }
  { h = 1, attrh = tmp = 0.,
    predv = [|../../../c/intlist-fold-sum.c:(11,1)|],
    succv = [|../../../c/intlist-fold-sum.c:(12,3)|] }
  { h = 2, attrh = xi = tmp,
    predv = [|../../../c/intlist-fold-sum.c:(12,3)|],
    succv = [|../../../c/intlist-fold-sum.c:(12,4)|] }
  { h = 3, attrh = xi = x, predv = [|../../../c/intlist-fold-sum.c:(12,4)|],
    succv = [|../../../c/intlist-fold-sum.c:(13,5)|] }
  { h = 4, attrh = IF xi - 0. <> 0,
    predv = [|../../../c/intlist-fold-sum.c:(13,5)|],
    succv = [|../../../c/intlist-fold-sum.c:(14,8)|] }
  { h = 5, attrh = IF true,
    predv = [|../../../c/intlist-fold-sum.c:(14,-2)|],
    succv = [|../../../c/intlist-fold-sum.c:(14,6)|] }
  { h = 6, attrh = IF xi - 0. = 0,
    predv = [|../../../c/intlist-fold-sum.c:(13,5)|],
    succv = [|../../../c/intlist-fold-sum.c:(14,-2)|] }
  { h = 7, attrh = IF true, predv = [|../../../c/intlist-fold-sum.c:(14,9)|],
    succv = [|../../../c/intlist-fold-sum.c:(14,8)|] }
  { h = 8, attrh = sum = sum +_i,n cast_i,n xi,
    predv = [|../../../c/intlist-fold-sum.c:(14,8)|],
    succv = [|../../../c/intlist-fold-sum.c:(15,10)|] }
  { h = 9, attrh = tmp = cast_f,n xi,
    predv = [|../../../c/intlist-fold-sum.c:(15,10)|],
    succv = [|../../../c/intlist-fold-sum.c:(16,11)|] }
  { h = 10, attrh = xi = 0.,
    predv = [|../../../c/intlist-fold-sum.c:(16,11)|],
    succv = [|../../../c/intlist-fold-sum.c:(17,12)|] }
  { h = 11, attrh = xi = tmp,
    predv = [|../../../c/intlist-fold-sum.c:(17,12)|],
    succv = [|../../../c/intlist-fold-sum.c:(18,13)|] }
  { h = 12, attrh = tmp = 0.,
    predv = [|../../../c/intlist-fold-sum.c:(18,13)|],
    succv = [|../../../c/intlist-fold-sum.c:(19,14)|] }
  { h = 13, attrh = IF true,
    predv = [|../../../c/intlist-fold-sum.c:(19,14)|],
    succv = [|../../../c/intlist-fold-sum.c:(13,5)|] }
  { h = 14, attrh = \return = sum,
    predv = [|../../../c/intlist-fold-sum.c:(14,6)|],
    succv = [|../../../c/intlist-fold-sum.c:(21,15)|] }
  info = { procinfo = [fold_sum => { pstart = ../../../c/intlist-fold-sum.c:(9,0);
                                     pexit = ../../../c/intlist-fold-sum.c:(21,15);
                                     pinput = [|x|];
                                     pinput_old = [|_%oldx%_|]; pinput_ini = [|
                                     _%inix%_|];
                                     poutput = [|\return|];
                                     plocal = [|xi; tmp; sum; x|];
                                     penv = [|0> \return:int; 1> sum:int;
                                              2> _%inix%_:real;
                                              3> _%oldx%_:real; 4> tmp:real;
                                              5> x:real; 6> xi:real|];
                                     pinput_tmp = [|_%in0%_|];
                                     poutput_tmp = [|_%out0%_|];
                                     prequires = [|x > 0|] }];
           callret = [];
           pointenv = [../../../c/intlist-fold-sum.c:(15,10) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(21,15) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(14,-2) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(14,-2) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(14,9) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(14,8) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(17,12) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(16,11) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(18,13) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(12,3) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(11,1) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(12,4) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(19,14) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(14,6) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(14,-1) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(14,-1) => [|0> \return:int;
                                                                  1> sum:int;
                                                                  2> _%inix%_:real;
                                                                  3> _%oldx%_:real;
                                                                  4> tmp:real;
                                                                  5> x:real;
                                                                  6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(13,5) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(9,0) => [|0> \return:int;
                                                                1> sum:int;
                                                                2> _%inix%_:real;
                                                                3> _%oldx%_:real;
                                                                4> tmp:real;
                                                                5> x:real;
                                                                6> xi:real|];
                       ../../../c/intlist-fold-sum.c:(12,2) => [|0> \return:int;
                                                                 1> sum:int;
                                                                 2> _%inix%_:real;
                                                                 3> _%oldx%_:real;
                                                                 4> tmp:real;
                                                                 5> x:real;
                                                                 6> xi:real|]];
           counter = 15; }
   ]