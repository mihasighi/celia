/*
 * ucons_predicate.c
 *
 * Predicates on uconss, extraction functions
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include "ucons.h"
#include "ucons_internal.h"
#include "ucons_fun.h"
#include "ap_generic.h"

#include <stdio.h>



/* ============================================================ */
/* Tests */
/* ============================================================ */

/* ********************************************************************** */
/* I. ucons_pattern_key_set_t */
/* ********************************************************************** */

/* ********************************************************************** */
/* III. pattern_t */
/* ********************************************************************** */

/* ********************************************************************** */
/* IV. ucons_pattern_t */
/* ********************************************************************** */

/* ********************************************************************** */
/* V. ucons_array_t */
/* ********************************************************************** */

/* The bottom value is the empty set. */
bool
ucons_is_bottom (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_IS_BOTTOM, 0);
	return (!a || ap_abstract0_is_bottom (pr->man_dcons, a->econs));
}

/* The top value is top uucons in the first position. */
bool
ucons_is_top (ap_manager_t * man, ucons_t * a)
{
	size_t i;
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_IS_TOP, 0);

	pattern_t *s;
	for(s=a->udcons;s!=NULL;s=s->hh.next)
		if((s->dcons==NULL) || (ap_abstract0_is_top (pr->man_dcons, s->dcons)==false))
			return false;

	return (a && ap_abstract0_is_top (pr->man_dcons, a->econs));
}

/* TODO: priority 3 */
bool
ucons_is_leq (ap_manager_t * man, ucons_t * a1, ucons_t * a2)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_IS_LEQ, 0);

	if (ucons_is_bottom (man, a1))
		return true;
	if (ucons_is_bottom (man, a2))
		return false;
	if (a1->datadim != a2->datadim || a1->segmentdim != a2->segmentdim)
		return false;
	bool is_leq = ap_abstract0_is_leq (pr->man_dcons, a1->econs, a2->econs);

	if (!is_leq )
		return false;
	pattern_t * s1, *s2;
	size_t u_seg;
	unsigned keylen;

	for(s1 = a1->udcons; s1!= NULL; s1=s1->hh.next){
		u_seg = pr->PI[s1->key.type].u_seg;
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);
		HASH_FIND(hh,a2->udcons,&s1->key.type,keylen,s2);
		if(s2){
			if (!ap_abstract0_is_leq (pr->man_dcons, s1->dcons, s2->dcons))
				return false;
		}
	}


	return true;

	//	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
	//			"not implemented");
}

/* TODO: priority 3 */
bool
ucons_is_eq (ap_manager_t * man, ucons_t * a1, ucons_t * a2)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_IS_EQ, 0);

	if ((ucons_is_bottom (man, a1) && ucons_is_bottom (man, a2)) ||
			(ucons_is_top (man, a1) && ucons_is_top (man, a2)))
		return true;
	if (a1->datadim != a2->datadim || a1->segmentdim != a2->segmentdim)
		return false;
	if(! ap_abstract0_is_eq (pr->man_dcons, a1->econs, a2->econs))
		return false;


	pattern_t * s1, *s2;
	size_t u_seg;
	unsigned keylen;

	for(s1 = a1->udcons; s1!= NULL; s1=s1->hh.next){
		u_seg = pr->PI[s1->key.type].u_seg;
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);
		HASH_FIND(hh,a2->udcons,&s1->key.type,keylen,s2);
		if(s2){
			if (!ap_abstract0_is_eq (pr->man_dcons, s1->dcons, s2->dcons))
				return false;
		}
	}

	return true;
//	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
//			"not implemented");
}

/* TODO: priority 1 */
/*
 * Possibly needed for checking linear constraints representing - aliasing
 * between pointer variables, e.g., x = y, - or constraints between the
 * program variables (lengths and data) in the assert statements.
 */
bool
ucons_sat_lincons (ap_manager_t * man, ucons_t * a, ap_lincons0_t * lincons)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_SAT_LINCONS, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return true;
}

/* TODO: priority 1 */
bool
ucons_sat_tcons (ap_manager_t * man, ucons_t * a, ap_tcons0_t * cons)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_SAT_TCONS, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return true;
}

/* TODO: priority 1 */
/* Interval constraints are only between non-pointer variables */
bool
ucons_sat_interval (ap_manager_t * man, ucons_t * a,
		ap_dim_t dim, ap_interval_t * i)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_SAT_INTERVAL, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return true;
}

/* TODO: priority 1 */
bool
ucons_is_dimension_unconstrained (ap_manager_t * man, ucons_t * a,
		ap_dim_t dim)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_IS_DIMENSION_UNCONSTRAINED, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return false;
}

/* ============================================================ */
/* Extraction of properties */
/* ============================================================ */

/* NOT IMPLEMENTED */
ap_interval_t *
ucons_bound_linexpr (ap_manager_t * man, ucons_t * a, ap_linexpr0_t * expr)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_BOUND_LINEXPR, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return NULL;
}

/* NOT IMPLEMENTED */
ap_interval_t *
ucons_bound_texpr (ap_manager_t * man, ucons_t * a, ap_texpr0_t * expr)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_BOUND_TEXPR, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return NULL;
}

/* NOT IMPLEMENTED */
ap_interval_t *
ucons_bound_dimension (ap_manager_t * man, ucons_t * a, ap_dim_t dim)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_BOUND_DIMENSION, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return NULL;
}

/* NOT IMPLEMENTED */
ap_lincons0_array_t
ucons_to_lincons_array (ap_manager_t * man, ucons_t * a)
{
	ap_lincons0_array_t ar;
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_TO_LINCONS_ARRAY, 0);
	ar = ap_lincons0_array_make (1);
	ar.p[0] = ap_lincons0_make_unsat ();
	return ar;
}

/* NOT IMPLEMENTED */
ap_tcons0_array_t
ucons_to_tcons_array (ap_manager_t * man, ucons_t * a)
{
	return ap_generic_to_tcons_array (man, a);
}

/* NOT IMPLEMENTED */
ap_interval_t **
ucons_to_box (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_TO_BOX, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return NULL;
}

/* NOT IMPLEMENTED */
ap_generator0_array_t
ucons_to_generator_array (ap_manager_t * man, ucons_t * a)
{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_TO_GENERATOR_ARRAY, 0);
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return ap_generator0_array_make (0);
}
