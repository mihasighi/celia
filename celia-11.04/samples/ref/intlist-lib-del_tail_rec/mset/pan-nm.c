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
intlist del_tail_rec(intlist head ) ;
/*Loc=../../../c/intlist-lib-del_tail_rec.c(6,6)*//*@ requires //id:67
                                                               acyclic{
                                                               Here}(head);
                                                      behavior default:
                                                        
                                                    
                                                  */
                                                  intlist del_tail_rec(
intlist head ) 
{
  intlist curr ;
  intlist tmp ;
  intlist __retres ;
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(8,8)*//*sid:1*/
  { /*undefined sequence*/ 
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(8,8)*//*sid:2*/
    tmp = (struct intlist_ *)((struct char_P *)0);
    /*effects: tmp <- */
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(8,8)*//*sid:3*/
    curr = tmp;
    /*effects: curr <- */
  }
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(9,9)*//*sid:5*/
  if (head == (struct char_P *)0) {
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(10,10)*//*sid:6*/
    __retres = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(10,10)*//*sid:21*/
    goto return_label;
  }
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(11,11)*//*sid:8*/
  curr = head->next;
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(12,12)*//*sid:10*/
  if (curr == (struct char_P *)0) {
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(13,13)*//*sid:11*/
    free((struct char_P *)head);
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(14,14)*//*sid:12*/
    __retres = (struct intlist_ *)((struct char_P *)0);
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(14,14)*//*sid:22*/
    goto return_label;
  }
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(16,16)*//*sid:14*/
  tmp = del_tail_rec(curr);
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(17,17)*//*sid:15*/
  head->next = (struct intlist_ *)((struct char_P *)0);
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(18,18)*//*sid:16*/
  head->next = tmp;
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(19,19)*//*sid:17*/
  { /*undefined sequence*/ 
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(19,19)*//*sid:18*/
    tmp = (struct intlist_ *)((struct char_P *)0);
    /*effects: tmp <- */
    /*Loc=../../../c/intlist-lib-del_tail_rec.c:(19,19)*//*sid:19*/
    curr = tmp;
    /*effects: curr <- */
  }
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(20,20)*//*sid:20*/
  __retres = head;
  /*Loc=../../../c/intlist-lib-del_tail_rec.c:(20,20)*/return_label: /* internal */ 
  /*sid:23*/
  return (__retres);
}

