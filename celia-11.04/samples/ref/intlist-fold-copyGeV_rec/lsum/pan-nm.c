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
intlist fold_rec_copyGeV(intlist x , int v ) ;
/*Loc=../../../c/intlist-fold-copyGeV_rec.c(9,9)*//*@ requires //id:67
                                                               acyclic{
                                                               Here}(x);
                                                      behavior default:
                                                        
                                                    
                                                  */
                                                  intlist fold_rec_copyGeV(
intlist x , int v ) 
{
  intlist y ;
  intlist z ;
  intlist tmp ;
  /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(11,11)*//*sid:1*/
  { /*undefined sequence*/ 
    /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(11,11)*//*sid:2*/
    tmp = (struct intlist_ *)((struct char_P *)0);
    /*effects: tmp <- */
    /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(11,11)*//*sid:3*/
    z = tmp;
    /*effects: z <- */
    /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(11,11)*//*sid:4*/
    y = z;
    /*effects: y <- */
  }
  /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(12,12)*//*sid:6*/
  if (x != (struct char_P *)0) {
    /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(13,13)*//*sid:7*/
    z = x->next;
    /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(14,14)*//*sid:9*/
    if (x->data >= v) {
      /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(15,15)*//*sid:10*/
      y = (intlist )malloc(sizeof(struct intlist_ ));
      /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(16,16)*//*sid:11*/
      y->data = x->data;
      /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(17,17)*//*sid:12*/
      y->next = (struct intlist_ *)((struct char_P *)0);
      /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(18,18)*//*sid:13*/
      tmp = fold_rec_copyGeV(z,v);
      /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(19,19)*//*sid:14*/
      y->next = tmp;
      /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(20,20)*//*sid:15*/
      tmp = (struct intlist_ *)((struct char_P *)0);
    }
    else {
      /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(22,22)*//*sid:16*/
      y = fold_rec_copyGeV(z,v);
    }
    /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(23,23)*//*sid:17*/
    z = (struct intlist_ *)((struct char_P *)0);
  }
  /*Loc=../../../c/intlist-fold-copyGeV_rec.c:(25,25)*//*sid:19*/
  return (y);
}


