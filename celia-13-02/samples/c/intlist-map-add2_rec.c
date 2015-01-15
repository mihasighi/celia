
#include "intlist.h"

// Adds 2 to each element of head.
// Recursive version.

/*@ requires acyclic(head);
 */
void map_rec_add2(intlist head) {
    if (head != NULL) {
      head->data = head->data + 2;
      intlist curr;
      curr = NULL;
      curr = head->next;
      map_rec_add2(curr);
    }
}

