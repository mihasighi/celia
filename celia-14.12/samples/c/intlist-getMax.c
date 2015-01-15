#include "linked_list.h"


/*@ requires acyclic(x); */

int
listMax (intlist x)
{
  intlist xi = x;
  int max = x->data;
  while (xi != NULL)
    {
      if (max < xi->data)
	max = xi->data;
      xi = xi->next;
    }
  return max;
}

void
main ()
{
  intlist head;
  int i;

  head = NULL;

  head = allocate (head, 10);	//list of 10 to 1 

  i = listMax (head);
  printf ("List 1 is  \n");
  printlist (head);
  printf ("Max value in the list is %d \n", i);

}
