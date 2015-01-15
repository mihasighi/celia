/*
 * ushape_resize.c
 *
 * Projection, changes of dimension, variable permutation.
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

#include "ushape.h"
#include "ushape_internal.h"


/* ============================================================ */
/* Projections */
/* ============================================================ */

/* TODO: priority 0 */
/*
 * not used because we suppose that all ptr variables are declared from the
 * beginning
 */
ushape_t *
ushape_forget_array (ap_manager_t * man,
		     bool destructive, ushape_t * a,
		     ap_dim_t * tdim, size_t size, bool project)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_FORGET_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}


/* ============================================================ */
/* Change and permutation of dimensions */
/* ============================================================ */

/* TODO: priority 0 */
ushape_t *
ushape_add_dimensions (ap_manager_t * man,
		       bool destructive, ushape_t * a,
		       ap_dimchange_t * dimchange, bool project)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
ushape_t *
ushape_remove_dimensions (ap_manager_t * man,
			  bool destructive, ushape_t * a,
			  ap_dimchange_t * dimchange)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_REMOVE_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
ushape_t *
ushape_permute_dimensions (ap_manager_t * man,
			   bool destructive, ushape_t * a,
			   ap_dimperm_t * permutation)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* Apply permutation of nodes, i.e., perm->size == a->h->size */
void
ushape_apply_dimperm (ushape_internal_t * pr, ushape_t * a, ushape_t * b,
		      ap_dimperm_t * perm)
{
  size_t i;
  assert (a && a->h && (perm->size >= a->h->size));
  ap_dimperm_t dimperm;
  ap_dimperm_init (&dimperm, a->datadim + perm->size);
  ap_dimperm_set_id (&dimperm);
  shape_dimperm_copy (&dimperm, a->datadim, perm);
  for (i = 0; i < pr->size_scons; i++)
    b->scons[i] =
      ap_abstract0_permute_dimensions (pr->man_scons[i], (a == b),
				       a->scons[i], &dimperm);
  ap_dimperm_clear (&dimperm);
  ap_dimperm_clear (perm);
}

/* Add dimension and map it to n, then apply permutation of nodes,
 * i.e., perm->size == a->h->size */
void
ushape_apply_dimperm_dimchange (ushape_internal_t * pr, ushape_t * a,
				ushape_t * b, ap_dimperm_t * perm, size_t n)
{
  size_t i;
  ap_dimperm_t dimperm;
  ap_dimchange_t dimchange;
  assert (a && a->h && (perm->size == a->h->size));
  ap_dimchange_init (&dimchange, 0, 1);
  dimchange.dim[0] = a->datadim + a->h->size;
  ap_dimperm_init (&dimperm, a->datadim + a->h->size + 1);
  ap_dimperm_set_id (&dimperm);
  shape_dimperm_copy (&dimperm, a->datadim, perm);
  ap_dimperm_clear (perm);
  dimperm.dim[a->h->size] = n;
  for (i = 0; i < pr->size_scons; i++)
    {
#ifndef NDEBUG
      fprintf (stdout, "\n==== dimperm_dimchange input \n");
      ap_abstract0_fprint (stdout, pr->man_scons[i], a->scons[i], NULL);
#endif
      b->scons[i] =
	ap_abstract0_add_dimensions (pr->man_scons[i],
				     false, a->scons[i], &dimchange, false);
#ifndef NDEBUG
      fprintf (stdout, "\n=== after dimchange ");
      ap_abstract0_fprint (stdout, pr->man_scons[i], b->scons[i], NULL);
#endif
      b->scons[i] =
	ap_abstract0_permute_dimensions (pr->man_scons[i],
					 true, b->scons[i], &dimperm);
#ifndef NDEBUG
      fprintf (stdout, "\n=== after dimchangperm ");
      ap_abstract0_fprint (stdout, pr->man_scons[i], b->scons[i], NULL);
#endif
    }
  ap_dimperm_clear (&dimperm);
  ap_dimchange_clear (&dimchange);
}


/* ============================================================ */
/* Expansion and folding of dimensions */
/* ============================================================ */

/* TODO: priority 0 */
ushape_t *
ushape_expand (ap_manager_t * man,
	       bool destructive, ushape_t * a, ap_dim_t dim, size_t n)
{
  ushape_internal_t *pr = ushape_init_from_manager (man, AP_FUNID_EXPAND, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

void
ushape_expand_internal (ushape_internal_t * pr, ushape_t * a, ushape_t * b,
			ap_dim_t dim, size_t n)
{
  if (!a || !b || !b->scons || !dim || !n)
    return;
  size_t i;
  for (i = 0; i < pr->size_scons; i++)
    b->scons[i] = ap_abstract0_expand (pr->man_scons[i], false, a->scons[i],
				       dim, n);
}

ushape_t *
ushape_fold (ap_manager_t * man,
	     bool destructive, ushape_t * a, ap_dim_t * tdim, size_t size)
{
  if (!a)
    return NULL;
  ushape_internal_t *pr = ushape_init_from_manager (man, AP_FUNID_FOLD, 0);
  if (!size)
    return (destructive) ? a : ushape_copy_internal (pr, a);
  size_t i;
  ushape_t *b = ushape_alloc_internal (pr, a->datadim, a->ptrdim);
  b->h = hgraph_copy_internal (pr, a->h);
  b->closed = hgraph_copy_internal (pr, a->closed);
  for (i = 0; i < pr->size_scons; i++)
    b->scons[i] =
      ap_abstract0_fold (pr->man_scons[i], false, a->scons[i], tdim, size);
  if (destructive)
    ushape_free_internal (pr, a);
  return b;
}


void
ushape_fold_internal (ushape_internal_t * pr,
		      ushape_t * a, ushape_t * b, ap_dim_array2_t * anon)
{
  if (!a || !b || !b->scons || !anon->size)
    return;
  size_t i, s, changed;
  ap_dimperm_t perm;
  ap_dim_array2_t fanon;	// array for folded dimensions
#ifndef NDEBUG
  fprintf (stdout, "\n\tushape_fold_internal: with anon [");
  for (i = 0; i < anon->size; i++)
    fprintf (stdout, "%zu(%d),", anon->p[i].size, anon->p[i].p[0]);
  fprintf (stdout, "]\n");
#endif
  // prepare the array of folded dimension to pass the ap_abstract0_fold, i.e.,
  // (1) all dimensions shall be real xor integer
  // (2) all dimensions shall be sorted
  // GO
  fanon.size = anon->size;
  fanon.p = (ap_dim_array_t *) malloc (fanon.size * sizeof (ap_dim_array_t));
  // (1) add a->datadim to all anon in order to obtain only real dimensions
  for (s = 0; s < anon->size; s++)
    {
      fanon.p[s].size = anon->p[s].size;
      fanon.p[s].p =
	(ap_dim_t *) malloc (anon->p[s].size * sizeof (ap_dim_t));
      for (i = 0; i < anon->p[s].size; i++)
	fanon.p[s].p[i] = anon->p[s].p[i] + a->datadim;
    }
  // (2) sort each entry of fanon and generate a permutation of dimensions folded
  ap_dimperm_init (&perm, a->datadim + a->h->size);
  ap_dimperm_set_id (&perm);
  changed = 0;
  for (s = 0; s < fanon.size; s++)
    {				// insert sort entry s
      for (i = 1; i < fanon.p[s].size; i++)
	{
	  ap_dim_t m, n;
	  size_t j = 0;
	  while (j < i && fanon.p[s].p[j] <= fanon.p[s].p[i])
	    j++;
	  m = fanon.p[s].p[i];
	  while (j < i)
	    {
	      n = fanon.p[s].p[j];
	      fanon.p[s].p[j] = m;
	      m = n;
	      j++;
	    }
	  fanon.p[s].p[i] = m;
	}
      // generate permutation: note that anon->p[s] are pairwise disjoint
      for (i = 0; i < anon->p[s].size; i++)
	if (fanon.p[s].p[i] != (a->datadim + anon->p[s].p[i]))
	  {
	    perm.dim[a->datadim + anon->p[s].p[i]] = fanon.p[s].p[i];
	    changed = 1;
	  }
    }
  // apply permutation before folding
  if (changed)
    {
      for (i = 0; i < pr->size_scons; i++)
	b->scons[i] =
	  ap_abstract0_permute_dimensions (pr->man_scons[i], (a == b),
					   a->scons[i], &perm);
#ifndef NDEBUG
      fprintf (stdout, "\n\tushape_fold_internal: permutation [");
      ap_dimperm_fprint (stdout, &perm);
      fprintf (stdout, "]\n");
#endif
    }
  // apply folding
  for (i = 0; i < pr->size_scons; i++)
    {
      b->scons[i] =
	ap_abstract0_fold (pr->man_scons[i], ((changed) ? true : (a == b)),
			   a->scons[i], (ap_dim_t *) fanon.p[0].p,
			   fanon.p[0].size);
      for (s = 1; s < anon->size; s++)
	b->scons[i] = ap_abstract0_fold (pr->man_scons[i], true, b->scons[i],
					 (ap_dim_t *) fanon.p[s].p,
					 fanon.p[s].size);
      // apply fold with 0 anonymous to mark end of folding
      b->scons[i] = ap_abstract0_fold (pr->man_scons[i], true, b->scons[i],
                                       (ap_dim_t *) fanon.p[0].p,
                                       1);
    }
  // apply the reverse permutation to obtain the final result
  if (changed)
    {
      ap_dimperm_t nperm;
      ap_dimperm_init (&nperm, perm.size);
      ap_dimperm_invert (&nperm, &perm);
#ifndef NDEBUG

      fprintf (stdout, "\n\tushape_fold_internal: after folding ");
      ushape_fdump (stdout, pr->man, b);
      fprintf (stdout, "\n\tushape_fold_internal: reverse permutation [");
      ap_dimperm_fprint (stdout, &nperm);
      fprintf (stdout, "]\n");
#endif
      for (i = 0; i < pr->size_scons; i++)
	b->scons[i] =
	  ap_abstract0_permute_dimensions (pr->man_scons[i], true,
					   a->scons[i], &nperm);
      ap_dimperm_clear (&nperm);
#ifndef NDEBUG
      fprintf (stdout, "\n\tushape_fold_internal: after permute ");
      ushape_fdump (stdout, pr->man, b);
      fprintf (stdout, "\n");
#endif
    }
  ap_dimperm_clear (&perm);
  // free all anon
  hgraph_node_anon_array_clear (pr, &fanon);
  hgraph_node_anon_array_clear (pr, anon);
}
