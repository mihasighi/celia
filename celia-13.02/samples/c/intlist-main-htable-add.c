
#include "intlist.h"


void
main ()
{

  intlist h_0, h_1, h_2, h_3, h_other;
  intlist x;
  // temporary vars
  intlist tmp, xi;
  int d;

  d = 3; h_0 = init(d);
  d = 2; h_1 = init(d);
  d = 1; h_2 = init(d);
  d = 2; h_3 = init(d);
  h_other = init(d);
  d = 4; x = init(d);

  d = 0; initV(h_0,d);
  d = 1; initV(h_1,d);
  d = 2; initV(h_2,d);
  d = 3; initV(h_3,d);
  d = 4; map_initSeq(h_other,d);
  d = 0; map_initSeq(x,d);

  xi = tmp = NULL;
  xi = x;
  while (xi) {
    d = xi->data;
    if (d == 0) {
       tmp = add(h_0,d);
       h_0 = NULL;
       h_0 = tmp;
       tmp = NULL;
    }
    else if (d == 1) {
       tmp = add(h_1,d);
       h_1 = NULL;
       h_1 = tmp;
       tmp = NULL;
    }
    else if (d == 2) {
       tmp = add(h_2,d);
       h_2 = NULL;
       h_2 = tmp;
       tmp = NULL;
    }
    else if (d == 3) {
       tmp = add(h_3,d);
       h_3 = NULL;
       h_3 = tmp;
       tmp = NULL;
    }
    else {
       tmp = add(h_other,d);
       h_other = NULL;
       h_other = tmp;
       tmp = NULL;
    }
    tmp = xi->next;
    xi = NULL;
    xi = tmp;
    tmp = NULL;
  } 

}
