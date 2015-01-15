#include "intlist.h"

// Deletes the last element of head.
// Iterative and recursive versions.

/*@ requires acyclic(head);
  @
 */
intlist del_tail(intlist head) {
    intlist h, hp;
    h = hp = NULL;
    h = head;
    while (h != NULL && h->next != NULL) {
        hp = NULL;
        hp = h;
        h = NULL;
        h = hp->next;
    }
    if (h != NULL) {
        free(h);
        if (hp == NULL)
            h = NULL;
        else
            hp->next = NULL;
    }
    return h;
}

