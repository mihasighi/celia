
#include "intlist.h"

// Inserts an integer into a sorted list
// Iterative version

/*@ requires acyclic(x) && len(x)>=1 && sorted(x);
 */
intlist
sorted_insert(intlist x, int v) {
    intlist curr, succ, item;
    curr = succ = item = NULL;
    curr = x;
    succ = x->next;
    while (succ != NULL) {
        if (succ->data < v) {
            curr = NULL;
            curr = succ;
            succ = NULL;
            succ = curr->next;
        }
        else break;
    }
    item = (intlist) malloc(sizeof(struct intlist_));
    item->data = v;
    item->next = NULL;
    if (x->data > v) {
        item->next = x;
        x = NULL;
        x = item;
    }
    else {
        item->next = succ;
        curr->next = NULL;
        curr->next = item;
    }
    item = succ = curr = NULL; // no more used
    return x;
}
