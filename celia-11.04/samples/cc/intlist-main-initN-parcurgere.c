
#include "linked_list.h"

/*@ requires acyclic(head);
  @
 */
void
printlist(intlist head) {
    intlist tmp;
    intlist curr = head;

    while (curr) {
        printf("%d ", curr->data);
        tmp = curr->next;
        curr = NULL;
        curr = tmp;
        tmp = NULL;
    }
    printf("\n");
}

/*@ requires len >= 1;
 */
intlist initn(int len) {
    intlist curr, head;
    int i;

    head = NULL;
    curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = len - i;
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
 // head = init (i);	//list of 10 integers 0
  i=10;
  hd = initn(i);
  i=i+1;

//  map_add2(hd);
//  i=i+1;
 printlist (head);
 i = i+1;

}
