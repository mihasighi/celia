[ { v = ../../../c/intlist-lib-empty.c:(9,3), attrv = , predh = [0],
    succh = [1] }
  { v = ../../../c/intlist-lib-empty.c:(7,0), attrv = , predh = [],
    succh = [2 4] }
  { v = ../../../c/intlist-lib-empty.c:(8,2), attrv = , predh = [3 4],
    succh = [5] }
  { v = ../../../c/intlist-lib-empty.c:(10,5), attrv = , predh = [5],
    succh = [6] }
  { v = ../../../c/intlist-lib-empty.c:(9,6), attrv = , predh = [],
    succh = [3] }
  { v = ../../../c/intlist-lib-empty.c:(10,7), attrv = , predh = [1 6],
    succh = [] }
  { v = ../../../c/intlist-lib-empty.c:(8,-1), attrv = , predh = [2],
    succh = [0] }
  { h = 0, attrh = __retres = 0,
    predv = [|../../../c/intlist-lib-empty.c:(8,-1)|],
    succv = [|../../../c/intlist-lib-empty.c:(9,3)|] }
  { h = 1, attrh = IF true, predv = [|../../../c/intlist-lib-empty.c:(9,3)|],
    succv = [|../../../c/intlist-lib-empty.c:(10,7)|] }
  { h = 2, attrh = IF head - 0. = 0,
    predv = [|../../../c/intlist-lib-empty.c:(7,0)|],
    succv = [|../../../c/intlist-lib-empty.c:(8,-1)|] }
  { h = 3, attrh = IF true, predv = [|../../../c/intlist-lib-empty.c:(9,6)|],
    succv = [|../../../c/intlist-lib-empty.c:(8,2)|] }
  { h = 4, attrh = IF head - 0. <> 0,
    predv = [|../../../c/intlist-lib-empty.c:(7,0)|],
    succv = [|../../../c/intlist-lib-empty.c:(8,2)|] }
  { h = 5, attrh = __retres = 1,
    predv = [|../../../c/intlist-lib-empty.c:(8,2)|],
    succv = [|../../../c/intlist-lib-empty.c:(10,5)|] }
  { h = 6, attrh = \return = __retres,
    predv = [|../../../c/intlist-lib-empty.c:(10,5)|],
    succv = [|../../../c/intlist-lib-empty.c:(10,7)|] }
  info = { procinfo = [empty => { pstart = ../../../c/intlist-lib-empty.c:(7,0);
                                  pexit = ../../../c/intlist-lib-empty.c:(10,7);
                                  pinput = [|head|];
                                  pinput_old = [|_%oldhead%_|]; pinput_ini = [|
                                  _%inihead%_|];
                                  poutput = [|\return|];
                                  plocal = [|__retres; head|];
                                  penv = [|0> \return:int; 1> __retres:int;
                                           2> _%inihead%_:real;
                                           3> _%oldhead%_:real; 4> head:real|];
                                  pinput_tmp = [|_%in0%_|];
                                  poutput_tmp = [|_%out0%_|];
                                  prequires = [|head > 0|] }];
           callret = [];
           pointenv = [../../../c/intlist-lib-empty.c:(8,-1) => [|0> \return:int;
                                                                  1> __retres:int;
                                                                  2> _%inihead%_:real;
                                                                  3> _%oldhead%_:real;
                                                                  4> head:real|];
                       ../../../c/intlist-lib-empty.c:(10,7) => [|0> \return:int;
                                                                  1> __retres:int;
                                                                  2> _%inihead%_:real;
                                                                  3> _%oldhead%_:real;
                                                                  4> head:real|];
                       ../../../c/intlist-lib-empty.c:(10,5) => [|0> \return:int;
                                                                  1> __retres:int;
                                                                  2> _%inihead%_:real;
                                                                  3> _%oldhead%_:real;
                                                                  4> head:real|];
                       ../../../c/intlist-lib-empty.c:(7,0) => [|0> \return:int;
                                                                 1> __retres:int;
                                                                 2> _%inihead%_:real;
                                                                 3> _%oldhead%_:real;
                                                                 4> head:real|];
                       ../../../c/intlist-lib-empty.c:(9,6) => [|0> \return:int;
                                                                 1> __retres:int;
                                                                 2> _%inihead%_:real;
                                                                 3> _%oldhead%_:real;
                                                                 4> head:real|];
                       ../../../c/intlist-lib-empty.c:(8,-2) => [|0> \return:int;
                                                                  1> __retres:int;
                                                                  2> _%inihead%_:real;
                                                                  3> _%oldhead%_:real;
                                                                  4> head:real|];
                       ../../../c/intlist-lib-empty.c:(8,2) => [|0> \return:int;
                                                                 1> __retres:int;
                                                                 2> _%inihead%_:real;
                                                                 3> _%oldhead%_:real;
                                                                 4> head:real|];
                       ../../../c/intlist-lib-empty.c:(9,3) => [|0> \return:int;
                                                                 1> __retres:int;
                                                                 2> _%inihead%_:real;
                                                                 3> _%oldhead%_:real;
                                                                 4> head:real|]];
           counter = 7; }
   ]