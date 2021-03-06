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
void map2_rec_copy(intlist dst , intlist src ) ;
/*Loc=../../../c/intlist-map2-copy_rec.c(9,9)*//*@ requires //id:67
                                                            ((acyclic{
                                                              Here}(src)
                                                             ∧
                                                             acyclic{
                                                             Here}(dst))
                                                            ∧
                                                            disjoint{
                                                            Here}(src, dst));
                                                   requires //id:36
                                                            ((acyclic{
                                                              Here}(dst)
                                                             ∧
                                                             acyclic{
                                                             Here}(src))
                                                            ∧
                                                            disjoint{
                                                            Here}(dst, src));
                                                   behavior default:
                                                     
                                                 
                                               */
                                               void map2_rec_copy(intlist dst ,
                                                                  intlist src ) 
{
  intlist srci ;
  intlist dsti ;
  /*Loc=../../../c/intlist-map2-copy_rec.c:(10,10)*//*sid:2*/
  if (dst != (struct char_P *)0) {
    /*Loc=../../../c/intlist-map2-copy_rec.c:(11,11)*//*sid:4*/
    if (src != (struct char_P *)0) {
      /*Loc=../../../c/intlist-map2-copy_rec.c:(13,13)*//*sid:5*/
      { /*undefined sequence*/ 
        /*Loc=../../../c/intlist-map2-copy_rec.c:(13,13)*//*sid:6*/
        dsti = (struct intlist_ *)((struct char_P *)0);
        /*effects: dsti <- */
        /*Loc=../../../c/intlist-map2-copy_rec.c:(13,13)*//*sid:7*/
        srci = dsti;
        /*effects: srci <- */
      }
      /*Loc=../../../c/intlist-map2-copy_rec.c:(14,14)*//*sid:8*/
      srci = src->next;
      /*Loc=../../../c/intlist-map2-copy_rec.c:(15,15)*//*sid:9*/
      dsti = dst->next;
      /*Loc=../../../c/intlist-map2-copy_rec.c:(16,16)*//*sid:10*/
      dst->data = src->data;
      /*Loc=../../../c/intlist-map2-copy_rec.c:(17,17)*//*sid:11*/
      map2_rec_copy(dsti,srci);
      /*Loc=../../../c/intlist-map2-copy_rec.c:(18,18)*//*sid:12*/
      { /*undefined sequence*/ 
        /*Loc=../../../c/intlist-map2-copy_rec.c:(18,18)*//*sid:13*/
        dsti = (struct intlist_ *)((struct char_P *)0);
        /*effects: dsti <- */
        /*Loc=../../../c/intlist-map2-copy_rec.c:(18,18)*//*sid:14*/
        srci = dsti;
        /*effects: srci <- */
      }
    }
  }
  /*Loc=../../../c/intlist-map2-copy_rec.c:(20,20)*//*sid:18*/
  return;
}


