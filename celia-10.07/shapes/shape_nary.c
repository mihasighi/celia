/*
 * shape_nary.c
 *
 * N-ary shapes functions: meet, join, widening, narrowing & related.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 *
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include "apron2shape.h"
#include "ushape.h"
#include "ushape_internal.h"
#include "shape.h"
#include "shape_internal.h"
#include "shape_macros.h"
#include "ap_generic.h"

/* ============================================================ */
/* Meet and Join */
/* ============================================================ */

shape_t *
shape_meet (ap_manager_t * man, bool destructive, shape_t * a1, shape_t * a2)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_MEET, 0);
  size_t i, j, size;
  ushape_array_t arr;
  shape_t *r;
  /* simple cases */
  if (!a1 || !a2)
    return NULL;
  if (shape_is_bottom (man, a1))
    {
      if (destructive)
	{
	  shape_free_internal (pr, a2);
	  return a1;
	}
      else
	return shape_bottom (man, a2->intdim, a2->realdim);
    }
  if (shape_is_bottom (man, a2))
    {
      if (destructive)
	{
	  shape_free_internal (pr, a1);
	  return a2;
	}
      else
	return shape_bottom (man, a2->intdim, a2->realdim);
    }
#ifndef NDEBUG1
  fprintf (stdout, "\n shape_meet:\n\t on ");
  shape_fdump (stdout, man, a1);
  fprintf (stdout, " and ");
  shape_fdump (stdout, man, a2);
  fflush (stdout);
#endif
  /* general case: do meet between each pair of ushapes */
  arr.p = NULL;
  arr.size = 0;
  ushape_array_init (pr, &arr, a1->m.size);
  size = 0;
  for (i = 0; i < a1->m.size; i++)
    if (a1->m.p[i])
      {
	for (j = 0; j < a2->m.size; j++)
	  if (a2->m.p[j])
	    {
	      ushape_t *ur = ushape_meet (man, false, a1->m.p[i], a2->m.p[j]);
	      if (ur && !ushape_is_bottom (man, ur))
		size += ushape_array_add (pr, true, &arr, size, false, false, ur);	/* do not copy and
											 * destroy */
	      else if (!ur)
		ushape_free (man, ur);
	    }
      }
  if (size == 0)
    r = shape_bottom (man, a1->intdim, a1->realdim);
  else
    {
      ushape_array_t *rr;
      r = shape_alloc_internal (pr, size);
      rr = ushape_array_copy (pr, &arr, size);
      r->m = *rr;
      rr->p = NULL;
      free (rr);
      r->set = true;
      r->closed = a1->closed && a2->closed;
      r->msize = size;
      r->intdim = a1->intdim;
      r->realdim = a1->realdim;
    }
#ifndef NDEBUG1
  fprintf (stdout, "\n\t returns ");
  shape_fdump (stdout, man, r);
  fprintf (stdout, "\n");
  fflush (stdout);
#endif
  if (destructive)
    {
      shape_free (man, a1);
      shape_free (man, a2);
    }
  return r;
}

shape_t *
shape_join (ap_manager_t * man, bool destructive, shape_t * a1, shape_t * a2)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_JOIN, 0);
  shape_t *r;
  size_t i, j, size;
  bool *isin1, *isin2;
  /* simple cases */
  if (!a1 && !a2)
    return NULL;
  if (shape_is_bottom (man, a1))
    {
      if (destructive)
	{
	  shape_free_internal (pr, a1);
	  return a2;
	}
      else
	return shape_copy (man, a2);
    }
  if (shape_is_bottom (man, a2))
    {
      if (destructive)
	{
	  shape_free_internal (pr, a2);
	  return a1;
	}
      else
	return shape_copy (man, a1);
    }

#ifndef NDEBUG1
  fprintf (stdout, "\n shape_join:\n\t on ");
  shape_fdump (stdout, man, a1);
  fprintf (stdout, " and ");
  shape_fdump (stdout, man, a2);
  fflush (stdout);
#endif
  /* general case, do the union */
  r = shape_alloc_internal (pr, a1->msize + a2->msize);
  isin1 = (bool *) malloc (a1->msize * sizeof (bool));
  memset (isin1, 0, a1->msize * sizeof (bool));
  isin2 = (bool *) malloc (a2->msize * sizeof (bool));
  memset (isin2, 0, a2->msize * sizeof (bool));
  size = 0;
  for (j = 0; j < a2->msize; j++)
    {
      for (i = 0; i < a1->msize; i++)
	if (!isin1[i])
	  {
	    ushape_t *ur = ushape_join (man, false, a1->m.p[i], a2->m.p[j]);
	    if (ur && !ushape_is_bottom (man, ur))
	      {
		isin1[i] = true;
		isin2[j] = true;
		size += ushape_array_add (pr, true, &r->m, size, false, false, ur);	/* do not copy, nor
											 * destroy */
	      }
	  }
      if (!isin2[j])
	// nothing added for a2->m.p[i]
	size += ushape_array_add (pr, true, &r->m, size, true, false, a2->m.p[j]);	/* copy but not destroy */
    }
  for (i = 0; i < a1->msize; i++)
    if (!isin1[i])
      size +=
	ushape_array_add (pr, true, &r->m, size, true, false, a1->m.p[i]);
  r->set = true;
  r->closed = false;
  r->msize = size;
  r->intdim = a1->intdim;
  r->realdim = a1->realdim;
  if (destructive)
    {
      shape_free_internal (pr, a1);
      shape_free_internal (pr, a2);
    }
#ifndef NDEBUG1
  fprintf (stdout, "\n\t returns ");
  shape_fdump (stdout, man, r);
  fprintf (stdout, "\n");
  fflush (stdout);
#endif
  return r;
}

shape_t *
shape_meet_array (ap_manager_t * man, shape_t ** tab, size_t size)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_MEET_ARRAY, 0);
  arg_assert (size > 0, return NULL;
    );
  shape_t *r = shape_copy_internal (pr, tab[0]);
  size_t i;
  for (i = 1; i < size && !shape_is_bottom (man, r); i++)
    {
      shape_t *rr = shape_meet (man, false, tab[i], r);
      shape_free (man, r);
      r = rr;
    }
  return r;
}

shape_t *
shape_join_array (ap_manager_t * man, shape_t ** tab, size_t size)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_JOIN_ARRAY, 0);
  arg_assert (size > 0, return NULL;
    );
  shape_t *r = shape_copy_internal (pr, tab[0]);
  size_t i;
  for (i = 1; i < size && !shape_is_top (man, r); i++)
    {
      shape_t *rr = shape_join (man, false, tab[i], r);
      shape_free (man, r);
      r = rr;
    }
  return r;
}

/* ============================================================ */
/* Meet constraints and Join generators */
/* ============================================================ */

/*
 * Compute the effect of constraints in array (of size elements) on the shape
 * a. Better performances are obtained if array is sorted, @see
 * <code>shape_pcons_array_of_lincons_array</code>
 */
shape_t *
shape_meet_pcons_array (shape_internal_t * pr, bool destructive,
			shape_t * a, pcons0_array_t * array)
{
  ushape_array_t *r = NULL;
  shape_t *rs;
  size_t i;
  for (i = 0; i < a->msize; i++)
    {
      ushape_array_t *rr =
	ushape_meet_pcons_array (pr, false, a->m.p[i], array);
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
shape_meet_lincons_array (ap_manager_t * man,
			  bool destructive, shape_t * a,
			  ap_lincons0_array_t * array)
{
  if (shape_is_bottom (man, a) || !array || array->size == 0)
    /* nothing to do */
    return (destructive) ? a : shape_copy (man, a);
  else
    {
      shape_t *b;
      pcons0_array_t *arr;
      shape_t *r;
      shape_internal_t *pr =
	shape_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
      if (!destructive)
	b = shape_copy_internal (pr, a);
      else
	b = a;
      /* compute in arr the constraints sorted */
      arr =
	shape_pcons_array_of_lincons_array (pr, array, a->intdim, a->realdim);
#ifndef NDEBUG1
      fprintf (stdout, "\n shape_meet_lincons_array: \n\t with constraint ");
      ap_lincons0_array_fprint (stdout, array, NULL);
      fprintf (stdout, " (i.e, ");
      shape_pcons_array_fdump (stdout, arr, a->intdim, a->realdim);
      fprintf (stdout, ")\n\t on  ");
      shape_fdump (stdout, man, b);
      fprintf (stdout, "\n");
      fflush (stdout);
#endif
      /* go */
      r = shape_meet_pcons_array (pr, false, b, arr);
      shape_free_internal (pr, b);
#ifndef NDEBUG1
      fprintf (stdout, "\n\t returns ");
      shape_fdump (stdout, man, r);
      fprintf (stdout, "\n");
      fflush (stdout);
#endif
      return r;
    }
}

shape_t *
shape_meet_tcons_array (ap_manager_t * man,
			bool destructive, shape_t * a,
			ap_tcons0_array_t * array)
{
  if (shape_is_bottom (man, a) || !array || array->size == 0)
    /* nothing to do */
    return (destructive || !a) ? a : shape_copy (man, a);
  else
    {
      bool exact;
      size_t i;
      shape_t *b;
      pcons0_array_t *arr;
      shape_t *r;
      shape_internal_t *pr =
	shape_init_from_manager (man, AP_FUNID_MEET_TCONS_ARRAY, 0);
      if (!destructive)
	b = shape_copy_internal (pr, a);
      else
	b = a;
      /* compute in arr the constraints sorted */
      arr =
	shape_pcons_array_of_tcons_array (pr, array, a->intdim, a->realdim);
#ifndef NDEBUG1
      fprintf (stdout, "\n shape_meet_tcons_array: \n\t with constraint ");
      //ap_tcons0_array_fprint (stdout, array,NULL);
      //fprintf (stdout, "\t\t (i.e, ");
      shape_pcons_array_fdump (stdout, arr, a->intdim, a->realdim);
      //fprintf (stdout, ")\n");
      fprintf (stdout, "\n\t on ");
      shape_fdump (stdout, man, b);
      fflush (stdout);
#endif
      /* go */
      r = shape_meet_pcons_array (pr, false, b, arr);
      shape_free_internal (pr, b);
#ifndef NDEBUG1
      fprintf (stdout, "\n\t returns ");
      shape_fdump (stdout, man, r);
      fflush (stdout);
#endif
      return r;
    }
}

/* NOT IMPLEMENTED */
shape_t *
shape_add_ray_array (ap_manager_t * man,
		     bool destructive, shape_t * a,
		     ap_generator0_array_t * array)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_ADD_RAY_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}


/* ============================================================ */
/* Widening, Narrowing */
/* ============================================================ */

/* Windening is done between elements of shapes that have an
 * isomorphic hgraph; otherwie, these elements are joined to the shape.
 */
shape_t *
shape_widening (ap_manager_t * man, shape_t * a1, shape_t * a2)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_WIDENING, 0);
  shape_t *r;
  size_t i, j, size;
  bool *isin1, *isin2;
  /* simple cases */
  if (!a1 || !a2 || shape_is_bottom (man, a1) || shape_is_bottom (man, a2))
    return shape_bottom (man, pr->intdim, pr->realdim);

#ifndef NDEBUG1
  fprintf (stdout, "\n shape_widening:\n\t on ");
  shape_fdump (stdout, man, a1);
  fprintf (stdout, " and ");
  shape_fdump (stdout, man, a2);
  fflush (stdout);
#endif
  /* general case, apply widening on ushapes with isomorphic graphs */
  r = shape_alloc_internal (pr, a1->msize + a2->msize);
  isin1 = (bool *) malloc (a1->msize * sizeof (bool));
  memset (isin1, 0, a1->msize * sizeof (bool));
  isin2 = (bool *) malloc (a2->msize * sizeof (bool));
  memset (isin2, 0, a2->msize * sizeof (bool));
  size = 0;
  for (j = 0; j < a2->msize; j++)
    {
      for (i = 0; i < a1->msize; i++)
	if (!isin1[i])
	  {
	    ushape_t *ur = ushape_widening (man, a1->m.p[i], a2->m.p[j]);
	    if (ur && !ushape_is_bottom (man, ur))
	      {
		isin1[i] = true;	/* only isomorphic graphs are widen */
		isin2[j] = true;
		size += ushape_array_add (pr, true, &r->m, size, false, false, ur);	/* do not copy, nor
											 * destroy */
	      }
	  }
      if (!isin2[j])
	// nothing added for a2->m.p[i]
	size += ushape_array_add (pr, true, &r->m, size, true, false, a2->m.p[j]);	/* copy but not destroy */
    }
  /*
     for (i = 0; i < a1->msize; i++)
     if (!isin1[i])
     size += ushape_array_add (pr, true, &r->m, size, true, false, a1->m.p[i]);
   */
  r->set = true;
  r->closed = false;		/* TODO: widening conserves closed property */
  r->msize = size;
  r->intdim = a1->intdim;
  r->realdim = a1->realdim;
#ifndef NDEBUG1
  fprintf (stdout, "\n\t returns ");
  shape_fdump (stdout, man, r);
  fprintf (stdout, "\n");
  fflush (stdout);
#endif
  return r;
}

/* NOT IMPLEMENTED */
shape_t *
shape_widening_thresholds (ap_manager_t * man,
			   shape_t * a1, shape_t * a2,
			   ap_scalar_t ** array, size_t nb)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_WIDENING, nb + 1);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a2;
}

/* NOT IMPLEMENTED */
shape_t *
shape_narrowing (ap_manager_t * man, shape_t * a1, shape_t * a2)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_WIDENING, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a2;
}
