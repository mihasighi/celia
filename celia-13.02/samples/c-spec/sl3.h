/** ACSL definitions used in the SL3 annotations.
 */

/* =================================================================== */
/* Heap vertices as type declaration */
/* =================================================================== */
/** The set of vertices of the heap is modeled by the type @code{Node}. 
 *  The values in @code{Node} are vectors of @code{intlist}
 *  denoting list segments with no pointer variables pointing inside.
 */

/*@ type Node = intlist;
*/

/** The value for the `null' (sharp) node.
 */
/*@ logic Node nilNode = \null;
*/

/* =================================================================== */
/* Terms as functions */
/* =================================================================== */

/** The term `len(n)' is modeled using the function @code{len} below.
 *  Warning: The definition of function @code{len} below is not correct;
 *           we use the term without its axiomatisation.
 */

/*@ logic int len(Node n) = n->data ;
 */

/** The terms `dt(n)' and `n[y]' are modeled using the function below.
 *  Warning: The definition of function @code{data} below is not correct;
 *           we use the term without its axiomatisation.
 */
/*@ logic int data(Node n, int y) = n->data;
 */


/** In addition to the terms of the SL3 logic, we introduce terms
 *  to allow the specification of the collection of data in list segments
 *  like in the CAV'10 paper.
 *
 *  These terms concern:
 *  - the sum of elements and 
 *  - the multiset of elements
 *  of a list segment.
 *
 *  Warning: ACSL provides the notation @code{\sum} but it is not supported
 *           by (1) the current version of Frama-C and (2) the typing of nodes.
 *  Warning: ACSL provides notation for the set of elements of an array
 *           n[0 .. ] but the Carbon version of Frama-C does not support it! 
 *  Warning: The axiomatisations below are not used, the terms are not 
 *           interpreted. Thus, we use the set notations for the bag.
 */

/*@ logic int sum(Node n) = n->data ;
*/
/*@ logic set<int> mset(Node n) = { n->data } ;
 */

/* =================================================================== */
/* Atomic formulas as predicates */
/* =================================================================== */

/** The edge formulas `ls(n,m)' and `ls(n,#)' are modeled using 
 *  the predicate @code{ls} below.
 *  Warning: As for the @code{len} function, we do not use the axiomatisation
 *           below, but only the notation.
 */

/*@ predicate ls(Node n, Node m) = len(n) >= 1;
  @ predicate acyclic(Node n) = ls(n, nilNode);
  @ predicate cyclic(Node n) = ls(n, n);
  @ predicate reach(Node n, Node m) = ls(n, m);
 */

/** The pointer formula `x(n)' is modeling using the predicate @code{label}
 *  below.
 */

/*@ predicate label(intlist x, Node n) = x==n ;
 */

/** The separation connector in edge formulas is modeled using the 
 *  ACSL predicate @code{\separated}.
 *  To correctly type @code{\separated}, we need the @code{sloc} below.
 */

/*@ logic set<intlist> sloc(Node n) = { n->next }; 
 */


/** The sequence formulas use the notations above and the ACSL notations.
 *  For example, the ACSL formula below expresses the sorted-ness property
 *  for the list segment starting from n:
 *
 *  sorted(n):
 *     (\forall int y1, y2 ; Gle2(y1,y2,len(n)) ==>
 *                         data(n,y1) <= data(n,y2)) 
 *  && (\forall int y ; Gall(y,n) ==> data(n,0) <= data(n,y))
 *
 */

/** The predicates below are useful to easily recognize the guards of the
 *  implemented fragment of SL3.
 *  Warning: The current implementation of the verification plugin and
 *  the one of the decision procedure for SL3 do not support arbitrary guards! 
 *  Please use the guards below!
 */
/*@ predicate Gall(int y, Node n) = 1<= y < len(n);
  @ predicate Gle2(int y1, int y2, Node n) = 1<= y1 <= y2 < len(n);
  @ predicate Gsucc2(int y1, int y2, Node n) = 1<= y1 <= y2 < len(n) && y2==y1+1;
  @ predicate Gfst(int y, Node n) = 1<= y < len(n) && y==1;
  @ predicate Glst(int y, Node n) = 1<= y < len(n) && y==len(n)-1;
  @ predicate Geq2(int y1, Node n1, int y2, Node n2) = 1<= y1 < len(n1) && 1<= y2 < len(n2) && y1==y2;
  @ predicate Gsl2(int y1, Node n1, int y2, Node n2, int c) = 1<= y1 < len(n1) && 1 <= y2 <len(n2) && y1+c==y2;
  @ predicate Gsr2(int y1, Node n1, int y2, Node n2, int c) = 1<= y1 < len(n1) && 1 <= y2 <len(n2) && y1==y2+c;
*/
