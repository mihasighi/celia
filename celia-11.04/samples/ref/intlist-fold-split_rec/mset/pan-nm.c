/* Generated by Frama-C */
struct struct_intlist__xP {
   struct intlist_ *struct_intlist__xM ;
};
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
/*Loc=../../../c/intlist-fold-split_rec.c(12,12)*//*@ requires //id:67
                                                               acyclic{
                                                               Here}(x);
                                                      behavior default:
                                                        
                                                    
                                                  */
                                                  void split(intlist x ,
                                                             struct struct_intlist__xP *l1 ,
                                                             struct struct_intlist__xP *l2 ) 
{
  intlist tmp ;
  intlist nxt ;
  intlist ll1 ;
  intlist ll2 ;
  /*Loc=../../../c/intlist-fold-split_rec.c:(16,16)*//*sid:2*/
  if (x == (struct char_P *)0) {
    /*Loc=../../../c/intlist-fold-split_rec.c:(17,17)*//*sid:3*/
    l1->struct_intlist__xM = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-fold-split_rec.c:(18,18)*//*sid:4*/
    l2->struct_intlist__xM = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-fold-split_rec.c:(19,19)*//*sid:37*/
    goto return_label;
  }
  /*Loc=../../../c/intlist-fold-split_rec.c:(22,22)*//*sid:7*/
  nxt = x->next;
  /*Loc=../../../c/intlist-fold-split_rec.c:(23,23)*//*sid:8*/
  ll1 = (intlist )malloc(sizeof(struct intlist_ ));
  /*Loc=../../../c/intlist-fold-split_rec.c:(24,24)*//*sid:9*/
  ll1->data = x->data;
  /*Loc=../../../c/intlist-fold-split_rec.c:(25,25)*//*sid:10*/
  ll1->next = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-fold-split_rec.c:(27,27)*//*sid:12*/
  if (nxt == (struct char_P *)0) {
    /*Loc=../../../c/intlist-fold-split_rec.c:(28,28)*//*sid:13*/
    l1->struct_intlist__xM = ll1;
    /*Loc=../../../c/intlist-fold-split_rec.c:(29,29)*//*sid:14*/
    l2->struct_intlist__xM = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-fold-split_rec.c:(30,30)*//*sid:15*/
    { /*undefined sequence*/ 
      /*Loc=../../../c/intlist-fold-split_rec.c:(30,30)*//*sid:16*/
      ll1 = (struct intlist_ *)((struct char_P *)0);
      /*effects: ll1 <- */
      /*Loc=../../../c/intlist-fold-split_rec.c:(30,30)*//*sid:17*/
      nxt = ll1;
      /*effects: nxt <- */
    }
    /*Loc=../../../c/intlist-fold-split_rec.c:(31,31)*//*sid:38*/
    goto return_label;
  }
  /*Loc=../../../c/intlist-fold-split_rec.c:(34,34)*//*sid:20*/
  ll2 = (intlist )malloc(sizeof(struct intlist_ ));
  /*Loc=../../../c/intlist-fold-split_rec.c:(35,35)*//*sid:21*/
  ll2->data = nxt->data;
  /*Loc=../../../c/intlist-fold-split_rec.c:(36,36)*//*sid:22*/
  ll2->next = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-fold-split_rec.c:(37,37)*//*sid:23*/
  tmp = nxt->next;
  /*Loc=../../../c/intlist-fold-split_rec.c:(39,39)*//*sid:24*/
  split(tmp,l1,l2);
  /*Loc=../../../c/intlist-fold-split_rec.c:(41,41)*//*sid:25*/
  ll1->next = l1->struct_intlist__xM;
  /*Loc=../../../c/intlist-fold-split_rec.c:(42,42)*//*sid:26*/
  ll2->next = l2->struct_intlist__xM;
  /*Loc=../../../c/intlist-fold-split_rec.c:(43,43)*//*sid:27*/
  l1->struct_intlist__xM = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-fold-split_rec.c:(43,43)*//*sid:28*/
  l1->struct_intlist__xM = ll1;
  /*Loc=../../../c/intlist-fold-split_rec.c:(44,44)*//*sid:29*/
  l2->struct_intlist__xM = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-fold-split_rec.c:(44,44)*//*sid:30*/
  l2->struct_intlist__xM = ll2;
  /*Loc=../../../c/intlist-fold-split_rec.c:(46,46)*//*sid:31*/
  { /*undefined sequence*/ 
    /*Loc=../../../c/intlist-fold-split_rec.c:(46,46)*//*sid:32*/
    nxt = (struct intlist_ *)((struct char_P *)0);
    /*effects: nxt <- */
    /*Loc=../../../c/intlist-fold-split_rec.c:(46,46)*//*sid:33*/
    tmp = nxt;
    /*effects: tmp <- */
    /*Loc=../../../c/intlist-fold-split_rec.c:(46,46)*//*sid:34*/
    ll2 = tmp;
    /*effects: ll2 <- */
    /*Loc=../../../c/intlist-fold-split_rec.c:(46,46)*//*sid:35*/
    ll1 = ll2;
    /*effects: ll1 <- */
  }
  /*Loc=../../../c/intlist-fold-split_rec.c:(48,48)*/return_label: /* internal */ 
  /*sid:39*/
  return;
}


