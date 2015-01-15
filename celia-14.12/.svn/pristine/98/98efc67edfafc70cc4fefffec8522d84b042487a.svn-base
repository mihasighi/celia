#include "intlist.h"

// Deletes the first element of head.

/*@ requires acyclic(head);
  @
 */
intlist del(intlist head) {
    intlist curr;
    curr = NULL;
    curr = head->next;
    free(head);
    return curr;
}

