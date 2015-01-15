/*
 * ucons_print.c
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

#include "ucons.h"
#include "ucons_fun.h"
#include "ucons_internal.h"
#include "apron2shape.h"

/* ============================================================ */
/* Printing */
/* ============================================================ */

void
pattern_key_fprint (FILE * stream, ucons_internal_t *pr, pattern_key_t * a,
              char **name_of_dim)
{
  size_t i,j;
  size_t u_seg;
  if (!a)
    fprintf(stream, "(null)");
  else {
    // print using the form of the pattern
    switch(pr->PI[a->type].kind)
    {
      case pattern_1:
        fprintf(stream, "y1 in [%s], l[%s]>=2 ", name_of_dim[a->segments[0]], name_of_dim[a->segments[0]]);
        break;
      case pattern_1_2:
        fprintf(stream, "y1, y2 in [%s], y1 < y2 /\\ l[%s]>=3 ", name_of_dim[a->segments[0]], name_of_dim[a->segments[0]]);
        break;
      case pattern_2_1:
        fprintf(stream, "y1 in [%s], y2 in [%s], y1==y2 /\\ l[%s]>=2 /\\ l[%s]>=2 ",
            name_of_dim[a->segments[0]], name_of_dim[a->segments[1]],
            name_of_dim[a->segments[0]], name_of_dim[a->segments[1]]);
        break;
      case pattern_3_1:
        fprintf(stream, "y1 in [%s], y2 in [%s], y3 in [%s], y1==y2==y3 /\\ l[%s]>=2 /\\ l[%s]>=2 /\\ l[%s]>=2 ",
            name_of_dim[a->segments[0]], name_of_dim[a->segments[1]], name_of_dim[a->segments[2]],
            name_of_dim[a->segments[0]], name_of_dim[a->segments[1]], name_of_dim[a->segments[2]]);
        break;
      default:
        fprintf(stream, "(pattern unknown)");
        break;
    }
    // TODO: use the general king of printing below
    /*
    u_seg=pr->PI[a->type].u_seg;
    for (i=0; i<u_seg; i++) {
      for(j=0;j<pr->PI[a->type].uvar[i].size;j++)
        fprintf(stream, "y%zu_%zu",i,j);
      fprintf(stream, "in [%s]",name_of_dim[a->segments[i]]);
    }
    for (i=0; i<u_seg; i++) {
      if (pr->PI[a->type].uvar[i].size>1) fprintf(stream, "y%zu_0",i);
      for(size_t j=1;j<pr->PI[a->type].uvar[i].size;j++)
        if(pr->PI[a->type].uvar[i].reach[j-1])
          fprintf(stream, "--+--> y%zu_%zu",i,j);
        else fprintf(stream, "--1--> y%zu_%zu",i,j);

    }*/
  }
  fflush(stream);
}


void
ucons_fprint (FILE * stream, ap_manager_t * man, ucons_t * a,
    char **name_of_dim)
{
  ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_FPRINT, 0);
  size_t i;
  if (shape_name_of_dim == NULL)
    shape_init_name_of_dim (a->datadim);
  if (name_of_dim == NULL)
    name_of_dim = shape_name_of_dim;
  if (!a)
    fprintf (stream, "ucons EMPTY\n");
  else
    {
      char **dname;
      size_t i, size;
      fprintf (stream, "ucons of [datadim=%zu, ptrdim=%zu] \n", a->datadim, a->segmentdim);
      // prepare names for the domain
      size = (a->datadim + 2 * a->segmentdim + 2*3); /* TODO: max 3 universally quantified vars */
      dname = (char **) malloc (size * sizeof (char *));
      for (i = 0; i < a->datadim; i++)
        dname[i] = (name_of_dim) ? name_of_dim[i] : shape_name_of_dim[i];
      for (i = 0; i < a->segmentdim; i++)
        {
          size_t lsize = (4 + strlen (name_of_dim[a->datadim + i]));
          dname[a->datadim + i] =
            (char *) malloc (lsize * sizeof (char));
          snprintf (dname[a->datadim + i], lsize, "d(%s)",
                    name_of_dim[a->datadim + i]);
          dname[a->datadim + a->segmentdim + i] =
            (char *) malloc (lsize * sizeof (char));
          snprintf (dname[a->datadim + a->segmentdim + i],
                    lsize, "l[%s]", name_of_dim[a->datadim + i]);
        }
      for (i = 0; i < 6; i++) // TODO: max 6
        dname[a->datadim + 2*a->segmentdim+i] = (char *) malloc (8 * sizeof (char));
      // print the existential constraints
      if (a->econs)
        ap_abstract0_fprint(stream, pr->man_dcons, a->econs,dname);
      else
        fprintf(stream, "true");
      // print the universal constraints
      pattern_t * r =  a->udcons;
      while(r!=NULL) {
        fprintf(stream, " /\\ ");
        // print PR ==> dcons
        pattern_key_fprint(stream, pr, &r->key, &name_of_dim[a->datadim]);
        fprintf(stream, " ==> ");
        if (r->dcons)
          {
            // prepare dname for this pattern
            for (i=0;i<pr->PI[r->key.type].nr_y && i < 3;i++) // TODO: max 3
              {
                snprintf (dname[a->datadim + 2*a->segmentdim + i],
                    8, "y%zu", (i+1));
                snprintf (dname[a->datadim + 2*a->segmentdim + pr->PI[r->key.type].nr_y+i],
                    8, "d(y%zu)", (i+1));
              }
            // print with the names
            ap_abstract0_fprint(stream, pr->man_dcons, r->dcons, dname);
          }
        else
          fprintf(stream, "true");
        r=r->hh.next;
      }

 //     if(!a->n2p)
 //       fprintf (stream, "\n n2p EMPTY\n");
 //     else
 //       for(i=0;i<a->segmentdim;i++){
 //         size_t j;
 //         fprintf(stream,"\n %zu: ",i);
 //         for(j=0;j<a->n2p[i].size;j++)
 //           pattern_key_fprint (stream, pr, a->n2p[i].p[j], &name_of_dim[a->datadim]);
  //        fprintf(stream,"\n");
  //      }
    }
}

void
ucons_fprintdiff (FILE * stream, ap_manager_t * man,
                  ucons_t * a1, ucons_t * a2, char **name_of_dim)
{
  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_FPRINTDIFF, 0);
  fprintf (stream, "ucons1 (size %zu) and ucons2 (size %zu)\n",
           a1->size,a2->size);
  /* TODO: priority 1 */
}

void
ucons_fdump (FILE * stream, ap_manager_t * man, ucons_t * a)
{
  if (shape_name_of_dim == NULL)
    shape_init_name_of_dim (a->datadim);
  ucons_fprint(stream, man, a, shape_name_of_dim);
}


/* ============================================================ */
/* Serialization */
/* ============================================================ */

/* TODO: priority 0 */
/* NOT IMPLEMENTED: do nothing */
ap_membuf_t
ucons_serialize_raw (ap_manager_t * man, ucons_t * a)
{
  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_SERIALIZE_RAW, 0);
  ap_membuf_t buf;
  buf.size = 0;
  buf.ptr = NULL;
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return buf;
}

/* Builds a value from a code!
 * The ptr is an array of 3 size_t [datadim, segmdim, codeop]
 */
ucons_t *
ucons_deserialize_raw (ap_manager_t * man, void *ptr, size_t * size)
{
  ucons_internal_t *pr =
    ucons_init_from_manager (man, AP_FUNID_DESERIALIZE_RAW, 0);


    size_t *ucons_raw = (size_t *) ptr;
    size_t datadim = ucons_raw[0];
    size_t segmdim = ucons_raw[1];
    size_t code = ucons_raw[2];
    ap_dim_t l = 0;
    size_t nodex = 1;
    size_t nodey = 2;
    size_t nodez = 3;
    ucons_t *r = ucons_top (man, datadim, segmdim);

    /* common constraints l[x]==_l and _l>=1 */
    if (code != 1)
      {
        assert (segmdim >= 2 && datadim >= 1);
        ap_lincons0_array_t arr = ap_lincons0_array_make (2);
        arr.p[0] =
          shape_lincons_x_y_v_cst (AP_CONS_EQ, var_ptr_length, 1,
                                   datadim + nodex, 0, 0, -1, l, 0, datadim,
                                   segmdim);
        arr.p[1] =
          shape_lincons_x_y_v_cst (AP_CONS_SUPEQ, var_ptr_length, 1,
                                   datadim + nodex, 0, 0, 0, 0, -1, datadim,
                                   segmdim);
        r = ucons_meet_lincons_array (man, true, r, &arr);
	ap_lincons0_array_clear (&arr);
       /*to remove : only for the copy_ab test */
   	/***********************/
	//ap_dim_t * tdim = (ap_dim_t*) malloc (sizeof(ap_dim_t));
        //tdim[0]=4;
	////tdim[1]=5;
	//r = ucons_forget_array(man, true, r, tdim, 1, false);
        /***********************/
      }
    /* common constraint l[x]==l[y] and l[y]>=1 */
    if (code == 2 || code == 4)
      {
      assert (segmdim >= 3 && datadim >= 1);
      ap_lincons0_array_t arr = ap_lincons0_array_make (2);
      arr.p[0] =
        shape_lincons_x_y_v_cst (AP_CONS_EQ, var_ptr_length, 1,
                                 datadim + nodex, -1, datadim + nodey, 0, 0, 0, datadim,
                                 segmdim);
      arr.p[1] =
        shape_lincons_x_y_v_cst (AP_CONS_SUPEQ, var_ptr_length, 1,
                                 datadim + nodey, 0, 0, 0, 0, -1, datadim,
                                 segmdim);
      r = ucons_meet_lincons_array (man, true, r, &arr);
      ap_lincons0_array_clear (&arr);
    }
    /* specific constraints */
    switch (code)
    {
    case 1:                     /* l[x]+l[y]==_l and l[x]>=1 and l[y]>=1 */
      {
        assert (segmdim >= 2 && datadim >= 1);
        ap_lincons0_array_t arr = ap_lincons0_array_make (3);
        arr.p[0] =
          shape_lincons_x_y_v_cst (AP_CONS_EQ, var_ptr_length, 1,
              datadim + nodex, 1, datadim + nodey, -1, l,
              0, datadim, segmdim);
        arr.p[1] =
          shape_lincons_x_y_v_cst (AP_CONS_SUPEQ, var_ptr_length, 1,
              datadim + nodex, 0, 0, 0, 0, -1, datadim,
              segmdim);
        arr.p[2] =
          shape_lincons_x_y_v_cst (AP_CONS_SUPEQ, var_ptr_length, 1,
              datadim + nodey, 0, 0, 0, 0, -1, datadim,
              segmdim);
        r = ucons_meet_lincons_array (man, true, r, &arr);
        ap_lincons0_array_clear (&arr);
        break;
      }
      case 3:                     /* non-equal length lists: l[y]+1<=_l and l[y]>=1 */
        {
          assert (segmdim >= 3 && datadim >= 1);
          ap_lincons0_array_t arr = ap_lincons0_array_make (2);
          arr.p[0] =
            shape_lincons_x_y_v_cst (AP_CONS_SUPEQ, var_ptr_length, -1,
                                     datadim + nodey, 0, 0, 1, l, -1, datadim,
                                     segmdim);
          arr.p[1] =
            shape_lincons_x_y_v_cst (AP_CONS_SUPEQ, var_ptr_length, 1,
                                     datadim + nodey, 0, 0, 0, 0, -1, datadim,
                                     segmdim);
          r = ucons_meet_lincons_array (man, true, r, &arr);
          ap_lincons0_array_clear (&arr);
          break;
        }
      case 4:                     /* equal length lists: l[z]=l[x] */
        {
          assert (segmdim >= 4);
          ap_lincons0_array_t arr = ap_lincons0_array_make (1);
          arr.p[0] =
            shape_lincons_x_y_v_cst (AP_CONS_EQ, var_ptr_length, -1,
                                     datadim + nodex, 1, datadim + nodez, 0, 0, 0, datadim,
                                     segmdim);
          r = ucons_meet_lincons_array (man, true, r, &arr);
          ap_lincons0_array_clear (&arr);
          break;
        }
      default:
        break;
      }
    return r;
}
