/*
 * ucons_representation.c
 *
 * Functions related to the representation of universal constraints.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include <stdio.h>
#include <stdlib.h>
#include "shape_macros.h"
#include "ucons.h"
#include "ucons_internal.h"
#include "ap_generic.h"
#include "apron2shape.h"
#include "ap_abstract0.h"
#include "box.h"
#include "oct.h"
#include "pk.h"





void initialize_PI( ucons_internal_t *pr){
	pattern_info_t* PI;
	size_t PI_size;

	checked_malloc(PI, pattern_info_t ,5,sizeof(pattern_info_t),return ; );

	PI_size = 5;
	/* \forall y. */
	pattern_info_t p1,p2,p3,p4,p5;
	p1.kind=pattern_1;
	p1.e_seg=0;
	p1.u_seg=1;
	//p1.uvar=NULL;
	checked_malloc(p1.uvar,pattern_var_t,1,sizeof(pattern_var_t),return ;);
	p1.uvar[0].size=1;
	p1.uvar[0].reach=NULL;
	p1.lcons=-1;// no constraints
	p1.nr_y = 1;

	/*\forall y1 in ?_1 \forall y2 in ?_2 */
	p2.kind=pattern_2_1;
	p2.e_seg=0;
	p2.u_seg=2;
	//p2.uvar=NULL;
	checked_malloc(p2.uvar,pattern_var_t,2,sizeof(pattern_var_t),return ;);
	p2.uvar[0].size=1;
	p2.uvar[0].reach=NULL;
	p2.uvar[1].size=1;
	p2.uvar[1].reach=NULL;
	p2.lcons=-1;// no constraints y1=y2
	p2.nr_y = 2;

	p3.kind=pattern_3_1;
	p3.e_seg=0;
	p3.u_seg=3;
	//p3.uvar=NULL;
	checked_malloc(p3.uvar,pattern_var_t,3,sizeof(pattern_var_t),return ;);
	p3.uvar[0].size=1;
	p3.uvar[0].reach=NULL;
	p3.uvar[1].size=1;
	p3.uvar[1].reach=NULL;
	p3.uvar[2].size=1;
	p3.uvar[2].reach=NULL;
	p3.lcons=-1;// no constraints
	p3.nr_y = 3;

	/*\forall y1, y2. y1 < y2  */
	p4.kind=pattern_1_2;
	p4.e_seg=0;
	p4.u_seg=1;
	checked_malloc(p4.uvar,pattern_var_t,1,sizeof(pattern_var_t),return ;);
	p4.uvar[0].size=2;
	checked_malloc(p4.uvar[0].reach,bool,1,sizeof(bool),return ;);
	p4.uvar[0].reach[0]=true; //y1<y2
	p4.lcons=-1;// no constraints
	p4.nr_y = 2;


	p5.kind=pattern_succ1_3;
	p5.e_seg=0;
	p5.u_seg=1;
	checked_malloc(p5.uvar,pattern_var_t,1,sizeof(pattern_var_t),return ;);
	p5.uvar[0].size=3;
	checked_malloc(p5.uvar[0].reach,bool,2,sizeof(bool),return ;);
	p5.uvar[0].reach[0]=false; //y2 = y1 + 1;
	p5.uvar[0].reach[1]=false; //y3 = y2 + 1;
	p5.lcons=-1;// no constraints
	p5.nr_y = 3;

	PI[0] = p1; PI[1] = p2; PI[2] = p3; PI[3] = p4; PI[4] = p5;

	pr->PI=PI;
	pr->PI_size = PI_size;



}



/* ********************************************************************** */
/* V. ucons_t */
/* ********************************************************************** */

/* ============================================================ */
/* Internal management */
/* ============================================================ */

/* generic allocation routine, returns ... */

/* generic allocation routine */

/* y : PI[0]*/
void pattern_instance_pattern_1(ucons_internal_t *pr,ucons_t *r){
	pattern_key_t * key;
	size_t i,j;
	unsigned keylen;
	size_t u_seg;
	pattern_t *rn, *rt;


	u_seg = pr->PI[0].u_seg;
	keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);


	for(j=1;j<r->segmentdim;j++){

		checked_malloc(rn,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return ;);
		memset(rn, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

		rn->dcons=NULL;
		rn->key.type=0;
		rn->key.segments[0]=j;
		HASH_FIND(hh,r->udcons,&rn->key, keylen,rt);

		if(rt==NULL){

			HASH_ADD(hh,r->udcons,key,keylen,rn);

			checked_realloc(r->n2p[j].p,
					pattern_key_t*,(r->n2p[j].size+1),sizeof(pattern_key_t *),return ;);
			checked_malloc(r->n2p[j].p[r->n2p[j].size], 
					pattern_key_t,1,sizeof(pattern_key_t)+1*sizeof(size_t),return ;);

			r->n2p[j].size=r->n2p[j].size+1;
			r->n2p[j].p[r->n2p[j].size-1]->type = 0;
			r->n2p[j].p[r->n2p[j].size-1]->segments[0]=j;

		}

	}


}

/* y1 < y2  : PI[3] */
void pattern_instance_pattern_1_2(ucons_internal_t *pr,ucons_t *r){
	pattern_key_t * key;
	size_t i,j;
	unsigned keylen;
	size_t u_seg;
	pattern_t *rn, *rt;


	u_seg = pr->PI[3].u_seg;
	keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);


	for(j=1;j<r->segmentdim;j++){

		checked_malloc(rn,pattern_t,1,sizeof(pattern_t) + (u_seg*sizeof(size_t)),return ;);
		memset(rn, 0, sizeof(pattern_t) + (u_seg*sizeof(size_t)));

		rn->dcons=NULL;
		rn->key.type=3;
		rn->key.segments[0]=j;
		HASH_FIND(hh,r->udcons, &rn->key, keylen, rt);

		if(rt==NULL){

			HASH_ADD(hh,r->udcons,key,keylen,rn);

			checked_realloc(r->n2p[j].p, pattern_key_t*,
					(r->n2p[j].size+1),sizeof(pattern_key_t *),return ;);
			checked_malloc(r->n2p[j].p[r->n2p[j].size], 
					pattern_key_t, 1, sizeof(pattern_key_t)+sizeof(size_t),return ;);

			r->n2p[j].size = r->n2p[j].size + 1;
			r->n2p[j].p[r->n2p[j].size-1]->type = 3;
			r->n2p[j].p[r->n2p[j].size-1]->segments[0] = j;

		}

	}


}


/* y1 in ? , y2 in ?? : PI[1]*/
void pattern_instance_pattern_2_1( ucons_internal_t *pr,ucons_t *r){
	pattern_key_t * key;
	size_t i,j,k,size_k,size_j;

	unsigned keylen;
	size_t u_seg;
	pattern_t *rn, *rt;

	u_seg = pr->PI[1].u_seg;
	keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

	for(j=1;j<r->segmentdim;j++)
		for(k=j+1;k<r->segmentdim;k++)
		{

			checked_malloc(rn,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return ;);
			memset(rn, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

			rn->dcons=NULL;
			rn->key.type=1;
			rn->key.segments[0]=j;
			rn->key.segments[1]=k;

			HASH_FIND(hh,r->udcons,&rn->key, keylen,rt);

			if(rt==NULL){

				HASH_ADD(hh,r->udcons,key,keylen,rn);

				checked_realloc(r->n2p[j].p, pattern_key_t*,
							(r->n2p[j].size+1),sizeof(pattern_key_t*),return ;);
				checked_realloc(r->n2p[k].p, pattern_key_t*,
							(r->n2p[k].size+1),sizeof(pattern_key_t*),return ;);


				size_k=r->n2p[k].size;
				size_j=r->n2p[j].size;

				checked_malloc(r->n2p[j].p[size_j],
						pattern_key_t,1, sizeof(pattern_key_t)+(2*sizeof(size_t)), return;);
				//r->n2p[j].p[size_j]=(pattern_key_t *)malloc(((sizeof(pattern_key_t)+2*sizeof(size_t))));
				checked_malloc(r->n2p[k].p[size_k],
						pattern_key_t,1,sizeof(pattern_key_t)+2*sizeof(size_t),return;);
				//r->n2p[k].p[size_k]=(pattern_key_t *)malloc(((sizeof(pattern_key_t)+2*sizeof(size_t))));

				r->n2p[j].size++;
				r->n2p[k].size++;

				r->n2p[j].p[r->n2p[j].size-1]->type = 1;
				r->n2p[j].p[r->n2p[j].size-1]->segments[0]=j;
				r->n2p[j].p[r->n2p[j].size-1]->segments[1]=k;

				r->n2p[k].p[r->n2p[k].size-1]->type = 1;
				r->n2p[k].p[r->n2p[k].size-1]->segments[1]=k;
				r->n2p[k].p[r->n2p[k].size-1]->segments[0]=j;

			}

		}


}

/* y1 in ? , y2 in ?? , y3 in ??? : PI[2] */
void pattern_instance_pattern_3_1( ucons_internal_t *pr,ucons_t *r){
	pattern_key_t * key;
	size_t i,j,k,l,size_k,size_j,size_l;

	unsigned keylen;
	size_t u_seg;
	pattern_t *rn, *rt;

	u_seg = pr->PI[2].u_seg;
	keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

	for(j=1;j<r->segmentdim;j++)
		for(k=j+1;k<r->segmentdim;k++)
			for(l=k+1;l<r->segmentdim;l++)
			{

				checked_malloc(rn,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return ;);
				memset(rn, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

				rn->dcons=NULL;//p_abstract0_bottom(pr->man_dcons, r->datadim, r->segmentdim+3 );

				rn->key.type=1;

				rn->key.segments[0]=j;
				rn->key.segments[1]=k;
				rn->key.segments[2]=l;

				HASH_FIND(hh,r->udcons,&rn->key, keylen,rt);

				if(rt==NULL){

					HASH_ADD(hh,r->udcons,key,keylen,rn);

					checked_realloc(r->n2p[j].p, pattern_key_t*,(r->n2p[j].size+1),sizeof(pattern_key_t*),return ;);
					checked_realloc(r->n2p[k].p, pattern_key_t*,(r->n2p[k].size+1),sizeof(pattern_key_t*),return ;);
					checked_realloc(r->n2p[l].p, pattern_key_t*,(r->n2p[l].size+1),sizeof(pattern_key_t*),return ;);

					size_k=r->n2p[k].size;
					size_j=r->n2p[j].size;
					size_l=r->n2p[l].size;

					checked_malloc(r->n2p[j].p[size_j],pattern_key_t,1, sizeof(pattern_key_t)+3*sizeof(size_t),return;);
					//r->n2p[j].p[size_j]=(pattern_key_t *)malloc(((sizeof(pattern_key_t)+2*sizeof(size_t))));
					checked_malloc(r->n2p[k].p[size_k],pattern_key_t,1,sizeof(pattern_key_t)+3*sizeof(size_t),return;);
					//r->n2p[k].p[size_k]=(pattern_key_t *)malloc(((sizeof(pattern_key_t)+2*sizeof(size_t))));
					checked_malloc(r->n2p[l].p[size_l],pattern_key_t,1,sizeof(pattern_key_t)+3*sizeof(size_t),return;);

					r->n2p[j].size++;
					r->n2p[k].size++;
					r->n2p[l].size++;

					r->n2p[j].p[r->n2p[j].size-1]->type = 2;

					r->n2p[j].p[r->n2p[j].size-1]->segments[0]=j;
					r->n2p[j].p[r->n2p[j].size-1]->segments[1]=k;
					r->n2p[j].p[r->n2p[j].size-1]->segments[2]=l;

					r->n2p[k].p[r->n2p[k].size-1]->type = 2;
					r->n2p[k].p[r->n2p[k].size-1]->segments[0]=k;
					r->n2p[k].p[r->n2p[k].size-1]->segments[1]=j;
					r->n2p[k].p[r->n2p[k].size-1]->segments[2]=l;

					r->n2p[l].p[r->n2p[l].size-1]->type = 2;
					r->n2p[l].p[r->n2p[l].size-1]->segments[0]=k;
					r->n2p[l].p[r->n2p[l].size-1]->segments[1]=j;
					r->n2p[l].p[r->n2p[l].size-1]->segments[2]=l;

				}

			}


}



inline ucons_t *
ucons_alloc_internal (ucons_internal_t * pr, size_t intdim, size_t realdim)
{

	ucons_t *r;
	size_t i;

	checked_malloc (r, ucons_t, sizeof (ucons_t), 1, return NULL;);


	r->datadim = intdim;
	r->segmentdim = realdim;

	r->econs=NULL;

	checked_malloc (r->n2p, pattern_key_set_t,realdim, sizeof(pattern_key_set_t), return NULL;);

	r->n2p->size = r->segmentdim;
	for(i=0;i<realdim;i++){
		r->n2p[i].size=0;
		r->n2p[i].p=NULL;
	}

	/* initialization to null of the hash table*/
	r->udcons = NULL;

	/* instantiation of patterns */

#if defined (UCONS_DCONS_OCT_P11) || defined (UCONS_DCONS_POLY_P11)
	pattern_instance_pattern_1(pr,r);
#elif defined (UCONS_DCONS_OCT_P12) || defined (UCONS_DCONS_POLY_P12)
	pattern_instance_pattern_1(pr,r);
	pattern_instance_pattern_1_2(pr,r);
#elif defined (UCONS_DCONS_OCT_P21) || defined (UCONS_DCONS_POLY_P21)
	pattern_instance_pattern_2_1(pr,r);
	//	r = add_pattern_1_3( pr, r,a->segmentdim);
#endif

	//pattern_instance_pattern_1_2(pr,r);
	//pattern_instance_pattern_1_3(pr,r);

	return r;
}

/* returns a top ucons */
inline ucons_t *
ucons_alloc_top (ucons_internal_t * pr, size_t intdim, size_t dim)
{
	ucons_t *r = ucons_alloc_internal (pr, intdim,dim);
	size_t i,nr_y,u_seg;

	r->econs = ap_abstract0_top(pr->man_dcons, r->datadim + 2*r->segmentdim,0);
	pattern_t *p;
	for(p=r->udcons;p!=NULL;p=p->hh.next){

		u_seg = pr->PI[p->key.type].u_seg;
		nr_y=0;
		for(i=0;i<u_seg;i++){
			nr_y+=pr->PI[p->key.type].uvar[i].size;
		}

		p->dcons=ap_abstract0_top(pr->man_dcons, r->datadim +2* r->segmentdim + 2*nr_y,0);
	}
#ifndef NDEBUG1
	printf("\n \t alloc top returns \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif
	return r;
}



inline void
ucons_free_internal (ucons_internal_t * pr, ucons_t * a)
{
	pattern_t *s;
	size_t i,j;

	if (a)
	{

		if (a->econs)
		{
			ap_abstract0_free (pr->man_dcons, a->econs);
			a->econs = NULL;
		}

		while(a->udcons) {
			s = a->udcons;
			if(s->dcons){
				ap_abstract0_free (pr->man_dcons, s->dcons);
				s->dcons = NULL;
			}
			HASH_DEL(a->udcons,s);
			free(s);
		}
		if(a->n2p){
			for(i=0;i<a->segmentdim;i++)
				for(j=0;j<a->n2p[i].size;j++)
					if(a->n2p[i].p[j]) {
						free(a->n2p[i].p[j]);
						a->n2p[i].p[j]=NULL;
					}

			free(a->n2p);
		}
		free (a);
	}
	return;
}

inline ucons_t *
ucons_copy_internal (ucons_internal_t * pr, ucons_t * a)
{
	arg_assert (a, return NULL;);
	ucons_t *r = ucons_alloc_internal (pr,a->datadim , a->segmentdim);

	pattern_t* s;
	pattern_t* ra, *rt;
	size_t u_seg;
	size_t i;
	unsigned keylen;

	r->econs = ap_abstract0_copy (pr->man_dcons, a->econs);

	for(s=a->udcons;s!=NULL;s=s->hh.next)
	{
		u_seg=pr->PI[s->key.type].u_seg;

		checked_malloc(ra,pattern_t,1,(sizeof(pattern_t)+u_seg*sizeof(size_t)),return NULL;);
		memset(ra, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

		ra->dcons = NULL;
		ra->key.type=s->key.type;

		for(i=0;i<u_seg;i++)
			ra->key.segments[i]=s->key.segments[i];

		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

		HASH_FIND(hh,r->udcons,&ra->key,keylen,rt);
		if(rt)
			//HASH_DEL(r->udcons,rt);
			rt->dcons=(s->dcons) ?
					ap_abstract0_copy (pr->man_dcons, s->dcons) : NULL;

		else {
			ra->dcons =(s->dcons) ?
					ap_abstract0_copy (pr->man_dcons, s->dcons) : NULL;
			HASH_ADD(hh,r->udcons,key,keylen,ra);
		}
	}
	r->n2p = pattern_key_set_copy(pr,a->n2p,a->segmentdim);

	return r;
}


/* ============================================================ */
/* Memory */
/* ============================================================ */
pattern_key_set_t * pattern_key_set_copy(ucons_internal_t * pr,pattern_key_set_t * a,size_t size){

	pattern_key_set_t *r;
	size_t u_seg,i,j,k;



	checked_malloc (r, pattern_key_set_t,size, sizeof(pattern_key_set_t), return NULL;);

	r->size = size;
	for(i=0;i<size;i++){
		r[i].size=a[i].size;
		r[i].p=NULL;
		//copy a[i].p into r[i].p
		checked_malloc(r[i].p, pattern_key_t*,(r[i].size),sizeof(pattern_key_t*),return NULL ;);
		for(j=0;j<r[i].size;j++){
			u_seg = pr->PI[a[i].p[j]->type].u_seg;
			checked_malloc(r[i].p[j], pattern_key_t,1,sizeof(pattern_key_t)+u_seg*sizeof(size_t),return NULL ;);

			r[i].p[j]->type = a[i].p[j]->type;
			for(k=0;k<u_seg;k++)
				r[i].p[j]->segments[k]=a[i].p[j]->segments[k];

		}

	}
	return r;
}


ucons_t *
ucons_copy (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_COPY, 0);
	return (a) ? ucons_copy_internal (pr, a) : NULL;
}

void
ucons_free (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_FREE, 0);
	ucons_free_internal (pr, a);
}

size_t
ucons_size (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_ASIZE, 0);
	return a->size;
}


/* ============================================================ */
/* Control of internal representation */
/* ============================================================ */

/* TODO: priority 3 */
/* Return the set of minimal elements */
void
ucons_minimize (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_MINIMIZE, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
}

/* TODO: priority 3 */
/* Return the set of canonical elements */
void
ucons_canonicalize (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_CANONICALIZE, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
}

/* TODO: priority 0 */
int
ucons_hash (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_HASH, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return 0;
}

/* NOT IMPLEMENTED: do nothing */
void
ucons_approximate (ap_manager_t * man, ucons_t * a, int algorithm)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_APPROXIMATE, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
}

/* TODO: priority 3 */
bool
ucons_is_minimal (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_CANONICALIZE, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return true;
}

/* TODO: priority 3 */
bool
ucons_is_canonical (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_CANONICALIZE, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return true;
}

/* ============================================================ */
/* Basic Constructors */
/* ============================================================ */

ucons_t *
ucons_bottom (ap_manager_t * man, size_t intdim, size_t realdim)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_BOTTOM, 0);

	return NULL;
}

ucons_t *
ucons_top (ap_manager_t * man, size_t intdim, size_t realdim)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_TOP, 0);
	ucons_t *r = ucons_alloc_top (pr, intdim,  realdim);
	return r;
}

/* TODO: priority 3 */
/* put constraints on data variables */
ucons_t *
ucons_of_box (ap_manager_t * man, size_t intdim, size_t realdim,
		ap_interval_t ** t)
		{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_OF_BOX, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return NULL;
		}

/* NOT IMPLEMENTED */
ucons_t *
ucons_of_generator_array (ap_manager_t * man, size_t intdim, size_t realdim,
		ap_generator0_array_t * ar)
		{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_ADD_RAY_ARRAY, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return NULL;
		}


/* ============================================================ */
/* Accessors */
/* ============================================================ */

ap_dimension_t
ucons_dimension (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_DIMENSION, 0);
	ap_dimension_t r;
	// if (!a->a || (a->a && !a->a[0]))
	if (!a)
	{
		r.realdim = 0;
		r.intdim = 0;
	}
	else
	{
		r.realdim = a->segmentdim ;
		r.intdim = a->datadim;
	}
	return r;
}

/* ********************************************************************** */
/* VI. ucons_array_t */
/* ********************************************************************** */


/* ============================================================ */
/* Managers */
/* ============================================================ */

void
ucons_internal_free (ucons_internal_t * pr)
{
	/* TODO: free PI */
	pr->PI = NULL;
	/* TODO: free data manager */
	pr->man_dcons = NULL;
	free (pr);
}

ap_manager_t *
ucons_manager_alloc (void)
{
	size_t i;
	ap_manager_t *man;
	ucons_internal_t *pr;
	char domain[128];


	pr = (ucons_internal_t *) malloc (sizeof (ucons_internal_t));
	assert (pr);

	switch (UCONS_DCONS_DOMAIN)
	{
	case DOM_BOX:
		pr->man_dcons = box_manager_alloc ();
		break;
	case DOM_OCT_P11:
		pr->man_dcons = oct_manager_alloc ();
		break;
	case DOM_OCT_P12:
		pr->man_dcons = oct_manager_alloc ();
		break;
	case DOM_OCT_P21:
		pr->man_dcons = oct_manager_alloc ();
		break;
	case DOM_POLY_P11:
		pr->man_dcons = pk_manager_alloc (false);
		break;
	case DOM_POLY_P12:
		pr->man_dcons = pk_manager_alloc (false);
		break;
	case DOM_POLY_P21:
		pr->man_dcons = pk_manager_alloc (false);
		break;
	default:
		pr->man_dcons = pk_manager_alloc (false);
		break;
	}

	i = snprintf (domain, 127, "0.1 with (dcons=%s)",
			((UCONS_DCONS_DOMAIN ==
					DOM_BOX) ? "Box" : ((UCONS_DCONS_DOMAIN ==
							DOM_OCT) ? "Oct" : "Polka")) );
	domain[i] = '\0';

	man = ap_manager_alloc ("ucons", domain, pr,
			(void (*)(void *)) ucons_internal_free);

	pr->man = man;

	man->funptr[AP_FUNID_COPY] = &ucons_copy;
	man->funptr[AP_FUNID_FREE] = &ucons_free;
	man->funptr[AP_FUNID_ASIZE] = &ucons_size;
	man->funptr[AP_FUNID_MINIMIZE] = &ucons_minimize;
	man->funptr[AP_FUNID_CANONICALIZE] = &ucons_canonicalize;
	man->funptr[AP_FUNID_HASH] = &ucons_hash;
	man->funptr[AP_FUNID_APPROXIMATE] = &ucons_approximate;
	man->funptr[AP_FUNID_FPRINT] = &ucons_fprint;
	man->funptr[AP_FUNID_FPRINTDIFF] = &ucons_fprintdiff;
	man->funptr[AP_FUNID_FDUMP] = &ucons_fdump;
	man->funptr[AP_FUNID_SERIALIZE_RAW] = &ucons_serialize_raw;
	man->funptr[AP_FUNID_DESERIALIZE_RAW] = &ucons_deserialize_raw;
	man->funptr[AP_FUNID_BOTTOM] = &ucons_bottom;
	man->funptr[AP_FUNID_TOP] = &ucons_top;
	man->funptr[AP_FUNID_OF_BOX] = &ucons_of_box;
	man->funptr[AP_FUNID_DIMENSION] = &ucons_dimension;
	man->funptr[AP_FUNID_IS_BOTTOM] = &ucons_is_bottom;
	man->funptr[AP_FUNID_IS_TOP] = &ucons_is_top;
	man->funptr[AP_FUNID_IS_LEQ] = &ucons_is_leq;
	man->funptr[AP_FUNID_IS_EQ] = &ucons_is_eq;
	man->funptr[AP_FUNID_IS_DIMENSION_UNCONSTRAINED] =
			&ucons_is_dimension_unconstrained;
	man->funptr[AP_FUNID_SAT_INTERVAL] = &ucons_sat_interval;
	man->funptr[AP_FUNID_SAT_LINCONS] = &ucons_sat_lincons;
	man->funptr[AP_FUNID_SAT_TCONS] = &ucons_sat_tcons;
	man->funptr[AP_FUNID_BOUND_DIMENSION] = &ucons_bound_dimension;
	man->funptr[AP_FUNID_BOUND_LINEXPR] = &ucons_bound_linexpr;
	man->funptr[AP_FUNID_BOUND_TEXPR] = &ucons_bound_texpr;
	man->funptr[AP_FUNID_TO_BOX] = &ucons_to_box;
	man->funptr[AP_FUNID_TO_LINCONS_ARRAY] = &ucons_to_lincons_array;
	man->funptr[AP_FUNID_TO_TCONS_ARRAY] = &ucons_to_tcons_array;
	man->funptr[AP_FUNID_TO_GENERATOR_ARRAY] = &ucons_to_generator_array;
	man->funptr[AP_FUNID_MEET] = &ucons_meet;
	man->funptr[AP_FUNID_MEET_ARRAY] = &ucons_meet_array;
	man->funptr[AP_FUNID_MEET_LINCONS_ARRAY] = &ucons_meet_lincons_array;
	man->funptr[AP_FUNID_MEET_TCONS_ARRAY] = &ucons_meet_tcons_array;
	man->funptr[AP_FUNID_JOIN] = &ucons_join;
	man->funptr[AP_FUNID_JOIN_ARRAY] = &ucons_join_array;
	man->funptr[AP_FUNID_ADD_RAY_ARRAY] = &ucons_add_ray_array;
	man->funptr[AP_FUNID_ASSIGN_LINEXPR_ARRAY] = &ucons_assign_linexpr_array;
	man->funptr[AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY] =
			&ucons_substitute_linexpr_array;
	man->funptr[AP_FUNID_ASSIGN_TEXPR_ARRAY] = &ucons_assign_texpr_array;
	man->funptr[AP_FUNID_SUBSTITUTE_TEXPR_ARRAY] =
			&ucons_substitute_texpr_array;
	man->funptr[AP_FUNID_ADD_DIMENSIONS] = &ucons_add_dimensions;
	man->funptr[AP_FUNID_REMOVE_DIMENSIONS] = &ucons_remove_dimensions;
	man->funptr[AP_FUNID_PERMUTE_DIMENSIONS] = &ucons_permute_dimensions;
	man->funptr[AP_FUNID_FORGET_ARRAY] = &ucons_forget_array;
	man->funptr[AP_FUNID_EXPAND] = &ucons_expand;
	man->funptr[AP_FUNID_FOLD] = &ucons_fold;
	man->funptr[AP_FUNID_WIDENING] = &ucons_widening;
	man->funptr[AP_FUNID_CLOSURE] = &ucons_closure;

	for (i = 0; i < AP_EXC_SIZE; i++)
		ap_manager_set_abort_if_exception (man, i, false);

	initialize_PI(pr);

	return man;
}

ucons_t *
ucons_of_abstract0 (ap_abstract0_t * a)
{
	return (ucons_t *) a->value;
}

ap_abstract0_t *
abstract0_of_ucons (ap_manager_t * man, ucons_t * a)
{
	ap_abstract0_t *r = malloc (sizeof (ap_abstract0_t));
	assert (r);
	r->value = a;
	r->man = ap_manager_copy (man);
	return r;
}
