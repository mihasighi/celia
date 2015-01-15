
#include "intlist.h"

// Splits a list into two list by dispatching one element from 2
// Recursive version
// which respects the semantics allowed by cinv: output parameters are not
// input parameters!
//

/*@ requires acyclic(x);
 */
void split(intlist x, intlist* l1, intlist* l2) {

    intlist tmp, nxt, ll1, ll2, ln1, ln2;

    if (x == NULL) {
        *l1 = NULL;
        *l2 = NULL;
        return;
    }

    nxt = x->next;
    ll1 = (intlist) malloc(sizeof (struct intlist_));
    ll1->data = x->data;
    ll1->next = NULL;

    if (nxt == NULL) {
        *l1 = ll1;
        *l2 = NULL; 
        nxt = ll1 = NULL;
        return;
    }

    ll2 = (intlist) malloc(sizeof (struct intlist_));
    ll2->data = nxt->data;
    ll2->next = NULL;
    tmp = nxt->next;

    split(tmp, l1, l2);

    ll1->next = *l1;
    ll2->next = *l2;
    *l1 = NULL; *l1 = ll1;
    *l2 = NULL; *l2 = ll2;
    
    ll1 = ll2 = tmp = nxt = NULL; /* no more used */

} // end split function

