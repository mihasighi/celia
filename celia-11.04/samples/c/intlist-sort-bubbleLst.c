
#include "intlist.h"

// Sort the entry list using the bubble sort algorithm

/*@ requires acyclic(x);
 */
intlist
bubblesort(intlist x) {
    intlist xn, y, yn, prev, last, tmp;
    xn = y = yn = prev = last = tmp = NULL;
    xn = x->next; // x has at least one element
    while (xn != last) {
        // put the maximum element from x to last
        y = x;
        yn = y->next;
        while (yn != last) {
            if (y->data > yn->data) {
                // swap
                tmp = yn->next;
                y->next = NULL;
                y->next = tmp;
                tmp = NULL;
                yn->next = NULL;
                yn->next = y;
                if (prev == NULL) {
                    x = NULL;
                    x = yn;
                }
                else {
                    prev->next = NULL;
                    prev->next = yn;
                }
            }
            else {
                prev = NULL;
                prev = y;
                y = NULL;
                y = yn;
                yn = NULL;
                yn = y->next;
            }
        }
        last = prev = NULL;
        last = y;
        y = NULL;
        yn = NULL;
        xn = NULL;
        xn = x->next;
    }
    return x;
}
