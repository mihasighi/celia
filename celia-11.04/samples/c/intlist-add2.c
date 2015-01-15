
#include "linked_list.h"

void
main ()
{

  intlist head;
  int i;

  head = NULL;
  i=10;
  head = init (i);	//list of 10 integers 0
  printlist (head);

  map_add2 (head);

  printf ("Output after Adding 2 to every node of list\n");
  printlist (head);

}
