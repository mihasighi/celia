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
int fold2_equal(intlist x , intlist y ) ;
/*Loc=../../../c/intlist-fold2-equal.c(9,9)*//*@ requires //id:67
                                                          ((acyclic{Here}(x)
                                                           ∧
                                                           acyclic{Here}(y))
                                                          ∧
                                                          disjoint{Here}
                                                          (x, y));
                                                 behavior default:
                                                   
                                               
                                             */
                                             int fold2_equal(intlist x ,
                                                             intlist y ) 
{
  intlist xi ;
  intlist yi ;
  intlist tmp ;
  int __retres ;
  /*Loc=../../../c/intlist-fold2-equal.c:(11,11)*//*sid:1*/
  { /*undefined sequence*/ 
    /*Loc=../../../c/intlist-fold2-equal.c:(11,11)*//*sid:2*/
    tmp = (struct intlist_ *)((struct char_P *)0);
    /*effects: tmp <- */
    /*Loc=../../../c/intlist-fold2-equal.c:(11,11)*//*sid:3*/
    yi = tmp;
    /*effects: yi <- */
    /*Loc=../../../c/intlist-fold2-equal.c:(11,11)*//*sid:4*/
    xi = yi;
    /*effects: xi <- */
  }
  /*Loc=../../../c/intlist-fold2-equal.c:(12,12)*//*sid:5*/
  xi = x;
  /*Loc=../../../c/intlist-fold2-equal.c:(13,13)*//*sid:6*/
  yi = y;
  /*Loc=../../../c/intlist-fold2-equal.c:(14,14)*//*sid:7*/
  while (xi != (struct char_P *)0) {
    /*Loc=../../../c/intlist-fold2-equal.c:(15,15)*//*sid:12*/
    if (yi != (struct char_P *)0) {
      /*Loc=../../../c/intlist-fold2-equal.c:(16,16)*//*sid:14*/
      if (xi->data != yi->data) {
        /*Loc=../../../c/intlist-fold2-equal.c:(17,17)*//*sid:15*/
        __retres = 0;
        /*Loc=../../../c/intlist-fold2-equal.c:(17,17)*//*sid:34*/
        goto return_label;
      }
      /*Loc=../../../c/intlist-fold2-equal.c:(18,18)*//*sid:17*/
      tmp = xi->next;
      /*Loc=../../../c/intlist-fold2-equal.c:(19,19)*//*sid:18*/
      xi = (struct intlist_ *)((struct char_P *)0);
      /*Loc=../../../c/intlist-fold2-equal.c:(20,20)*//*sid:19*/
      xi = tmp;
      /*Loc=../../../c/intlist-fold2-equal.c:(21,21)*//*sid:20*/
      tmp = (struct intlist_ *)((struct char_P *)0);
      /*Loc=../../../c/intlist-fold2-equal.c:(22,22)*//*sid:21*/
      tmp = yi->next;
      /*Loc=../../../c/intlist-fold2-equal.c:(23,23)*//*sid:22*/
      yi = (struct intlist_ *)((struct char_P *)0);
      /*Loc=../../../c/intlist-fold2-equal.c:(24,24)*//*sid:23*/
      yi = tmp;
      /*Loc=../../../c/intlist-fold2-equal.c:(25,25)*//*sid:24*/
      tmp = (struct intlist_ *)((struct char_P *)0);
    }
    else { /*Loc=../../../c/intlist-fold2-equal.c:(26,26)*//*sid:25*/ break;
    }
  }
  /*Loc=../../../c/intlist-fold2-equal.c:(28,28)*//*sid:27*/
  if (xi == (struct char_P *)0) {
    /*Loc=../../../c/intlist-fold2-equal.c:(29,29)*//*sid:29*/
    if (yi == (struct char_P *)0) {
      /*Loc=../../../c/intlist-fold2-equal.c:(30,30)*//*sid:30*/
      __retres = 1;
      /*Loc=../../../c/intlist-fold2-equal.c:(30,30)*//*sid:35*/
      goto return_label;
    }
  }
  /*Loc=../../../c/intlist-fold2-equal.c:(31,31)*//*sid:33*/
  __retres = 0;
  /*Loc=../../../c/intlist-fold2-equal.c:(31,31)*/return_label: /* internal */ 
  /*sid:36*/
  return (__retres);
}


