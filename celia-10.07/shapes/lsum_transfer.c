/*
 * lsum_transfer.c
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
#include "lsum.h"
#include "lsum_internal.h"
#include "apron2shape.h"
#include "ap_generic.h"
#include "ap_linexpr0.h"

/* ============================================================ */
/* Meet constraints and Join generators */
/* ============================================================ */

/* The constraint has size a->datadim+a->segmdim.
 * Encoding of constraints:
 * if coeff is _data then all realdim are data of nodes
 * if coeff is _free then all realdim are (data+sums) of nodes
 * if coeff is _len  then all realdim are lengths of nodes
 */
lsum_t *
lsum_meet_lincons (lsum_internal_t * pr, bool destructive,
		   lsum_t * a, ap_lincons0_t * lcons)
{
  lsum_t *r = lsum_alloc_internal (pr, a->datadim, a->segmdim);
  ap_linexpr0_t *lexpr;
  size_t i, dim, kind;
  ap_coeff_t *coeff = NULL;
  kind = var_ptr_null;		/* unknown */
  if (lcons->scalar)
    {
      if (!ap_scalar_cmp_int (lcons->scalar, var_ptr_data))
	kind = var_ptr_data;	/* data */
      else if (!ap_scalar_cmp_int (lcons->scalar, var_ptr_free))
	kind = var_ptr_free;	/* sum */
      else if (!ap_scalar_cmp_int (lcons->scalar, var_ptr_length))
	kind = var_ptr_length;	/* len */
      else
	assert (0);
    }
#ifdef LSUM_REL
  lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, a->datadim + 3 * a->segmdim);
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
	    else if (kind == var_ptr_free)
	      {
		ap_coeff_set (&lexpr->p.coeff[dim], coeff);	/* d[n] */
		ap_coeff_set (&lexpr->p.coeff[dim + a->segmdim], coeff);	/* M(n) */
	      }
	    else if (kind == var_ptr_length)
	      ap_coeff_set (&lexpr->p.coeff[dim + 2 * a->segmdim], coeff);
	    else
	      assert (0);
	  }
      }
  }
#else
  /* build an expression on lengths xor data! */
  if (kind == var_ptr_data ||
      kind == var_ptr_null ||
      kind == var_ptr_free)
      lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE,
							 DATA_DIM (a->datadim,
								   0) +
							 2 *
							 a->
							 segmdim);
                             else
    lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE,
		       LEN_DIM (a->datadim, 0) + a->segmdim);
  ap_linexpr0_ForeachLinterm (lcons->linexpr0, i, dim, coeff)
  {
    if (coeff && !ap_coeff_zero (coeff))
      {
	if (dim < LEN_DIM (a->datadim, 0))
	  ap_coeff_set (&lexpr->p.coeff[dim], coeff);
	else if (dim < a->datadim)
	  ap_coeff_set (&lexpr->p.coeff[dim - LEN_DIM (a->datadim, 0)],
			coeff);
	else			// dim >= LEN_DIM (a->datadim, 0)
	  {
	    if (kind == var_ptr_data || kind == var_ptr_null)
	      ap_coeff_set (&lexpr->p.coeff[dim - LEN_DIM (a->datadim, 0)],
			    coeff);
	    else if (kind == var_ptr_free)
	      {
		ap_coeff_set (&lexpr->p.coeff[dim - LEN_DIM (a->datadim, 0)], coeff);	/* d[n] */
		ap_coeff_set (&lexpr->p.coeff[dim + a->segmdim - LEN_DIM (a->datadim, 0)], coeff);	/* S(n) */
	      }
	    else if (kind == var_ptr_length)
	      ap_coeff_set (&lexpr->p.coeff[dim - DATA_DIM (a->datadim, 0)],
			    coeff);
	    else
	      assert (0);
	  }
      }
  }
#endif
  ap_linexpr0_get_cst (coeff, lcons->linexpr0);
  ap_linexpr0_set_cst (lexpr, coeff);

  ap_lincons0_array_t arr = ap_lincons0_array_make (1);
  arr.p[0] = ap_lincons0_make (lcons->constyp, lexpr, NULL);
#ifdef LSUM_REL
  r->dcons =
    ap_abstract0_meet_lincons_array (pr->man_dcons, false, a->dcons, &arr);
#else
  if (kind == var_ptr_length)
    {
      r->lcons =
	ap_abstract0_meet_lincons_array (pr->man_lcons, false, a->lcons,
					 &arr);
      r->dcons = ap_abstract0_copy (pr->man_dcons, a->dcons);
    }
  else
    {
      r->dcons =
	ap_abstract0_meet_lincons_array (pr->man_dcons, false, a->dcons,
					 &arr);
      r->lcons = ap_abstract0_copy (pr->man_lcons, a->lcons);
    }
#endif
  ap_lincons0_array_clear(&arr); // free lexpr also
  if (destructive)
    lsum_free_internal (pr, a);
  return r;
}

/* The constraints in array have size a->datadim+2*a->segmdim,
 * where first a->segmdim is for data dereference,
 * and the second a->segmdim is for lengths of segments!
 */
lsum_t *
lsum_meet_lincons_array (ap_manager_t * man,
			 bool destructive, lsum_t * a,
			 ap_lincons0_array_t * array)
{
  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
  if (!a)
    return NULL;
  assert (array && array->size > 0 && array->p);
  size_t i;
  lsum_t *r = lsum_meet_lincons (pr, false, a, &array->p[0]);
  for (i = 1; i < array->size; i++)
    r = lsum_meet_lincons (pr, true, r, &array->p[i]);
  if (destructive)
    lsum_free_internal (pr, a);
  return r;
}

/* TODO: priority 0 */
/* not used */
lsum_t *
lsum_meet_tcons_array (ap_manager_t * man,
		       bool destructive, lsum_t * a,
		       ap_tcons0_array_t * array)
{
  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* NOT IMPLEMENTED */
lsum_t *
lsum_add_ray_array (ap_manager_t * man,
		    bool destructive, lsum_t * a,
		    ap_generator0_array_t * array)
{
  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_ADD_RAY_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}


/* ============================================================ */
/* Assignement and Substitutions */
/* ============================================================ */

/* The expression concerns only data or data dereference of
 * pointer variables. Its size is a->datadim+a->segmdim! */
lsum_t *
lsum_assign_linexpr (ap_manager_t * man,
		     bool destructive, lsum_t * a,
		     ap_dim_t d, ap_linexpr0_t * expr, lsum_t * dest)
{
  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
  if (!a)
    return NULL;
  assert (expr && ap_linexpr0_size (expr) == (a->datadim + a->segmdim) &&
	  d < (a->datadim + a->segmdim) && !dest);
  lsum_t *r = lsum_alloc_internal (pr, a->datadim, a->segmdim);
#ifdef LSUM_REL
  ap_linexpr0_realloc (expr, a->datadim + 3 * a->segmdim);
#ifndef NDEBUG
      fprintf (stdout, "to dcons %d:=", d);
      ap_linexpr0_fprint (stdout, expr, NULL);
      fprintf (stdout, "\n");
#endif
  r->dcons =
    ap_abstract0_assign_linexpr (pr->man_dcons, false, a->dcons, d, expr,
				 NULL);
#ifndef NDEBUG
      fprintf (stdout, "returns ");
      ap_abstract0_fprint (stdout, pr->man_dcons, r->dcons, NULL);
      fprintf (stdout, "\n");
#endif
#else
  if (d < LEN_DIM (a->datadim, 0))
    {
      ap_linexpr0_realloc (expr, LEN_DIM (a->datadim, 0));
      ap_linexpr0_realloc (expr, LEN_DIM (a->datadim, 0) + a->segmdim);
      r->lcons =
	ap_abstract0_assign_linexpr (pr->man_lcons, false, a->lcons, d, expr,
				     NULL);
      r->dcons = ap_abstract0_copy (pr->man_dcons, a->dcons);
    }
  else
    {
      ap_linexpr0_t *expr1 = ap_linexpr0_alloc (AP_LINEXPR_DENSE,
						DATA_DIM (a->datadim,
							  0) +
						2 * a->segmdim);
      size_t i, dim;
      ap_coeff_t *coeff;
      ap_linexpr0_ForeachLinterm (expr, i, dim, coeff)
      {
	if (!ap_coeff_zero (coeff))
	  {
	    ap_dim_t newdim = dim - LEN_DIM (a->datadim, 0);
	    ap_coeff_set (expr1->p.coeff + newdim, coeff);
	  }
      }
      coeff = ap_coeff_alloc (AP_COEFF_SCALAR);
      ap_linexpr0_get_cst (coeff, expr);
      ap_linexpr0_set_cst (expr1, coeff);
      ap_coeff_free (coeff);
      dim = d - LEN_DIM (a->datadim, 0);
#ifndef NDEBUG
      fprintf (stdout, "to dcons %zu:=", dim);
      ap_linexpr0_fprint (stdout, expr1, NULL);
      fprintf (stdout, "\n");
#endif
      r->dcons =
	ap_abstract0_assign_linexpr (pr->man_dcons, false, a->dcons, dim,
				     expr1, NULL);
#ifndef NDEBUG
      fprintf (stdout, "returns ");
      ap_abstract0_fprint (stdout, pr->man_dcons, r->dcons, NULL);
      fprintf (stdout, "\n");
#endif
      ap_linexpr0_free (expr1);
      r->lcons = ap_abstract0_copy (pr->man_lcons, a->lcons);
    }
#endif
  if (destructive)
    lsum_free_internal (pr, a);
  return r;
}

/* TODO: priority 0 */
/* used for pre-image computation */
lsum_t *
lsum_substitute_linexpr (ap_manager_t * man,
			 bool destructive, lsum_t * a,
			 ap_dim_t d, ap_linexpr0_t * expr, lsum_t * dest)
{
  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
/* not used */
lsum_t *
lsum_assign_linexpr_array (ap_manager_t * man,
			   bool destructive, lsum_t * a,
			   ap_dim_t * tdim,
			   ap_linexpr0_t ** texpr, size_t size, lsum_t * dest)
{
  if (size == 1)
    return lsum_assign_linexpr (man, destructive, a, tdim[0], texpr[0], dest);
  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
/* used for pre-image computation */
lsum_t *
lsum_substitute_linexpr_array (ap_manager_t * man,
			       bool destructive, lsum_t * a,
			       ap_dim_t * tdim,
			       ap_linexpr0_t ** texpr,
			       size_t size, lsum_t * dest)
{
  if (size == 1)
    return lsum_substitute_linexpr (man, destructive, a, tdim[0], texpr[0],
				    dest);

  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_SUBSTITUTE_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
/* not used */
lsum_t *
lsum_assign_texpr_array (ap_manager_t * man,
			 bool destructive, lsum_t * a,
			 ap_dim_t * tdim,
			 ap_texpr0_t ** texpr, size_t size, lsum_t * dest)
{
  lsum_internal_t *pr =
    lsum_init_from_manager (man, AP_FUNID_ASSIGN_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
/* used only for pre-image computation */
lsum_t *
lsum_substitute_texpr_array (ap_manager_t * man,
			     bool destructive, lsum_t * a,
			     ap_dim_t * tdim,
			     ap_texpr0_t ** texpr, size_t size, lsum_t * dest)
{
  return ap_generic_substitute_texpr_array (man, destructive, a, tdim, texpr,
					    size, dest);
}
