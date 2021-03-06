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
intlist add(intlist head , int value ) ;
/*Loc=../../../c/intlist-lib-add.c(9,9)*//*@ requires //id:67
                                                      acyclic{Here}(head);
                                             behavior default:
                                               
                                           
                                         */
                                         intlist add(intlist head ,
                                                     int value ) 
{
  intlist curr ;
  /*Loc=../../../c/intlist-lib-add.c:(11,11)*//*sid:1*/
  curr = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-lib-add.c:(12,12)*//*sid:2*/
  curr = (intlist )malloc(sizeof(struct intlist_ ));
  /*Loc=../../../c/intlist-lib-add.c:(13,13)*//*sid:3*/
  curr->data = value;
  /*Loc=../../../c/intlist-lib-add.c:(14,14)*//*sid:4*/
  curr->next = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-lib-add.c:(15,15)*//*sid:5*/
  curr->next = head;
  /*Loc=../../../c/intlist-lib-add.c:(16,16)*//*sid:6*/
  return (curr);
}


