
#include "linked_list.h"

/* =================================================================== */
/* MAP class: iterates over a list to initialize its elements */
/* =================================================================== */

// Initializes all elements of head with value v.
// Iterative and recursive versions.

/*@ requires acyclic(head);
 */
void map_initV(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = v;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(head);
 */
void map_rec_initV(intlist head, int v) {
    if (head != NULL) {
        intlist h = NULL;
        head->data = v;
        h = head->next;
        map_rec_initV(h, v);
    }
}

// Initializes with the sequence starting from v the elements of head.
// Iterative and recursive versions.

/*@ requires acyclic(head);
 */
void map_initSeq(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = v;
        v = v + 1;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(head);
 */
void map_rec_initSeq(intlist head, int v) {
    if (head != NULL) {
        int vv;
        intlist curr;
        head->data = v;
        vv = v + 1;
        curr = NULL;
        curr = head->next;
        map_rec_initSeq(curr, vv);
    }
}

// Initializes head with the sequence of even numbers starting from v.
// Iterative and recursive versions.

/*@ requires acyclic(head);
 */
void map_initSeqEven(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = v;
        v = v + 2;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(head);
 */
void map_rec_initSeqEven(intlist head, int v) {
    if (head != NULL) {
        int vv;
        intlist curr;
        head->data = v;
        vv = v + 2;
        curr = NULL;
        curr = head->next;
        map_rec_initSeq(curr, vv);
    }
}

// Initializes head with the Fibonacci sequence.
// Iterative and recursive versions.

/*@ requires acyclic(head);
 */
void map_initFibo(intlist head) {
    int m1 = 1;
    int m2 = 0;
    intlist xi, tmp;
    xi = tmp = NULL;
    xi = head;
    while (xi != NULL) {
        xi->data = m1 + m2;
        m1 = m2;
        m2 = xi->data;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
}

// Initializes head with the sequence of modulo 2.
// Iterative and recursive versions.

/*@ requires acyclic(head);
 */
void map_initMod2(intlist head) {
    intlist xi, tmp;
    int k = 0;
    xi = tmp = NULL;
    xi = head;
    while (xi != NULL) {
        xi->data = k;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        k = 1 - k;
    }
}

/*@ requires acyclic(head);
 */
void map_rec_initMod2(intlist head, int v) {
    if (head != NULL) {
        intlist curr;
        int k;
        k = 1 - v;
        curr = NULL;
        curr = head->next;
        map_rec_initMod2(curr, k);
    }
}

// Adds to each element of head the value v.
// Iterative and recursive versions.
/*@ requires acyclic(head);
 */
void map_addV(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = h->data + v;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(head);
 */
void map_rec_addV(intlist head, int v) {
    if (head != NULL) {
      head->data = head->data + v;
      intlist curr;
      curr = NULL;
      curr = head->next;
      map_rec_addV(curr,v);
    }
}

// Adds 2 to each element of head.
// Iterative and recusrive versions.
/*@ requires acyclic(head);
 */
void map_add2(intlist head) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = h->data + 2;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(head);
 */
void map_rec_add2(intlist head) {
    if (head != NULL) {
      head->data = head->data + 2;
      intlist curr;
      curr = NULL;
      curr = head->next;
      map_rec_add2(curr);
    }
}

