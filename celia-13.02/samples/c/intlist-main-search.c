
#include "intlist.h"

/*@ requires d >= 1 && e >= d;
 */
int main_search (int d, int e) {

    intlist head, x, y;
    head = x = y = NULL;
    /* ... */ 
    head = initSeq(d);
    /* ... */
    x = search(head, e);
    /* ... */
    y = x->next; /*@ " pointernull " @*/
    /* ... */
    return 0;
}

