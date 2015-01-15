
#include "linked_list.h"

/*@ requires acyclic(head);
 */
void map_add2(intlist head) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = h->data + 2;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}


/*@ requires acyclic(x);
 */
intlist
listcopy (intlist x)
{
  intlist xi = x;
  intlist tmp = NULL;
  intlist y = NULL;
  intlist yi = NULL;;
  intlist z = NULL;
  while (xi != NULL)
    {
      z = (intlist) malloc (sizeof (struct intlist_));
      z->data = xi->data+2;
      z->next = NULL;
      if (y == NULL)
        y = z;
      else{
	yi->next = NULL;
        yi->next = z;
	}
      yi = NULL;
      yi = z;
      tmp = xi->next;
      xi = NULL;
      xi = tmp;
      tmp = NULL; 
      z = NULL;  
  }
  return y;
}

/*@ requires len >= 1;
 */
intlist init(int len) {
    intlist curr, head;
    int i;

    head = NULL;
    curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = 0;
        curr->next = head;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    return head;
}

void
main ()
{

  intlist head,hd;
  int i;
  hd = NULL;
  head = NULL;
  i=10;
  hd = init (i);	//list of 10 integers 0
  i=10;
  //hd = init (i);
  head = listcopy(hd);
  i=i+1;
  map_add2(head);
//  i=i+1;
// printlist (head);
 i = i+1;

}
