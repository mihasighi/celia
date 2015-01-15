/* Generated by Frama-C */
struct char_P {
   char char_M ;
};
struct void_P {
   
};
/*@ behavior default:
      assigns \nothing;  */
void free(struct char_P *ptr ) ;
typedef unsigned int size_t;
struct intlist_;
typedef struct intlist_ *intlist;
struct intlist_ {
   int data ;
   intlist next ;
};
/*@ behavior default:
      assigns \at(\result,Post) \from __size;  */
extern  __attribute__((__nothrow__)) struct char_P *malloc(size_t __size )  __attribute__((
__malloc__)) ;
/*@
predicate reach{L}(intlist from, intlist to) =
  ((from ≡ to) ∨ (\valid(from) ∧ reach(from->next, to)));
 */
/*@ predicate acyclic{L}(intlist l) = reach{L}(l, \null);
 */
/*@
predicate disjoint{L}(intlist l1, intlist l2) =
  ¬((∃ struct intlist_ *l_0; (reach{L}(l1, l_0) ∧ reach{L}(l2, l_0))));
 */
/*@
predicate disjoint3{L}(intlist l1_0, intlist l2_0, intlist l3) =
  ((disjoint{L}(l1_0, l2_0) ∧ disjoint{L}(l2_0, l3)) ∧
  disjoint{L}(l1_0, l3));
 */
/*@ logic ℤ len(intlist ptr) = ptr≡\null? 0: 1+len(ptr->next);
 */
intlist add_tail(intlist head , int value ) ;
/*Loc=../../../c/intlist-lib-add_tail.c(9,9)*//*@ requires //id:67
                                                           acyclic{Here}
                                                           (head);
                                                  behavior default:
                                                    
                                                
                                              */
                                              intlist add_tail(intlist head ,
                                                               int value ) 
{
  intlist curr ;
  intlist h ;
  /*Loc=../../../c/intlist-lib-add_tail.c:(11,11)*//*sid:1*/
  { /*undefined sequence*/ 
    /*Loc=../../../c/intlist-lib-add_tail.c:(11,11)*//*sid:2*/
    h = (struct intlist_ *)((struct char_P *)0);
    /*effects: h <- */
    /*Loc=../../../c/intlist-lib-add_tail.c:(11,11)*//*sid:3*/
    curr = h;
    /*effects: curr <- */
  }
  /*Loc=../../../c/intlist-lib-add_tail.c:(12,12)*//*sid:4*/
  h = head;
  /*Loc=../../../c/intlist-lib-add_tail.c:(13,13)*//*sid:5*/
  while (h != (struct char_P *)0) {
    /*Loc=../../../c/intlist-lib-add_tail.c:(14,14)*//*sid:9*/
    curr = h->next;
    /*Loc=../../../c/intlist-lib-add_tail.c:(15,15)*//*sid:11*/
    if (curr != (struct char_P *)0) {
      /*Loc=../../../c/intlist-lib-add_tail.c:(16,16)*//*sid:12*/
      h = (struct intlist_ *)((struct char_P *)0);
      /*Loc=../../../c/intlist-lib-add_tail.c:(17,17)*//*sid:13*/
      h = curr;
      /*Loc=../../../c/intlist-lib-add_tail.c:(18,18)*//*sid:14*/
      curr = (struct intlist_ *)((struct char_P *)0);
    }
    else { /*Loc=../../../c/intlist-lib-add_tail.c:(20,20)*//*sid:15*/ break;
    }
  }
  /*Loc=../../../c/intlist-lib-add_tail.c:(22,22)*//*sid:16*/
  curr = (intlist )malloc(sizeof(struct intlist_ ));
  /*Loc=../../../c/intlist-lib-add_tail.c:(23,23)*//*sid:17*/
  curr->data = value;
  /*Loc=../../../c/intlist-lib-add_tail.c:(24,24)*//*sid:18*/
  curr->next = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-lib-add_tail.c:(25,25)*//*sid:20*/
  if (h != (struct char_P *)0) {
    /*Loc=../../../c/intlist-lib-add_tail.c:(26,26)*//*sid:21*/
    h->next = curr;
    /*Loc=../../../c/intlist-lib-add_tail.c:(27,27)*//*sid:22*/
    h = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-lib-add_tail.c:(28,28)*//*sid:23*/
    h = head;
  }
  else {
    /*Loc=../../../c/intlist-lib-add_tail.c:(30,30)*//*sid:24*/
    h = curr;
  }
  /*Loc=../../../c/intlist-lib-add_tail.c:(31,31)*//*sid:25*/
  curr = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-lib-add_tail.c:(32,32)*//*sid:26*/
  return (h);
}

