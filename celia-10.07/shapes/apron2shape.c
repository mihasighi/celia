/*
 * apron2shape.c
 *
 * Translation of the Apron constraints/expressions to Shape ones.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include "apron2shape.h"
#include "shape_macros.h"

/* ============================================================ */
/* Constants */
/* ============================================================ */

/* TODO: read these from a file */

size_t var_ptr_data = 0;
size_t var_ptr_free = 1;
size_t var_ptr_length = 2;
size_t var_ptr_new = 3;
size_t var_ptr_next = 4;
size_t var_ptr_null = 5;

size_t lendim = 2;

/* ============================================================ */
/* Expressions and constraints */
/* ============================================================ */

/* Search an entry in the htable of pointer constraints */
pcons0_t *
shape_pcons_search (shape_internal_t * pr, ap_lincons0_t * lcons,
		    ap_tcons0_t * tcons)
{
  pcons0_t *r, rr;
  unsigned keylen;
  /* search in the htable */
  keylen = offsetof (pcons0_t, hh) - offsetof (pcons0_t, lcons);
  r = NULL;
  rr.lcons = lcons;
  rr.tcons = tcons;
  HASH_FIND (hh, pr->pcons, &rr.lcons, keylen, r);
  return r;
}

/*
 * Add an entry in the htable of pointer constraints and return a pointer to
 * it
 */
pcons0_t *
shape_pcons_add (shape_internal_t * pr, pcons0_t * cons)
{
  pcons0_t *r;
  unsigned keylen;
  /* search in the htable */
  keylen = offsetof (pcons0_t, hh) - offsetof (pcons0_t, lcons);
  r = NULL;
  HASH_FIND (hh, pr->pcons, &cons->lcons, keylen, r);
  if (!r)
    {
      HASH_ADD (hh, pr->pcons, lcons, keylen, cons);
      HASH_FIND (hh, pr->pcons, &cons->lcons, keylen, r);
    }
#ifndef NDEBUG
  fprintf (stdout, "shape_pcons add: ");
  shape_pcons_fdump (stdout, cons, 2, 9);
  fprintf (stdout, ", return: ");
  shape_pcons_fdump (stdout, r, 2, 9);
  fprintf (stdout, "\n");
#endif

  return r;
}

/* Extract a (partial) constraint from a linear expression */
void
shape_pcons_of_linexpr (shape_internal_t * pr, ap_linexpr0_t * e,
			pcons0_t * r, size_t intdim, size_t realdim)
{
  size_t i;
  ap_coeff_t *coeff;
  ap_dim_t d;
  arg_assert (r, return;
    );
#ifndef NDEBUG
  fprintf (stdout, "\nlinexpr: (");
  ap_linexpr0_fprint (stdout, e, NULL);
  fprintf (stdout, ")\n");
#endif
  switch (r->type)
    {
    case EQ_CONS:
    case NE_CONS:
    case REACH_CONS:
      /* linear expression shall be x - y */
      ap_linexpr0_ForeachLinterm (e, i, d, coeff)
      {
	if (!ap_coeff_zero (coeff))
	  {
	    int coef = ap_scalar_sgn (coeff->val.scalar);
	    arg_assert ((coeff->discr == AP_COEFF_SCALAR), return;
	      );
	    arg_assert (IS_PTRVAR (pr, d, intdim, realdim), return;
	      );
	    if (coef == -1)
	      r->info.ptr.y = d;
	    else if (coef == 1)
	      r->info.ptr.x = d;
	    else
	      arg_assert (0, return;
	      );
	    r->info.ptr.nextx = 0;
	    r->info.ptr.nexty = 0;
	  }
      }
      break;

    case DATA_CONS:
      r->info.data.linexpr0 = ap_linexpr0_copy (e);
      break;


    case REACHL_CONS:
      /* linear expression shall be x -l -y */
      ap_linexpr0_ForeachLinterm (e, i, d, coeff)
      {
	if (!ap_coeff_zero (coeff))
	  {
	    int coef = ap_scalar_sgn (coeff->val.scalar);
	    arg_assert ((coeff->discr == AP_COEFF_SCALAR), return;
	      );
	    if (IS_PTRVAR (pr, d, intdim, realdim))
	      {
		if (coef == -1)
		  r->info.ptr.y = d;
		else if (coef == 1 && !IS_NULLVAR (pr, d, intdim, realdim))
		  r->info.ptr.x = d;
		else
		  arg_assert (0, return;
		  );
	      }
	    else if (IS_INTDIM (d, intdim, realdim))
	      {
		if (coef == -1)
		  r->info.ptr.l = d;
		else
		  arg_assert (0, return;
		  );
	      }
	    else
	      arg_assert (0, return;
	      );
	    r->info.ptr.nextx = 0;
	    r->info.ptr.nexty = 0;
	  }
      }
      break;

    case OTHER_CONS:
      {
	/* linear constraint shall be only one variable y */
	bool found = false;
	ap_linexpr0_ForeachLinterm (e, i, d, coeff)
	{
	  if (!ap_coeff_zero (coeff))
	    {
	      int coef = ap_scalar_sgn (coeff->val.scalar);
	      arg_assert (!found && (coeff->discr == AP_COEFF_SCALAR), return;
		);
	      found = true;
	      if (IS_PTRVAR (pr, d, intdim, realdim) && (coef == 1))
		r->info.ptr.x = d;
	      else
		arg_assert (0, return;
		);
	      r->info.ptr.nextx = 0;
	    }
	}
	break;
      }
    }
}

pcons0_t *
shape_pcons_of_lincons (shape_internal_t * pr, ap_lincons0_t * c,
			size_t intdim, size_t realdim)
{
  pcons0_t *r;
#ifndef NDEBUG
  fprintf (stdout, "shape_pcons_of_lincons\n");
#endif
  /*
   * lincons may be: - only reals (ptr vars) for x == y or x <> y or x
   * ---> y (coded in x > y) - reals and integers    for x --l-> y
   * (coded by x -l -y > 0) - only integers for constraints on data and
   * length variables
   */
  if (!c)
    return NULL;
  else if ((r = shape_pcons_search (pr, c, NULL)) != NULL)
    /* already in the htable */
    return r;
  else
    {
      size_t i;
      ap_linexpr0_t *e = c->linexpr0;
      checked_malloc (r, pcons0_t, sizeof (pcons0_t), 1, return NULL;
	);
      if (ap_linexpr0_is_real (e, intdim))
	{			/* only constraints x - y == 0 or x - y > 0 */
	  switch (c->constyp)
	    {
	    case AP_CONS_EQ:
	      r->type = EQ_CONS;
	      break;
	    case AP_CONS_DISEQ:
	      r->type = NE_CONS;
	      break;

	    case AP_CONS_SUPEQ:
	    case AP_CONS_SUP:
	      r->type = REACH_CONS;
	      break;
	    default:		/* ERROR */
	      arg_assert (0, return r;
		);
	      break;
	    }
	}
      else if (ap_linexpr0_is_integer (e, intdim))
	{			/* only constraints on data */
	  r->type = DATA_CONS;
	  r->info.data.constyp = c->constyp;
	  r->info.data.scalar = c->scalar;
	}
      else
	{			/* reachability constraint with length  x - l
				 * - y >,>=,== 0 */
	  switch (c->constyp)
	    {
	    case AP_CONS_EQ:
	    case AP_CONS_SUPEQ:
	    case AP_CONS_SUP:
	      r->type = REACHL_CONS;
	      break;
	    default:		/* ERROR */
	      arg_assert (0, return r;
		);
	      break;
	    }
	}
      /* fill from expression */
      shape_pcons_of_linexpr (pr, c->linexpr0, r, intdim, realdim);
      r->lcons = c;
      r->tcons = NULL;
//TODO:free r ?
      return shape_pcons_add (pr, r);
    }
}


/*
 * Extract in l a data constraint from tree expression t; coef is the
 * cumulated coefficient for the expression. We suppose that t is a linear
 * expression with terms vars, nb * vars, nb * (ptr * data) and some constant!
 * The scalar is mandatory because multiplication of coefficients is allowed.
 *
 * @return the scalar if t is scalar or NULL otherwise
 */
ap_scalar_t *
shape_dataexpr_of_texpr (shape_internal_t * pr, ap_texpr0_t * t,
			 ap_scalar_t * coef,
			 ap_linexpr0_t * l, size_t intdim, size_t realdim)
{
  ap_scalar_t *r = NULL;
  double dcoef = 0.0;

  if (!t)
    return r;
  if (coef)
    ap_double_set_scalar (&dcoef, coef, GMP_RNDN);
  else
    dcoef = 1.0;
  switch (t->discr)
    {
    case AP_TEXPR_CST:
      if (t->val.cst.discr == AP_COEFF_SCALAR)
	{
	  double ncoef;
	  ap_double_set_scalar (&ncoef, t->val.cst.val.scalar, GMP_RNDN);
	  ncoef = dcoef * ncoef;
	  r = ap_scalar_alloc_set_double (ncoef);
	}
      else
	arg_assert (0, return r;
	);
      break;

    case AP_TEXPR_DIM:
      if (t->val.dim < intdim)
	{
	  ap_coeff_t *dimcoef = ap_linexpr0_coeffref (l, t->val.dim);
	  if (dimcoef && dimcoef->discr == AP_COEFF_SCALAR)
	    {
	      double oldcoef;
	      ap_double_set_scalar (&oldcoef, dimcoef->val.scalar, GMP_RNDN);
	      dcoef = (oldcoef == 0.0) ? dcoef : (dcoef * oldcoef);
	      ap_linexpr0_set_coeff_scalar_double (l, t->val.dim, dcoef);
	    }
	  else
	    arg_assert (0, return r;
	    );
	}
      else			/* see the case of ptr vars below */
	arg_assert (0, return r;
	);
      break;

    case AP_TEXPR_NODE:
      switch (t->val.node->op)
	{
	case AP_TEXPR_ADD:
	case AP_TEXPR_SUB:
	  {
	    ap_scalar_t *ncoef = (t->val.node->op == AP_TEXPR_SUB) ?
	      ap_scalar_alloc_set_double (-dcoef) : coef;
	    /* recursively apply on subexpressions */
	    ap_scalar_t *r1 =
	      shape_dataexpr_of_texpr (pr, t->val.node->exprA, coef, l,
				       intdim,
				       realdim);
	    ap_scalar_t *r2 =
	      shape_dataexpr_of_texpr (pr, t->val.node->exprB, ncoef, l,
				       intdim,
				       realdim);
	    double d1, d2;
	    if (r1)
	      ap_double_set_scalar (&d1, r1, GMP_RNDN);
	    else
	      d1 = 0.0;
	    if (r2)
	      ap_double_set_scalar (&d2, r2, GMP_RNDN);
	    else
	      d2 = 0.0;
	    d1 += d2;
	    if (r1 || r2)
	      {
		ap_coeff_t *oldcoeff = ap_coeff_alloc (AP_COEFF_SCALAR);
		double oldd;
		ap_linexpr0_get_cst (oldcoeff, l);
		ap_double_set_scalar (&oldd, oldcoeff->val.scalar, GMP_RNDN);
		d1 += oldd;
		ap_linexpr0_set_cst_scalar_double (l, d1);
		ap_coeff_free (oldcoeff);
	      }
	    if (r1)
	      ap_scalar_free (r1);
	    if (r2)
	      ap_scalar_free (r2);
	    if (ncoef != coef)
	      ap_scalar_free (ncoef);
	    break;
	  }

	case AP_TEXPR_MUL:
	  /*
	   * can be either nb * vars, nb * (ptr * data), ptr *
	   * data
	   */
	  switch (t->val.node->exprA->discr)
	    {
	    case AP_TEXPR_CST:
	      {
		ap_scalar_t *newcoef =
		  shape_dataexpr_of_texpr (pr, t->val.node->exprA, coef, l,
					   intdim, realdim);
		shape_dataexpr_of_texpr (pr, t->val.node->exprB, newcoef, l,
					 intdim, realdim);
		break;
	      }

	    case AP_TEXPR_DIM:
	      {
		/* shall be a ptr variable */
		ap_dim_t pdim;
		pdim = t->val.node->exprA->val.dim;
		if (IS_PTRVAR (pr, pdim, intdim, realdim)
		    && !IS_NULLVAR (pr, pdim, intdim, realdim))
		  {
		    /* second term is data */
		    arg_assert (t->val.node->exprB->discr == AP_TEXPR_DIM &&
				IS_DATAVAR (pr, t->val.node->exprB->val.dim,
					    intdim, realdim), return r;
		      );
		    /*
		     * set coef to dimension
		     * intdim + ptr TODO: macro?
		     */
		    ap_linexpr0_set_coeff_scalar_double (l, pdim, dcoef);
		  }
		else
		  arg_assert (0, return r;
		  );
		break;
	      }

	    default:
	      arg_assert (0, return r;
		);
	    }
	  break;

	case AP_TEXPR_NEG:
	  {
	    ap_scalar_t *newcoef = ap_scalar_alloc_set_double (-dcoef);
	    shape_dataexpr_of_texpr (pr, t->val.node->exprA, newcoef, l,
				     intdim, realdim);
	    ap_scalar_free (newcoef);
	    break;
	  }
	default:		/* no other operators than ADD, SUB or MUL */
	  arg_assert (0, return r;
	    );
	}
    }
  return r;
}


/*
 * Extract a partial constraint from a tree expression. @return if a
 * dereferencing (/ next or /data) is used
 */
bool
shape_pcons_of_texpr (shape_internal_t * pr, ap_texpr0_t * e,
		      pcons0_t * r, size_t intdim, size_t realdim)
{
  ap_dim_t *dim_array = ap_texpr0_dimlist (e);
  bool isptr, isdata, isdatafield, isnextfield, isderef;
  int i;
#ifndef NDEBUG
  fprintf (stdout, "\n(");
  ap_texpr0_fprint (stdout, e, NULL);
  fprintf (stdout, ")\n");
#endif
  for (isptr = isdata = isdatafield = isnextfield = false, i = 0;
       dim_array[i] != AP_DIM_MAX; i++)
    if (IS_INTDIM (dim_array[i], intdim, realdim))
      isdata = true;
    else if (IS_PTRDIM (dim_array[i], intdim, realdim))
      {
	isptr = true;
	if (IS_DATAVAR (pr, dim_array[i], intdim, realdim))
	  isdatafield = true;
	if (IS_NEXTVAR (pr, dim_array[i], intdim, realdim))
	  isnextfield = true;
      }
    else
      arg_assert (0, return false;
    );				/* bad dimension */
  free (dim_array);
  /* keep i unused, since used below to build linear constraints */

  isderef = false;
  if (isptr && !isdata && !isdatafield)
    {
      /*
       * pure pointer constraints, only x[*next] - y[*next] or
       * x, x*next, or x/next for assign
       */
      arg_assert (r->type != DATA_CONS, return false;
	);

      if (e->discr == AP_TEXPR_DIM)
	{
	  r->info.ptr.x = e->val.dim;
	  r->info.ptr.nextx = 0;
	}
      else if (e->discr == AP_TEXPR_NODE)
	{
	  /* operand here can be only -, +, * or / */
	  /* fill x, y */
	  ap_texpr0_t *x, *y;
	  x = y = NULL;
	  switch (e->val.node->op)
	    {
	    case AP_TEXPR_ADD:
	      arg_assert (r->type != OTHER_CONS, return false;
		);
	      /* one of subnodes shall be NEG */
	      /* first operand */
	      if (e->val.node->exprA &&
		  e->val.node->exprA->discr == AP_TEXPR_NODE)
		{
		  if (e->val.node->exprA->val.node->op != AP_TEXPR_NEG
		      && x == NULL)
		    x = e->val.node->exprA;
		  else
		    {
		      if (e->val.node->exprA->val.node->op == AP_TEXPR_NEG &&
			  y == NULL)
			y = e->val.node->exprA->val.node->exprA;
		      else
			arg_assert (0, return false;
			);
		    }
		}
	      else
		{
		  if (e->val.node->exprA &&
		      e->val.node->exprA->discr == AP_TEXPR_DIM && x == NULL)
		    x = e->val.node->exprA;
		  else
		    arg_assert (0, return false;
		    );
		}
	      /* second operand */
	      if (e->val.node->exprB
		  && e->val.node->exprB->discr == AP_TEXPR_NODE)
		{
		  if (e->val.node->exprB->val.node->op != AP_TEXPR_NEG
		      && x == NULL)
		    x = e->val.node->exprB;
		  else
		    {
		      if (e->val.node->exprB->val.node->op == AP_TEXPR_NEG &&
			  y == NULL)
			y = e->val.node->exprB->val.node->exprA;
		      else
			arg_assert (0, return false;
			);
		    }
		}
	      else
		{
		  if (e->val.node->exprB &&
		      e->val.node->exprB->discr == AP_TEXPR_DIM && x == NULL)
		    x = e->val.node->exprB;
		  else		/* for ADD, both operands exist */
		    arg_assert (0, return false;
		    );
		}
	      break;

	    case AP_TEXPR_SUB:
	      arg_assert (r->type != OTHER_CONS, return false;
		);
	      /* first operand */
	      if (e->val.node->exprA && x == NULL &&
		  (e->val.node->exprA->discr == AP_TEXPR_NODE ||
		   e->val.node->exprA->discr == AP_TEXPR_DIM))
		x = e->val.node->exprA;
	      else
		arg_assert (0, return false;
		);

	      /* second operand */
	      if (e->val.node->exprB && y == NULL)
		y = e->val.node->exprB;
	      // TODO: e->val.node->exprB->discr == AP_TEXPR_CST accepted only used for coding INIT
	      else
		arg_assert (0, return false;
		);
	      break;

	    case AP_TEXPR_MUL:
	      arg_assert (r->type == OTHER_CONS, return false;
		);
	      /* (x * next) * next ... */
	      arg_assert (e->val.node->exprB->discr == AP_TEXPR_DIM &&
			  IS_NEXTVAR (pr, e->val.node->exprB->val.dim, intdim,
				      realdim), return false;
		);
	      x = e;
	      break;

	    case AP_TEXPR_DIV:
	      arg_assert (r->type == OTHER_CONS, return true;
		);
	      /* x / next */
	      arg_assert (e->val.node->exprA->discr == AP_TEXPR_DIM &&
			  e->val.node->exprB->discr == AP_TEXPR_DIM &&
			  IS_NEXTVAR (pr, e->val.node->exprB->val.dim, intdim,
				      realdim), return true;
		);
	      x = e->val.node->exprA;
	      isderef = true;
	      break;

	    default:
	      arg_assert (0, return false;
		);
	    }
	  /* fill r from x */
	  r->info.ptr.nextx = 0;
	  while (x && x->discr != AP_TEXPR_DIM)
	    if (x->discr == AP_TEXPR_NODE &&
		x->val.node->op == AP_TEXPR_MUL &&
		x->val.node->exprB->discr == AP_TEXPR_DIM &&
		IS_NEXTVAR (pr, x->val.node->exprB->val.dim, intdim, realdim))
	      {
		r->info.ptr.nextx += 1;
		x = x->val.node->exprA;
	      }
	    else
	      arg_assert (0, return false;
	    );
	  if (x)
	    r->info.ptr.x = x->val.dim;

	  /* fill r from y */
	  r->info.ptr.nexty = 0;
	  while (y && y->discr == AP_TEXPR_NODE)
	    if (y->val.node->op == AP_TEXPR_MUL &&
		y->val.node->exprB->discr == AP_TEXPR_DIM &&
		IS_NEXTVAR (pr, y->val.node->exprB->val.dim, intdim, realdim))
	      {
		r->info.ptr.nexty += 1;
		y = y->val.node->exprA;
	      }
	    else
	      arg_assert (0, return false;
	    );
	  if (y)
	    {
	      if (y->discr == AP_TEXPR_DIM)
		r->info.ptr.y = y->val.dim;
	      else		// y->discr == AP_TEXPR_CST`
		{
		  if (ap_scalar_equal_int (y->val.cst.val.scalar, 0))
		    r->info.ptr.y = intdim + 0;
		  else if (ap_scalar_equal_int (y->val.cst.val.scalar, 1))
		    r->info.ptr.y = intdim + 1;
		  else if (ap_scalar_equal_int (y->val.cst.val.scalar, 2))
		    r->info.ptr.y = intdim + 2;
		  else if (ap_scalar_equal_int (y->val.cst.val.scalar, 3))
		    r->info.ptr.y = intdim + 3;
		  else if (ap_scalar_equal_int (y->val.cst.val.scalar, 4))
		    r->info.ptr.y = intdim + 4;
		  else
		    r->info.ptr.y = intdim + 5;
		}
	    }
	  else
	    r->info.ptr.y = var_ptr_null;
	}
      else
	arg_assert (0, return false;
	);
    }
  else if (isptr && isdata && !isdatafield && !isnextfield)
    {
      /* mixed constraint x -l -y */

    }
  else if (isdatafield && !isnextfield)
    {
      /*
       * mixed constraints, transform into a linear constraint by
       * replacing x->data by one new var of dimension datadim + x
       */
      ap_texpr0_t *linexpr;
      arg_assert (e->discr == AP_TEXPR_NODE, return false;
	);
      /* for assign expression, fill dereferencing field */
      if (e->val.node->op == AP_TEXPR_DIV)
	{
	  /* texpr / data */
	  arg_assert (r->type == OTHER_CONS &&
		      e->val.node->exprB->discr == AP_TEXPR_DIM &&
		      IS_DATAVAR (pr, e->val.node->exprB->val.dim, intdim,
				  realdim), return false;
	    );
	  isderef = true;
	  linexpr = e->val.node->exprA;
	}
      else
	linexpr = e;
      r->type = DATA_CONS;

      /* fill r->info.data.linexpr0 field from linexpr */
      r->info.data.linexpr0 =
	ap_linexpr0_alloc (AP_LINEXPR_DENSE, intdim + realdim);
      ap_scalar_t *cst =
	shape_dataexpr_of_texpr (pr, linexpr, NULL, r->info.data.linexpr0,
				 intdim, realdim);
      if (cst)
	{
	  ap_linexpr0_set_cst_scalar (r->info.data.linexpr0, cst);
	  ap_scalar_free (cst);
	}
    }
  else if (isdata && !isptr)
    {
      /* shall be a linear constraint on data vars only */
      r->type = DATA_CONS;
      r->info.data.linexpr0 =
	ap_linexpr0_alloc (AP_LINEXPR_DENSE, intdim + realdim);
      shape_dataexpr_of_texpr (pr, e, NULL, r->info.data.linexpr0,
			       intdim, realdim);
    }
  else if (!isptr && !isdata && !isdatafield && !isnextfield)
    {
      /* shall be a constant */
      r->type = DATA_CONS;
      r->info.data.linexpr0 =
	ap_linexpr0_alloc (AP_LINEXPR_DENSE, intdim + realdim);
      ap_scalar_t *cst =
	shape_dataexpr_of_texpr (pr, e, NULL, r->info.data.linexpr0,
				 intdim, realdim);
      ap_linexpr0_set_cst_scalar (r->info.data.linexpr0, cst);
      ap_scalar_free (cst);
    }
  else				/* no mixure of data and next dereferencing */
    arg_assert (0, return false;
    );
  return isderef;
}


pcons0_t *
shape_pcons_of_tcons (shape_internal_t * pr, ap_tcons0_t * c,
		      size_t intdim, size_t realdim)
{
  pcons0_t *r;
#ifndef NDEBUG
  fprintf (stdout, "shape_pcons_of_tcons\n");
  ap_tcons0_fprint (stdout, c, NULL);
#endif
  /*
   * tcons may be: - only ptr vars for x[->next]+ == y[->next]+ or
   * x[->next]+ <> y[->next]+ - ptr and data  for constraints using
   * x->data (coded by x*data) or x->next->data (coded by x*next*data)
   * - only data     is an ERROR Only scalar coefficients are allowed.
   */
  if (!c || !c->texpr0 || !ap_tcons0_is_scalar (c))
    return NULL;
  else if ((r = shape_pcons_search (pr, NULL, c)) != NULL)
    /* already in the htable */
    return r;
  else
    {
      size_t i;
      ap_texpr0_t *e = c->texpr0;
      checked_malloc (r, pcons0_t, sizeof (pcons0_t), 1, return NULL;
	);
      switch (c->constyp)
	{
	case AP_CONS_EQ:
	  r->type = EQ_CONS;
	  break;
	case AP_CONS_DISEQ:
	  r->type = NE_CONS;
	  break;

	case AP_CONS_SUPEQ:
	case AP_CONS_SUP:
	  r->type = DATA_CONS;
	  break;
	default:		/* ERROR */
	  arg_assert (0, return r;
	    );
	  break;
	}

      /* fill expression */
      shape_pcons_of_texpr (pr, e, r, intdim, realdim);

      /* put comparison op for data constraints */
      if (r->type == DATA_CONS)
	{
	  r->info.data.constyp = c->constyp;
	  r->info.data.scalar = NULL;
	}
      else			/* pointer constraints */
	arg_assert ((c->constyp == AP_CONS_EQ || c->constyp == AP_CONS_DISEQ),
		    return r;
	);
//TODO:free r ?
      r->lcons = NULL;
      r->tcons = c;
      return shape_pcons_add (pr, r);
    }
}

/* Code for the following functions */
#define PCONS_ARRAY_OF_CONS_ARRAY(pr,array,f,intdim,datadim)      \
   pcons0_array_t *arr1, *arr2, *arr3, *arr4;                     \
   size_t i, s1, s2, s3, s4;                   \
   pcons0_t *cons;                       \
   checked_malloc(arr, pcons0_array_t, sizeof(pcons0_array_t), 1, return NULL;);      \
   checked_malloc(arr1, pcons0_array_t, sizeof(pcons0_array_t), 1, return NULL;);      \
   checked_malloc(arr2, pcons0_array_t, sizeof(pcons0_array_t), 1, return NULL;);      \
   checked_malloc(arr3, pcons0_array_t, sizeof(pcons0_array_t), 1, return NULL;);      \
   checked_malloc(arr4, pcons0_array_t, sizeof(pcons0_array_t), 1, return NULL;);      \
   arr->size = arr1->size = arr2->size = arr3->size = arr4->size = array->size;      \
   s = s1 = s2 = s3 = s4 = 0;                \
   checked_malloc(arr->p, pcons0_t*, sizeof(pcons0_t*), array->size, return NULL;);      \
   checked_malloc(arr1->p, pcons0_t*, sizeof(pcons0_t*), array->size, return NULL;);      \
   checked_malloc(arr2->p, pcons0_t*, sizeof(pcons0_t*), array->size, return NULL;);      \
   checked_malloc(arr3->p, pcons0_t*, sizeof(pcons0_t*), array->size, return NULL;);      \
   checked_malloc(arr4->p, pcons0_t*, sizeof(pcons0_t*), array->size, return NULL;);      \
   for (i = 0; i < array->size; i++)    \
     {                                  \
     cons = f(pr, &array->p[i], intdim, realdim);       \
     if (cons != NULL && cons->type < OTHER_CONS)       \
     {  \
       if (IS_NULLVAR(pr,cons->info.ptr.x,intdim,realdim) ||      \
           IS_NULLVAR(pr,cons->info.ptr.y,intdim,realdim))      \
         { arr->p[s] = cons;    \
         s++;   \
         }      \
       else if (cons->type == EQ_CONS)  \
         { arr1->p[s1] = cons;  \
         s1++;  \
         }      \
       else if (cons->type == REACH_CONS || cons->type == REACHL_CONS)  \
         { arr2->p[s2] = cons;  \
         s2++;  \
         }      \
       else if (cons->type == NE_CONS) \
       { arr3->p[s3] = cons;  \
       s3++;  \
       }      \
     else { arr4->p[s4] = cons;  \
     s4++;  \
     }      \
     } \
     } \
     for (i=0; i < s1; i++)     \
       { arr->p[s] = arr1->p[i];        \
       arr1->p[i] = NULL;       \
       s++;     \
       }        \
     free (arr1->p);   free(arr1); \
     for (i=0; i < s2; i++)     \
       { arr->p[s] = arr2->p[i];        \
       arr2->p[i] = NULL;       \
       s++;     \
       }        \
       free(arr2->p); free (arr2);    \
     for (i=0; i < s3; i++)     \
       { arr->p[s] = arr3->p[i];      \
       arr3->p[i] = NULL;       \
       s++;    \
       }        \
       free(arr3->p); free (arr3);    \
     for (i=0; i < s4; i++)     \
       { arr->p[s] = arr4->p[i];        \
       arr4->p[i] = NULL;       \
       s++;     \
       }        \
       free (arr4->p); free (arr4);


pcons0_array_t *
shape_pcons_array_of_lincons_array (shape_internal_t * pr,
				    ap_lincons0_array_t * array,
				    size_t intdim, size_t realdim)
{
  pcons0_array_t *arr;
  size_t s;
  PCONS_ARRAY_OF_CONS_ARRAY (pr, array, shape_pcons_of_lincons, intdim,
			     realdim);

  return arr;
}

pcons0_array_t *
shape_pcons_array_of_tcons_array (shape_internal_t * pr,
				  ap_tcons0_array_t * array, size_t intdim,
				  size_t realdim)
{
  pcons0_array_t *arr;
  size_t s;
  PCONS_ARRAY_OF_CONS_ARRAY (pr, array, shape_pcons_of_tcons, intdim,
			     realdim);
  return arr;
}

ap_lincons0_t
shape_lincons_of_node (shape_internal_t * pr,
		       ap_lincons0_t * c,
		       size_t * v2n,
		       size_t size, size_t intdim, size_t realdim)
{
  ap_linexpr0_t *l =
    shape_linexpr_of_node (pr, c->linexpr0, v2n, size, intdim, realdim);
  return ap_lincons0_make (c->constyp, l,
			   ((!c->scalar) ? NULL : ap_scalar_alloc_set (c->
								       scalar)));
}

/* ============================================================ */
/* Assignments */
/* ============================================================ */

/* Search a value in the htable using keys lhs,lexpr, and texpr */
passign0_t *
shape_passign_search (shape_internal_t * pr, ap_dim_t lhs,
		      ap_linexpr0_t * lexpr, ap_texpr0_t * texpr)
{
  passign0_t *r, rr;
  unsigned keylen;
  /* search in the htable */
  keylen = offsetof (passign0_t, hh) - offsetof (passign0_t, lhs);
  r = NULL;
  rr.lhs = lhs;
  rr.lexpr = lexpr;
  rr.texpr = texpr;
  HASH_FIND (hh, pr->passigns, &rr.lhs, keylen, r);
  return r;
}

/* Add a value to the htable */
passign0_t *
shape_passign_add (shape_internal_t * pr, passign0_t * a)
{
  passign0_t *r;
  unsigned keylen;
  /* search in the htable */
  keylen = offsetof (passign0_t, hh) - offsetof (passign0_t, lhs);
  r = NULL;
  HASH_FIND (hh, pr->passigns, &a->lhs, keylen, r);
  if (!r)
    {
      HASH_ADD (hh, pr->passigns, lhs, keylen, a);
      HASH_FIND (hh, pr->passigns, &a->lhs, keylen, r);
    }
  return r;
}

passign0_t *
shape_passign_of_linexpr (shape_internal_t * pr,
			  ap_dim_t lhs, ap_linexpr0_t * rhs,
			  size_t intdim, size_t realdim)
{
  size_t i;
  passign0_t *r;

  arg_assert (rhs && lhs < intdim + realdim, return NULL;
    );

  if ((r = shape_passign_search (pr, lhs, rhs, NULL)) != NULL)
    return r;

  checked_malloc (r, passign0_t, sizeof (passign0_t), 1, return NULL;
    );

  /* fill lhs part */
  r->x = lhs;
  r->nextx = 0;
  if (IS_PTRDIM (lhs, intdim, realdim) && ap_linexpr0_is_real (rhs, intdim))
    {
      /* x = y ptr assignment, the expression can be only y */
      pcons0_t *c;
      checked_malloc (c, pcons0_t, sizeof (pcons0_t), 1, return NULL;
	);
      c->type = OTHER_CONS;
      shape_pcons_of_linexpr (pr, rhs, c, intdim, realdim);
      r->info.ptr.y = c->info.ptr.x;
      r->info.ptr.nexty = 0;
      free (c);
    }
  else if (IS_INTDIM (lhs, intdim, realdim)
	   && ap_linexpr0_is_integer (rhs, intdim))
    {
      r->info.data = ap_linexpr0_copy (rhs);
    }
  else
    arg_assert (0, return NULL;
    );
//TODO:free r ?
  r->type = NO_MODIFIER;
  r->lhs = lhs;
  r->lexpr = rhs;
  r->texpr = NULL;
  return shape_passign_add (pr, r);
}

passign0_array_t *
shape_passign_of_linexpr_array (shape_internal_t * pr,
				ap_dim_t * lhs, ap_linexpr0_t ** rhs,
				size_t size, size_t intdim, size_t realdim)
{
  size_t i;
  passign0_array_t *r = NULL;
  checked_malloc (r, passign0_array_t, sizeof (passign0_array_t), 1,
		  return NULL;
    );
  r->size = size;
  checked_malloc (r->p, passign0_t *, sizeof (passign0_t *), size,
		  return NULL;
    );
  for (i = 0; i < size; i++)
    r->p[i] = shape_passign_of_linexpr (pr, lhs[i], rhs[i], intdim, realdim);
  return r;
}

passign0_t *
shape_passign_of_texpr (shape_internal_t * pr,
			ap_dim_t lhs, ap_texpr0_t * rhs,
			size_t intdim, size_t realdim)
{
  size_t i;
  passign0_t *r;
  pcons0_t *c;
  bool isderef;

  arg_assert (rhs && lhs < intdim + realdim, return NULL;
    );

  if ((r = shape_passign_search (pr, lhs, NULL, rhs)) != NULL)
    return r;

  checked_malloc (r, passign0_t, sizeof (passign0_t), 1, return NULL;
    );

  /* fill lhs part with a ptr dimension and set dereferencing part */
  r->x = lhs;
  checked_malloc (c, pcons0_t, sizeof (pcons0_t), 1, return NULL;
    );
  c->type = OTHER_CONS;
  isderef = shape_pcons_of_texpr (pr, rhs, c, intdim, realdim);
  if (IS_PTRDIM (lhs, intdim, realdim))
    {
      /* pointer assignment */
      r->nextx = (isderef) ? 1 : 0;
      if (c->type == DATA_CONS)
	{
	  r->type = DATA_MODIFIER;
	  r->info.data = c->info.data.linexpr0;
	  c->info.data.linexpr0 = NULL;	/* to secure free c  */
	}
      else
	{
	  r->type = (isderef) ? NEXT_MODIFIER : NO_MODIFIER;
	  r->info.ptr.y = c->info.ptr.x;
	  r->info.ptr.nexty = c->info.ptr.nextx;
	}
    }
  else if (lhs < intdim)
    {
      arg_assert (!isderef && c->type == DATA_CONS, return NULL;
	);
      r->nextx = 0;
      r->type = NO_MODIFIER;
      r->info.data = c->info.data.linexpr0;
      c->info.data.linexpr0 = NULL;	/* to secure free c  */
    }
  else
    arg_assert (0, return NULL;
    );
  free (c);
  r->lhs = lhs;
  r->lexpr = NULL;
  r->texpr = rhs;
//TODO:free r ?
  return shape_passign_add (pr, r);
}

passign0_array_t *
shape_passign_of_texpr_array (shape_internal_t * pr,
			      ap_dim_t * lhs, ap_texpr0_t ** rhs, size_t size,
			      size_t intdim, size_t realdim)
{
  size_t i;
  passign0_array_t *r = NULL;
  checked_malloc (r, passign0_array_t, sizeof (passign0_array_t), 1,
		  return NULL;
    );
  r->size = size;
  checked_malloc (r->p, passign0_t *, sizeof (passign0_t *), size,
		  return NULL;
    );
  for (i = 0; i < size; i++)
    r->p[i] = shape_passign_of_texpr (pr, lhs[i], rhs[i], intdim, realdim);
  return r;
}


/* ============================================================ */
/* Apron expressions for Shapes */
/* ============================================================ */

const char *exprname[] = { "ptr ", "reach ", "reachl ", "lindata ",
  "next ", "deref ", "data ", "deref_data"
};

ap_linexpr0_t *
shape_linexpr_random (shape_internal_t * pr, exprmode_t mode, size_t datadim,
		      size_t ptrdim)
{
  int dim = datadim + ptrdim;
  ap_linexpr0_t *l = ap_linexpr0_alloc (AP_LINEXPR_DENSE, dim);
  long int x, y, len;
  if (mode <= expr_reachl)
    {
      /* x */
      x = RANDOM_PTRVAR (pr, datadim, ptrdim);
      ap_coeff_set_scalar_int (l->p.coeff + x, 1);
      /* - y */
      do
	{
	  y = RANDOM_PTRVAR_OR_NULL (pr, datadim, ptrdim);
	}
      while (y == x);
      if (mode >= expr_reach)
	ap_coeff_set_scalar_int (l->p.coeff + y, -1);
      /* - len */
      len = lrand48 () % datadim;
      if (mode == expr_reachl)
	ap_coeff_set_scalar_int (l->p.coeff + len, -1);
    }
  else
    {
      size_t i;
      if (lrand48 () % 20 > 12)
	{
	  for (i = LEN_DIM (datadim, ptrdim); i < datadim; i++)
	    {
	      int n = lrand48 () % 20 - 10;
	      ap_coeff_set_scalar_int (l->p.coeff + i, n);
	    }
	}
      else
	{
	  for (i = 0; i < LEN_DIM (datadim, ptrdim); i++)
	    {
	      int n = lrand48 () % 3 - 1;
	      ap_coeff_set_scalar_int (l->p.coeff + i, n);
	    }
	}
    }
  return l;
}

/* build radomly tree expressions */
ap_texpr0_t *
shape_texpr_random (shape_internal_t * pr, exprmode_t mode, size_t datadim,
		    size_t ptrdim)
{
  size_t x;

  switch (mode)
    {
    case expr_next:		/* generate x * next */
      /* choose a variable x randomly, either if it points to null */
      x = RANDOM_PTRVAR (pr, datadim, ptrdim);
      return ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_dim (x),
			      ap_texpr0_dim (datadim + var_ptr_next),
			      AP_RTYPE_INT, AP_RDIR_NEAREST);

    case expr_deref:		/* x / next or x*next/next */
      /* choose a variable x randomly, either if it points to null */
      x = RANDOM_PTRVAR_OR_NULL (pr, datadim, ptrdim);
      if (lrand48 () % 10 < 2 && x != (datadim + var_ptr_null))
	return ap_texpr0_binop (AP_TEXPR_DIV,
				ap_texpr0_binop (AP_TEXPR_MUL,
						 ap_texpr0_dim (x),
						 ap_texpr0_dim (datadim +
								var_ptr_next),
						 AP_RTYPE_INT,
						 AP_RDIR_NEAREST),
				ap_texpr0_dim (datadim + var_ptr_next),
				AP_RTYPE_INT, AP_RDIR_NEAREST);
      else
	return ap_texpr0_binop (AP_TEXPR_DIV,
				ap_texpr0_dim (x),
				ap_texpr0_dim (datadim + var_ptr_next),
				AP_RTYPE_INT, AP_RDIR_NEAREST);

    case expr_reach:
      {				/* generate x[* next] - y[* next] */
	size_t y;
	ap_texpr0_t *xn, *yn;
	/* choose a variable x randomly, either if it points to null */
	x = RANDOM_PTRVAR (pr, datadim, ptrdim);
	/*
	 * choose a variable y randomly but different from x, either if it
	 * points to null
	 */
	do
	  {
	    y = RANDOM_PTRVAR (pr, datadim, ptrdim);
	  }
	while (x == y);
	if (lrand48 () % 10 > 8)
	  {
	    xn = ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_dim (x),
				  ap_texpr0_dim (datadim + var_ptr_next),
				  AP_RTYPE_INT, AP_RDIR_NEAREST);
	    yn = ap_texpr0_dim (y);
	  }
	else
	  {
	    yn = ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_dim (y),
				  ap_texpr0_dim (datadim + var_ptr_next),
				  AP_RTYPE_INT, AP_RDIR_NEAREST);
	    xn = ap_texpr0_dim (x);
	  }
	return ap_texpr0_binop (AP_TEXPR_SUB, xn, yn,
				AP_RTYPE_INT, AP_RDIR_NEAREST);
      }

    case expr_data:		/* v1 + x*data or cst + x*data */
      {
	ap_texpr0_t *e;
	x = RANDOM_PTRVAR (pr, datadim, ptrdim);
	e =
	  ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_dim (x),
			   ap_texpr0_dim ((ap_dim_t)
					  (datadim + var_ptr_data)),
			   AP_RTYPE_INT, AP_RDIR_NEAREST);
	switch (lrand48 () % 4)
	  {
	  case 0:		/* cst + x*data */
	    e = ap_texpr0_binop (AP_TEXPR_MUL,
				 ap_texpr0_cst_scalar_double ((double)
							      (lrand48 () %
							       5 + 1)), e,
				 AP_RTYPE_INT, AP_RDIR_NEAREST);
	    break;
	  case 1:		/* cst + cst * x*data */
	    e = ap_texpr0_binop (AP_TEXPR_MUL,
				 ap_texpr0_cst_scalar_double ((double)
							      (lrand48 () %
							       5 + 1)), e,
				 AP_RTYPE_INT, AP_RDIR_NEAREST);
	    e =
	      ap_texpr0_binop (AP_TEXPR_ADD,
			       ap_texpr0_cst_scalar_double ((double)
							    (lrand48 () % 5 +
							     1)), e,
			       AP_RTYPE_INT, AP_RDIR_NEAREST);
	    break;
	  case 2:		/* v + x*data */
	    e =
	      ap_texpr0_binop (AP_TEXPR_ADD,
			       ap_texpr0_dim (lrand48 () % datadim), e,
			       AP_RTYPE_INT, AP_RDIR_NEAREST);
	    break;
	  default:		/* v + cst*x*data */
	    e = ap_texpr0_binop (AP_TEXPR_MUL,
				 ap_texpr0_cst_scalar_double ((double)
							      (lrand48 () %
							       5 + 1)), e,
				 AP_RTYPE_INT, AP_RDIR_NEAREST);
	    e =
	      ap_texpr0_binop (AP_TEXPR_ADD,
			       ap_texpr0_dim (lrand48 () % datadim), e,
			       AP_RTYPE_INT, AP_RDIR_NEAREST);
	    break;
	  }
	return e;
      }
    default:			/* expr_data / data */
      return ap_texpr0_binop (AP_TEXPR_DIV,
			      shape_texpr_random (pr, expr_data, datadim,
						  ptrdim),
			      ap_texpr0_dim (datadim + var_ptr_data),
			      AP_RTYPE_INT, AP_RDIR_NEAREST);
    }
  return NULL;
}

/** Build constraint x op cst */
ap_lincons0_t
shape_lincons_x_cst (ap_constyp_t op, size_t x, int cst,
		     size_t datadim, size_t ptrdim)
{
  int dim = datadim + ptrdim;
  ap_linexpr0_t *l = ap_linexpr0_alloc (AP_LINEXPR_DENSE, dim);
  ap_scalar_t *c = ap_scalar_alloc ();
  ap_coeff_set_scalar_int (l->p.coeff + x, 1);
  ap_scalar_set_int (c, cst);
  return ap_lincons0_make (op, l, c);
}

/** Build expression x - y */
ap_linexpr0_t *
shape_linexpr_x_y_l (int cx, size_t x, int cy, size_t y, int cl, size_t l,
		     size_t datadim, size_t ptrdim)
{
  int dim = datadim + ptrdim;
  ap_linexpr0_t *lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, dim);
  ap_coeff_set_scalar_int (lexpr->p.coeff + x, cx);
  ap_coeff_set_scalar_int (lexpr->p.coeff + y, cy);
  ap_coeff_set_scalar_int (lexpr->p.coeff + l, cl);
  return lexpr;
}

ap_lincons0_t
shape_lincons_x_y_l (ap_constyp_t op, int cx, size_t x, int cy, size_t y,
		     int cl, size_t l, size_t datadim, size_t ptrdim)
{
  return ap_lincons0_make (op,
			   shape_linexpr_x_y_l (cx, x, cy, y, cl, l, datadim,
						ptrdim), NULL);
}

/* Generate linear constraint:
 * cx*x + cy*y + cv*v + cst op codeop
 */
ap_lincons0_t
shape_lincons_x_y_v_cst (ap_constyp_t op, size_t code, int cx, size_t x,
			 int cy, size_t y, int cv, size_t v, int cst,
			 size_t datadim, size_t ptrdim)
{
  int dim = datadim + ptrdim;
  ap_linexpr0_t *lexpr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, dim);
  ap_coeff_set_scalar_int (lexpr->p.coeff + x, cx);
  ap_coeff_set_scalar_int (lexpr->p.coeff + y, cy);
  ap_coeff_set_scalar_int (lexpr->p.coeff + v, cv);
  ap_linexpr0_set_cst_scalar_int (lexpr, cst);
  ap_scalar_t *codeop = ap_scalar_alloc ();
  ap_scalar_set_int (codeop, code);
  return ap_lincons0_make (op, lexpr, codeop);
}

/** Build expression x(*next)- y(*next) */
ap_texpr0_t *
shape_texpr_x_y (size_t x, bool nextx, size_t y, bool nexty, size_t datadim,
		 size_t ptrdim)
{
  ap_texpr0_t *tx, *ty, *tnext;
  tx = ap_texpr0_dim (x);
  ty = ap_texpr0_dim (y);
  tnext = ap_texpr0_dim (datadim + var_ptr_next);
  if (nextx)
    tx = ap_texpr0_binop (AP_TEXPR_MUL, tx, tnext, AP_RTYPE_INT, AP_RDIR_RND);
  if (nexty)
    ty = ap_texpr0_binop (AP_TEXPR_MUL, ty, tnext, AP_RTYPE_INT, AP_RDIR_RND);
  return ap_texpr0_binop (AP_TEXPR_SUB, tx, ty, AP_RTYPE_INT, AP_RDIR_RND);
}

ap_tcons0_t
shape_tcons_x_y (ap_constyp_t op, size_t x, bool nextx, size_t y, bool nexty,
		 size_t datadim, size_t ptrdim)
{
  return ap_tcons0_make (op,
			 shape_texpr_x_y (x, nextx, y, nexty, datadim,
					  ptrdim), NULL);
}

/** Build expression x(*data)+y*data+v+cst */
ap_texpr0_t *
shape_texpr_x_y_v_cst_data (int cx, size_t x, int cy, size_t y,
			    int cv, size_t v, int cst, bool rhs,
			    size_t datadim, size_t ptrdim)
{
  ap_texpr0_t *tdata, *t;
  tdata = ap_texpr0_dim (datadim + var_ptr_data);
  if (cx)
    t = ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_cst_scalar_int (cx),
			 ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_dim (x),
					  tdata, AP_RTYPE_INT, AP_RDIR_RND),
			 AP_RTYPE_INT, AP_RDIR_RND);
  else
    t = NULL;
  if (cy)
    {
      ap_texpr0_t *ty =
	ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_cst_scalar_int (cy),
			 ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_dim (y),
					  tdata, AP_RTYPE_INT, AP_RDIR_RND),
			 AP_RTYPE_INT, AP_RDIR_RND);
      t =
	(t == NULL) ? ty : ap_texpr0_binop (AP_TEXPR_ADD, t, ty, AP_RTYPE_INT,
					    AP_RDIR_RND);
    }
  if (cv)
    {
      ap_texpr0_t *tv =
	ap_texpr0_binop (AP_TEXPR_MUL, ap_texpr0_cst_scalar_int (cv),
			 ap_texpr0_dim (v),
			 AP_RTYPE_INT, AP_RDIR_RND);
      t =
	(t == NULL) ? tv : ap_texpr0_binop (AP_TEXPR_ADD, t, tv, AP_RTYPE_INT,
					    AP_RDIR_RND);
    }
  if (cst || !t)
    {
      ap_texpr0_t *tcst = ap_texpr0_cst_scalar_int (cst);
      t =
	(t == NULL) ? tcst : ap_texpr0_binop (AP_TEXPR_ADD, t, tcst,
					      AP_RTYPE_INT, AP_RDIR_RND);
    }
  if (t && rhs)
    t = ap_texpr0_binop (AP_TEXPR_DIV, t, tdata, AP_RTYPE_INT, AP_RDIR_RND);
  return t;
}

/** Build expression x(*next) */
ap_texpr0_t *
shape_texpr_x_next (size_t x, int nextx, size_t datadim, size_t ptrdim)
{
  ap_texpr0_t *tx, *tnext;
  tx = ap_texpr0_dim (x);
  tnext = ap_texpr0_dim (datadim + var_ptr_next);
  if (nextx > 0)
    tx = ap_texpr0_binop (AP_TEXPR_MUL, tx, tnext, AP_RTYPE_INT, AP_RDIR_RND);
  else if (nextx < 0)
    tx = ap_texpr0_binop (AP_TEXPR_DIV, tx, tnext, AP_RTYPE_INT, AP_RDIR_RND);
  return tx;
}


/** Build expression x(*next)- cst */
ap_texpr0_t *
shape_texpr_x_cst (size_t x, bool nextx, int cst, size_t datadim,
		   size_t ptrdim)
{
  ap_texpr0_t *tx, *ty;
  tx = shape_texpr_x_next (x, (nextx) ? 1 : 0, datadim, ptrdim);
  ty = ap_texpr0_cst_scalar_int (cst);
  return ap_texpr0_binop (AP_TEXPR_SUB, tx, ty, AP_RTYPE_INT, AP_RDIR_RND);
}

ap_tcons0_t
shape_tcons_x_cst (ap_constyp_t op, size_t x, bool nextx, int cst,
		   size_t datadim, size_t ptrdim)
{
  return ap_tcons0_make (op,
			 shape_texpr_x_cst (x, nextx, cst, datadim,
					    ptrdim), NULL);
}

ap_texpr0_t *
shape_texpr_singleton (int n)
{
  return ap_texpr0_cst_scalar_int (n);
}

/* Create tree expression src/new */
ap_texpr0_t *
shape_texpr_split (size_t src, size_t new)
{
  ap_texpr0_t *expr_src = ap_texpr0_dim ((ap_dim_t) src);
  ap_texpr0_t *expr_new = ap_texpr0_dim ((ap_dim_t) new);
  return ap_texpr0_binop (AP_TEXPR_DIV, expr_src, expr_new, AP_RTYPE_REAL,
			  AP_RDIR_RND);
}

/* Create an array of texpr of size from the array nodes[size][max] given by
 * n_1 / (n2 /  .... / n_max)
 */
ap_texpr0_t **
shape_texpr_merge (size_t ** nodes, size_t size, size_t max)
{
  assert (size > 0 && max > 0);
  size_t i, j;
  ap_texpr0_t **r = (ap_texpr0_t **) malloc (size * sizeof (ap_texpr0_t *));
  for (i = 0; i < size; i++)
    {				// create r[i] from nodes[i]
      ap_texpr0_t *tmp = ap_texpr0_dim ((ap_dim_t) nodes[i][max - 1]);
      if (max >= 2)
	for (j = max - 2; j != ((size_t) - 1); j--)
	  tmp =
	    ap_texpr0_binop (AP_TEXPR_DIV,
			     ap_texpr0_dim ((ap_dim_t) nodes[i][j]), tmp,
			     AP_RTYPE_REAL, AP_RDIR_RND);
      r[i] = tmp;
    }
  return r;
}

/* Modify e wrt v2n[realdim]
 *
 */
ap_linexpr0_t *
shape_linexpr_of_node (shape_internal_t * pr,
		       ap_linexpr0_t * e,
		       size_t * v2n,
		       size_t size, size_t intdim, size_t realdim)
{
  ap_linexpr0_t *l = ap_linexpr0_alloc (AP_LINEXPR_DENSE, intdim + size);
  size_t i, d;
  ap_coeff_t *coeff;
  ap_linexpr0_ForeachLinterm (e, i, d, coeff)
  {
    if (!ap_coeff_zero (coeff))
      {
	arg_assert ((coeff->discr == AP_COEFF_SCALAR), return NULL;
	  );
	if (IS_PTRVAR (NULL, d, intdim, realdim))
	  {
	    size_t v = DIM2PTR (d, intdim);
	    size_t n = 0;
	    arg_assert (v > PTRVAR_NULL (pr, intdim, realdim)
			&& v < realdim, return NULL;
	      );
	    n = v2n[v];
	    ap_coeff_set (l->p.coeff + intdim + n, coeff);
	  }
	else
	  ap_coeff_set (l->p.coeff + d, coeff);
      }
  }
  /* constant */
  coeff = ap_coeff_alloc (AP_COEFF_SCALAR);
  ap_linexpr0_get_cst (coeff, e);
  ap_linexpr0_set_cst (l, coeff);
  ap_coeff_free (coeff);
#ifndef NDEBUG
  fprintf (stdout, "\n\t shape_linexpr_of_node: ");
  ap_linexpr0_fprint (stdout, l, NULL);
  fprintf (stdout, "\n");
#endif
  return l;
}


void
shape_dimperm_copy (ap_dimperm_t * dst, size_t begin, ap_dimperm_t * src)
{
  size_t i;
  for (i = 0; i < src->size && (i + begin) < dst->size; i++)
    dst->dim[begin + i] = begin + src->dim[i];

#ifndef NDEBUG
  fprintf (stdout, "\nsrc: ");
  ap_dimperm_fprint (stdout, src);
  fprintf (stdout, " dst: ");
  ap_dimperm_fprint (stdout, dst);
  fprintf (stdout, "\n");
#endif

}

void
shape_dimchange_sort (ap_dimchange_t * p)
{
  size_t i, j, k, size;
  ap_dim_t d;
  size = p->intdim + p->realdim;
  if (!p || !p->dim || !size)
    return;
  /* insertion sort */
#ifndef NDEBUG
  fprintf (stdout, "*** before insertion sort: [");
  for (i = 0; i < size; i++)
    fprintf (stdout, "%d,", p->dim[i]);
  fprintf (stdout, "]\n");
#endif
  for (i = 1; i < size; i++)
    {
      j = 0;
      while (j != i && p->dim[j] <= p->dim[i])
	j++;
      if (j < i)
	{
	  d = p->dim[i];
	  for (k = i - 1; k >= j; k--)
	    p->dim[k + 1] = p->dim[k];
	  p->dim[j] = d;
	}
    }
#ifndef NDEBUG
  fprintf (stdout, "*** after insertion sort: [");
  for (i = 0; i < size; i++)
    fprintf (stdout, "%d,", p->dim[i]);
  fprintf (stdout, "]\n");
#endif
}

/* ============================================================ */
/* Printing */
/* ============================================================ */

char **shape_name_of_dim = NULL;
void
shape_init_name_of_dim (size_t datadim)
{

  size_t i;
  shape_name_of_dim =
    (char **) malloc ((datadim + MAX_SEGM_DIM) * sizeof (char *));
  for (i = 0; i < LEN_DIM (datadim, 0); i++)
    {
      shape_name_of_dim[i] = (char *) malloc (3 * sizeof (char));
      shape_name_of_dim[i][0] = 'l';
      shape_name_of_dim[i][1] = '0' + i;
      shape_name_of_dim[i][2] = '\0';
    }
  for (; i < datadim; i++)
    {
      shape_name_of_dim[i] = (char *) malloc (4 * sizeof (char));
      shape_name_of_dim[i][0] = 'd';
      shape_name_of_dim[i][1] = '0' + i / 10;
      shape_name_of_dim[i][2] = '0' + i % 10;
      shape_name_of_dim[i][3] = '\0';
    }
  for (i = 0; i < MAX_SEGM_DIM; i++)
    {
      shape_name_of_dim[datadim + i] = (char *) malloc (4 * sizeof (char));
      shape_name_of_dim[datadim + i][0] = 'n';
      shape_name_of_dim[datadim + i][1] = '0' + i / 10;
      shape_name_of_dim[datadim + i][2] = '0' + i % 10;
      shape_name_of_dim[datadim + i][3] = '\0';
    }
}
