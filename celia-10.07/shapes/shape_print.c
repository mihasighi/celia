/*
 * shape_print.c
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

#include "shape.h"
#include "shape_internal.h"


/* ============================================================ */
/* Printing */
/* ============================================================ */


inline FILE *
shape_fdump_new_file (FILE * stream, shape_internal_t * pr)
{
  FILE *r = NULL;
  char filename[20];
  memset (filename, 0, 20 * sizeof (char));
  snprintf (filename, 20, "f_%ld.shp", pr->filenum++);
  r = fopen (filename, "w");
  assert (r);
#ifndef NDEBUG1
  fprintf (stream, "<file:%s>", filename);
#endif
  return r;
}

void
shape_fprint (FILE * stream, ap_manager_t * man, shape_t * a,
	      char **name_of_dim)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_FPRINT, 0);
  size_t i;
  FILE *shape_stream = shape_fdump_new_file (stream, pr);
  fprintf (shape_stream, "shape of size (%zu)\n", a->msize);
  for (i = 0; i < a->msize; i++)
    {
      fprintf (shape_stream, "\n***ushape %zd:\n", i);
      ushape_fprint (shape_stream, man, a->m.p[i], name_of_dim);
    }
  fflush (shape_stream);
  fclose (shape_stream);
  fflush (stream);
}

void
shape_fprintdiff (FILE * stream, ap_manager_t * man,
		  shape_t * a1, shape_t * a2, char **name_of_dim)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_FPRINTDIFF, 0);
  fprintf (stream, "shape1 (size %zu) and shape2 (%zu)\n",
	   a1->msize, a2->msize);
  shape_fprint (stream, man, a1, name_of_dim);
  fprintf (stream, "\n<------>\n");
  shape_fprint (stream, man, a2, name_of_dim);
  fflush (stream);
}

void
shape_fdump (FILE * stream, ap_manager_t * man, shape_t * a)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_FDUMP, 0);
  size_t i;
  if (!a)
    fprintf (stream, "NULL");
  else
    {
      FILE *shape_stream = shape_fdump_new_file (stream, pr);
      fprintf (shape_stream, "shape of size (%zu) and dim(%zu,%zu)\n",
	       a->msize, a->intdim, a->realdim);
      for (i = 0; i < a->msize; i++)
	{
	  fprintf (shape_stream, "\n***ushape %zd:\n", i);
	  ushape_fdump (shape_stream, man, a->m.p[i]);
	}
      fflush (shape_stream);
      fclose (shape_stream);
    }
  fflush (stream);
}


/* ============================================================ */
/* Serialization */
/* ============================================================ */

/* TODO: priority 0 */
/* NOT IMPLEMENTED: do nothing */
ap_membuf_t
shape_serialize_raw (ap_manager_t * man, shape_t * a)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_SERIALIZE_RAW, 0);
  ap_membuf_t buf;
  buf.size = 0;
  buf.ptr = NULL;
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return buf;
}

/* TODO: priority 0 */
/* NOT IMPLEMENTED: do nothing */
shape_t *
shape_deserialize_raw (ap_manager_t * man, void *ptr, size_t * size)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_DESERIALIZE_RAW, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return NULL;
}
