[ { v = ../../../c/intlist-lib-add.c:(14,4), attrv = , predh = [3],
    succh = [4] }
  { v = ../../../c/intlist-lib-add.c:(9,0), attrv = , predh = [], succh = [0] }
  { v = ../../../c/intlist-lib-add.c:(13,3), attrv = , predh = [2],
    succh = [3] }
  { v = ../../../c/intlist-lib-add.c:(15,5), attrv = , predh = [4],
    succh = [5] }
  { v = ../../../c/intlist-lib-add.c:(12,2), attrv = , predh = [1],
    succh = [2] }
  { v = ../../../c/intlist-lib-add.c:(11,1), attrv = , predh = [0],
    succh = [1] }
  { v = ../../../c/intlist-lib-add.c:(16,6), attrv = , predh = [5],
    succh = [] }
  { h = 0, attrh = curr = 0., predv = [|../../../c/intlist-lib-add.c:(9,0)|],
    succv = [|../../../c/intlist-lib-add.c:(11,1)|] }
  { h = 1, attrh = curr = sqrt_f,n 1,
    predv = [|../../../c/intlist-lib-add.c:(11,1)|],
    succv = [|../../../c/intlist-lib-add.c:(12,2)|] }
  { h = 2, attrh = curr = cast_d,0 value,
    predv = [|../../../c/intlist-lib-add.c:(12,2)|],
    succv = [|../../../c/intlist-lib-add.c:(13,3)|] }
  { h = 3, attrh = curr = cast_d,n 0.,
    predv = [|../../../c/intlist-lib-add.c:(13,3)|],
    succv = [|../../../c/intlist-lib-add.c:(14,4)|] }
  { h = 4, attrh = curr = cast_d,n head,
    predv = [|../../../c/intlist-lib-add.c:(14,4)|],
    succv = [|../../../c/intlist-lib-add.c:(15,5)|] }
  { h = 5, attrh = \return = curr,
    predv = [|../../../c/intlist-lib-add.c:(15,5)|],
    succv = [|../../../c/intlist-lib-add.c:(16,6)|] }
  info = { procinfo = [add => { pstart = ../../../c/intlist-lib-add.c:(9,0);
                                pexit = ../../../c/intlist-lib-add.c:(16,6);
                                pinput = [|head; value|];
                                pinput_old = [|_%oldhead%_; _%oldvalue%_|]; pinput_ini = [|
                                _%inihead%_|];
                                poutput = [|\return|];
                                plocal = [|curr; head; value|];
                                penv = [|0> _%oldvalue%_:int; 1> value:int;
                                         2> \return:real;
                                         3> _%inihead%_:real;
                                         4> _%oldhead%_:real; 5> curr:real;
                                         6> head:real|];
                                pinput_tmp = [|_%in0%_|];
                                poutput_tmp = [|_%out0%_|];
                                prequires = [|head > 0|] }];
           callret = [../../../c/intlist-lib-add.c:(11,1) => ../../../c/intlist-lib-add.c:(12,2)];
           pointenv = [../../../c/intlist-lib-add.c:(12,2) => [|0> _%oldvalue%_:int;
                                                                1> value:int;
                                                                2> \return:real;
                                                                3> _%inihead%_:real;
                                                                4> _%oldhead%_:real;
                                                                5> curr:real;
                                                                6> head:real|];
                       ../../../c/intlist-lib-add.c:(14,4) => [|0> _%oldvalue%_:int;
                                                                1> value:int;
                                                                2> \return:real;
                                                                3> _%inihead%_:real;
                                                                4> _%oldhead%_:real;
                                                                5> curr:real;
                                                                6> head:real|];
                       ../../../c/intlist-lib-add.c:(13,3) => [|0> _%oldvalue%_:int;
                                                                1> value:int;
                                                                2> \return:real;
                                                                3> _%inihead%_:real;
                                                                4> _%oldhead%_:real;
                                                                5> curr:real;
                                                                6> head:real|];
                       ../../../c/intlist-lib-add.c:(11,1) => [|0> _%oldvalue%_:int;
                                                                1> value:int;
                                                                2> \return:real;
                                                                3> _%inihead%_:real;
                                                                4> _%oldhead%_:real;
                                                                5> curr:real;
                                                                6> head:real|];
                       ../../../c/intlist-lib-add.c:(9,0) => [|0> _%oldvalue%_:int;
                                                               1> value:int;
                                                               2> \return:real;
                                                               3> _%inihead%_:real;
                                                               4> _%oldhead%_:real;
                                                               5> curr:real;
                                                               6> head:real|];
                       ../../../c/intlist-lib-add.c:(15,5) => [|0> _%oldvalue%_:int;
                                                                1> value:int;
                                                                2> \return:real;
                                                                3> _%inihead%_:real;
                                                                4> _%oldhead%_:real;
                                                                5> curr:real;
                                                                6> head:real|];
                       ../../../c/intlist-lib-add.c:(16,6) => [|0> _%oldvalue%_:int;
                                                                1> value:int;
                                                                2> \return:real;
                                                                3> _%inihead%_:real;
                                                                4> _%oldhead%_:real;
                                                                5> curr:real;
                                                                6> head:real|]];
           counter = 6; }
   ]