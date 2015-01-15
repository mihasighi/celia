/* Generated by Frama-C */
struct char_P {
   char char_M ;
};
struct void_P {
   
};
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
int fold_sum(intlist x ) ;
/*Loc=../../../c/intlist-fold-sum.c(9,9)*//*@ requires //id:67
                                                       acyclic{Here}(x);
                                              behavior default:
                                                
                                            
                                          */
                                          int fold_sum(intlist x ) 
{
  intlist xi ;
  intlist tmp ;
  int sum ;
  /*Loc=../../../c/intlist-fold-sum.c:(11,11)*//*sid:1*/
  sum = 0;
  /*Loc=../../../c/intlist-fold-sum.c:(12,12)*//*sid:2*/
  { /*undefined sequence*/ 
    /*Loc=../../../c/intlist-fold-sum.c:(12,12)*//*sid:3*/
    tmp = (struct intlist_ *)((struct char_P *)0);
    /*effects: tmp <- */
    /*Loc=../../../c/intlist-fold-sum.c:(12,12)*//*sid:4*/
    xi = tmp;
    /*effects: xi <- */
  }
  /*Loc=../../../c/intlist-fold-sum.c:(13,13)*//*sid:5*/
  xi = x;
  /*Loc=../../../c/intlist-fold-sum.c:(14,14)*//*sid:6*/
  while (xi != (struct char_P *)0) {
    /*Loc=../../../c/intlist-fold-sum.c:(15,15)*//*sid:10*/
    sum += xi->data;
    /*Loc=../../../c/intlist-fold-sum.c:(16,16)*//*sid:11*/
    tmp = xi->next;
    /*Loc=../../../c/intlist-fold-sum.c:(17,17)*//*sid:12*/
    xi = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-fold-sum.c:(18,18)*//*sid:13*/
    xi = tmp;
    /*Loc=../../../c/intlist-fold-sum.c:(19,19)*//*sid:14*/
    tmp = (struct intlist_ *)((struct char_P *)0);
  }
  /*Loc=../../../c/intlist-fold-sum.c:(21,21)*//*sid:15*/
  return (sum);
}

