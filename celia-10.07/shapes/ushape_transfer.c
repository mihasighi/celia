/*
 * ushape_transfer.c
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

#include "ushape.h"
#include "ushape_internal.h"
#include "hgraph_internal.h"
#include "apron2shape.h"
#include "shape_macros.h"
#include "ap_generic.h"


/* ============================================================ */
/* Assignement and Substitutions */
/* ============================================================ */

ushape_t *
ushape_assign_data (ushape_internal_t * pr, ushape_t * a, size_t lhs,
		    ap_linexpr0_t * lexpr)
{
  /* TODO: check that lhs is not ptr null */

  ap_dim_t d = (lhs < a->datadim) ? lhs : NODE_DATADIM (a->datadim,
							a->h->size,
							VAR2NODE (a->h,
								  DIM2PTR
								  (lhs,
								   a->
								   datadim)));
  size_t *v2n = hgraph_get_var2node (a->h);
  ap_linexpr0_t *exprn = shape_linexpr_of_node (pr, lexpr, v2n, a->h->size,
						a->datadim, a->ptrdim);
#ifndef NDEBUG
  fprintf (stdout, "%d:=", d);
  ap_linexpr0_fprint (stdout, exprn, NULL);
  fprintf (stdout, "\n");
  fflush (stdout);
#endif
  free (v2n);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  b->h = hgraph_copy_internal (pr, a->h);
  b->closed = hgraph_copy_internal (pr, a->closed);
  bool isbot = false;
  size_t i;
  for (i = 0; i < pr->size_scons; i++)
    {
      b->scons[i] =
	ap_abstract0_assign_linexpr (pr->man_scons[i], false, a->scons[i],
				     d, exprn, NULL);
      isbot = ap_abstract0_is_bottom (pr->man_scons[i], b->scons[i]);
    }
  ap_linexpr0_free (exprn);
  if (isbot)
    ushape_free_internal (pr, b);
  return b;
}

ushape_t *
ushape_assign_x_y (ushape_internal_t * pr, ushape_t * a, size_t vx, size_t vy)
{
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ap_dimperm_set_id (&perm);
  hgraph_t *h = hgraph_assign_x_y (pr, a->h, vx, vy, &perm);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, a, b, &perm);
  return b;
}

ushape_t *
ushape_assign_x_next_y (ushape_internal_t * pr, ushape_t * a, size_t vx,
			size_t vy)
{
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ap_dimperm_set_id (&perm);
  hgraph_t *h = hgraph_assign_x_next_y (pr, a->h, vx, vy, &perm);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, a, b, &perm);
  return b;
}


ushape_t *
ushape_assign_x_y_next (ushape_internal_t * pr, bool conservative,
			ushape_t * a, size_t vx, size_t vy)
{
  ap_dimperm_t perm;
  if (conservative)
    ap_dimperm_init (&perm, a->h->size);
  else
    ap_dimperm_init (&perm, a->h->size + 1);
  ap_dimperm_set_id (&perm);
  hgraph_t *h =
    hgraph_assign_x_y_next (pr, conservative, a->h, vx, vy, &perm);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_expand_internal (pr, a, b, (ap_dim_t) VAR2NODE (a->h, vy),
			  (conservative) ? 1 : 2);
  ushape_apply_dimperm (pr, b, b, &perm);
  return b;
}

ushape_t *
ushape_assign_x_null (ushape_internal_t * pr, bool isfree, ushape_t * a,
		      size_t vx)
{
  ap_dimperm_t perm;
  ap_dim_array2_t anon;
  ap_dimperm_init (&perm, a->h->size);
  ap_dimperm_set_id (&perm);
  anon.size = 0;
  anon.p = NULL;
  hgraph_t *h;
  if (isfree)
    h = hgraph_assign_x_free (pr, a->h, vx, &perm, &anon);
  else
    h = hgraph_assign_x_null (pr, a->h, vx, &perm, &anon);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
#ifndef NDEBUG
  fprintf (stdout, "\nushape_assign_null: on ");
  ushape_fdump (stdout, pr->man, a);
  fflush (stdout);
#endif
  ushape_fold_internal (pr, a, b, &anon);
#ifndef NDEBUG
  fprintf (stdout, "\n\tafter fold: ");
  ushape_fdump (stdout, pr->man, b);
  fflush (stdout);
#endif
  ushape_apply_dimperm (pr, (anon.size == 0) ? a : b, b, &perm);
#ifndef NDEBUG
  fprintf (stdout, "\n\tafter dimperm: ");
  ushape_fdump (stdout, pr->man, b);
  fflush (stdout);
#endif
  return b;
}

ushape_t *
ushape_assign_x_next_null (ushape_internal_t * pr, bool isfree, ushape_t * a,
			   size_t vx)
{
  ap_dimperm_t perm;
  ap_dim_array2_t anon;
  ap_dimperm_init (&perm, a->h->size);
  ap_dimperm_set_id (&perm);
  anon.size = 0;
  anon.p = NULL;
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h;
  if (isfree)
    h = hgraph_assign_x_next_free (pr, a->h, vx, &perm, &anon);
  else
    h = hgraph_assign_x_next_null (pr, a->h, vx, &perm, &anon);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_expand_internal (pr, a, b, (ap_dim_t) VAR2NODE (a->h, vx), 1);
  ushape_fold_internal (pr, b, b, &anon);
  ushape_apply_dimperm (pr, b, b, &perm);
  return b;
}

ushape_t *
ushape_assign_x_new (ushape_internal_t * pr, ushape_t * a, size_t vx)
{
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ap_dimperm_set_id (&perm);
  hgraph_t *h = hgraph_assign_x_new (pr, a->h, vx, &perm);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  node_t nn = VAR2NODE (b->h, vx);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, nn);
  // add constraint l[new-node] == 1
  ushape_expand_internal (pr, b, b, nn, 1);
  return b;
}

ushape_array_t *
ushape_assign_passign (ushape_internal_t * pr, ushape_t * a, passign0_t * op)
{
  ushape_array_t *r;
  ushape_t *b;
  size_t i, rsize;

  if (ushape_is_bottom (pr->man, a))
    return NULL;
  arg_assert (a && op, return NULL;
    );

  r = ushape_array_make (pr, 1);
  rsize = 0;
  // TODO: set all variables mapped to NODE_T_TOP in a->h to NULL node
  if (op->lhs < a->datadim || op->type == DATA_MODIFIER)
    {				// data assignment, only a->scons affected
      b = ushape_assign_data (pr, a, op->x, op->info.data);
      rsize += ushape_array_add (pr, true, r, rsize, true, true, b);
      return r;
    }
  else
    {				// pure pointer assignment
      if (!IS_PTRVAR (pr, op->lhs, a->datadim, a->ptrdim))
	{
	  ERROR ("Bad assignment: not a pointer left value!", return NULL;
	    );
	}
      if (!IS_PTRDIM (op->info.ptr.y, a->datadim, a->ptrdim))
	{
	  ERROR ("Bad assignment: not a pointer right value!", return NULL;
	    );
	}

      size_t vx = DIM2PTR (op->x, a->datadim);
      node_t nx = VAR2NODE (a->h, vx);
      size_t vy = DIM2PTR (op->info.ptr.y, a->datadim);
      node_t ny = VAR2NODE (a->h, vy);

      if ((nx == 0 && op->nextx >= 1) || (ny == 0 && op->info.ptr.nexty >= 1))
	{
	  ERROR ("Bad assignment: null pointer dereference!", return NULL;
	    );
	}

      if (op->type == NO_MODIFIER)
	{			// x := [null | new | free | y | y->next]
	  if (IS_PTRVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) &&
	      !IS_NULLVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) &&
	      ny != 0 && op->info.ptr.nexty == 0)
	    {			// x := y
	      b = ushape_assign_x_y (pr, a, vx, vy);
	      rsize += ushape_array_add (pr, true, r, 0, false, false, b);	/* put b as built in r */
	    }
	  else if (IS_PTRVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) &&
		   !IS_NULLVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) &&
		   op->info.ptr.nexty == 1)
	    {			// x := y->next
	      // case (1): no new node
	      b = ushape_assign_x_y_next (pr, true, a, vx, vy);
	      rsize += ushape_array_add (pr, true, r, 0, false, false, b);
	      // case (2): new node
	      b = ushape_assign_x_y_next (pr, false, a, vx, vy);
	      rsize += ushape_array_add (pr, true, r, 0, false, false, b);
	    }
	  else if (IS_NEWVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim))
	    {			// x := new
	      b = ushape_assign_x_new (pr, a, vx);
	      rsize += ushape_array_add (pr, true, r, 0, false, false, b);	/* put b as built in r */
	    }
	  else if (IS_FREEVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim))
	    {			// x := free
	      b = ushape_assign_x_null (pr, true, a, vx);
	      rsize += ushape_array_add (pr, true, r, 0, false, false, b);
	    }
	  else if (IS_NULLVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) ||
		   ny == 0)
	    {			// x := null | y (with y==null)
	      if (nx != 0)
		{
		  b = ushape_assign_x_null (pr, false, a, vx);
		  rsize += ushape_array_add (pr, true, r, 0, false, false, b);
		}
	      else
		{		// else nothing to do, put b (copy of a) in r
		  b = ushape_copy_internal (pr, a);
		  rsize += ushape_array_add (pr, true, r, 0, false, false, b);	/* put b as built in r */
		}
	    }
	  else
	    {
	      ERROR ("Bad assignment: not an allowed right value!",
		     return NULL;
		);
	    }
	}
      else
	{			// op->type == NEXT_MODIFIER && IS_PTRDIM(op->lhs,a->datadim,a->ptrdim)
	  // x->next = [null | y]
	  if (op->nextx != 1 && op->info.ptr.nexty != 0)
	    {
	      ERROR ("Bad assignment: not an elementary assignment!",;
		);
	    }
	  if (IS_PTRVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) &&
	      !IS_NULLVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) &&
	      ny != 0)
	    {			// x->next := y, x->next is null
	      b = ushape_assign_x_next_y (pr, a, vx, vy);
	      rsize += ushape_array_add (pr, true, r, 0, false, false, b);	/* put b as built in r */
	    }
	  else if (IS_NULLVAR (pr, op->info.ptr.y, a->datadim, a->ptrdim) ||
		   ny == 0)
	    {			// x->next = null
	      b = ushape_assign_x_next_null (pr, false, a, vx);
	      rsize += ushape_array_add (pr, true, r, 0, false, false, b);
	    }
	  else
	    {
	      ERROR ("Bad assignment: not an elementary assignment!",;
		);
	    }
	}
    }
  if (rsize)
    ushape_array_resize (pr, r, rsize);
  else if (r)
    {
      ushape_array_clear (pr, r, r->size);
      free (r->p);
      free (r);
      r = NULL;
    }
  return r;
}

ushape_array_t *
ushape_assign_passign_array (ushape_internal_t * pr, bool destructive,
			     ushape_t * a, passign0_array_t * arr)
{
  ushape_array_t *r = ushape_array_make (pr, 1);
  r->p[0] = ushape_copy_internal (pr, a);
  size_t i;
  for (i = 0; i < arr->size; i++)
    {				// apply each operation of the list on all elements of r
      ushape_array_t *rr = ushape_array_make (pr, r->size);
      size_t j;
      for (j = 0; j < r->size; j++)
	if (r->p[j])
	  {
	    ushape_array_t *rj =
	      ushape_assign_passign (pr, r->p[j], arr->p[i]);
	    if (rj)
	      {
		ushape_array_add_array (pr, true, rr, rj);
		ushape_array_init (pr, rj, rj->size);
		free (rj);
	      }
	  }
      ushape_array_init (pr, r, r->size);
      r = rr;
    }
  if (destructive)
    ushape_free_internal (pr, a);
  return r;
}

ushape_t *
ushape_assign_linexpr (ap_manager_t * man,
		       bool destructive, ushape_t * a,
		       ap_dim_t d, ap_linexpr0_t * expr, ushape_t * dest)
{
  if (ushape_is_bottom (man, a))
    /* nothing to do */
    return (destructive) ? a : ushape_copy (man, a);
  else
    {
      ushape_t *b;
      passign0_array_t *op;
      ushape_array_t *r;
      ushape_internal_t *pr =
	ushape_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
      ap_dimension_t dim = ushape_dimension (man, a);
      if (!destructive)
	b = ushape_copy_internal (pr, a);
      else
	b = a;
      op =
	shape_passign_of_linexpr_array (pr, &d, &expr, 1, dim.intdim,
					dim.realdim);
#ifndef NDEBUG
      printf ("\n========== shape assign with passign:\n");
      shape_passign_array_fdump (stdout, op, dim.intdim, dim.realdim);
#endif
      /* go */
      r = ushape_assign_passign_array (pr, false, b, op);
      ushape_free_internal (pr, b);
      b = NULL;
      if (r)
	b = r->p[0];
      if (dest && b)
	{			/* intersect r with dest */
	  ushape_t *rr = ushape_meet (pr->man, false, b, dest);
	  ushape_free_internal (pr, b);
	  return rr;
	}
      return b;
    }
}

/* TODO: priority 0 */
/* used for pre-image computation */
ushape_t *
ushape_substitute_linexpr (ap_manager_t * man,
			   bool destructive, ushape_t * a,
			   ap_dim_t d, ap_linexpr0_t * expr, ushape_t * dest)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

ushape_t *
ushape_assign_linexpr_array (ap_manager_t * man,
			     bool destructive, ushape_t * a,
			     ap_dim_t * tdim,
			     ap_linexpr0_t ** texpr,
			     size_t size, ushape_t * dest)
{
  if (ushape_is_bottom (man, a))
    /* nothing to do */
    return (destructive) ? a : ushape_copy (man, a);
  else
    {
      ushape_t *b;
      passign0_array_t *op;
      ushape_array_t *r;
      ushape_internal_t *pr =
	ushape_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
      ap_dimension_t dim = ushape_dimension (man, a);
      if (!destructive)
	b = ushape_copy_internal (pr, a);
      else
	b = a;
      op =
	shape_passign_of_linexpr_array (pr, tdim, texpr, size, dim.intdim,
					dim.realdim);
#ifndef NDEBUG
      printf ("\n========== shape assign with passign:\n");
      shape_passign_array_fdump (stdout, op, dim.intdim, dim.realdim);
#endif
      /* go */
      r = ushape_assign_passign_array (pr, false, b, op);
      ushape_free_internal (pr, b);
      b = NULL;
      if (r)
	b = r->p[0];
      if (dest && b)
	{			/* intersect r with dest */
	  ushape_t *rr = ushape_meet (pr->man, false, b, dest);
	  ushape_free_internal (pr, b);
	  return rr;
	}
      return b;
    }
}

/* TODO: priority 0 */
/* used for pre-image computation */
ushape_t *
ushape_substitute_linexpr_array (ap_manager_t * man,
				 bool destructive, ushape_t * a,
				 ap_dim_t * tdim,
				 ap_linexpr0_t ** texpr,
				 size_t size, ushape_t * dest)
{
  if (size == 1)
    return ushape_substitute_linexpr (man, destructive, a, tdim[0], texpr[0],
				      dest);

  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_SUBSTITUTE_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

ushape_t *
ushape_assign_texpr_array (ap_manager_t * man,
			   bool destructive, ushape_t * a,
			   ap_dim_t * tdim,
			   ap_texpr0_t ** texpr, size_t size, ushape_t * dest)
{
  if (ushape_is_bottom (man, a))
    /* nothing to do */
    return (destructive) ? a : ushape_copy (man, a);
  else
    {
      ushape_t *b;
      passign0_array_t *op;
      ushape_array_t *r;
      ushape_internal_t *pr =
	shape_init_from_manager (man, AP_FUNID_ASSIGN_TEXPR_ARRAY, 0);
      ap_dimension_t dim = ushape_dimension (man, a);
      if (!destructive)
	b = ushape_copy_internal (pr, a);
      else
	b = a;
      op =
	shape_passign_of_texpr_array (pr, tdim, texpr, size, dim.intdim,
				      dim.realdim);
#ifndef NDEBUG
      printf ("\n========== ushape assign with passign:\n");
      shape_passign_array_fdump (stdout, op, dim.intdim, dim.realdim);
#endif
      /* go */
      r = ushape_assign_passign_array (pr, false, b, op);
      ushape_free_internal (pr, b);
      b = NULL;
      if (r)
	b = r->p[0];
      if (dest && b)
	{			/* intersect r with dest */
	  ushape_t *rr = ushape_meet (pr->man, false, b, dest);
	  ushape_free_internal (pr, b);
	  return rr;
	}
      return b;
    }
}

/* TODO: priority 0 */
/* used only for pre-image computation */
ushape_t *
ushape_substitute_texpr_array (ap_manager_t * man,
			       bool destructive, ushape_t * a,
			       ap_dim_t * tdim,
			       ap_texpr0_t ** texpr,
			       size_t size, ushape_t * dest)
{
  return ap_generic_substitute_texpr_array (man, destructive, a, tdim, texpr,
					    size, dest);
}
