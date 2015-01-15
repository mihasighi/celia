/**************************************************************************/
/*                                                                        */
/*  This file is part of CINV.                                            */
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
#include "mset.h"
#include "mset_internal.h"
#include "apron2shape.h"
#include "ap_generic.h"
#include "ap_linexpr0.h"

/* ============================================================ */
/* Strengthen mset constraint with equalities from data constraints */
/* ============================================================ */

/* Strengthen after the assignment d = expr.
 * @code{expr} is over a->datadim+a->segmdim
 */
void
mset_strengthen_assign(mset_internal_t *pr,
        mset_t *a, ap_dim_t d, ap_linexpr0_t *expr) {
    /* Heuristic 1: expr contain only one element e.
     * Assign ms(d) with ms(e)
     */
    // Step 1.1: test that expr has only one element inside (no coeff)
    size_t nelem = 0;
    ap_coeff_t *coeff = NULL;
    ap_coeff_t *dcoeff = NULL;
    size_t i;
    ap_dim_t dim;
#ifndef NDEBUG2
    fprintf(stdout, "\n++++mset_strengthen_assign: with assign (%zu)=(", d);
    ap_linexpr0_fprint(stdout, expr, NULL);
    fprintf(stdout, ")\n");
    fflush(stdout);
#endif

    ap_linexpr0_ForeachLinterm(expr, i, dim, coeff) {
        if (coeff && !ap_coeff_zero(coeff)) {
            nelem++;
            dcoeff = coeff;
        }
    }
    coeff = NULL;
    coeff = ap_linexpr0_cstref(expr); // do not free coeff
    if (nelem == 1 && ap_coeff_zero(coeff)) {
        coeff = NULL;
        coeff = ap_coeff_alloc(AP_COEFF_SCALAR);
        ap_coeff_set_scalar_int(coeff, 1);
        if (ap_coeff_cmp(coeff, dcoeff) == 0) {
            // is the good case
            // Step 1.2: propagate the equality to the mset
            ap_linexpr0_t* lexpr = ap_linexpr0_copy(expr);
            ap_linexpr0_realloc(lexpr, DATA_DIM(a->datadim, 0) + 2 * a->segmdim);
            a->mscons =
                    ap_abstract0_assign_linexpr(pr->man_mscons, true, a->mscons, d, lexpr,
                    NULL);
            ap_linexpr0_free(lexpr);
#ifndef NDEBUG2
            fprintf(stdout, "\n++++mset_strengthen_assign: returns (");
            ap_abstract0_fprint(stdout, pr->man_mscons, a->mscons, NULL);
            fprintf(stdout, ")\n");
            fflush(stdout);
#endif
        }
        ap_coeff_free(coeff);
        // do not free dcoeff which points to some element of expr
    } else {
        // if not heuristics applies, assign to undetermined value
        a->mscons = ap_abstract0_forget_array(pr->man_mscons, true,
                a->mscons,
                &d, 1, false);
    }

}

/* ============================================================ */
/* Meet constraints and Join generators */
/* ============================================================ */

/* The constraint has size a->datadim+a->segmdim.
 * Encoding of constraints:
 * if coeff is OFFSET_DATA then all realdim are data of nodes
 * if coeff is OFFSET_MSET then all realdim are (data+sums) of nodes
 * if coeff is OFFSET_LEN  then all realdim are lengths of nodes
 * if coeff is OFFSET_SUM or OFFSET_UCONS then ignore (return a copy)
 * otherwise is ISOMOPRHISM
 */
mset_t *
mset_meet_lincons(mset_internal_t * pr, bool destructive,
        mset_t * a, ap_lincons0_t * lcons) {
    mset_t *r = mset_alloc_internal(pr, a->datadim, a->segmdim);
    ap_linexpr0_t *lexpr;
    size_t i, dim;
    bool eqstruct = false;
    ap_lincons0_array_t arr;
    ap_coeff_t *coeff = NULL;
    ap_coeff_t *c_lcons = NULL;
    ap_constyp_t op = lcons->constyp;
    int kind = OFFSET_OTHER; /* unknown */

    /* Copy in r values of a */
#ifndef MSET_REL
    r->lcons = ap_abstract0_copy(pr->man_lcons, a->lcons);
#endif
    r->dcons = ap_abstract0_copy(pr->man_dcons, a->dcons);
    r->mscons = ap_abstract0_copy(pr->man_mscons, a->mscons);


    /* Analyze the kind of constraint stored in lcons->scalar */
    if (lcons->scalar) {
        if (!ap_scalar_cmp_int(lcons->scalar, OFFSET_DATA))
            kind = OFFSET_DATA; /* data */
        else if (!ap_scalar_cmp_int(lcons->scalar, OFFSET_MSET))
            kind = OFFSET_MSET; /* MSET */
        else if (!ap_scalar_cmp_int(lcons->scalar, OFFSET_LEN))
            kind = OFFSET_LEN; /* len */
        else if (!ap_scalar_cmp_int(lcons->scalar, OFFSET_SUM) ||
                !ap_scalar_cmp_int(lcons->scalar, OFFSET_UCONS))
            goto return_meet_lincons;
        else if (op == AP_CONS_EQ || op == AP_CONS_EQMOD) {
            eqstruct = true;
        } else
            assert(0);
    }

#ifndef NDEBUG1
    fprintf(stdout, "====mset_meet_lincons: with lincons=(");
    ap_lincons0_fprint(stdout, lcons, NULL);
    fprintf(stdout, ") (kind %d, eqstruct=%d) on (", kind, eqstruct);
    mset_fdump(stdout, pr->man, a);
    fprintf(stdout, ") copied in r = (");
    mset_fdump(stdout, pr->man, r);
    fprintf(stdout, ")\n");
    fflush(stdout);
#endif

    /* Build the array of linear constraints */
    arr = ap_lincons0_array_make(1);
    // c_lcons shall be allocated before set above
    c_lcons = ap_coeff_alloc(AP_COEFF_SCALAR);
    ap_linexpr0_get_cst(c_lcons, lcons->linexpr0);

    /* For length constraints */
    if (kind == OFFSET_LEN || eqstruct) {
#ifndef MSET_REL
        lexpr = ap_linexpr0_alloc(AP_LINEXPR_DENSE,
                LEN_DIM(a->datadim, 0) + a->segmdim);

        ap_linexpr0_ForeachLinterm(lcons->linexpr0, i, dim, coeff) {
            if (coeff && !ap_coeff_zero(coeff)) {
                if (!eqstruct && dim < LEN_DIM(a->datadim, 0))
                    ap_coeff_set(&lexpr->p.coeff[dim], coeff);
                else if (dim >= a->datadim) {
                    ap_coeff_set(&lexpr->p.coeff[dim - DATA_DIM(a->datadim, 0)], coeff);
                }
            }
        }
        // coeff is already allocated above
        ap_linexpr0_set_cst(lexpr, c_lcons);
        arr.p[0] = ap_lincons0_make(op, lexpr, NULL);
        r->lcons = ap_abstract0_meet_lincons_array(pr->man_lcons, true, r->lcons, &arr);
#else
        lexpr = ap_linexpr0_alloc(AP_LINEXPR_DENSE, a->datadim + 2 * a->segmdim);

        ap_linexpr0_ForeachLinterm(lcons->linexpr0, i, dim, coeff) {
            if (coeff && !ap_coeff_zero(coeff)) {
                if (!eqstruct && dim < a->datadim)
                    ap_coeff_set(&lexpr->p.coeff[dim], coeff);
                else if (dim >= a->datadim) {
                    ap_coeff_set(&lexpr->p.coeff[a->segmdim + dim], coeff);
                }
            }
        }
        ap_linexpr0_set_cst(lexpr, c_lcons);
        arr.p[0] = ap_lincons0_make(op, lexpr, NULL);
        r->dcons = ap_abstract0_meet_lincons_array(pr->man_dcons, true, r->dcons, &arr);
#endif
#ifndef NDEBUG1
        fprintf(stdout, "====mset_meet_lincons: builds length cons=(");
        ap_lincons0_array_fprint(stdout, &arr, NULL);
        fprintf(stdout, ") returns (");
        mset_fdump(stdout, pr->man, r);
        fprintf(stdout, ")\n");
        fflush(stdout);
#endif
        // clear structures allocated
        ap_lincons0_clear(arr.p); // frees also lexpr
    }

    /* For data constraints */
    if (kind == OFFSET_DATA || eqstruct) {
#ifndef MSET_REL
        lexpr = ap_linexpr0_alloc(AP_LINEXPR_DENSE, DATA_DIM(a->datadim, 0) + a->segmdim);

        ap_linexpr0_ForeachLinterm(lcons->linexpr0, i, dim, coeff) {
            if (coeff && !ap_coeff_zero(coeff)) {
                if (!eqstruct && LEN_DIM(a->datadim, 0) <= dim && dim < a->datadim)
                    ap_coeff_set(&lexpr->p.coeff[dim - LEN_DIM(a->datadim, 0)], coeff);
                else if (dim >= a->datadim) {
                    ap_coeff_set(&lexpr->p.coeff[dim - LEN_DIM(a->datadim, 0)], coeff);
                }
            }
        }
        ap_linexpr0_set_cst(lexpr, c_lcons);
        arr.p[0] = ap_lincons0_make(op, lexpr, NULL);
        r->dcons = ap_abstract0_meet_lincons_array(pr->man_dcons, true, r->dcons, &arr);
#else
        lexpr = ap_linexpr0_alloc(AP_LINEXPR_DENSE, a->datadim + 2 * a->segmdim);

        ap_linexpr0_ForeachLinterm(lcons->linexpr0, i, dim, coeff) {
            if (coeff && !ap_coeff_zero(coeff)) {
                if (!eqstruct && dim < a->datadim)
                    ap_coeff_set(&lexpr->p.coeff[dim], coeff);
                else if (dim >= a->datadim) {
                    ap_coeff_set(&lexpr->p.coeff[dim], coeff);
                }
            }
        }
        ap_linexpr0_set_cst(lexpr, c_lcons);
        arr.p[0] = ap_lincons0_make(op, lexpr, NULL);
        r->dcons = ap_abstract0_meet_lincons_array(pr->man_dcons, true, r->dcons, &arr);
#endif
#ifndef NDEBUG1
        fprintf(stdout, "====mset_meet_lincons: builds data cons=(");
        ap_lincons0_array_fprint(stdout, &arr, NULL);
        fprintf(stdout, ") returns (");
        mset_fdump(stdout, pr->man, r);
        fprintf(stdout, ")\n");
        fflush(stdout);
#endif
        // clear structures allocated
        ap_lincons0_clear(arr.p); // frees also lexpr
    }

    /* For mset constraints */
    if (kind == OFFSET_MSET || eqstruct) {
        lexpr = ap_linexpr0_alloc(AP_LINEXPR_DENSE, DATA_DIM(a->datadim, 0) + 2 * a->segmdim);

        ap_linexpr0_ForeachLinterm(lcons->linexpr0, i, dim, coeff) {
            if (coeff && !ap_coeff_zero(coeff)) {
                // Here, any equality shall be expressed in multiset
                if (LEN_DIM(a->datadim, 0) <= dim && dim < a->datadim)
                    ap_coeff_set(&lexpr->p.coeff[dim - LEN_DIM(a->datadim, 0)], coeff);
                else if (dim >= a->datadim) {
                    ap_coeff_set(&lexpr->p.coeff[dim - LEN_DIM(a->datadim, 0)], coeff); // mshd(n)
                    ap_coeff_set(&lexpr->p.coeff[a->segmdim + dim - LEN_DIM(a->datadim, 0)], coeff); // mstl(n)
                }
            }
        }
        ap_linexpr0_set_cst(lexpr, c_lcons);
        arr.p[0] = ap_lincons0_make(op, lexpr, NULL);
        r->mscons = ap_abstract0_meet_lincons_array(pr->man_mscons, true, r->mscons, &arr);
#ifndef NDEBUG1
        fprintf(stdout, "====mset_meet_lincons: builds mset cons=(");
        ap_lincons0_array_fprint(stdout, &arr, NULL);
        fprintf(stdout, ") returns (");
        mset_fdump(stdout, pr->man, r);
        fprintf(stdout, ")\n");
        fflush(stdout);
#endif
        ap_lincons0_array_clear(&arr); // frees also the lexpr
    }

#ifndef NDEBUG1
    fprintf(stdout, "====mset_meet_lincons: with lincons=(");
    ap_lincons0_fprint(stdout, lcons, NULL);
    fprintf(stdout, ") returns (");
    mset_fdump(stdout, pr->man, r);
    fprintf(stdout, ")\n");
    fflush(stdout);
#endif

return_meet_lincons:
    if (destructive)
        mset_free_internal(pr, a);
    return r;
}

/* The constraints in array have size a->datadim+2*a->segmdim,
 * where first a->segmdim is for data dereference,
 * and the second a->segmdim is for lengths of segments!
 */
mset_t *
mset_meet_lincons_array(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_lincons0_array_t * array) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
    if (!a)
        return NULL;
    if (!array || array->size == 0 || !array->p)
        return (destructive) ? a : mset_copy(man, a);
#ifndef NDEBUG2
    fprintf(stdout, "====mset_meet_lincons_array: on a=(");
    mset_fdump(stdout, man, a);
    fprintf(stdout, ") with array=(");
    ap_lincons0_array_fprint(stdout, array, NULL);
    fprintf(stdout, ")\n");
    fflush(stdout);
#endif
    size_t i;
    mset_t *r = mset_meet_lincons(pr, false, a, &array->p[0]);
    for (i = 1; i < array->size; i++)
        r = mset_meet_lincons(pr, true, r, &array->p[i]);
    if (destructive)
        mset_free_internal(pr, a);
    return r;
}

/* TODO: priority 0 */

/* not used */
mset_t *
mset_meet_tcons_array(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_tcons0_array_t * array) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_MEET_LINCONS_ARRAY, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return a;
}

/* NOT IMPLEMENTED */
mset_t *
mset_add_ray_array(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_generator0_array_t * array) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_ADD_RAY_ARRAY, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return a;
}


/* ============================================================ */
/* Assignement and Substitutions */
/* ============================================================ */

/* The expression concerns only data or data dereference of
 * pointer variables. Its size is a->datadim+a->segmdim! */
mset_t *
mset_assign_linexpr(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_dim_t d, ap_linexpr0_t * expr, mset_t * dest) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
    if (!a)
        return NULL;
    assert(expr && ap_linexpr0_size(expr) == (a->datadim + a->segmdim) &&
            d < (a->datadim + a->segmdim) && !dest);
    mset_t *r = mset_alloc_internal(pr, a->datadim, a->segmdim);
    ap_linexpr0_t* lexpr = ap_linexpr0_copy(expr);
#ifdef MSET_REL
    ap_linexpr0_realloc(lexpr, a->datadim + 2 * a->segmdim);
#ifndef NDEBUG
    fprintf(stdout, "\n====mset_assign_linexpr to dcons %d:=", d);
    ap_linexpr0_fprint(stdout, lexpr, NULL);
    fprintf(stdout, "\n");
#endif
    r->dcons =
            ap_abstract0_assign_linexpr(pr->man_dcons, false, a->dcons, d, lexpr,
            NULL);
#ifndef NDEBUG
    fprintf(stdout, "\n====mset_assign_linexpr returns ");
    ap_abstract0_fprint(stdout, pr->man_dcons, r->dcons, NULL);
    fprintf(stdout, "\n");
#endif
#else
    if (d < LEN_DIM(a->datadim, 0)) {
        ap_linexpr0_realloc(lexpr, LEN_DIM(a->datadim, 0));
        ap_linexpr0_realloc(lexpr, LEN_DIM(a->datadim, 0) + a->segmdim);
        r->lcons =
                ap_abstract0_assign_linexpr(pr->man_lcons, false, a->lcons, d, lexpr,
                NULL);
        r->dcons = ap_abstract0_copy(pr->man_dcons, a->dcons);
    } else {
        ap_linexpr0_t *expr1 = ap_linexpr0_alloc(AP_LINEXPR_DENSE,
                DATA_DIM(a->datadim,
                0) +
                a->segmdim);
        size_t i, dim;
        ap_coeff_t *coeff;

        ap_linexpr0_ForeachLinterm(lexpr, i, dim, coeff) {
            if (!ap_coeff_zero(coeff)) {
                ap_dim_t newdim = dim - LEN_DIM(a->datadim, 0);
                ap_coeff_set(expr1->p.coeff + newdim, coeff);
            }
        }
        coeff = ap_coeff_alloc(AP_COEFF_SCALAR);
        ap_linexpr0_get_cst(coeff, expr);
        ap_linexpr0_set_cst(expr1, coeff);
        ap_coeff_free(coeff);
        dim = d - LEN_DIM(a->datadim, 0);
#ifndef NDEBUG
        fprintf(stdout, "\n====mset_assign_linexpr to dcons %zu:=", dim);
        ap_linexpr0_fprint(stdout, expr1, NULL);
        fprintf(stdout, "\n");
#endif
        r->dcons =
                ap_abstract0_assign_linexpr(pr->man_dcons, false, a->dcons, dim,
                expr1, NULL);
#ifndef NDEBUG
        fprintf(stdout, "\n====mset_assign_linexpr returns ");
        ap_abstract0_fprint(stdout, pr->man_dcons, r->dcons, NULL);
        fprintf(stdout, "\n");
#endif
        ap_linexpr0_free(expr1);
        r->lcons = ap_abstract0_copy(pr->man_lcons, a->lcons);
    }
#endif
    r->mscons = ap_abstract0_copy(pr->man_mscons, a->mscons);
    // only the exact equality assignment of data shall be propagated to mset constraints
    mset_strengthen_assign(pr, r, d, lexpr);
    ap_linexpr0_free(lexpr);

    if (destructive)
        mset_free_internal(pr, a);
    return r;
}

/* TODO: priority 0 */

/* used for pre-image computation */
mset_t *
mset_substitute_linexpr(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_dim_t d, ap_linexpr0_t * expr, mset_t * dest) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return a;
}

/* TODO: priority 0 */

/* not used */
mset_t *
mset_assign_linexpr_array(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_dim_t * tdim,
        ap_linexpr0_t ** texpr, size_t size, mset_t * dest) {
    if (size == 1)
        return mset_assign_linexpr(man, destructive, a, tdim[0], texpr[0], dest);
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_ASSIGN_LINEXPR_ARRAY, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return a;
}

/* TODO: priority 0 */

/* used for pre-image computation */
mset_t *
mset_substitute_linexpr_array(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_dim_t * tdim,
        ap_linexpr0_t ** texpr,
        size_t size, mset_t * dest) {
    if (size == 1)
        return mset_substitute_linexpr(man, destructive, a, tdim[0], texpr[0],
            dest);

    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_SUBSTITUTE_TEXPR_ARRAY, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return a;
}

/* TODO: priority 0 */

/* not used because of transformation to linexpr */
mset_t *
mset_assign_texpr_array(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_dim_t * tdim,
        ap_texpr0_t ** texpr, size_t size, mset_t * dest) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_ASSIGN_TEXPR_ARRAY, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return a;
}

/* TODO: priority 0 */

/* used only for pre-image computation */
mset_t *
mset_substitute_texpr_array(ap_manager_t * man,
        bool destructive, mset_t * a,
        ap_dim_t * tdim,
        ap_texpr0_t ** texpr, size_t size, mset_t * dest) {
    return ap_generic_substitute_texpr_array(man, destructive, a, tdim, texpr,
            size, dest);
}
