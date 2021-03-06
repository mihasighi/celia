/* Generated by Frama-C */
struct char_P {
   char char_M ;
};
struct void_P {
   
};
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
      assigns \nothing;  */
extern  __attribute__((__nothrow__)) void free(struct char_P *__ptr ) ;
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
intlist del(intlist head ) ;
/*Loc=../../../c/intlist-lib-del.c(8,8)*//*@ requires //id:67
                                                      acyclic{Here}(head);
                                             behavior default:
                                               
                                           
                                         */
                                         intlist del(intlist head ) 
{
  intlist curr ;
  /*Loc=../../../c/intlist-lib-del.c:(10,10)*//*sid:1*/
  curr = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-lib-del.c:(11,11)*//*sid:2*/
  curr = head->next;
  /*Loc=../../../c/intlist-lib-del.c:(12,12)*//*sid:3*/
  free((struct char_P *)head);
  /*Loc=../../../c/intlist-lib-del.c:(13,13)*//*sid:4*/
  return (curr);
}


