
#include "intlist.h"

// Quick sort: version creating new list

#define clone fold_clone
#define split fold_split
#define concat fold2_concat

/*@ requires acyclic(a);
 */
intlist quicksort(intlist a) {
    intlist *left, *right;
    intlist pivot, res, start, tmp;
    int d, l;
    if (a == NULL)
        res = clone(a);
    else {
        start = a->next;
        if (start == NULL)
            res = clone(a);
        else {
            d = a->data;
            l = 1;
            pivot = init(l);
            pivot->data = d;
            split(start, d, left, right);
            start = NULL; /* no more used */
            tmp = quicksort(*left);
            *left = NULL;
            *left = tmp;
            tmp = NULL;
            tmp = quicksort(*right);
            *right = NULL;
            *right = tmp;
            tmp = NULL;
            tmp = concat(*left, pivot);
            res = concat(tmp, *right);
            tmp = *left = *right = pivot = NULL;
        }
    }
    return res;
}
