
#include "intlist.h"

// Create a list of elements 0 ... d-1
// Iterative version.

/*@ requires d>=0;
 */
intlist initSeq(int d) {
    intlist h, tmp;
    h = tmp = NULL;
    while (d >= 1) {
        tmp = (intlist) malloc (sizeof (struct intlist_));
        tmp->data = d - 1;
        tmp->next = NULL;
        tmp->next = h;
        h = NULL;
        h = tmp;
        tmp = NULL;
        d = d - 1;
    }
    return h;
}

