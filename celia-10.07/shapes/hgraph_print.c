/*
 * hgraph_print.c
 *
 * Printing and serialization
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

#include "hgraph.h"
#include "hgraph_internal.h"
#include "apron2shape.h"


/* ============================================================ */
/* Printing */
/* ============================================================ */

/* Get more informations about the htable */
void
hgraph_fdump_internal (FILE * stream, hgraph_internal_t * pr, hgraph_t * a)
{
  /* print some hash informations */
  fprintf (stream, "\n\tHGraphs table of size %d\n",
	   HASH_CNT (hh, pr->hgraphs));
  hgraph_fdump (stream, pr->man, a);
}

inline void
hgraph_fprint_dot (FILE * stream, ap_manager_t * man,
		   hgraph_t * a, char **name_of_dim, char *name)
{
  size_t i, j;
  /* print graph name */
  fprintf (stream, "digraph %s {\n", name);
  /*
   * print cut nodes (in boxed) with label information, print edges for
   * each node
   */
  for (i = 0; i < a->size; i++)
    {
      fprintf (stream, "n%zu -> n%zu;\n", i, NODE_NEXT (a, i));
      if (NODE_VAR_NEXT (a, i) == 0 && NODE_VAR (a, i) < a->ptrdim)
	{
	  fprintf (stream, "n%zu [shape=box, label=\"", i);
	  /* print all labels */
	  for (j = 0; j < a->ptrdim; j++)
	    if (VAR2NODE (a, j) == i)
	      fprintf (stream, "%s\n", name_of_dim[a->datadim + j]);
	  fprintf (stream, "\"]\n");
	}
    }
  fprintf (stream, "}\n");
}

inline void
hgraph_fprint_reach (FILE * stream, ap_manager_t * man,
		     hgraph_t * a, char **name_of_dim, char *name)
{
  size_t i, j;
  fprintf (stream, "%s:\n", name);
  /* print successor relation */
  for (i = 0; i < a->size; i++)
    fprintf (stream, "n%zu -> n%zu;\n", i, NODE_NEXT (a, i));

  /* print all labels */
  for (i = 0; i < a->size; i++)
    if (NODE_VAR_NEXT (a, i) == 0 && NODE_VAR (a, i) < a->ptrdim)
      {
	fprintf (stream, "n%zu [vars=\"", i);
	for (j = 0; j < a->ptrdim; j++)
	  if (VAR2NODE (a, j) == i)
	    fprintf (stream, "%s,", name_of_dim[a->datadim + j]);
	fprintf (stream, "\"]\n");
      }
}

void
hgraph_fprint (FILE * stream, ap_manager_t * man,
	       hgraph_t * a, char **name_of_dim)
{
  hgraph_internal_t *pr = hgraph_init_from_manager (man, AP_FUNID_FPRINT, 0);
  if (hgraph_is_bottom (man, a))
    fprintf (stream, "bottom");
  else if (hgraph_is_top (man, a))
    fprintf (stream, "top");
  else
    hgraph_fprint_reach (stream, man, a, name_of_dim, "hgraph");
}

void
hgraph_fprintdiff (FILE * stream, ap_manager_t * man,
		   hgraph_t * a1, hgraph_t * a2, char **name_of_dim)
{
  hgraph_internal_t *pr =
    hgraph_init_from_manager (man, AP_FUNID_FPRINTDIFF, 0);
  hgraph_fprint (stream, man, a1, name_of_dim);
  hgraph_fprint (stream, man, a2, name_of_dim);
}

inline void
hgraph_fdump_info (FILE * stream, int mode, node_info_t a[], size_t size,
		   size_t datadim, size_t ptrdim)
{
  size_t i;
  switch (mode)
    {
    case 0:
      for (i = 0; i < size; i++)
	fprintf (stream, "\tn%zu -> n%zu\n", i, a[ptrdim + i].s);
      break;
    case 1:
      for (i = 0; i < size; i++)
	fprintf (stream, "\tx%zu ->next^%zu-> n%zu\n",
		 a[ptrdim + i].v + datadim, a[ptrdim + i].nn, i);
      break;
    default:
      /* nothing */
      break;
    }
}

inline void
hgraph_fdump_vars (FILE * stream, node_info_t a[], size_t datadim,
		   size_t ptrdim)
{
  size_t i;
  for (i = var_ptr_null; i < ptrdim; i++)
    fprintf (stream, " x%zu(n%zu), ", i + datadim, a[i].s);
  fprintf (stream, "\n");
}

void
hgraph_fdump (FILE * stream, ap_manager_t * man, hgraph_t * a)
{
  unsigned int count;
  hgraph_internal_t *pr = hgraph_init_from_manager (man, AP_FUNID_FDUMP, 0);
  /* print graph */
  if (!a)
    {
      fprintf (stream, "bottom");
      return;
    }
  if (hgraph_is_bottom (man, a))
    fprintf (stream, "bottom, i.e.,");
  else if (hgraph_is_top (man, a))
    fprintf (stream, "top, i.e.,");

  fprintf (stream, "hgraph of size %zu, ptrdim %zu, datadim %zu, %sclosed\n",
	   a->size, a->ptrdim, a->datadim, (a->closed) ? " " : "not ");
  if (a->info)
    {
      fprintf (stream, "succ matrix:\n");
      hgraph_fdump_info (stream, 0, a->info, a->size, a->datadim, a->ptrdim);

      fprintf (stream, "ptr vars labels for nodes:\n");
      hgraph_fdump_info (stream, 1, a->info, a->size, a->datadim, a->ptrdim);

      fprintf (stream, "ptr var to node: ");
      hgraph_fdump_vars (stream, a->info, a->datadim, a->ptrdim);
    }
  else
    fprintf (stream, "info matrix: NULL\n");
}

void
hgraph_array_fdump (FILE * stream, ap_manager_t * man, hgraph_array_t * a)
{
  if (!a)
    fprintf (stream, "EMPTY ARRAY");
  else
    {
      size_t i;
      fprintf (stream, "[");
      for (i = 0; i < a->size; i++)
	hgraph_fdump (stream, man, a->p[i]);
      fprintf (stream, "\n]\n");
    }
}

/* ============================================================ */
/* Serialization */
/* ============================================================ */

/* TODO: priority 0 */
/* NOT IMPLEMENTED: do nothing */
ap_membuf_t
hgraph_serialize_raw (ap_manager_t * man, hgraph_t * a)
{
  hgraph_internal_t *pr =
    hgraph_init_from_manager (man, AP_FUNID_SERIALIZE_RAW, 0);
  ap_membuf_t buf;
  buf.size = 0;
  buf.ptr = NULL;
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return buf;
}

/* TODO: priority 0 */
/* NOT IMPLEMENTED: do nothing */
hgraph_t *
hgraph_deserialize_raw (ap_manager_t * man, void *ptr, size_t * size)
{
  hgraph_internal_t *pr =
    hgraph_init_from_manager (man, AP_FUNID_DESERIALIZE_RAW, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return NULL;
}
