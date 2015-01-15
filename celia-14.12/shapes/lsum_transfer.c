/**************************************************************************/
/*                                                                        */
/*  CINV Library / Shape Domain                                           */
/*                                                                        */
/*  Copyright (C) 2009-2011                                               */
/*    LIAFA (University of Paris Diderot and CNRS)                        */
/*                                                                        */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 3.                                                */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 3.                  */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/


#include <assert.h>
#include "lsum.h"
#include "lsum_internal.h"
#include "apron2shape.h"
#include "apron/ap_generic.h"
#include "apron/ap_linexpr0.h"
#include "shad.h"

/* ============================================================ */
/* Generate from SL3 formulas */

/* ============================================================ */

/**
 * Meets a with the disjunct disj of formula f.
 * In place (modifies a).
 */
lsum_t*
lsum_meet_formula (lsum_internal_t* pr, lsum_t* a,
                   sh_formula_t* f, size_t disj)
{
  // check dimensions
  size_t datadim = f->env->intdim;
  size_t segmdim = f->form[disj]->nodes->realdim + 1;
  if (!a || datadim != a->datadim || segmdim != a->segmdim)
    return NULL;

  // initialize the result to a
  lsum_t* r = a;

  // go through the data constraints 
  // select those which can be dealt by this domain
  // warning for the others
  size_t i;
  for (i = 0; i < f->form[disj]->length_dform; i++)
    {
      // deal with the constraint f->form[disj]->dform[i]
      sh_dataform_t df = f->form[disj]->dform[i];
      // build the array of linear constraints from the linear term of df
      ap_lincons0_array_t arr = ap_lincons0_array_make (1);
      ap_linexpr0_t* lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, datadim + 3 * segmdim);
      sh_linterm_t* ti = df.p;
      for (ti = df.p; ti != NULL; ti = ti->next)
        {
          // compute the dimension corresponding to this term
          ap_dim_t dim;
          switch (ti->t.funid)
            {
            case SH_F_SYMBOL: // may be only intdim
              dim = ti->t.p0; // ok dimension already computed
              break;
            case SH_F_LEN: // parameter is node but not nilNode!
              dim = datadim + 2 * segmdim + (ti->t.p0 + 1);
              break;
            case SH_F_DATA: // the second parameter is supposed to be 0!
              if (ti->t.p1 != AP_DIM_MAX)
                {
#ifndef NDEBUG1
                  fprintf (stdout, "====lsum_of_formula: error for data(%d,%d!=AP_DIM_MAX)\n",
                           ti->t.p0, ti->t.p1);
                  fflush (stdout);
#endif
                  goto end_of_dataform;
                }
              dim = datadim + ti->t.p0 + 1;
              break;
            case SH_F_SUM: // parameter is node but not nilNode!
              dim = datadim + segmdim + ti->t.p0 + 1;
              break;
            default: /* not dealt, break the for loop */
#ifndef NDEBUG1
              fprintf (stdout, "====lsum_of_formula: constraint not dealt (MSET)\n");
              fflush (stdout);
#endif
              goto end_of_dataform;
              break;
            }
          if (ti->coeff)
            ap_coeff_set_scalar_int (&lexpr->p.coeff[dim], ti->coeff);
        }
      ap_linexpr0_set_cst_scalar_int (lexpr, df.cst);
      arr.p[0] = ap_lincons0_make (df.constyp, lexpr, NULL);
      r->dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, r->dcons, &arr);
end_of_dataform:
      ap_lincons0_array_clear (&arr); // free also lexpr
    }

  return r;
}

/* ============================================================ */
/* Meet constraints and Join generators */
/* ============================================================ */

/* The constraint has size a->datadim+a->segmdim.
 * Encoding of constraints:
 * if coeff is OFFSET_DATA then all realdim are data of nodes
 * if coeff is OFFSET_SUM then all realdim are (data+sums) of nodes
 * if coeff is OFFSET_LEN  then all realdim are lengths of nodes
 * if coeff is OFFSET_MSET or OFFSET_UCONS then ignore (return a copy)
 * if coeff is OFFSET_SL3 then translate sh_crt->form[lcons->lexpr->scalar]->dform
 * otherwise is ISOMOPRHISM
 */
lsum_t *
lsum_meet_lincons (lsum_internal_t * pr, bool destructive,
                   lsum_t * a, ap_lincons0_t * lcons)
{
  lsum_t *r = lsum_alloc_internal (pr, a->datadim, a->segmdim);
  ap_linexpr0_t *lexpr;
  size_t i, dim;
  bool eqstruct = false;
  ap_lincons0_array_t arr;
  ap_coeff_t *coeff = NULL;
  ap_coeff_t *c_lcons = NULL;
  ap_constyp_t op = lcons->constyp;
  int kind = OFFSET_NONE; /* unknown */

  /* Copy in r values of a */
  r->dcons = ap_abstract0_copy (pr->man_dcons, a->dcons);

  /* Analyze the kind of constraint stored in lcons->scalar */
  if (lcons->scalar)
    {
      if (!ap_scalar_cmp_int (lcons->scalar, a->datadim + OFFSET_DATA))
        kind = OFFSET_DATA; /* data */
      else if (!ap_scalar_cmp_int (lcons->scalar, OFFSET_SUM))
        kind = OFFSET_SUM; /* sum */
      else if (!ap_scalar_cmp_int (lcons->scalar, OFFSET_LEN))
        kind = OFFSET_LEN; /* len */
      else if (!ap_scalar_cmp_int (lcons->scalar, OFFSET_MSET) ||
               !ap_scalar_cmp_int (lcons->scalar, OFFSET_UCONS))
        goto return_meet_lincons;
      else if (!ap_scalar_cmp_int (lcons->scalar, OFFSET_SL3))
        kind = OFFSET_SL3; /* SL3 */
      else if (op == AP_CONS_EQ || op == AP_CONS_EQMOD)
        {
          eqstruct = true;
        }
      else
        assert (0);
    }

#ifndef NDEBUG1
  fprintf (stdout, "====lsum_meet_lincons: with lincons=(");
  ap_lincons0_fprint (stdout, lcons, NULL);
  fprintf (stdout, ") (kind %d, eqstruct=%d) on (", kind, eqstruct);
  lsum_fdump (stdout, pr->man, a);
  fprintf (stdout, ") copied in r = (");
  lsum_fdump (stdout, pr->man, r);
  fprintf (stdout, ") \n");
  fflush (stdout);
#endif

  /* Build the array of linear constraints */
  arr = ap_lincons0_array_make (1);
  // c_lcons shall be allocated before set above
  c_lcons = ap_coeff_alloc (AP_COEFF_SCALAR);
  ap_linexpr0_get_cst (c_lcons, lcons->linexpr0);

  /* For SL3 constraints, call specific function */
  if (kind == OFFSET_SL3)
    {
      double code;
      ap_double_set_scalar (&code, c_lcons->val.scalar, GMP_RNDN);
      r = lsum_meet_formula (pr, r,
                             sh_crt, (size_t) code);
#ifndef NDEBUG1
      fprintf (stdout, "====lsum_meet_lincons: (SL3) returns (");
      lsum_fdump (stdout, pr->man, r);
      fprintf (stdout, ")\n");
      fflush (stdout);
#endif
    }

  /* For length constraints */
  if (kind == OFFSET_LEN || eqstruct)
    {
      lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, a->datadim + 3 * a->segmdim);

      ap_linexpr0_ForeachLinterm (lcons->linexpr0, i, dim, coeff)
      {
        if (coeff && !ap_coeff_zero (coeff))
          {
            if (!eqstruct && dim < a->datadim)
              ap_coeff_set (&lexpr->p.coeff[dim], coeff);
            else if (dim >= a->datadim)
              {
                ap_coeff_set (&lexpr->p.coeff[2 * a->segmdim + dim], coeff);
              }
          }
      }
      ap_linexpr0_set_cst (lexpr, c_lcons);
      arr.p[0] = ap_lincons0_make (op, lexpr, NULL);
      r->dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, r->dcons, &arr);
#ifndef NDEBUG1
      fprintf (stdout, "====lsum_meet_lincons: builds length cons=(");
      ap_lincons0_array_fprint (stdout, &arr, NULL);
      fprintf (stdout, ") returns (");
      lsum_fdump (stdout, pr->man, r);
      fprintf (stdout, ")\n");
      fflush (stdout);
#endif
    }

  /* For data constraints */
  if (kind == OFFSET_DATA || eqstruct)
    {
      lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, a->datadim + 3 * a->segmdim);

      ap_linexpr0_ForeachLinterm (lcons->linexpr0, i, dim, coeff)
      {
        if (coeff && !ap_coeff_zero (coeff))
          {
            if (!eqstruct && dim < a->datadim)
              ap_coeff_set (&lexpr->p.coeff[dim], coeff);
            else if (dim >= a->datadim)
              {
                ap_coeff_set (&lexpr->p.coeff[dim], coeff);
              }
          }
      }
      ap_linexpr0_set_cst (lexpr, c_lcons);
      arr.p[0] = ap_lincons0_make (op, lexpr, NULL);
      r->dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, r->dcons, &arr);
#ifndef NDEBUG1
      fprintf (stdout, "====lsum_meet_lincons: builds data cons=(");
      ap_lincons0_array_fprint (stdout, &arr, NULL);
      fprintf (stdout, ") returns (");
      lsum_fdump (stdout, pr->man, r);
      fprintf (stdout, ")\n");
      fflush (stdout);
#endif
    }

  /* For sum constraints */
  if (kind == OFFSET_SUM || eqstruct)
    {
      lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, a->datadim + 3 * a->segmdim);

      ap_linexpr0_ForeachLinterm (lcons->linexpr0, i, dim, coeff)
      {
        if (coeff && !ap_coeff_zero (coeff))
          {
            if (!eqstruct && dim < a->datadim)
              ap_coeff_set (&lexpr->p.coeff[dim], coeff);
            else if (dim >= a->datadim)
              {
                ap_coeff_set (&lexpr->p.coeff[dim], coeff); // data(n)
                ap_coeff_set (&lexpr->p.coeff[a->segmdim + dim], coeff); // sum(n)
              }
          }
      }
      ap_linexpr0_set_cst (lexpr, c_lcons);
      arr.p[0] = ap_lincons0_make (op, lexpr, NULL);
      r->dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, r->dcons, &arr);
#ifndef NDEBUG1
      fprintf (stdout, "====lsum_meet_lincons: builds sum cons=(");
      ap_lincons0_array_fprint (stdout, &arr, NULL);
      fprintf (stdout, ") returns (");
      lsum_fdump (stdout, pr->man, r);
      fprintf (stdout, ")\n");
      fflush (stdout);
#endif
    }
  // clear structures allocated
  ap_lincons0_array_clear (&arr); // frees also the lexpr

#ifndef NDEBUG1
  fprintf (stdout, "====lsum_meet_lincons: with lincons=(");
  ap_lincons0_fprint (stdout, lcons, NULL);
  fprintf (stdout, ") returns (");
  lsum_fdump (stdout, pr->man, r);
  fprintf (stdout, ")\n");
  fflush (stdout);
#endif

return_meet_lincons:
  if (destructive)
    lsum_free_internal (pr, a);
  return r;
}

/* The constraints in array have size a->datadim+2*a->segmdim,
 * where first a->segmdim is for data dereference,
 * and the second a->segmdim is for lengths of segments!
 */
lsum_t *
lsum_meet_lincons_array (ap_manager_t * man,
                         bool destructive, lsum_t * a,
                         ap_lincons0_array_t * array)
{
  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
  if (!a)
    return NULL;
  if (!array || array->size == 0 || !array->p)
    return (destructive) ? a : lsum_copy (man, a);
#ifndef NDEBUG2
  fprintf (stdout, "====lsum_meet_lincons_array: on a=(");
  lsum_fdump (stdout, man, a);
  fprintf (stdout, ") with array=(");
  ap_lincons0_array_fprint (stdout, array, NULL);
  fprintf (stdout, ")\n");
  fflush (stdout);
#endif
  size_t i;
  lsum_t *r = lsum_meet_lincons (pr, false, a, &array->p[0]);
  for (i = 1; i < array->size; i++)
    r = lsum_meet_lincons (pr, true, r, &array->p[i]);
  if (destructive)
    lsum_free_internal (pr, a);
  return r;
}

/* TODO: priority 0 */

/* not used */
lsum_t *
lsum_meet_tcons_array (ap_manager_t * man,
                       bool destructive, lsum_t * a,
                       ap_tcons0_array_t * array)
{
  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* NOT IMPLEMENTED */
lsum_t *
lsum_add_ray_array (ap_manager_t * man,
                    bool destructive, lsum_t * a,
                    ap_generator0_array_t * array)
{
  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_ADD_RAY_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}


/* ============================================================ */
/* Assignement and Substitutions */
/* ============================================================ */

/* The expression concerns only data or data dereference of
 * pointer variables. Its size is a->datadim+a->segmdim! */
lsum_t *
lsum_assign_linexpr (ap_manager_t * man,
                     bool destructive, lsum_t * a,
                     ap_dim_t d, ap_linexpr0_t * expr, lsum_t * dest)
{
  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
  if (!a)
    return NULL;
  assert (expr && ap_linexpr0_size (expr) == (a->datadim + a->segmdim) &&
          d < (a->datadim + a->segmdim) && !dest);
  lsum_t *r = lsum_alloc_internal (pr, a->datadim, a->segmdim);
  ap_linexpr0_t* lexpr = ap_linexpr0_copy (expr);
  ap_linexpr0_realloc (lexpr, a->datadim + 3 * a->segmdim);
#ifndef NDEBUG
  fprintf (stdout, "\n====lsum_assign_linexpr to dcons %d:=", d);
  ap_linexpr0_fprint (stdout, lexpr, NULL);
  fprintf (stdout, "\n");
#endif
  r->dcons =
          ap_abstract0_assign_linexpr (pr->man_dcons, false, a->dcons, d, lexpr,
                                       NULL);
#ifndef NDEBUG
  fprintf (stdout, "\n====lsum_assign_linexpr returns ");
  ap_abstract0_fprint (stdout, pr->man_dcons, r->dcons, NULL);
  fprintf (stdout, "\n");
#endif
  ap_linexpr0_free (lexpr);
  if (destructive)
    lsum_free_internal (pr, a);
  return r;
}

/* TODO: priority 0 */

/* used for pre-image computation */
lsum_t *
lsum_substitute_linexpr (ap_manager_t * man,
                         bool destructive, lsum_t * a,
                         ap_dim_t d, ap_linexpr0_t * expr, lsum_t * dest)
{
  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
  if (!a)
    return NULL;
  assert (expr && ap_linexpr0_size (expr) == (a->datadim + a->segmdim) &&
          d < (a->datadim + a->segmdim));
  lsum_t *r = lsum_alloc_internal (pr, a->datadim, a->segmdim);
  ap_linexpr0_t* lexpr = ap_linexpr0_copy (expr);
  ap_linexpr0_realloc (lexpr, a->datadim + 3 * a->segmdim);
#ifndef NDEBUG
  fprintf (stdout, "\n====lsum_substitute_linexpr to dcons %d:=", d);
  ap_linexpr0_fprint (stdout, lexpr, NULL);
  fprintf (stdout, "\n");
#endif
  r->dcons =
          ap_abstract0_substitute_linexpr (pr->man_dcons, false, a->dcons, d, lexpr,
                                           NULL);
#ifndef NDEBUG
  fprintf (stdout, "\n====lsum_substitute_linexpr returns ");
  ap_abstract0_fprint (stdout, pr->man_dcons, r->dcons, NULL);
  fprintf (stdout, "\n");
#endif

  ap_linexpr0_free (lexpr);
  if (destructive)
    lsum_free_internal (pr, a);
  if (r && dest)
    {
      lsum_t *rr = lsum_meet (man, false, r, dest);
      lsum_free_internal (pr, r);
      return rr;
    }
  return r;
}

/* TODO: priority 0 */

/* not used */
lsum_t *

lsum_assign_linexpr_array (ap_manager_t * man,
                           bool destructive, lsum_t * a,
                           ap_dim_t * tdim,
                           ap_linexpr0_t ** texpr, size_t size, lsum_t * dest)
{
  if (size == 1)
    return lsum_assign_linexpr (man, destructive, a, tdim[0], texpr[0], dest);
  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* TODO: priority 0 */

/* used for pre-image computation */
lsum_t *
lsum_substitute_linexpr_array (ap_manager_t * man,
                               bool destructive, lsum_t * a,
                               ap_dim_t * tdim,
                               ap_linexpr0_t ** texpr,
                               size_t size, lsum_t * dest)
{
  if (size == 1)
    return lsum_substitute_linexpr (man, destructive, a, tdim[0], texpr[0],
                                    dest);

  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_SUBSTITUTE_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* TODO: priority 0 */

/* not used because of transformation to linexpr */
lsum_t *
lsum_assign_texpr_array (ap_manager_t * man,
                         bool destructive, lsum_t * a,
                         ap_dim_t * tdim,
                         ap_texpr0_t ** texpr, size_t size, lsum_t * dest)
{
  lsum_internal_t *pr =
          lsum_init_from_manager (man, AP_FUNID_ASSIGN_TEXPR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
                              "not implemented");
  return a;
}

/* TODO: priority 0 */

/* used only for pre-image computation */
lsum_t *
lsum_substitute_texpr_array (ap_manager_t * man,
                             bool destructive, lsum_t * a,
                             ap_dim_t * tdim,
                             ap_texpr0_t ** texpr, size_t size, lsum_t * dest)
{
  return ap_generic_substitute_texpr_array (man, destructive, a, tdim, texpr,
                                            size, dest);
}
