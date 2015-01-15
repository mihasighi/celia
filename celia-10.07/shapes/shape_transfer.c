/*
 * shape_transfer.c
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

#include "shape.h"
#include "shape_internal.h"
#include "apron2shape.h"
#include "shape_macros.h"
#include "ap_generic.h"



/* ============================================================ */
/* Assignement and Substitutions */
/* ============================================================ */

shape_t *
shape_assign_passign_array (shape_internal_t * pr,
			    bool destructive, shape_t * a,
			    passign0_array_t * arr)
{
  ushape_array_t *r = NULL;
  shape_t *rs;
  size_t i;
  for (i = 0; i < a->msize; i++)
    {
      ushape_array_t *rr =
	ushape_assign_passign_array (pr, false, a->m.p[i], arr);
      if (rr)
	{
	  r = ushape_array_add_array (pr, true, r, rr);
	  ushape_array_init (pr, rr, rr->size);
	  free (rr);
	}
    }
  if (!r)
    return NULL;
  /* compute msize */
  for (i = 0; i < r->size; i++)
    if (!r->p[i])
      break;
  checked_malloc (rs, shape_t, sizeof (shape_t), 1, return NULL;
    );
  rs->m = *r;
  r->p = NULL;
  free (r);
  rs->msize = i;
  rs->set = rs->closed = false;
  rs->intdim = a->intdim;
  rs->realdim = a->realdim;
  if (destructive)
    shape_free_internal (pr, a);
  return rs;
}

shape_t *
shape_assign_linexpr (ap_manager_t * man,
		      bool destructive, shape_t * a,
		      ap_dim_t d, ap_linexpr0_t * expr, shape_t * dest)
{
  if (shape_is_bottom (man, a))
    /* nothing to do */
    return (destructive) ? a : shape_copy (man, a);
  else
    {
      shape_t *b;
      passign0_array_t *op;
      shape_t *r;
      shape_internal_t *pr =
	shape_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
      if (!destructive)
	b = shape_copy_internal (pr, a);
      else
	b = a;
      op =
	shape_passign_of_linexpr_array (pr, &d, &expr, 1, a->intdim,
					a->realdim);
#ifndef NDEBUG1
      fprintf (stdout, " (i.e., ");
      shape_passign_array_fdump (stdout, op, a->intdim, a->realdim);
      fprintf (stdout, " )");
      fflush (stdout);
#endif
      /* go */
      r = shape_assign_passign_array (pr, false, b, op);
      shape_free_internal (pr, b);
      if (dest)
	{			/* intersect r with dest */
	  shape_t *rr = shape_meet (pr->man, false, r, dest);
	  shape_free_internal (pr, r);
	  return rr;
	}
#ifndef NDEBUG1
      fprintf (stdout, " return  ");
      shape_fdump (stdout, man, r);
      fflush (stdout);
#endif
      return r;
    }
}

/* TODO: priority 0 */
/* used for pre-image computation */
shape_t *
shape_substitute_linexpr (ap_manager_t * man,
			  bool destructive, shape_t * a,
			  ap_dim_t d, ap_linexpr0_t * expr, shape_t * dest)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

shape_t *
shape_assign_linexpr_array (ap_manager_t * man,
			    bool destructive, shape_t * a,
			    ap_dim_t * tdim,
			    ap_linexpr0_t ** texpr,
			    size_t size, shape_t * dest)
{
  if (shape_is_bottom (man, a))
    /* nothing to do */
    return (destructive) ? a : shape_copy (man, a);
  else
    {
      shape_t *b;
      passign0_array_t *op;
      shape_t *r;
      shape_internal_t *pr =
	shape_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
      if (!destructive)
	b = shape_copy_internal (pr, a);
      else
	b = a;
      op =
	shape_passign_of_linexpr_array (pr, tdim, texpr, size, a->intdim,
					a->realdim);
#ifndef NDEBUG1
      fprintf (stdout, "\n shape_assign_linexpr_array:\n\t with assign ");
      size_t i;
      for (i = 0; i < size; i++)
	{
	  fprintf (stdout, "x%d := ", tdim[i]);
	  ap_linexpr0_fprint (stdout, texpr[i], NULL);
	  fprintf (stdout, ",  ");
	}
      fflush (stdout);
      fprintf (stdout, " (i.e.  ");
      shape_passign_array_fdump (stdout, op, a->intdim, a->realdim);
      fprintf (stdout, ") \n\t on ");
      shape_fdump (stdout, man, b);
      fflush (stdout);
#endif
      /* go */
      r = shape_assign_passign_array (pr, false, b, op);
      shape_free_internal (pr, b);
      if (dest)
	{			/* intersect r with dest */
	  shape_t *rr = shape_meet (pr->man, false, r, dest);
	  shape_free_internal (pr, r);
	  return rr;
	}
#ifndef NDEBUG1
      fprintf (stdout, "\n\t returns ");
      shape_fdump (stdout, man, r);
      fflush (stdout);
#endif
      return r;
    }
}

/* TODO: priority 0 */
/* used for pre-image computation */
shape_t *
shape_substitute_linexpr_array (ap_manager_t * man,
				bool destructive, shape_t * a,
				ap_dim_t * tdim,
				ap_linexpr0_t ** texpr,
				size_t size, shape_t * dest)
{
  if (size == 1)
    return shape_substitute_linexpr (man, destructive, a, tdim[0], texpr[0],
				     dest);

  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_SUBSTITUTE_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

shape_t *
shape_assign_texpr_array (ap_manager_t * man,
			  bool destructive, shape_t * a,
			  ap_dim_t * tdim,
			  ap_texpr0_t ** texpr, size_t size, shape_t * dest)
{
  if (shape_is_bottom (man, a))
    /* nothing to do */
    return (destructive) ? a : shape_copy (man, a);
  else
    {
      passign0_array_t *op;
      shape_t *r;
      shape_internal_t *pr =
	shape_init_from_manager (man, AP_FUNID_ASSIGN_TEXPR_ARRAY, 0);
      op =
	shape_passign_of_texpr_array (pr, tdim, texpr, size, a->intdim,
				      a->realdim);
#ifndef NDEBUG1
      fprintf (stdout, "\n shape_assign_texpr_array:\n\t with assign ");
      size_t i;
      for (i = 0; i < size; i++)
	{
	  fprintf (stdout, "x%d := ", tdim[i]);
	  ap_texpr0_fprint (stdout, texpr[i], NULL);
	  fprintf (stdout, ",  ");
	}
      fflush (stdout);
      fprintf (stdout, " (i.e.  ");
      shape_passign_array_fdump (stdout, op, a->intdim, a->realdim);
      fprintf (stdout, ")\n\t on ");
      shape_fdump (stdout, man, a);
      fflush (stdout);
#endif
      /* go */
      r = shape_assign_passign_array (pr, false, a, op);
      if (dest)
	{			/* intersect r with dest */
	  shape_t *rr = shape_meet (pr->man, false, r, dest);
	  shape_free_internal (pr, r);
	  return rr;
	}
      if (destructive)
	shape_free_internal (pr, a);
#ifndef NDEBUG1
      fprintf (stdout, "\n\t returns ");
      shape_fdump (stdout, man, r);
      fflush (stdout);
#endif
      return r;
    }
}

/* TODO: priority 0 */
/* used only for pre-image computation */
shape_t *
shape_substitute_texpr_array (ap_manager_t * man,
			      bool destructive, shape_t * a,
			      ap_dim_t * tdim,
			      ap_texpr0_t ** texpr,
			      size_t size, shape_t * dest)
{
  return ap_generic_substitute_texpr_array (man, destructive, a, tdim, texpr,
					    size, dest);
}
