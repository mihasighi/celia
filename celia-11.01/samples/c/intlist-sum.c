#include "linked_list.h"

/*@ requires acyclic(x); */
int
listSum (intlist x)
{
  intlist xi = x;
  int sum = 0;
  while (xi != NULL)
    {
      sum = sum + xi->data;
      xi = xi->next;
    }
  return sum;
}

void
main ()
{
  intlist head;

  int sum;

  head = NULL;

  head = allocate (head, 10);	//list of 10 to 1 
  printf ("List 1 \n");
  printlist (head);
  sum = listSum (head);

  printf ("Sum of elements of the list %d\n", sum);

}
