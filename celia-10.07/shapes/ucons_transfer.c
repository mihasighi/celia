/*
 * ucons_transfer.c
 *
 * Assignment, substitution, guard transfer functions.
 *
 * APRON Library / Shapes Domain
 *
 * Copyright (C) LIAFA 2009
 *
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include <assert.h>
#include "ucons.h"
#include "ucons_internal.h"
#include "apron2shape.h"
#include "ap_generic.h"
#include "ap_linexpr0.h"


/* ============================================================ */
/* Meet constraints and Join generators */
/* ============================================================ */



ucons_t *
ucons_meet_lincons (ucons_internal_t * pr, bool destructive,
		ucons_t * a, ap_lincons0_t * lcons)
		{
		if (!a)
			return NULL;
	ucons_t *r = ucons_alloc_internal (pr, a->datadim, a->segmentdim);

	ap_linexpr0_t *lexpr;
	size_t i, dim, kind;
	ap_coeff_t *coeff = NULL;
	kind = var_ptr_null;		/* unknown */
	if (lcons->scalar)
	{
		if (!ap_scalar_cmp_int (lcons->scalar, var_ptr_data))
			kind = var_ptr_data;	/* data */
		else if (!ap_scalar_cmp_int (lcons->scalar, var_ptr_length))
			kind = var_ptr_length;	/* len */
		else
			assert (0);
	}
	lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, a->datadim + 2 * a->segmentdim);
	ap_linexpr0_ForeachLinterm (lcons->linexpr0, i, dim, coeff)
	{
		if (coeff && !ap_coeff_zero (coeff))
		{
			if (dim < a->datadim)
				ap_coeff_set (&lexpr->p.coeff[dim], coeff);
			else			// dim >= a->datadim
			{
				if (kind == var_ptr_data || kind == var_ptr_null)
					ap_coeff_set (&lexpr->p.coeff[dim], coeff);
				else
					if (kind == var_ptr_length)
						ap_coeff_set (&lexpr->p.coeff[dim +  a->segmentdim], coeff);
					else
						assert (0);
			}
		}
	}
	ap_linexpr0_get_cst (coeff, lcons->linexpr0);
	ap_linexpr0_set_cst (lexpr, coeff);

	ap_lincons0_array_t arr = ap_lincons0_array_make (1);
	arr.p[0] = ap_lincons0_make (lcons->constyp, lexpr, NULL);

	r->econs =
			ap_abstract0_meet_lincons_array (pr->man_dcons, false, a->econs, &arr);

	size_t u_seg;
	pattern_t * s, *ra,*rt;
	unsigned keylen;
	for(s=a->udcons;s!=NULL;s=s->hh.next){
		u_seg=pr->PI[s->key.type].u_seg;

		checked_malloc(ra,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return NULL;);
		memset(ra, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

//		ra->dcons =(s->dcons) ?
//				ap_abstract0_copy (pr->man_dcons, s->dcons) : NULL;
		ra->dcons = NULL;
		ra->key.type=s->key.type;

		for(i=0;i<u_seg;i++)
			ra->key.segments[i]=s->key.segments[i];

		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

		HASH_FIND(hh,r->udcons,&ra->key,keylen,rt);
		if(rt){
			rt->dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, false, s->dcons, &arr);
			free(ra);
		}

		else {
			ra->dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, false, s->dcons, &arr);
			HASH_ADD(hh,r->udcons,key,keylen,ra);
		}
	}

/*
 * TODO meet lincons array on every r->udcons
 * */
	if (destructive)
		ucons_free_internal (pr, a);
	return r;

		}

/* TODO: priority 1 */
/* TODO: detect when is needed */
ucons_t *
ucons_meet_lincons_array (ap_manager_t * man,
                          bool destructive, ucons_t * a,
                          ap_lincons0_array_t * array)
{
  ucons_internal_t *pr =
     ucons_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
   if (!a)
     return NULL;
   assert (array && array->size > 0 && array->p);
   size_t i;

   ucons_t *r = ucons_meet_lincons (pr, false, a, &array->p[0]);


   for (i = 1; i < array->size; i++)
     r = ucons_meet_lincons (pr, true, r, &array->p[i]);
   if (destructive)
     ucons_free_internal (pr, a);
   return r;

}

/* TODO: priority 1 */
/* TODO: detect when is needed */
ucons_t *
ucons_meet_tcons_array (ap_manager_t * man,
                        bool destructive, ucons_t * a,
                        ap_tcons0_array_t * array)
{
  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* NOT IMPLEMENTED */
ucons_t *
ucons_add_ray_array (ap_manager_t * man,
                     bool destructive, ucons_t * a,
                     ap_generator0_array_t * array)
{
  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_ADD_RAY_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}


/* ============================================================ */
/* Assignement and Substitutions */
/* ============================================================ */

/* TODO: priority 3 */
/* Only done if real dimension (ptr vars) are used in constraints */

ucons_t *
ucons_assign_linexpr (ap_manager_t * man,
		bool destructive, ucons_t * a,
		ap_dim_t d, ap_linexpr0_t * expr, ucons_t * dest)
		{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
	if (!a)
		return NULL;
	assert (expr && ap_linexpr0_size (expr) == (a->datadim + a->segmentdim) &&
			d < (a->datadim + a->segmentdim) && !dest);
	ucons_t *r = ucons_alloc_internal (pr, a->datadim, a->segmentdim);

	ap_linexpr0_realloc (expr, a->datadim + 2 * a->segmentdim);

#ifndef NDEBUG1
	fprintf (stdout, "to dcons %d:=", d);
	ap_linexpr0_fprint (stdout, expr, NULL);
	fprintf (stdout, "\n");

	fprintf (stdout, "to ucons a");
	ucons_fprint (stdout,pr->man,a, NULL);
	fprintf (stdout, "\n");
#endif


	//pattern_t *s,*aux;
	ap_abstract0_t *aux_dcons;

	size_t i,u_seg;
	unsigned keylen;


	pattern_t * s, *ra,*rt;

	for(s=a->udcons;s!=NULL;s=s->hh.next){
		/*
		 * TODO add dimensions to r->econs to cope with s->dcons; meet between r->econs and s->dcons ;
		 * */
		u_seg=pr->PI[s->key.type].u_seg;

		checked_malloc(ra,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return NULL;);
		memset(ra, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));


		ra->dcons = NULL;
		ra->key.type=s->key.type;

		for(i=0;i<u_seg;i++)
			ra->key.segments[i]=s->key.segments[i];

		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

		HASH_FIND(hh,r->udcons,&ra->key,keylen,rt);
		if(rt){
			rt->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, false, s->dcons, d, expr, NULL);
			free(ra);
		}

		else {
			ra->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, false, s->dcons, d, expr, NULL);
			HASH_ADD(hh,r->udcons,key,keylen,ra);
		}

	}

	r->econs = ap_abstract0_assign_linexpr (pr->man_dcons, false, a->econs, d, expr, NULL);

#ifndef NDEBUG1
	fprintf (stdout, "returns ");
	ucons_fprint (stdout, pr->man, r, NULL);
	fprintf (stdout, "\n");
#endif
	if (destructive)
		ucons_free_internal (pr, a);
	return r;
		}

/* TODO: priority 0 */
/* used for pre-image computation */
ucons_t *
ucons_substitute_linexpr (ap_manager_t * man,
                          bool destructive, ucons_t * a,
                          ap_dim_t d, ap_linexpr0_t * expr, ucons_t * dest)
{
  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* TODO: priority 3 */
/* Only done if real dimension (ptr vars) are used in constraints */
ucons_t *
ucons_assign_linexpr_array (ap_manager_t * man,
                            bool destructive, ucons_t * a,
                            ap_dim_t * tdim,
                            ap_linexpr0_t ** texpr,
                            size_t size, ucons_t * dest)
{
  if (size == 1)
    return ucons_assign_linexpr (man, destructive, a, tdim[0], texpr[0],
                                 dest);

  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* TODO: priority 0 */
/* used for pre-image computation */
ucons_t *
ucons_substitute_linexpr_array (ap_manager_t * man,
                                bool destructive, ucons_t * a,
                                ap_dim_t * tdim,
                                ap_linexpr0_t ** texpr,
                                size_t size, ucons_t * dest)
{
  if (size == 1)
    return ucons_substitute_linexpr (man, destructive, a, tdim[0], texpr[0],
                                     dest);

  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_SUBSTITUTE_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* TODO: priority 3 */
/* Only for constraints between realdim */
ucons_t *
ucons_assign_texpr_array (ap_manager_t * man,
                          bool destructive, ucons_t * a,
                          ap_dim_t * tdim,
                          ap_texpr0_t ** texpr, size_t size, ucons_t * dest)
{
  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_ASSIGN_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* TODO: priority 0 */
/* used only for pre-image computation */
ucons_t *
ucons_substitute_texpr_array (ap_manager_t * man,
                              bool destructive, ucons_t * a,
                              ap_dim_t * tdim,
                              ap_texpr0_t ** texpr,
                              size_t size, ucons_t * dest)
{
  return ap_generic_substitute_texpr_array (man, destructive, a, tdim, texpr,
                                            size, dest);
}
