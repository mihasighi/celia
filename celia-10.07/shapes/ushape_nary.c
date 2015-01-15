/*
 * ushape_nary.c
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

#include "hgraph_internal.h"
#include "ushape.h"
#include "ushape_internal.h"
#include "shape_macros.h"
#include "ap_generic.h"

/* ============================================================ */
/* Meet and Join */
/* ============================================================ */

ushape_t *
ushape_meet (ap_manager_t * man, bool destructive, ushape_t * a1,
	     ushape_t * a2)
{
  ushape_internal_t *pr = ushape_init_from_manager (man, AP_FUNID_MEET, 0);
  ushape_t *r;

  if (ushape_is_bottom (man, a1))
    return (destructive) ? a1 : ushape_bottom (man, a1->datadim, a1->ptrdim);
  if (ushape_is_bottom (man, a2))
    return (destructive) ? a2 : ushape_bottom (man, a2->datadim, a2->ptrdim);

  if (!hgraph_is_equal (a1->h, a2->h))
    r = NULL;			/* bottom */
  else
    {
      size_t i;
      bool isbottom = false;
      r = ushape_alloc_internal (pr, a1->datadim, a1->ptrdim);
      r->h = hgraph_copy_internal (pr, a1->h);
      for (i = 0; i < pr->size_scons && !isbottom; i++)
	{
	  r->scons[i] =
	    ap_abstract0_meet (pr->man_scons[i], false,
			       (a1->scons) ? a1->scons[i] : NULL,
			       (a2->scons) ? a2->scons[i] : NULL);
	  isbottom = ap_abstract0_is_bottom (pr->man_scons[i], r->scons[i]);
	}
      if (isbottom)
	{
	  ushape_free_internal (pr, r);
	  r = ushape_bottom (man, a1->datadim, a1->ptrdim);
	}
    }
  if (destructive)
    {
      ushape_free_internal (pr, a1);
      ushape_free_internal (pr, a2);
    }
  return r;
}


ushape_t *
ushape_join (ap_manager_t * man, bool destructive, ushape_t * a1,
	     ushape_t * a2)
{
  ushape_internal_t *pr = ushape_init_from_manager (man, AP_FUNID_JOIN, 0);

  if (ushape_is_bottom (man, a1) && ushape_is_bottom (man, a2))
    return (destructive) ? a1 : ushape_bottom (man, a1->datadim, a1->ptrdim);
  if (ushape_is_bottom (man, a1))
    return (destructive) ? a2 : ushape_copy (man, a2);
  if (ushape_is_bottom (man, a2))
    return (destructive) ? a1 : ushape_copy (man, a1);

  if (!hgraph_is_equal (a1->h, a2->h))
    return NULL;		/* bottom */
  else
    {
      size_t i;
      ushape_t *r = ushape_alloc_internal (pr, a1->datadim, a1->ptrdim);

      r->h = hgraph_copy_internal (pr, a1->h);
      for (i = 0; i < pr->size_scons; i++)
	{
	  r->scons[i] =
	    ap_abstract0_join (pr->man_scons[i], false, a1->scons[i],
			       a2->scons[i]);
	}
      if (destructive)
	{
	  ushape_free_internal (pr, a1);
	  ushape_free_internal (pr, a2);
	}
      return r;
    }
}


ushape_t *
ushape_meet_array (ap_manager_t * man, ushape_t ** tab, size_t size)
{
  ushape_internal_t *pr = ushape_init_from_manager (man, AP_FUNID_JOIN, 0);
  arg_assert (size > 0, return NULL;
    );
  ushape_t *r = ushape_copy_internal (pr, tab[0]);
  size_t i;
  bool isbottom = false;

  for (i = 1; i < size && !isbottom; i++)
    {
      ushape_t *rr = ushape_meet (man, false, tab[i], r);
      ushape_free (man, r);
      isbottom = ushape_is_bottom (man, rr);
      r = rr;
    }
  if (isbottom)
    {
      ushape_free (man, r);
      r = NULL;
    }
  return r;
}

ushape_t *
ushape_join_array (ap_manager_t * man, ushape_t ** tab, size_t size)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_JOIN_ARRAY, 0);
  arg_assert (size > 0, return NULL;
    );

  ushape_t *r = ushape_copy_internal (pr, tab[0]);
  size_t i;
  bool istop = false;

  for (i = 1; i < size && !istop; i++)
    {
      ushape_t *rr = ushape_join (man, false, tab[i], r);
      ushape_free (man, r);
      istop = ushape_is_top (man, rr);
      r = rr;
    }
  if (istop)
    {
      ushape_free (man, r);
      r = ushape_top (man, tab[0]->datadim, tab[0]->ptrdim);
    }
  return r;
}


/* ============================================================ */
/* Meet constraints and Join generators */
/* ============================================================ */

/* Functions used by meet constraints */
ushape_t *
ushape_copy_set_succ (ushape_internal_t * pr,
		      ushape_t * a, node_t nsrc, node_t ndst)
{
  ap_dimperm_t perm;
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_copy_mem (pr, a->h);
  ap_dimperm_init (&perm, a->h->size);
  hgraph_node_set_succ (h, nsrc, ndst, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, a, b, &perm);
  return b;
}

ushape_t *
ushape_add_succ_fixed (ushape_internal_t * pr, ushape_t * a, node_t v,
		       node_t nsucc)
{
  node_t n;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, nsucc, v, 0, &n, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, n);
  return b;
}

size_t
ushape_add_succ_fixed_all (ushape_internal_t * pr, ushape_t * a, size_t v1,
			   size_t v2, ushape_array_t * r, size_t rsize)
{
  node_t n1, n2;
  size_t i;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, NODE_T_TOP, v1, 0, &n1, &perm);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, n1);
  i = 0;
  hgraph_node_forall (h, n2)
  {
    if (n2 != n1)
      {
	ushape_t *b1 = ushape_alloc_internal (pr, b->datadim, b->ptrdim);
	hgraph_t *h1 = hgraph_copy_mem (pr, h);
	ap_dimperm_init (&perm, h1->size);
	hgraph_node_set_var (h1, n2, v2, &perm);
	hgraph_node_set_succ (h1, n1, n2, &perm);
	b->h = hgraph_copy_internal (pr, h1);
	ushape_apply_dimperm (pr, b, b1, &perm);
	b1->h = hgraph_copy_internal (pr, h1);
	i += ushape_array_add (pr, true, r, (rsize + i), true, true, b1);
	hgraph_free_internal (pr, h1);
	// ushape_free_internal(pr, b1);
      }
  }
  hgraph_free_internal (pr, h);
  ushape_free_internal (pr, b);
  return i;
}

ushape_t *
ushape_copy_set_var (ushape_internal_t * pr, ushape_t * a, node_t n1,
		     size_t v1, node_t n2, size_t v2)
{
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ap_dimperm_set_id (&perm);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_copy_mem (pr, a->h);
  hgraph_node_set_var (h, n1, v1, &perm);
  if (v2 && v1 != v2)
    hgraph_node_set_var (h, n2, v2, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, a, b, &perm);
  return b;
}

ushape_t *
ushape_add_set_var (ushape_internal_t * pr, ushape_t * a, node_t nsucc,
		    size_t vmin, size_t vmax)
{
  node_t n;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, NODE_T_TOP, vmin, 0, &n, &perm);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, n);
  if (vmax && vmin != vmax)
    {
      ap_dimperm_init (&perm, h->size);
      hgraph_node_set_var (h, n, vmax, &perm);
      b->h = hgraph_copy_internal (pr, h);
      ushape_apply_dimperm (pr, b, b, &perm);
      ap_dimperm_clear (&perm);
    }
  else
    b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  return b;
}

ushape_t *
ushape_add_set_info (ushape_internal_t * pr, ushape_t * a,
		     node_t nsucc, size_t v, size_t nn)
{
  node_t i;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, nsucc, v, nn, &i, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, i);
  return b;
}

ushape_t *
ushape_copy_set_info (ushape_internal_t * pr, ushape_t * a, node_t n,
		      node_t nsucc, size_t v, size_t nn)
{
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_copy_mem (pr, a->h);
  hgraph_node_set_var (h, n, v, &perm);
  hgraph_node_set_succ (h, n, nsucc, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, a, b, &perm);
  return b;
}

ushape_t *
ushape_set_new_succ (ushape_internal_t * pr, ushape_t * a, node_t n, size_t v)
{
  node_t nnew;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, NODE_T_TOP, v, 0, &nnew, &perm);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, nnew);	/* also clear perm */
  hgraph_node_set_succ (h, n, nnew, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, b, b, &perm);
  return b;
}

size_t
ushape_set_new_succ_all (ushape_internal_t * pr, ushape_t * a, size_t v1,
			 size_t v2, ushape_array_t * r, size_t rsize)
{
  node_t n1, n2;
  size_t i;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, NODE_T_TOP, v2, 0, &n2, &perm);
  b->h = hgraph_copy_internal (pr, h);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, n2);
  i = 0;
  hgraph_node_forall (h, n1)
  {
    if (n1 != n2 && NODE_T_TOP == hgraph_node_get_succ (h, n1))
      {
	ushape_t *b1 = ushape_alloc_internal (pr, b->datadim, b->ptrdim);
	hgraph_t *h1 = hgraph_copy_mem (pr, h);
	ap_dimperm_init (&perm, h->size);
	hgraph_node_set_var (h1, n1, v1, &perm);
	hgraph_node_set_succ (h1, n1, n2, &perm);
	b1->h = hgraph_copy_internal (pr, h1);
	ushape_apply_dimperm (pr, b, b1, &perm);
	i += ushape_array_add (pr, true, r, rsize + i, true, true, b1);	/* copy and destroy */
	hgraph_free_internal (pr, h1);
      }
  }
  hgraph_free_internal (pr, h);
  ushape_free_internal (pr, b);
  return i;
}

ushape_t *
ushape_add_loop (ushape_internal_t * pr, ushape_t * a, size_t vmin,
		 size_t vmax)
{
  node_t n;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, NODE_T_TOP, vmin,
				 0, &n, &perm);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, n);
  ap_dimperm_init (&perm, h->size);
  if (vmax && vmin != vmax)
    hgraph_node_set_var (h, n, vmax, &perm);
  hgraph_node_set_succ (h, n, n, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, b, b, &perm);
  return b;
}

ushape_t *
ushape_add_edge (ushape_internal_t * pr, ushape_t * a, size_t vsrc,
		 size_t vdst)
{
  node_t nsrc, ndst;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, NODE_T_TOP, vdst, 0, &ndst, &perm);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, ndst);
  hgraph_t *h1 = h;
  ap_dimperm_init (&perm, h->size);
  h = hgraph_node_add (pr, h1, ndst, vsrc, 0, &nsrc, &perm);
  hgraph_free_internal (pr, h1);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm_dimchange (pr, b, b, &perm, nsrc);
  return b;
}

ushape_t *
ushape_add_between (ushape_internal_t * pr, ushape_t * a, size_t vsrc,
		    node_t nsrc, node_t ndst)
{
  node_t i;
  ap_dimperm_t perm;
  ap_dimperm_init (&perm, a->h->size);
  ap_dimperm_set_id (&perm);
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  hgraph_t *h = hgraph_node_add (pr, a->h, ndst, vsrc, 1, &i, &perm);
  ushape_apply_dimperm_dimchange (pr, a, b, &perm, i);
  ap_dimperm_init (&perm, h->size);
  ap_dimperm_set_id (&perm);
  hgraph_node_set_succ (h, nsrc, i, &perm);
  b->h = hgraph_copy_internal (pr, h);
  hgraph_free_internal (pr, h);
  ushape_apply_dimperm (pr, b, b, &perm);
  return b;
}

ushape_array_t *
ushape_meet_pcons (ushape_internal_t * pr, bool destructive,
		   ushape_t * a, pcons0_t * c)
{
  ushape_array_t *r = NULL;
  ushape_t *b;
  hgraph_t *h;
  size_t i, rsize = 0;
  bool isbot;
  arg_assert (a && c, return NULL;
    );
#ifndef NDEBUG
  fprintf (stdout, "\n**********meet_pcons: \n\t--->");
  ushape_fdump (stdout, pr->man, a);
  fprintf (stdout, "\n\t--->pcons: \n\t");
  shape_pcons_fdump (stdout, c, a->datadim, a->ptrdim);
#endif
  if (ushape_is_bottom (pr->man, a))
    return NULL;
  if (c->type == DATA_CONS)
    {
      ap_lincons0_array_t arr = ap_lincons0_array_make (1);
      size_t *v2n = hgraph_get_var2node (a->h);
      arr.p[0] =
	shape_lincons_of_node (pr, &c->info.data, v2n, a->h->size,
			       a->datadim, a->ptrdim);
      free (v2n);
      r = ushape_array_make (pr, 1);
      r->p[0] = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
      r->p[0]->h = hgraph_copy_internal (pr, a->h);
      r->p[0]->closed = hgraph_copy_internal (pr, a->closed);
      rsize = 1;
      isbot = false;
      for (i = 0; i < pr->size_scons && !isbot; i++)
	{
	  r->p[0]->scons[i] =
	    ap_abstract0_meet_lincons_array (pr->man_scons[i], false,
					     a->scons[i], &arr);
	  isbot = ap_abstract0_is_bottom (pr->man_scons[i], a->scons[i]);
	}
      ap_lincons0_array_clear (&arr);
      if (isbot)
	{
	  ushape_array_clear (pr, r, r->size);
	  free (r);
	  r = NULL;
	  rsize = 0;
	}
    }
  else if (c->type < OTHER_CONS)
    {
      node_t vx, nx, nnx, vy, ny, nny;
      /* no more than 1 next dereference */
      if (c->info.ptr.nextx + c->info.ptr.nexty > 1)
	{
	  ap_manager_raise_exception (pr->man, AP_EXC_NOT_IMPLEMENTED,
				      pr->funid, "not implemented");
	  return NULL;
	}
      vx = DIM2PTR (c->info.ptr.x, a->datadim);
      nx = VAR2NODE (a->h, vx);
      nnx = hgraph_node_get_nsucc (a->h, nx, c->info.ptr.nextx);
      vy = DIM2PTR (c->info.ptr.y, a->datadim);
      ny = VAR2NODE (a->h, vy);
      nny = hgraph_node_get_nsucc (a->h, ny, c->info.ptr.nexty);
      arg_assert (vx >= PTRVAR_NULL (pr, a->datadim, a->ptrdim)
		  && vx < a->ptrdim, return NULL;
	);

      /* TODO: remove when assume is dealt correctly */
      if (vy < PTRVAR_NULL (pr, a->datadim, a->ptrdim)
	  && ushape_is_top (pr->man, a))
	{
	  r = ushape_array_make (pr, 1);
	  rsize = 1;
	  r->p[0] = ushape_make (pr, (size_t) vy, a->datadim, a->ptrdim);
	}
      else
	{
	  arg_assert (vy < a->ptrdim, return NULL;
	    );

	  if ((nx == 0 && c->info.ptr.nextx >= 1)
	      || (ny == 0 && c->info.ptr.nexty >= 1))
	    {
	      ERROR ("Null pointer dereference",;
		);
	      return NULL;
	    }
	  r = ushape_array_make (pr, 4);
	  rsize = 0;
	  switch (c->type)
	    {
	    case EQ_CONS:
	      {
		if (nnx != NODE_T_TOP && nny != NODE_T_TOP)
		  {
		    /* both nodes are defined, check their equality */
		    if (nnx == nny)
		      rsize += ushape_array_add (pr, true, r, rsize, true, false, a);	/* copy, not distroy */
		    /* else bottom result, keep NULL */
		  }
		else
		  if ((nx != NODE_T_TOP && nnx == NODE_T_TOP
		       && nny != NODE_T_TOP) || (ny != NODE_T_TOP
						 && nny == NODE_T_TOP
						 && nnx != NODE_T_TOP))
		  {
		    /* fix x->next == y resp. y->next == x */
		    size_t n1, n2;
		    if (nnx == NODE_T_TOP)
		      {
			n1 = nx;
			n2 = ny;
		      }
		    else
		      {
			n1 = ny;
			n2 = nx;
		      }
		    b = ushape_copy_set_succ (pr, a, n1, n2);
		    rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
		  }
		else if ((nx != NODE_T_TOP && ny == NODE_T_TOP) ||
			 (ny != NODE_T_TOP && nx == NODE_T_TOP))
		  {
		    node_t nfixed, nnfixed, nextfixed;
		    node_t nany, nextany, vany;
		    if (nx != NODE_T_TOP)
		      {
			nfixed = nx;
			nnfixed = nnx;
			nextfixed = c->info.ptr.nextx;
			nany = ny;
			nextany = c->info.ptr.nexty;
			vany = vy;
		      }
		    else
		      {
			nfixed = ny;
			nnfixed = nny;
			nextfixed = c->info.ptr.nexty;
			nany = nx;
			nextany = c->info.ptr.nextx;
			vany = vx;
		      }
		    if (nextany == 1)
		      {
			size_t n;
			/* case (1) vany->next = vfixed, new node for vany */
			b = ushape_add_succ_fixed (pr, a, vany, nfixed);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			/* case (2) vany->next = vfixed, existing node for vany */
			hgraph_node_forall (a->h, n)
			{
			  if (hgraph_node_get_succ (a->h, n) == nfixed)
			    {
			      b = ushape_copy_set_var (pr, a, n, vany, 0, 0);
			      rsize += ushape_array_add (pr, rsize, r, true, true, true, b);	/* copy and destroy */
			    }
			  else if (hgraph_node_get_succ (a->h, n) ==
				   NODE_T_TOP)
			    {
			      b =
				ushape_copy_set_info (pr, a, n, nfixed, vany,
						      0);
			      rsize += ushape_array_add (pr, rsize, r, true, true, true, b);	/* copy and destroy */
			    }
			  else
			    continue;
			}
		      }
		    else if (nnfixed != NODE_T_TOP)	/* nextany == 0 */
		      {		/* vfixed[->next] = vany */
			/* set vany to nnfixed */
			b = ushape_copy_set_var (pr, a, nnfixed, vany, 0, 0);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
		      }
		    else
		      {		/* nextany == 0 && nnfixed == NODE_T_TOP */
			/* case (1): vfixed->next = vany, new node for vany */
			node_t n;
			b = ushape_set_new_succ (pr, a, nfixed, vany);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			/* case (2) vfixed->next = vany, existing node for vany */
			hgraph_node_forall (a->h, n)
			{
			  b =
			    ushape_copy_set_info (pr, a, n, nfixed, vany, 0);
			  rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			}
		      }
		  }
		else
		  {		/* nx == NODE_T_TOP && ny == NODE_T_TOP */
		    if (c->info.ptr.nextx + c->info.ptr.nexty == 0)
		      {
			/* case (1): set x and y to new node */
			size_t v1, v2;
			node_t n;
			if (vx <= vy)
			  {
			    v1 = vx;
			    v2 = vy;
			  }
			else
			  {
			    v1 = vy;
			    v2 = vx;
			  }
			b = ushape_add_set_var (pr, a, NODE_T_TOP, v1, v2);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			/* case (2): set x and y to existing nodes */
			hgraph_node_forall (a->h, n)
			{
			  b = ushape_copy_set_var (pr, a, n, v1, n, v2);
			  rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			}
		      }
		    else
		      {
			/*
			 * nx == NODE_T_TOP && ny == NODE_T_TOP && there is a next
			 */
			/* v1->next == v2 */
			size_t v1, v2;
			node_t n, n2;
			hgraph_t *b1;
			if (c->info.ptr.nextx == 1)
			  {
			    v1 = vx;
			    v2 = vy;
			  }
			else
			  {
			    v1 = vy;
			    v2 = vx;
			  }
			/* case (1): v1 == v2 and label a new node */
			b = ushape_add_loop (pr, a, ((v1 <= v2) ? v1 : v2),
					     ((v1 < v2) ? v2 : v1));
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			/* case (2): v1 != v2 and both label new nodes */
			b = ushape_add_edge (pr, a, v1, v2);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			/* case (3): v1 and v2 are existing nodes */
			hgraph_node_forall (a->h, n)
			{
			  if (n != hgraph_node_get_null (a->h))
			    {
			      hgraph_node_forall (a->h, n2)
			      {
				if (n2 == hgraph_node_get_succ (a->h, n))
				  {
				    b =
				      ushape_copy_set_var (pr, a, n, v1, n2,
							   v2);
				    rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
				  }
			      }
			    }
			}
			/* case (4): v1 is new, v2 is an existing node */
			rsize +=
			  ushape_add_succ_fixed_all (pr, a, v1, v2, r, rsize);
			/* case (5): v1 is old with succ undefined, v2 is new */
			ushape_set_new_succ_all (pr, a, v1, v2, r, rsize);
		      }
		  }
		break;
	      }
	    case NE_CONS:
	      {
		node_t i;
		if (nnx != NODE_T_TOP && nny != NODE_T_TOP)
		  {
		    /* check only equality of nodes */
		    if (nnx != nny)
		      rsize +=
			ushape_array_add (pr, true, r, rsize, true, false, a);
		    else if (c->info.ptr.nextx + c->info.ptr.nexty == 1)
		      {		/* v1->next != v2 : create a new node s.t. v1
				 * -->new node --> v2 */
			node_t n1, n2;
			size_t v1, v2;
			if (c->info.ptr.nextx == 1)
			  {
			    n1 = nx;
			    v1 = vx;
			    n2 = ny;
			    v2 = vy;
			  }
			else
			  {
			    n1 = ny;
			    v1 = vy;
			    n2 = nx;
			    v2 = vx;
			  }
			b = ushape_add_between (pr, a, v1, n1, n2);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
		      }
		    /* else bottom result, so keep NULL */
		  }
		else
		  if ((nx != NODE_T_TOP && nnx == NODE_T_TOP
		       && nny != NODE_T_TOP) || (ny != NODE_T_TOP
						 && nny == NODE_T_TOP
						 && nnx != NODE_T_TOP))
		  {		/* v1->next != v2, v1->next undefined, v2
				 * fixed */
		    node_t n1, n2;
		    size_t v1, v2;
		    if (nnx == NODE_T_TOP)
		      {
			n1 = nx;
			v1 = vx;
			n2 = ny;
			v2 = vy;
		      }
		    else
		      {
			n1 = ny;
			v1 = vy;
			n2 = nx;
			v2 = vx;
		      }
		    /* case (1): fix v1->next to some new node, so different from n2 */
		    b = ushape_add_set_info (pr, a, NODE_T_TOP, v1, 1);
		    rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
		    /* case (2): fix v1->next to some existing node, different from n2 */
		    hgraph_node_forall (a->h, i)
		    {
		      if (n2 != hgraph_node_get_succ (a->h, i))
			{
			  b = ushape_copy_set_var (pr, a, i, v1, 0, 0);
			  rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			}
		    }
		  }
		else if ((nx == NODE_T_TOP && nny != NODE_T_TOP) ||
			 (ny == NODE_T_TOP && nnx != NODE_T_TOP))
		  {		/* case v1 != v2[->next] for v1 not yet fixed
				 * and v2[->next] fixed */
		    node_t n1, n2;
		    size_t v1, v2;
		    if (nx == NODE_T_TOP)
		      {
			n1 = nx;
			v1 = vx;
			n2 = nny;
			v2 = vy;
		      }
		    else
		      {
			n1 = ny;
			v1 = vy;
			n2 = nnx;
			v2 = vx;
		      }
		    /* case (1): fix v1 to some new node, so different from n2 */
		    b = ushape_add_set_info (pr, a, NODE_T_TOP, v1, 0);
		    rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
		    /* case (2): fix v1 to some existing node, different from n2 */
		    hgraph_node_forall (a->h, i)
		    {
		      if (n2 != i)
			{
			  b = ushape_copy_set_var (pr, a, i, v1, 0, 0);
			  rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			}
		    }
		  }
		else
		  {		/* TODO: the same cases like for equality,
				 * but more combinatorial explosion. */
		    ap_manager_raise_exception (pr->man,
						AP_EXC_NOT_IMPLEMENTED,
						pr->funid, "not implemented");
		    return NULL;
		  }
		break;
	      }
	    case REACH_CONS:
	    case REACHL_CONS:
	      {
		node_t i;
		/* no next dereference */
		if (c->info.ptr.nextx + c->info.ptr.nexty == 1)
		  {
		    ERROR
		      ("Not a reachability predicate (no next dereferencing)",;
		      );
		    return NULL;
		  }
		if (nx != NODE_T_TOP && ny != NODE_T_TOP)
		  {
		    /* check only reachability of nodes */
		    bool found = hgraph_node_is_reachable (a->h, nx, ny);
		    if (found && c->type == REACH_CONS)
		      rsize +=
			ushape_array_add (pr, true, r, rsize, true, false, a);
		    if (found && c->type == REACHL_CONS)
		      {
			// add constraint on lengths
			ushape_t *b =
			  ushape_alloc_internal (pr, a->datadim, a->ptrdim);
			ap_linexpr0_t *lexpr =
			  ap_linexpr0_alloc (AP_LINEXPR_DENSE,
					     a->datadim + 2 * a->h->size);
			i = nx;
			while (i != ny)
			  {
			    ap_linexpr0_set_coeff_scalar_int (lexpr,
							      a->datadim +
							      a->h->size + i,
							      1);
			    i = hgraph_node_get_succ (a->h, i);
			  }
			ap_linexpr0_set_coeff_scalar_int (lexpr,
							  c->info.ptr.l, -1);
			ap_linexpr0_set_cst_scalar_int (lexpr, 0);
			ap_lincons0_array_t arr = ap_lincons0_array_make (1);
			arr.p[0] = ap_lincons0_make (AP_CONS_EQ, lexpr, NULL);
			for (i = 0; i < pr->size_scons; i++)
			  b->scons[i] =
			    ap_abstract0_meet_lincons_array (pr->man_scons[i],
							     false,
							     a->scons[i],
							     &arr);
			b->h = hgraph_copy_internal (pr, a->h);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
		      }
		    /* else bottom result, keep NULL entry */
		  }
		else if (nx != NODE_T_TOP && ny == NODE_T_TOP)
		  {
		    /* set y to some next successor of nx */
		    /* TODO: the test is more complex because of lasso from x */
		    int j = 0;
		    ap_lincons0_array_t arr = ap_lincons0_array_make (1);
		    ap_linexpr0_t *lexpr =
		      ap_linexpr0_alloc (AP_LINEXPR_DENSE,
					 a->datadim + 2 * a->h->size);
		    arr.p[0] = ap_lincons0_make (AP_CONS_EQ, lexpr, NULL);
		    ap_linexpr0_set_coeff_scalar_int (lexpr, c->info.ptr.l,
						      -1);
		    ap_linexpr0_set_coeff_scalar_int (lexpr,
						      a->datadim +
						      a->h->size + nx, 1);
		    i = NODE_T_TOP;
		    ny = hgraph_node_get_succ (a->h, nx);
		    while (ny != NODE_T_TOP && ny != i && j <= 3)
		      {
			b = ushape_copy_set_var (pr, a, ny, vy, 0, 0);
			if (c->type == REACHL_CONS)
			  for (i = 0; i < pr->size_scons; i++)
			    b->scons[i] =
			      ap_abstract0_meet_lincons_array (pr->
							       man_scons[i],
							       true,
							       b->scons[i],
							       &arr);
			rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			i = ny;
			j++;
			ap_linexpr0_set_coeff_scalar_int (lexpr,
							  a->datadim +
							  a->h->size + i, 1);
			ny = hgraph_node_get_succ (a->h, i);
		      }
		  }
		else if (nx == NODE_T_TOP && ny != NODE_T_TOP)
		  {
		    /* case (1): set x to some new node with the next successor at ny */
		    /* TODO: the case is more complex since several nodes may exist to ny */
		    b = ushape_add_set_info (pr, a, ny, vx, 0);
		    if (c->type == REACHL_CONS)
		      {
			/* add constraint l[nx]=l */
			ap_lincons0_array_t arr = ap_lincons0_array_make (1);
			arr.p[0] =
			  shape_lincons_x_y_v_cst (AP_CONS_EQ, var_ptr_length,
						   1,
						   b->datadim +
						   VAR2NODE (b->h, vx), 0, 0,
						   -1, c->info.ptr.l, 0,
						   b->datadim, b->h->size);
			for (i = 0; i < pr->size_scons; i++)
			  b->scons[i] =
			    ap_abstract0_meet_lincons_array (pr->man_scons[i],
							     true,
							     b->scons[i],
							     &arr);
			ap_lincons0_array_clear (&arr);
		      }
		    rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
		    /* case (2): set x to some existing node predecessor of ny */
		    /* TODO: the case is more complex, we choose only immediate predecessor of ny */
		    hgraph_node_forall (a->h, i)
		    {
		      if (ny == hgraph_node_get_succ (a->h, i))
			{
			  b = ushape_copy_set_var (pr, a, i, vx, 0, 0);
			  if (c->type == REACHL_CONS)
			    {
			      ap_lincons0_array_t arr =
				ap_lincons0_array_make (1);
			      arr.p[0] =
				shape_lincons_x_y_v_cst (AP_CONS_EQ,
							 var_ptr_length, 1,
							 b->datadim +
							 VAR2NODE (b->h, vx),
							 0, 0, -1,
							 c->info.ptr.l, 0,
							 b->datadim,
							 b->h->size);
			      for (i = 0; i < pr->size_scons; i++)
				b->scons[i] =
				  ap_abstract0_meet_lincons_array (pr->
								   man_scons
								   [i], true,
								   b->
								   scons[i],
								   &arr);
			      ap_lincons0_array_clear (&arr);
			    }
			  rsize += ushape_array_add (pr, true, r, rsize, true, true, b);	/* copy and destroy */
			}
		    }
		  }
		else
		  {
		    /* nx and ny are undefined, build nx ---> ny */
		    size_t v1, v2;
		    if (vx <= vy)
		      {
			v1 = vx;
			v2 = vy;
		      }
		    else
		      {
			v1 = vy;
			v2 = vx;
		      }
		    ap_lincons0_array_t arr = ap_lincons0_array_make (1);
		    ap_linexpr0_t *lexpr =
		      ap_linexpr0_alloc (AP_LINEXPR_DENSE,
					 a->datadim + 2 * a->h->size);
		    arr.p[0] = ap_lincons0_make (AP_CONS_EQ, lexpr, NULL);
		    ap_linexpr0_set_coeff_scalar_int (lexpr, c->info.ptr.l,
						      -1);
		    /* case (1): x and y are the same new node */
		    b = ushape_add_loop (pr, a, v1, v2);	/* v1 <= v2 */
		    if (c->type == REACHL_CONS)
		      {
			ap_linexpr0_set_coeff_scalar_int (lexpr,
							  b->datadim +
							  b->h->size +
							  VAR2NODE (b->h, vx),
							  1);
			for (i = 0; i < pr->size_scons; i++)
			  b->scons[i] =
			    ap_abstract0_meet_lincons_array (pr->man_scons[i],
							     true,
							     b->scons[i],
							     &arr);
		      }
		    rsize += ushape_array_add (pr, true, r, rsize, true, false, b);	/* copy and destroy */
		    if (c->type == REACHL_CONS)
		      {
			ap_linexpr0_set_coeff_scalar_int (lexpr,
							  b->datadim +
							  b->h->size +
							  VAR2NODE (b->h, vx),
							  0);
		      }
		    ushape_free_internal (pr, b);
		    /* case (2): x and y are different new nodes */
		    b = ushape_add_edge (pr, a, vx, vy);
		    if (c->type == REACHL_CONS)
		      {
			ap_linexpr0_set_coeff_scalar_int (lexpr,
							  b->datadim +
							  b->h->size +
							  VAR2NODE (b->h, vx),
							  1);
			for (i = 0; i < pr->size_scons; i++)
			  b->scons[i] =
			    ap_abstract0_meet_lincons_array (pr->man_scons[i],
							     true,
							     b->scons[i],
							     &arr);
		      }
		    rsize += ushape_array_add (pr, true, r, rsize, true, false, b);	/* copy and destroy */
		    if (c->type == REACHL_CONS)
		      {
			ap_linexpr0_set_coeff_scalar_int (lexpr,
							  b->datadim +
							  b->h->size +
							  VAR2NODE (b->h, vx),
							  0);
		      }
		    ushape_free_internal (pr, b);
		    /* case (3): x new, y existing */
		    /* TODO: add length constraints */
		    rsize +=
		      ushape_add_succ_fixed_all (pr, a, vx, vy, r, rsize);
		    /* case (4): x existing but no succ, y new */
		    /* TODO: simple case here, no intermediate node between x and y */
		    /* TODO: add length constraints */
		    ushape_set_new_succ_all (pr, a, vx, vy, r, rsize);
		    /* case (5): both x and y are existing nodes */
		    /* TODO: more complex cases with more nodes between x and y */
		    hgraph_node_forall (a->h, nx)
		    {
		      if (nx != hgraph_node_get_null (a->h))
			{
			  hgraph_node_forall (a->h, ny)
			  {
			    if (ny == hgraph_node_get_succ (a->h, nx))
			      {
				b =
				  ushape_copy_set_var (pr, a, nx, vx, ny, vy);
				if (c->type == REACHL_CONS)
				  {
				    ap_linexpr0_set_coeff_scalar_int (lexpr,
								      b->
								      datadim
								      +
								      b->h->
								      size +
								      VAR2NODE
								      (b->h,
								       vx),
								      1);
				    for (i = 0; i < pr->size_scons; i++)
				      b->scons[i] =
					ap_abstract0_meet_lincons_array (pr->
									 man_scons
									 [i],
									 true,
									 b->
									 scons
									 [i],
									 &arr);
				  }
				rsize += ushape_array_add (pr, true, r, rsize, true, false, b);	/* copy and destroy */
				if (c->type == REACHL_CONS)
				  {
				    ap_linexpr0_set_coeff_scalar_int (lexpr,
								      b->
								      datadim
								      +
								      b->h->
								      size +
								      VAR2NODE
								      (b->h,
								       vx),
								      0);
				  }
				ushape_free_internal (pr, b);
			      }
			  }
			}
		    }
		    break;
		  }
	    default:
		break;
	      }
	    }
	}
    }
  if (destructive)
    ushape_free_internal (pr, a);
  if (rsize)
    ushape_array_resize (pr, r, rsize);
  else if (r)
    {
      ushape_array_clear (pr, r, r->size);
      free (r);
      r = NULL;
    }

#ifndef NDEBUG
  fprintf (stdout, "\n\t***returns: ");
  if (!r)
    fprintf (stdout, "null\n");
  else
    {
      size_t i;
      fprintf (stdout, "array of size %zu and elements \n\t", r->size);
      for (i = 0; i < r->size; i++)
	if (r->p[i])
	  ushape_fdump (stdout, pr->man, r->p[i]);
	else
	  fprintf (stdout, "NULL\n");
    }
#endif
  return r;
}

ushape_array_t *
ushape_meet_pcons_array (ushape_internal_t * pr,
			 bool destructive, ushape_t * a,
			 pcons0_array_t * array)
{
  ushape_array_t *r;
  size_t i, j;
  r = ushape_array_make (pr, 4);
  r->p[0] = ushape_copy_internal (pr, a);
  for (i = 0; i < array->size && r; i++)
    {
//TODO:apply the constraints but not iteratively
      ushape_array_t *rc = NULL;
      for (j = 0; j < r->size && r->p[j] != NULL; j++)
	{
	  ushape_array_t *lr =
	    ushape_meet_pcons (pr, false, r->p[j], array->p[i]);
	  if (lr)
	    {
	      rc = ushape_array_add_array (pr, true, rc, lr);	/* reused elements of lr */
	      ushape_array_init (pr, lr, lr->size);
	      free (lr);
	    }
	}
      ushape_array_clear (pr, r, r->size);
      /* if (destructive && rc)
         ushape_array_resize (pr, rc, 4); */
      r = rc;
    }
  if (destructive)
    ushape_free_internal (pr, a);
  return r;
}

ushape_t *
ushape_meet_lincons_array (ap_manager_t * man,
			   bool destructive, ushape_t * a,
			   ap_lincons0_array_t * array)
{
  if (ushape_is_bottom (man, a) || !array || array->size == 0)
    /* nothing to do */
    return (destructive) ? a : ushape_copy (man, a);
  else
    {
      ushape_t *b;
      pcons0_array_t *arr;
      ushape_array_t *r;
      ushape_internal_t *pr =
	ushape_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
      if (!destructive)
	b = ushape_copy_internal (pr, a);
      else
	b = a;
      /* compute in arr the constraints sorted */
      arr =
	shape_pcons_array_of_lincons_array (pr, array, a->datadim, a->ptrdim);
#ifndef NDEBUG
      printf ("\n========== meet with pcons array:\n");
      shape_pcons_array_fdump (stdout, arr, a->datadim, a->ptrdim);
#endif
      /* go */
      /* true below means to compute only one element and return it in b */
      r = ushape_meet_pcons_array (pr, true, b, arr);
      if (r && r->size > 0)
	{
	  b = r->p[0];
	  r->p[0] = NULL;
	  ushape_array_clear (pr, r, r->size);
	}
      else
	b = ushape_bottom (man, a->datadim, a->ptrdim);
      return b;
    }
}

ushape_t *
ushape_meet_tcons_array (ap_manager_t * man,
			 bool destructive, ushape_t * a,
			 ap_tcons0_array_t * array)
{
  if (ushape_is_bottom (man, a) || !array || array->size == 0)
    /* nothing to do */
    return (destructive) ? a : ushape_copy (man, a);
  else
    {
      ushape_t *b;
      pcons0_array_t *arr;
      ushape_array_t *r;
      ushape_internal_t *pr =
	ushape_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
      if (!destructive)
	b = ushape_copy_internal (pr, a);
      else
	b = a;
      /* compute in arr the constraints sorted */
      arr =
	shape_pcons_array_of_tcons_array (pr, array, a->datadim, a->ptrdim);
#ifndef NDEBUG
      printf ("\n========== meet with pcons array:\n");
      shape_pcons_array_fdump (stdout, arr, a->datadim, a->ptrdim);
#endif
      /* go */
      /* true below means to compute only one element and return it in b */
      r = ushape_meet_pcons_array (pr, true, b, arr);
      if (r && r->size > 0)
	{
	  b = r->p[0];
	  r->p[0] = NULL;
	  ushape_array_clear (pr, r, r->size);
	}
      else
	b = ushape_bottom (man, a->datadim, a->ptrdim);
      return b;
    }
}

/* NOT IMPLEMENTED */
ushape_t *
ushape_add_ray_array (ap_manager_t * man,
		      bool destructive, ushape_t * a,
		      ap_generator0_array_t * array)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_ADD_RAY_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* ============================================================ */
/* Widening, Narrowing */
/* ============================================================ */

ushape_t *
ushape_widening (ap_manager_t * man, ushape_t * a1, ushape_t * a2)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_WIDENING, 0);
  arg_assert (a1->datadim == a2->datadim
	      && a1->ptrdim == a2->ptrdim, return NULL;
    );
  // widening is done only between ushape with isomorphic graphs
  if (!hgraph_is_eq (pr->man, a1->h, a2->h))
    return NULL;
  ushape_t *b = ushape_alloc_internal (pr, a1->datadim, a1->ptrdim);
  size_t i;
  bool isbot = false;
  b->h = hgraph_copy_internal (pr, a1->h);
  for (i = 0; i < pr->size_scons && !isbot; i++)
    {
      b->scons[i] =
	ap_abstract0_widening (pr->man_scons[i], a1->scons[i], a2->scons[i]);
      isbot = ap_abstract0_is_bottom (pr->man_scons[i], b->scons[i]);
    }
  if (isbot)
    {
      ushape_free_internal (pr, b);
      b = NULL;
    }
  return b;
}

/* TODO: priority 1 */
ushape_t *
ushape_widening_thresholds (ap_manager_t * man,
			    ushape_t * a1, ushape_t * a2,
			    ap_scalar_t ** array, size_t nb)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_WIDENING, nb + 1);
  arg_assert (a1->datadim == a2->datadim, return NULL;
    );
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a2;
}

/* NOT IMPLEMENTED */
ushape_t *
ushape_narrowing (ap_manager_t * man, ushape_t * a1, ushape_t * a2)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_WIDENING, 0);
  arg_assert (a1->datadim == a2->datadim, return NULL;
    );
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a2;
}
