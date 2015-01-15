
#include "intlist.h"

// Returns the list celle in head with key equal with key
// Recursive version.

/*@ requires acyclic(head);
 */
intlist search(intlist head, int key) {
    intlist hi, tmp;
    hi = tmp = NULL;
    hi = head;
    while (hi != NULL) {
        if (hi->data == key)
	    break;
        tmp = hi->next;
	hi = NULL;
        hi = tmp;
        tmp = NULL;
    }
    return hi;
}


