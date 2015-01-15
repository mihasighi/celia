/**
 *  Library of singly linked lists with integer data.
 */

#include "stdlib.h"
#include "stdio.h"

#ifndef _INTLIST_H
#define _INTLIST_H

/* =================================================================== */
/* Type declaration */
/* =================================================================== */

typedef struct intlist_ *intlist;
struct intlist_
{
  int data;
  intlist next;
};

/* =================================================================== */
/* Predicates and functions used to specify intlist */
/* =================================================================== */

// - reachability predicate
/*@ predicate reach(intlist from, intlist to) =
  @   (from == to) || (\valid(from) && reach(from->next,to)) ;
 */

// - acyclic predicate
/*@ predicate acyclic(intlist l) = 
  @    reach(l,\null) ;
 */

// - disjoint predicate
/*@ predicate disjoint(intlist l1, intlist l2) =
  @   ! (\exists intlist l ; reach(l1,l) && reach(l2,l)) ;
  @ 
  @ predicate disjoint3(intlist l1, intlist l2, intlist l3) =
  @   disjoint(l1,l2) && disjoint(l2,l3) && disjoint(l1,l3) ;
  @ 
*/

// - length function
/*@ logic integer len(intlist ptr) =
  @    (ptr == \null) ? 0 : 1 + len(ptr->next)
  @ ;
 */

/* =================================================================== */
/* Basic functions on intlist */
/* =================================================================== */

/* see linked_list.c */

// Returns a list of length len and elements 0.
/*@ requires len >= 1;
 */
intlist init(int len);

// Adds to head the value at the beginning of head.
// Iterative version.
/*@ requires acyclic(head);
  @
 */
intlist add(intlist head, int value);

// Adds to head the value at the end of head.
// Iterative and recursive versions.
/*@ requires acyclic(head);
  @
 */
intlist add_tail(intlist head, int value);

/*@ requires acyclic(head);
  @
 */
intlist add_tail_rec(intlist head, int value);

// Deletes the first element of head.
/*@ requires acyclic(head);
  @
 */
intlist del(intlist head);

// Deletes the last element of head.
// Iterative and recursive versions.
/*@ requires acyclic(head);
  @
 */
intlist del_tail(intlist head);

/*@ requires acyclic(head);
  @
 */
intlist del_tail_rec(intlist head);

// Returns non zero if the list is not empty.
/*@ requires acyclic(head);
  @
 */
int empty(intlist head);

// Prints the list.
/*@ requires acyclic(head);
  @
 */
void printlist (intlist head);


/* =================================================================== */
/* MAP class: iterates over a list to initialize its elements */
/* =================================================================== */

/* see linked_list_map.c */

// Initializes all elements of head with value v.
// Iterative and recursive versions.
/*@ requires acyclic(head);
 */
void map_initV(intlist head, int v);
/*@ requires acyclic(head);
 */
void map_rec_initV(intlist head, int v);

// Initializes with the sequence starting from v the elements of head.
// Iterative and recursive versions.
/*@ requires acyclic(head);
 */
void map_initSeq(intlist head, int v);
/*@ requires acyclic(head);
 */
void map_rec_initSeq(intlist head, int v);

// Initializes head with the sequence of even numbers starting from v.
// Iterative and recursive versions.
/*@ requires acyclic(head);
 */
void map_initSeqEven(intlist head, int v);
/*@ requires acyclic(head);
 */
void map_rec_initSeqEven(intlist head, int v);

// Initializes head with the Fibonacci sequence.
// Iterative version.
/*@ requires acyclic(head);
 */
void map_initFibo(intlist head);

// Initializes head with the sequence of alternatining 0 and 1,
// starting with 0.
// Iterative and recursive versions.
/*@ requires acyclic(head);
 */
void map_initMod2(intlist head);
/*@ requires acyclic(head);
 */
void map_rec_initMod2(intlist head, int v);

// Adds to each element of head the value v.
// Iterative and recursive versions.
/*@ requires acyclic(head);
 */
void map_addV(intlist head, int v);
/*@ requires acyclic(head);
 */
void map_rec_addV(intlist head, int v);

// Adds 2 to each element of head.
// Iterative and recusrive versions.
/*@ requires acyclic(head);
 */
void map_add2(intlist head);
/*@ requires acyclic(head);
 */
void map_rec_add2(intlist head);

/* =================================================================== */
/* MAP2 class: Iterates over two lists to initialize their elements */
/* =================================================================== */

// Initializes each element of y with the corresponding element of x
// plus v.
// Iterative and recursive  versions.
/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
  @ requires len(x) == len(y);
 */
void map2_addV_eq(intlist x, intlist y, int v);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
  @ requires len(x) == len(y);
 */
void map2_rec_addV_eq(intlist x, intlist y, int v);

// Instatiate v with 2 to obtain relations between sums and lengths
/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
  @ requires len(x) == len(y);
 */
void map2_add2_eq(intlist x, intlist y);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
  @ requires len(x) == len(y);
 */
void map2_rec_add2_eq(intlist x, intlist y);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
void map2_addV(intlist x, intlist y, int v);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
void map2_rec_addV(intlist x, intlist y, int v);

// Does not test the length of the list.
/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
void map2_addV_err(intlist x, intlist y, int v);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
void map2_rec_addV_err(intlist x, intlist y, int v);

// Copies all elements of x in y.
// Iterative and recursive versions.
/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
void map2_copy(intlist x, intlist y);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
void map2_rec_copy(intlist x, intlist y);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
  @ requires len(x) == len(y);
 */
void map2_copy_eq(intlist x, intlist y);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
  @ requires len(x) == len(y);
 */
void map2_rec_copy_eq(intlist x, intlist y);

/* =================================================================== */
/* FOLD class: Iterates over one list to build a result */
/* =================================================================== */

// Builds a new list by copying from x all elements greater than v.
// Iterative and recursive versions.
/*@ requires acyclic(x);
 */
intlist fold_copyGeV(intlist x, int v);

/*@ requires acyclic(x);
 */
intlist fold_rec_copyGeV(intlist x, int v);

// Instantiate v with 5 to obtain relation between length and sum
/*@ requires acyclic(x);
 */
intlist fold_copyGe5(intlist x);

/*@ requires acyclic(x);
 */
intlist fold_rec_copyGe5(intlist x);

// Removes from x  all elements greater than the first element of v
// and returns the removed elements
intlist fold_delLtFst(intlist x);

// Removes from x all elements greater than v.
// Iterative and recursive versions.
/*@ requires acyclic(x);
 */
intlist fold_delGeV(intlist x, int v);

/*@ requires acyclic(x);
 */
intlist fold_rec_delGeV(intlist x, int v);

// Instantiate v with 5 to obtain relation between length and sum
/*@ requires acyclic(x);
 */
intlist fold_delGe5(intlist x);

/*@ requires acyclic(x);
 */
intlist fold_rec_delGe5(intlist x);

// Create a new list, the reverse of x.
/*@ requires acyclic(x);
 */
intlist fold_copyRev(intlist x);

/*@ requires acyclic(x);
 */
intlist fold_rec_copyRev(intlist x);

// Computes the maximum value in a list
// Iterative and recursive versions.
/*@ requires acyclic(x);
 */
int fold_max(intlist x);

/*@ requires acyclic(x);
 */
int fold_rec_max(intlist x);

// Computes the sum of values in a list
// Iterative and recursive versions.
/*@ requires acyclic(x);
 */
int fold_sum(intlist x);

/*@ requires acyclic(x);
 */
int fold_rec_sum(intlist x);

// Clones x in the result.
// Iterative and recursive versions.
/*@ requires acyclic(x);
 */
intlist fold_clone(intlist x);

/*@ requires acyclic(x);
 */
intlist fold_rec_clone(intlist x);

// Builds two lists l and u by dispatching x wrt v.
// Iterative and recursive versions.
/*@ requires acyclic(x);
 */
void fold_split(intlist x, int v, intlist* l, intlist* u);

/*@ requires acyclic(x);
 */
void fold_rec_split(intlist x, int v, intlist* l, intlist* u);

// Instantiate with v at 5 to obtain relations between sums and lengths
/*@ requires acyclic(x);
 */
void fold_split5(intlist x, intlist* l, intlist* u);

/*@ requires acyclic(x);
 */
void fold_rec_split5(intlist x, intlist* l, intlist* u);

/* =================================================================== */
/* FOLD2 class: Iterates over two list to build a result */
/* =================================================================== */

// Returns 1 if two lists are equal
/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
int fold2_equal(intlist x, intlist y);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
int fold2_rec_equal(intlist x, intlist y);

// Concatenates two lists in the result.
/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_concat(intlist x, intlist y); // new copy
intlist fold2_concat1(intlist x, intlist y); // side effects

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_rec_concat(intlist x, intlist y);

// Merge two lists in the result.
// If the two lists are sorted, the result is sorted.
/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_merge(intlist x, intlist y);

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_rec_merge(intlist x, intlist y);

/* =================================================================== */
/* Sorting algorithms */
/* =================================================================== */

// Insertion sort: version exchanging data
/*  requires acyclic(x);
 */
//void insertSortArr(intlist x);

// Insertion sort: version exchanging cells
/*  requires acyclic(x);
 */
//intlist insertSortArr(intlist x);

// Bubble sort: version exchanging data
/*  requires acyclic(x);
 */
//void bubbleSortArr(intlist x);

// Merge sort: version creating new list
/*  requires acyclic(x);
 */
//intlist mergeSort(intlist x);

// Quick sort: version creating new list
/*  requires acyclic(x);
 */
//intlist quickSort(intlist x);

#endif
