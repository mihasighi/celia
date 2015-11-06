#include "intlist.h"

// Returns non zero if the list is not empty.
/*@ requires acyclic(head);
  @
 */
int empty(intlist head) {
    if (head == NULL)
        return 0;
    return 1;
}

