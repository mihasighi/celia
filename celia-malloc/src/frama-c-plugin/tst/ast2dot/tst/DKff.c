// algorithm: DKff

/*
Features 
------------------------------------------------------------------
 -link together the avaliable segments:the fist word of each free storage area contains the size of that block and the address of the next free area;
 -variable AVAIL pointing to the frist free block;
 -first-fit;
 -does not split a free chunk if the remainder of the chunks is under some limit (8 bytes) during allocation;
 -AVAIL: points to the first abaliable block of storage
 -AVAIL is a head variable and can not be allocated (*)
 -Each block has two fields:SIZE(P),the number of words in the block; and
 -nlink(P), a pointer to the next available block, P is an address.
 -The last pointer is \deta(#) .
 -address sorted
 -acyclic free list
 
 
Pseudocode
------------------------------------------------------------------
Algorithm A:Allocation(Page:439 <The Art of Computer Programing> )

 s1.[initialise]
 Q is address of AVAIL
 Q=LOC(AVAIL)
 nlink(LOC(AVAIL)) = _|_ //
 P = nlink(Q) //P is the next block
 
 s2.[check pointer]
 if P  = nil return unsuccess;
 
 s3.[check size]
 if P! = nil
 if SIZE(P) < N
 Q=P and go s2
 else go s4
 
 s4.[split] //allocate fren the end of the block
 k=SIZE(P)-N
 if (k=0)// exaclly the whole free block
 nlink(Q) =nlink(P) //remove P
 else //the remaining block is set to P
 SIZE(P)=k;
 
 */

/*(Page:440 <The Art of Computer Programing> )
Algorithm B: with realsing an allocated block
 AVAIL list is sorted by memory location (LINK(P)>P)
 
 relase po with size N to AVAIL list
 
 s1:[initialize]
 q=LOC(AVAIL);  // q points to the head free block
 
 s2:[Advance P]
 p=LINK(q);     // p points to the first free block. p is next free block of q
 if p=nil or p>po // p is at right of the po:  [po||||||][p|||||]
 go s3
 else
 q=p; go s2 // to fine the the most close free block to po(at po's right)
 
 s3:[check upper bound]
 if po+N= p & p!=nil //po and p are adjacent
 N=N+SIZE(p); //  compact them as one bigger block
 LINK(po)=LINK(p);
 else
 LINK(po)=p; // keep free block list in address ordered
 
 s4:[check lower bound]
 if q+SIZE(q)=po  //q is at left of po
 SIZE(q)=SIZE(p)+SIZE(po)// combine them as bigger, still pointed by q
 LINK(q)=LINK(po);//  in fact, q pointes to p
 else
 LINK(Q)=po;
 
 */


 
#include <stdio.h>

typedef struct node{
    size_t size;
    struct node * nlink;  /* next free block */
}node_t;

/*global variables*/

#define  MAX_WORDS 2048

node_t  * AVAIL; //free list head,with size is 0
node_t  * nil=NULL;   // last pointer \deta

void kninit(void)
{
    node_t * p= (node_t*) malloc(MAX_WORDS * sizeof(node_t)); /* allocate a big memory block*/
    AVAIL= (node_t* ) malloc(sizeof(node_t));         /* a node points to the free block head*/
    
    p->size=MAX_WORDS;      /* aligned by sizeof(node_t)*/
    AVAIL->size=0;
    AVAIL->nlink=p;
    p->nlink=nil;           /* acylic free list */
}


/*
 Search from the free block list, two pointers, p and prev to record the location,
 prev is one step later than p.
 */
void * knalloc(size_t n){
     
    node_t * p= AVAIL->nlink;   /* from the free list head */
    node_t * pre=AVAIL;
    node_t * q;                 /* to store the block after spliting */
    while(p!=nil){
        if (p->size==n){         /* exaclly the same size */
            pre->nlink=p->nlink; /* remove p from list */
            q=p;
            return (void*) q;
        }
        if (p->size>n){          /*suitable block is bigger than n */
            p->size=p->size-n;
            q=p+p->size;        /* allocate tell end */
            q->size=n;
            return (void*)q+1;
        }
        p=p->nlink;
        pre=pre->nlink;
    }
    return NULL;
}

/* relase po with size N to AVAIL list */
void knfree(void * x){
    
    node_t * po=x-1;
    node_t * q= AVAIL;//
    node_t * p=q->nlink;//
    
    /*to find two block in address order as q < po <p */
    while ((p!=nil)|| p>po){
        /* find a block p is higher up in memory than returnee. */
        if(po+po->size==p){     /*they are contigous*/
            po->size+=p->size;
            po->nlink=p->nlink;
            break;
        }
        if (po+po->size<p){
            po->nlink=p;
            break;
        }
        q=q->nlink;
        p=q->nlink;
    }
    
    /*check lower*/
    if (q+q->size==po) { //there are contigous
        q->size+=po->size; //over merge po
        q->nlink=po->nlink;
    } else {
        q->nlink=po; //add po into free block list
    }
}

void test(){
 
}

int main(int argc, const char * argv[]) {
    
    test();
    return 0;
}


