/*
 *
 * Pointer constraints of level 0.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include "ap_pcons0.h"


/* To use only when htable of pcons is freed */
void
shape_pcons0_clear (pcons0_t * c)
{
  if (c->type == DATA_CONS)
    ap_lincons0_clear (&c->info.data);
}

void
shape_pcons0_array_clear (pcons0_array_t * array)
{
  size_t i;

  if (array->p != NULL)
    {
      for (i = 0; i < array->size; i++)
	if (array->p[i] != NULL)
	  {
	    //Warning:all pcons0_t values are hashed, so not to be freed !
	    //shape_pcons0_clear(array->p[i]);
	    //free(array->p[i]);
	    array->p[i] = NULL;
	  }
      free (array->p);
      array->p = NULL;
    }
}

void
shape_pcons_fdump (FILE * stream, pcons0_t * c, size_t intdim, size_t realdim)
{
  if (!c)
    fprintf (stream, "[NULL ptr constraint]");
  else
    {
      if (c->type == DATA_CONS)
	ap_lincons0_fprint (stream, &c->info.data, NULL);
      else
	{
	  size_t i;
	  fprintf (stream, "x%u", c->info.ptr.x);
	  for (i = 0; i < c->info.ptr.nextx; i++)
	    fprintf (stream, "->next");
	  switch (c->type)
	    {
	    case EQ_CONS:
	      fprintf (stream, " == ");
	      break;
	    case NE_CONS:
	      fprintf (stream, " != ");
	      break;
	    case REACH_CONS:
	      fprintf (stream, " --> ");
	      break;
	    case REACHL_CONS:
	      fprintf (stream, " -l%u-> ", c->info.ptr.l);
	      break;
	    default:
	      fprintf (stream, " Internal Error ");
	      break;
	    }
	  fprintf (stream, "x%u", c->info.ptr.y);
	  for (i = 0; i < c->info.ptr.nexty; i++)
	    fprintf (stream, "->next");
	}
    }
  fflush (stream);
}

void
shape_pcons_array_fdump (FILE * stream, pcons0_array_t * array, size_t intdim,
			 size_t realdim)
{
  if (!array || array->size == 0 || array->p == NULL)
    fprintf (stream, "[empty array]");
  else
    {
      size_t i;
      fprintf (stream, "[ ");
      for (i = 0; i < array->size; i++)
	{
	  shape_pcons_fdump (stream, array->p[i], intdim, realdim);
	  if (i % 4 == 3)
	    fprintf (stream, ",\n\t");
	  else
	    fprintf (stream, ", ");
	}
      fprintf (stream, "]");
    }
  fflush (stream);

}
