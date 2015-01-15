/*
 * ushape_print.c
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
#include "ushape.h"
#include "ushape_internal.h"


/* ============================================================ */
/* Printing */
/* ============================================================ */

void
ushape_fprint (FILE * stream, ap_manager_t * man,
	       ushape_t * a, char **name_of_dim)
{
  ushape_internal_t *pr = ushape_init_from_manager (man, AP_FUNID_FPRINT, 0);
  size_t i;
  fprintf (stream, "\t[hgraph:\n");
  if (a->h)
    hgraph_fprint (stream, man, a->h, name_of_dim);
  fprintf (stream, "\n\t]\n");
  fprintf (stream, "\t{segm constraints:\n");
  if (a->scons)
    {
      for (i = 0; i < pr->size_scons; i++)
	ap_abstract0_fprint (stream, pr->man_scons[i], a->scons[i],
			     name_of_dim);
    }
  else
    fprintf (stream, "\t[NULL]\n");
  fprintf (stream, "\t}\n");
}

void
ushape_fprintdiff (FILE * stream, ap_manager_t * man,
		   ushape_t * a1, ushape_t * a2, char **name_of_dim)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_FPRINTDIFF, 0);
  size_t i;
  hgraph_fprintdiff (stream, man, (a1) ? a1->h : NULL, (a2) ? a2->h : NULL,
		     name_of_dim);
  for (i = 0; i < pr->size_scons; i++)
    {
      fprintf (stream, "\t[univ constraint:\n");
      ap_abstract0_fprintdiff (stream, pr->man_scons[i],
			       (a1 && a1->scons) ? a1->scons[i] : NULL, (a2
									 &&
									 a2->
									 scons)
			       ? a2->scons[i] : NULL, name_of_dim);
      fprintf (stream, "\n\t]\n");
    }
}

void
ushape_fdump (FILE * stream, ap_manager_t * man, ushape_t * a)
{
  ushape_internal_t *pr = ushape_init_from_manager (man, AP_FUNID_FDUMP, 0);
  size_t i;
  if (!a)
    fprintf (stream, "\nushape NULL\n");
  else
    {
      fprintf (stream, "\nushape of dim (%zu,%zu)", a->datadim, a->ptrdim);
      fflush (stream);
      hgraph_fdump_internal (stream, pr, a->h);
      fflush (stream);
      fprintf (stream, "\nclosed ");
      hgraph_fdump (stream, man, a->closed);
      fflush (stream);
      fprintf (stream, "\n{segment constraints:\n");
      if (a->scons)
	{
	  for (i = 0; i < pr->size_scons; i++)
	    if (a->scons[i])
	      ap_abstract0_fdump (stream, pr->man_scons[i], a->scons[i]);
	    else
	      fprintf (stream, "[NULL]");
	}
      else
	fprintf (stream, "\t[NULL]\n");
      fprintf (stream, "}\n");
    }
}


/* ============================================================ */
/* Serialization */
/* ============================================================ */

/* NOT IMPLEMENTED: do nothing */
ap_membuf_t
ushape_serialize_raw (ap_manager_t * man, ushape_t * a)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_SERIALIZE_RAW, 0);
  ap_membuf_t buf;
  buf.size = 0;
  buf.ptr = NULL;
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return buf;
}

/* NOT IMPLEMENTED: do nothing */
ushape_t *
ushape_deserialize_raw (ap_manager_t * man, void *ptr, size_t * size)
{
  ushape_internal_t *pr =
    ushape_init_from_manager (man, AP_FUNID_DESERIALIZE_RAW, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return NULL;
}
