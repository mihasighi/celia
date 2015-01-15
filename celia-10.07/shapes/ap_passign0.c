/*
 * ap_passign0.c
 *
 * Pointer assignments of level 0.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include "ap_passign0.h"
#include "ap_linexpr0.h"


/* To use only when htable of passigns is freed */
void
shape_passign0_clear (passign0_t * c)
{
  if (c->type == DATA_MODIFIER)
    /* TODO: also if assigned vars is data prg var */
    ap_linexpr0_free (c->info.data);
}

void
shape_passign0_array_clear (passign0_array_t * array)
{
  size_t i;

  if (array->p != NULL)
    {
      for (i = 0; i < array->size; i++)
	if (array->p[i] != NULL)
	  {
	    //Warning:all passign0_t values are hashed, so not to be freed !
	    //shape_passign0_clear(array->p[i]);
	    //free(array->p[i]);
	    array->p[i] = NULL;
	  }
      free (array->p);
      array->p = NULL;
    }
}


void
shape_passign_fdump (FILE * stream, passign0_t * a,
		     size_t intdim, size_t realdim)
{
  size_t i;
  if (!a)
    fprintf (stream, "[NULL ptr assignement]");
  else
    {
      fprintf (stream, "x%u", a->x);
      if (a->type == DATA_MODIFIER)
	{
	  if (a->nextx != 1)
	    fprintf (stream, "Internal Error");
	  else
	    fprintf (stream, "->data");
	}
      else if (a->type == NEXT_MODIFIER)
	{
	  if (a->nextx != 1)
	    fprintf (stream, "Internal Error");
	  else
	    fprintf (stream, "->next");
	}
      fprintf (stream, " := ");
      if ((a->type == NO_MODIFIER && a->x < (unsigned int) intdim)
	  || a->type == DATA_MODIFIER)
	ap_linexpr0_fprint (stream, a->info.data, NULL);
      else
	{
	  fprintf (stream, "x%u", a->info.ptr.y);
	  for (i = 0; i < a->info.ptr.nexty; i++)
	    fprintf (stream, "->next");
	}
    }
  fflush (stream);
}

void
shape_passign_array_fdump (FILE * stream, passign0_array_t * array,
			   size_t intdim, size_t realdim)
{
  if (!array || array->size == 0 || array->p == NULL)
    fprintf (stream, "[empty array]");
  else
    {
      size_t i;
      fprintf (stream, "[");
      for (i = 0; i < array->size; i++)
	{
	  shape_passign_fdump (stream, array->p[i], intdim, realdim);
	  if (i % 4 == 3)
	    fprintf (stream, ",\n\t");
	  else
	    fprintf (stream, ", ");

	}
      fprintf (stream, "]");
    }
  fflush (stream);

}
