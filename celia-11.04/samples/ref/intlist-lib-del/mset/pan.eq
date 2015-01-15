[ { v = ../../../c/intlist-lib-del.c:(13,4), attrv = , predh = [3],
    succh = [] }
  { v = ../../../c/intlist-lib-del.c:(10,1), attrv = , predh = [0],
    succh = [1] }
  { v = ../../../c/intlist-lib-del.c:(12,3), attrv = , predh = [2],
    succh = [3] }
  { v = ../../../c/intlist-lib-del.c:(11,2), attrv = , predh = [1],
    succh = [2] }
  { v = ../../../c/intlist-lib-del.c:(8,0), attrv = , predh = [], succh = [0] }
  { h = 0, attrh = curr = 0., predv = [|../../../c/intlist-lib-del.c:(8,0)|],
    succv = [|../../../c/intlist-lib-del.c:(10,1)|] }
  { h = 1, attrh = curr = cast_f,n head,
    predv = [|../../../c/intlist-lib-del.c:(10,1)|],
    succv = [|../../../c/intlist-lib-del.c:(11,2)|] }
  { h = 2, attrh = head = sqrt_f,0 1,
    predv = [|../../../c/intlist-lib-del.c:(11,2)|],
    succv = [|../../../c/intlist-lib-del.c:(12,3)|] }
  { h = 3, attrh = \return = curr,
    predv = [|../../../c/intlist-lib-del.c:(12,3)|],
    succv = [|../../../c/intlist-lib-del.c:(13,4)|] }
  info = { procinfo = [del => { pstart = ../../../c/intlist-lib-del.c:(8,0);
                                pexit = ../../../c/intlist-lib-del.c:(13,4);
                                pinput = [|head|];
                                pinput_old = [|_%oldhead%_|]; pinput_ini = [|
                                _%inihead%_|];
                                poutput = [|\return|];
                                plocal = [|curr; head|];
                                penv = [|0> \return:real;
                                         1> _%inihead%_:real;
                                         2> _%oldhead%_:real; 3> curr:real;
                                         4> head:real|];
                                pinput_tmp = [|_%in0%_|];
                                poutput_tmp = [|_%out0%_|];
                                prequires = [|head > 0|] }];
           callret = [../../../c/intlist-lib-del.c:(11,2) => ../../../c/intlist-lib-del.c:(12,3)];
           pointenv = [../../../c/intlist-lib-del.c:(12,3) => [|0> \return:real;
                                                                1> _%inihead%_:real;
                                                                2> _%oldhead%_:real;
                                                                3> curr:real;
                                                                4> head:real|];
                       ../../../c/intlist-lib-del.c:(8,0) => [|0> \return:real;
                                                               1> _%inihead%_:real;
                                                               2> _%oldhead%_:real;
                                                               3> curr:real;
                                                               4> head:real|];
                       ../../../c/intlist-lib-del.c:(13,4) => [|0> \return:real;
                                                                1> _%inihead%_:real;
                                                                2> _%oldhead%_:real;
                                                                3> curr:real;
                                                                4> head:real|];
                       ../../../c/intlist-lib-del.c:(11,2) => [|0> \return:real;
                                                                1> _%inihead%_:real;
                                                                2> _%oldhead%_:real;
                                                                3> curr:real;
                                                                4> head:real|];
                       ../../../c/intlist-lib-del.c:(10,1) => [|0> \return:real;
                                                                1> _%inihead%_:real;
                                                                2> _%oldhead%_:real;
                                                                3> curr:real;
                                                                4> head:real|]];
           counter = 4; }
   ]