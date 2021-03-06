/* Generated by Frama-C */
struct char_P {
   char char_M ;
};
struct void_P {
   
};
struct char_P (* /*[2]*/  __string__)[2] ;
/*@ global invariant valid___string__: \valid_range(__string__,0,1);
 */
struct char_P (* /*[4]*/  __string__d_)[4] ;
/*@ global invariant valid___string__d_: \valid_range(__string__d_,0,3);
 */
/*@ behavior default:
      assigns \nothing;  */
void free(struct char_P *ptr ) ;
/*@ behavior default:
      assigns \nothing;  */
struct char_P *malloc(unsigned int size ) ;
struct intlist_;
typedef struct intlist_ *intlist;
struct intlist_ {
   int data ;
   intlist next ;
};
/*@ behavior default:
      assigns \at(\result,Post) \from __format->char_M;  */
extern int printf(struct char_P * __restrict  __format  , ...) ;
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
void printlist(intlist head ) ;
/*Loc=../../../c/intlist-lib-printlist.c(7,7)*//*@ requires //id:67
                                                            acyclic{Here}
                                                            (head);
                                                   behavior default:
                                                     
                                                 
                                               */
                                               void printlist(intlist head ) 
{
  intlist tmp ;
  intlist curr ;
  /*Loc=../../../c/intlist-lib-printlist.c:(9,9)*//*sid:2*/
  curr = head;
  /*Loc=../../../c/intlist-lib-printlist.c:(11,11)*//*sid:3*/
  while (curr) {
    /*Loc=../../../c/intlist-lib-printlist.c:(12,12)*//*sid:7*/
    printf(__string__d_,curr->data);
    /*Loc=../../../c/intlist-lib-printlist.c:(13,13)*//*sid:8*/
    tmp = curr->next;
    /*Loc=../../../c/intlist-lib-printlist.c:(14,14)*//*sid:9*/
    curr = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-lib-printlist.c:(15,15)*//*sid:10*/
    curr = tmp;
    /*Loc=../../../c/intlist-lib-printlist.c:(16,16)*//*sid:11*/
    tmp = (struct intlist_ *)((struct char_P *)0);
  }
  /*Loc=../../../c/intlist-lib-printlist.c:(18,18)*//*sid:12*/
  printf(__string__);
  /*Loc=../../../c/intlist-lib-printlist.c:(19,19)*//*sid:14*/
  return;
}


