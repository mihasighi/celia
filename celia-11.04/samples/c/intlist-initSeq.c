#include "linked_list.h"

/*@ requires acyclic(x); */

void
seqInit (intlist x, int m)
{
  int mp = m;
  intlist xi = x;
  while (xi != NULL)
    {
      xi->data = mp;
      xi = xi->next;
      mp = mp + 1;
    }
}

void
main ()
{
  intlist head;


  head = NULL;

  head = allocate_value (head, 10, 0);	//list of 10 to 1 
  seqInit (head, 5);

  printf ("sequential initialization after value 5 \n");
  printlist (head);

}
