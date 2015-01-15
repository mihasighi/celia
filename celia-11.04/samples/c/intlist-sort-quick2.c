
#include "intlist.h"

// Quick sort: version creating new list

#define clone fold_clone
#define split fold_split
#define concat3 fold3_concat

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
            pivot = (intlist) malloc(sizeof (struct intlist_));
            pivot->next = 0;
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
            tmp = concat3(*left, pivot,*right);
            tmp = *left = *right = pivot = NULL;
        }
    }
    return res;
}
