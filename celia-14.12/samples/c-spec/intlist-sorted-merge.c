
#include "intlist.h"
#include "sl3.h"

/**
 * Merge two sorted lists in the result.
 */

/*@ requires (\exists Node n1, Node n2 ; ls(n1,nilNode) && ls(n2,nilNode) && len(n1)>=1 && len(n2)>=1 &&
                  \separated(sloc(n1),sloc(n2)) &&
                  label(x,n1) && label(y,n2) &&
	          sorted(n1) && sorted(n2)
	     );
  @ ensures  (\exists Node n1, Node n2, Node n3 ; ls(n1,nilNode) && ls(n2,nilNode) && ls(n3,nilNode) &&
		  len(n1)>=1 && len(n2)>=1 && len(n3)=len(n1)+len(n2) &&
                  \separated(sloc(n1),sloc(n2),sloc(n3)) &&
                  label(x,n1) && label(y,n2) && label(\result,n3) &&
                  sorted(n1) && sorted(n2) && sorted(n3) &&
	          mset(n3)==mset(n1)+mset(n2)
	     );
 */
intlist fold2_merge(intlist x, intlist y) {
    intlist r, ri, xi, yi, tmp;
    int v;
    r = ri = xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    // There are 13 non-isomorphic graphs!
    /*@ loop invariant
	 	(\exists Node n1, Node n2, Node n6, Node n5 ; 
		     ls(n1,nilNode) && ls(n2,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,nilNode) && label(y,n2) && label(yi,nilNode) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n6)==1 && len(n1)+len(n2)==len(n5)+1 &&
		     sorted(n1) && sorted(n2) && sorted(n5) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n6,(int)0)))

		 )
	     ||  (\exists Node n1, Node n2, Node n5, Node n6, Node n7 ;
		     ls(n1,n7) && ls(n7,nilNode) && ls(n2,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,n7) && label(y,n2) && label(yi,nilNode) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6), sloc(n7)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n7)>=1 && len(n6)==1 && len(n1)+len(n2)==len(n5)+1 &&
		     sorted(n1) && sorted(n7) && sorted(n2) && sorted(n5) &&
		     (data(n7,(int)0)>=data(n6,(int)0) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n7,(int)0))) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n7)==>(data(n7,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n6,(int)0)))
		)
	    ||  (\exists Node n1, Node n2, Node n5, Node n6, Node n7 ;
		     ls(n1,nilNode) && ls(n2,n7) && ls(n7,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,nilNode) && label(y,n2) && label(yi,n7) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6), sloc(n7)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n7)>=1 && len(n6)==1 && len(n1)+len(n2)==len(n5)+1 &&
		     sorted(n1) && sorted(n2) && sorted(n5) && sorted(n7) &&
		     (data(n7,(int)0)>=data(n6,(int)0) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n7,(int)0))) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n7)==>(data(n7,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n6,(int)0)))
		)
	    ||  (\exists Node n1, Node n2, Node n5, Node n6 ;
		     ls(n1,nilNode) && ls(n2,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,nilNode) && label(y,n2) && label(yi,n2) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n6)==1 && len(n1)==len(n5)+1 &&
		     sorted(n1) && sorted(n2) && sorted(n5) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n6,(int)0)))
		)
	    ||  (\exists Node n1, Node n2, Node n5, Node n6 ;
		     ls(n1,nilNode) && ls(n2,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,n1) && label(y,n2) && label(yi,nilNode) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n6)==1 && len(n2)==len(n5)+1 &&
		     sorted(n1) && sorted(n2) && sorted(n5) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n6,(int)0)))
		)
	    ||  (\exists Node n1, Node n2, Node n5 ;
		     ls(n1,nilNode) && ls(n2,nilNode) && ls(n5,nilNode) &&
		     label(x,n1) && label(xi,n1) && label(y,n2) && label(yi,nilNode) && label(r,n5) && label(ri,n5) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n5)==1 && len(n2)==len(n5) &&
		     sorted(n1) && (data(n2,(int)0) == data(n5,(int)0)) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)>=data(n5,(int)0)))
		)
	    ||  (\exists Node n1, Node n2, Node n5 ;
		     ls(n1,nilNode) && ls(n2,nilNode) && ls(n5,nilNode) &&
		     label(x,n1) && label(xi,nilNode) && label(y,n2) && label(yi,n2) && label(r,n5) && label(ri,n5) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n5)==1 && len(n1)==len(n5) &&
		     sorted(n2) && (data(n1,(int)0) == data(n5,(int)0)) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)>=data(n5,(int)0)))
		)
	    ||  (\exists Node n1, Node n2, Node n5, Node n6, Node n7 ;
		     ls(n1,nilNode) && ls(n2,n7) && ls(n7,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,n1) && label(y,n2) && label(yi,n7) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6), sloc(n7)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n7)>=1 && len(n6)==1 && len(n2)==len(n5)+1 &&
		     sorted(n1) && sorted(n2) && sorted(n5) && sorted(n7) &&
		     (data(n7,(int)0)>=data(n6,(int)0) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n7,(int)0))) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n7)==>(data(n7,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n6,(int)0)))
		 )
	     ||  (\exists Node n1, Node n2, Node n5, Node n6, Node n7 ;
		     ls(n1,n7) && ls(n2,nilNode) && ls(n7,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,n7) && label(y,n2) && label(yi,n2) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6), sloc(n7)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n7)>=1 && len(n6)==1 && len(n1)==len(n5)+1 &&
		     sorted(n1) && sorted(n2) && sorted(n5) && sorted(n7) &&
		     (data(n7,(int)0)>=data(n6,(int)0) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n7,(int)0))) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n7)==>(data(n7,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n6,(int)0)))
	 	 ) 
             ||  (\exists Node n1, Node n2, Node n5, Node n6, Node n7, Node n8 ;
		     ls(n1,n7) && ls(n2,n8) && ls(n7,nilNode) && ls(n8,nilNode) && ls(n5,n6) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,n7) && label(y,n2) && label(yi,n8) && label(r,n5) && label(ri,n6) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6), sloc(n7), sloc(n8)) &&
		     len(n1)>=1 && len(n2)>=1 && len(n7)>=1 && len(n8)>=1 && len(n6)==1 && len(n1)+len(n2)==len(n5)+1 &&
		     sorted(n1) && sorted(n2) && sorted(n5) && sorted(n7) && sorted(n8)
		     (data(n7,(int)0)>=data(n6,(int)0) && (data(n8,(int)0)>=data(n6,(int)0) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n7,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n8,(int)0))) &&
		     (\forall int y ; Gall(y,n5)==>(data(n5,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n7)==>(data(n7,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n8)==>(data(n8,y)>=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)<=data(n6,(int)0))) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)<=data(n6,(int)0)))
		 )
	     ||  (\exists Node n1, Node n2, Node n5, Node n6 ;
		     ls(n1,nilNode) && ls(n2,n6) && ls(n5,nilNode) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,n1) && label(y,n2) && label(yi,n6) && label(r,n5) && label(ri,n5) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6)) &&
		     len(n1)>=1 && len(n2)==1 && len(n6)>=1 && len(n5)==1 &&
		     sorted(n1) && sorted(n6) &&
		     (data(n2,(int)0) == data(n5,(int)0)) &&
		     (\forall int y ; Gall(y,n1)==>(data(n1,y)>=data(n5,(int)0))) &&
		     (\forall int y ; Gall(y,n6)==>(data(n6,y)>=data(n5,(int)0)))
		 )
	     ||  (\exists Node n1, Node n2, Node n5, Node n6 ;
		     ls(n1,n6) && ls(n2,nilNode) && ls(n5,nilNode) && ls(n6,nilNode) &&
		     label(x,n1) && label(xi,n6) && label(y,n2) && label(yi,n2) && label(r,n5) && label(ri,n5) &&
		     \separated(sloc(n1), sloc(n2), sloc(n5), sloc(n6)) &&
		     len(n1)==1 && len(n2)>=1 && len(n6)>=1 && len(n5)==1 &&
		     sorted(n2) && sorted(n6) &&
		     (data(n1,(int)0) == data(n5,(int)0)) &&
		     (\forall int y ; Gall(y,n2)==>(data(n2,y)>=data(n5,(int)0))) &&
		     (\forall int y ; Gall(y,n6)==>(data(n6,y)>=data(n5,(int)0)))
		 )
	     ||  (\exists Node n1, Node n2 ;
		     ls(n1,nilNode) && ls(n2,nilNode) && 
		     label(x,n1) && label(xi,n1) && label(y,n2) && label(yi,n2) && label(r,nilNode) && label(ri,nilNode) &&
		     \separated(sloc(n1), sloc(n2)) &&
		     len(n1)>=1 && len(n2)>=1 &&
		     sorted(n1) && sorted(n2)
		)
	     ;
     */
    while (xi != NULL && yi != NULL) {
        if (xi->data < yi->data) {
            v = xi->data;
            tmp = xi->next;
            xi = NULL;
            xi = tmp;
            tmp = NULL;
        } else {
            v = yi->data;
            tmp = yi->next;
            yi = NULL;
            yi = tmp;
            tmp = NULL;
        }
        // add v at the end of r, pointed by ri
        tmp = (intlist) malloc(sizeof (struct intlist_));
        tmp->data = v;
        tmp->next = NULL;
        if (r == NULL) {
            r = tmp;
            ri = tmp;
        } else {
            ri->next = tmp;
            ri = NULL;
            ri = tmp;
        }
        tmp = NULL;
    }
    while (xi != NULL) {
        // add v at the end of r, pointed by ri
        tmp = (intlist) malloc(sizeof (struct intlist_));
        tmp->data = xi->data;
        tmp->next = NULL;
        if (r == NULL) {
            r = tmp;
            ri = tmp;
        } else {
            ri->next = tmp;
            ri = NULL;
            ri = tmp;
        }
        tmp = NULL;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
    while (yi != NULL) { // add v at the end of r, pointed by ri
        tmp = (intlist) malloc(sizeof (struct intlist_));
        tmp->data = yi->data;
        tmp->next = NULL;
        if (r == NULL) {
            r = tmp;
            ri = tmp;
        } else {
            ri->next = tmp;
            ri = NULL;
            ri = tmp;
        }
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
    ri = NULL; /* no more used */
    return r;
}

