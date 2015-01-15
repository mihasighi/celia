/*
 * shape_representation.c
 *
 * Functions related to the representation of shapes.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include <stdio.h>
#include "shape_options.h"
#include "ushape_internal.h"
#include "shape.h"
#include "shape_internal.h"
#include "shape_macros.h"
#include "ap_generic.h"
#include "box.h"
#include "oct.h"
#include "pk.h"
#include "ucons.h"
#include "lsum.h"
#include "mset.h"

/* ********************************************************************** */
/* Shapes */
/* ********************************************************************** */

/* ============================================================ */
/* Internal management */
/* ============================================================ */

/*
 * generic allocation routine, returns a shape with msize at 0 but size
 * allocated NULL ushapes
 */
  inline shape_t *
					  size_t size)
{



    );










/* returns a singleton set with a top ushape */
  inline shape_t *
				     size_t ptrdim)
{







    {







			 size_t ptrdim)
{







    /* TODO: generate more than 1 element */
}



{







    /* TODO: more than 1 element */
}

shape_free_internal (shape_internal_t * pr, shape_t * a)
{

    return;






/* TODO */
char
shape_check (shape_internal_t * pr, shape_t * a)
{




{



    );


    {






  else

    {












/* ============================================================ */
/* Memory */
/* ============================================================ */

{




shape_free (ap_manager_t * man, shape_t * a)
{







{





/* ============================================================ */
/* Control of internal representation */
/* ============================================================ */

/*
 * Return the set of minimal elements TODO: better complexity if an order of
 * ushapes is fixed
 */
void
shape_minimize (ap_manager_t * man, shape_t * a)
{

    shape_init_from_manager (man, AP_FUNID_MINIMIZE, 0);








      {


	       &&ushape_is_leq (pr->man, a->m.p[i], a->m.p[j]))



	  {			/* i is in the final set */










/* Return the set of canonical elements */
void
shape_canonicalize (ap_manager_t * man, shape_t * a)
{







      {














/* TODO: priority 0 */
int
shape_hash (ap_manager_t * man, shape_t * a)
{







/* NOT IMPLEMENTED: do nothing */
void
shape_approximate (ap_manager_t * man, shape_t * a, int algorithm)
{






/* TODO: better complexity if sorted set */
  bool
{







	     &&ushape_is_leq (pr->man, a->m.p[i], a->m.p[j]))





{










/* ============================================================ */
/* Basic Constructors */
/* ============================================================ */

			    size_t realdim)
{






    {







{






/* put constraints on data variables */
  shape_t *

{




    {



	{




      else

	{








/* ============================================================ */
/* Accessors */
/* ============================================================ */

{

    shape_init_from_manager (man, AP_FUNID_DIMENSION, 0);





    {




  else

    {







/* ============================================================ */
/* Managers */
/* ============================================================ */

shape_internal_free (shape_internal_t * pr)
{

    /* TODO: free htable */
    pr->hgraphs = NULL;



    /* TODO: free segment managers */
    pr->man_scons = NULL;


{










    /* input parameters */
    pr->intdim = 0;


    /* read parameters from input file */
    FILE * f = fopen ("./cinv.txt", "r");


    {




  else

    {





		 pr->segm_anon);

/*
  switch (SHAPE_DCONS_DOMAIN)
    {
    case DOM_BOX:
      pr->man_dcons = box_manager_alloc ();
      break;
    case DOM_OCT:
      pr->man_dcons = oct_manager_alloc ();
      break;
    default:
      pr->man_dcons = pk_manager_alloc (false);
      break;
    }
*/
    i = 0;










    (i == 0) ? NULL : (ap_manager_t **) malloc (i * sizeof (ap_manager_t *));




    /*
       if (SHAPE_SCONS_LEN)
       {
       switch (SHAPE_SCONS_LEN_DOMAIN)
       {
       case DOM_BOX:
       pr->man_scons[i] = box_manager_alloc ();
       break;
       case DOM_OCT:
       pr->man_scons[i] = oct_manager_alloc ();
       break;
       default:
       pr->man_scons[i] = pk_manager_alloc (false);
       break;
       }
       i++;
       }
     */
    if (SHAPE_SCONS_LEN)

    {
      pr->man_scons[i++] = oct_manager_alloc ();

  if (SHAPE_SCONS_UCONS)

    {
      pr->man_scons[i++] = ucons_manager_alloc ();

  if (SHAPE_SCONS_LSUM)

    {
      pr->man_scons[i] = lsum_manager_alloc ();

  if (SHAPE_SCONS_MSET)

    {
      pr->man_scons[i] = mset_manager_alloc ();

}

  memset (managers, 0, 40 * sizeof (char));
  sprintf (managers, "0.1 with %s, %s, %s, %s",
	   (SHAPE_SCONS_LEN) ? "LEN" : "-",
	   (SHAPE_SCONS_UCONS) ? "UCONS" : "-",
	   (SHAPE_SCONS_LSUM) ? "LSUM" : "-",
	           (SHAPE_SCONS_MSET) ? "MSET" : "-");
  man = ap_manager_alloc ("shape", managers, pr,
			  (void (*)(void *)) shape_internal_free);




























































{




{







