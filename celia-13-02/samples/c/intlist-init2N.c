#include "linked_list.h"

/*@ requires acyclic(x); */

void
init2N (intlist x)
{
  intlist xi = x;
  int m = 0;
  while (xi != NULL)
    {
      xi->data = m;
      xi = xi->next;
      m = m + 2;
    }
}

void
main ()
{
  intlist head;


  head = NULL;

  head = allocate_value (head, 10, 0);	//list of 10 to 1 
  printf ("List 1 is  \n");
  printlist (head);


  init2N (head);

  printf ("List after calling init2N  \n");
  printlist (head);

}
