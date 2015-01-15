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

#endif
