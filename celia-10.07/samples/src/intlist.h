#include <stdio.h>
typedef struct intlist_ * intlist;
struct intlist_ {
  int data;
  intlist next;
};
