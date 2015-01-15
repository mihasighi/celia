#include "intlist.h"

// Returns a list of length len and elements 0.

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
        curr->next = NULL;
        curr->next = head;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    return head;
}

