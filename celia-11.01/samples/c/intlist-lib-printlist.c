#include "intlist.h"

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
