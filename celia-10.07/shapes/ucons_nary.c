
/*
 * ucons_nary.c
 *
 * N-ary ucons functions: meet, join, widening, narrowing & related.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 *
 */

/* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution.
 */

#include "ucons.h"
#include "ucons_internal.h"
#include "ucons_fun.h"
#include "apron2shape.h"
#include "shape_macros.h"
#include "ap_generic.h"

/* ============================================================ */
/* Meet and Join */
/* ============================================================ */

/* TODO: priority 3 */
ucons_t *
ucons_meet (ap_manager_t * man, bool destructive, ucons_t * a1, ucons_t * a2)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_MEET, 0);

	if (ucons_is_bottom (man, a1) && ucons_is_bottom (man, a2))
		return (destructive) ? a1 : ucons_bottom (man, a1->datadim, a1->segmentdim);
	if (ucons_is_bottom (man, a1))
		return (destructive) ? a2 : ucons_copy (man, a2);
	if (ucons_is_bottom (man, a2))
		return (destructive) ? a1 : ucons_copy (man, a1);


	arg_assert (a1->datadim == a2->datadim
			&& a1->segmentdim == a2->segmentdim, return NULL;);

#ifndef NDEBUG1
		printf("  \n\t join a1 : \n");
		ucons_fprint(stdout,pr->man,a1,NULL);
		printf("\n with a2 \n");
		ucons_fprint(stdout,pr->man,a2,NULL);
		printf("\n");
#endif

	ucons_t * r  = ucons_alloc_internal(pr,  a1->datadim,a1->segmentdim );

	r->econs = ap_abstract0_meet (pr->man_dcons, destructive, a1->econs, a2->econs);

	r->n2p  = pattern_key_set_copy (pr, a1->n2p ,a1->segmentdim);

	pattern_t *sa1;
	pattern_t *sa2;

	pattern_t *ru=NULL; //universal constraint to add to r
	size_t u_seg;

	size_t i;
	unsigned keylen;
	for(sa1=a1->udcons; sa1 != NULL; sa1=sa1->hh.next) {

		u_seg = pr->PI[sa1->key.type].u_seg; // number of universal segments
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);



		HASH_FIND(hh,a2->udcons,&sa1->key,keylen,sa2);

		HASH_FIND(hh,r->udcons,&sa1->key,keylen,ru);

		if(sa2 && ru ){

			ru->dcons=ap_abstract0_meet(pr->man_dcons,destructive,sa1->dcons,sa2->dcons);

		}
		else
		{
			ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
					"not implemented for diffrent ucons object over differnt set of patterns ");
		}

	}
	if (destructive)
	{
		ucons_free_internal (pr, a1);
		ucons_free_internal (pr, a2);
	}
#ifndef NDEBUG1
		printf("  \n\t join returns : \n");
		ucons_fprint(stdout,pr->man,r,NULL);
		printf("\n");
#endif
	return r;

}

/* TODO: priority 3 */
ucons_t *
ucons_join (ap_manager_t * man, bool destructive, ucons_t * a1, ucons_t * a2)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_JOIN, 0);

	if (ucons_is_bottom (man, a1) && ucons_is_bottom (man, a2))
		return (destructive) ? a1 : ucons_bottom (man, a1->datadim, a1->segmentdim);
	if (ucons_is_bottom (man, a1))
		return (destructive) ? a2 : ucons_copy (man, a2);
	if (ucons_is_bottom (man, a2))
		return (destructive) ? a1 : ucons_copy (man, a1);


	arg_assert (a1->datadim == a2->datadim
			&& a1->segmentdim == a2->segmentdim, return NULL;);

#ifndef NDEBUG1
	fprintf(stdout,"\n \t join a1 \n ");
	ucons_fprint(stdout,man,a1,NULL);
	fprintf(stdout,"\n \t with  a2 \n ");
	ucons_fprint(stdout,man,a2,NULL);
	fprintf(stdout,"\n");
#endif

	ucons_t * r  = ucons_alloc_internal(pr,  a1->datadim,a1->segmentdim);

	r->econs = ap_abstract0_join (pr->man_dcons, destructive, a1->econs, a2->econs);


//#ifndef NDEBUG1
//		ap_lincons0_t cons1 = cons_k_m(r->datadim,r->segmentdim,1,2);
//		if (ap_abstract0_sat_lincons(pr->man_dcons,r->econs,&cons1))
//			printf("\n \t join : k<m \n");
//		else
//			printf("\n \t join: !!!(k<m) \n");
//#endif

	r->n2p  = pattern_key_set_copy (pr, a1->n2p ,a1->segmentdim);

	pattern_t *sa1;
	pattern_t *sa2;

	pattern_t *ru=NULL; //universal constraint to add to r
	size_t u_seg;

	size_t i;
	unsigned keylen;
	for(sa1=a1->udcons; sa1 != NULL; sa1=sa1->hh.next) {

		u_seg = pr->PI[sa1->key.type].u_seg; // number of universal segments
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);



		HASH_FIND(hh,a2->udcons,&sa1->key,keylen,sa2);

		HASH_FIND(hh,r->udcons,&sa1->key,keylen,ru);

		//checked_malloc(ru,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return NULL;);
		//memset(ru, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

		ap_abstract0_t *dcons1, *dcons2;
		ap_dimchange_t dimadd;
		size_t nr_y, i;
		if(sa2 && ru ){
					ru->dcons=ap_abstract0_join(pr->man_dcons,destructive,sa1->dcons,sa2->dcons);
		}

		else
		{
			ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
					"not implemented for diffrent ucons object over differnt set of patterns ");
		}

	}
	if (destructive)
	{
		ucons_free_internal (pr, a1);
		ucons_free_internal (pr, a2);
	}

#ifndef NDEBUG1
	fprintf(stdout,"\n \t join returns  \n ");
	ucons_fprint(stdout,man,r,NULL);
	fprintf(stdout,"\n");
#endif

	return r;
}



/* TODO: priority 3 */
ucons_t *
ucons_meet_array (ap_manager_t * man, ucons_t ** tab, size_t size)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_JOIN, 0);
	arg_assert (size > 0, return NULL;
	);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return tab[0];
}

/* TODO: priority 3 */
ucons_t *
ucons_join_array (ap_manager_t * man, ucons_t ** tab, size_t size)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_JOIN_ARRAY, 0);
	arg_assert (size > 0, return NULL;);
	ucons_t *r = ucons_copy_internal (pr, tab[0]);
	size_t i;
	for (i = 1; i < size; i++)
		r = ucons_join (man, false, tab[i], r);
	return r;

}


/* ============================================================ */
/* Widening, Narrowing */
/* ============================================================ */

/* TODO: priority 3 */
ucons_t *
ucons_widening (ap_manager_t * man, ucons_t * a1, ucons_t * a2)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_WIDENING, 0);
	ucons_t * r  = ucons_alloc_internal(pr, a1->datadim, a1->segmentdim );

	arg_assert (a1->datadim == a2->datadim
			&& a1->segmentdim == a2->segmentdim, return NULL;);

	r->econs = ap_abstract0_widening (pr->man_dcons, a1->econs, a2->econs);
	r->n2p  = pattern_key_set_copy (pr, a1->n2p ,a1->segmentdim);

	pattern_t *sa1;
	pattern_t *sa2;

	pattern_t *ru=NULL; //universal constraint to add to r
	size_t u_seg;

	size_t i;
	unsigned keylen;
	for(sa1=a1->udcons; sa1 != NULL; sa1=sa1->hh.next) {

		u_seg = pr->PI[sa1->key.type].u_seg; // number of universal segments
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

		HASH_FIND(hh,a2->udcons,&sa1->key,keylen,sa2);
		HASH_FIND(hh,r->udcons,&sa1->key,keylen,ru);

		if(sa2 && ru ){
			ru->dcons=ap_abstract0_widening(pr->man_dcons,sa1->dcons,sa2->dcons);
		}
		else
		{
			ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
					"not implemented for diffrent ucons object over differnt set of patterns ");
		}

	}

	return r;
}

/* TODO: priority 3 */
ucons_t *
ucons_widening_thresholds (ap_manager_t * man,
		ucons_t * a1, ucons_t * a2,
		ap_scalar_t ** array, size_t nb)
		{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_WIDENING, nb + 1);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return a2;
		}

/* NOT IMPLEMENTED */
ucons_t *
ucons_narrowing (ap_manager_t * man, ucons_t * a1, ucons_t * a2)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_WIDENING, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return a2;
}
