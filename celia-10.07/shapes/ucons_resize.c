/*
 * ucons_resize.c
 *
 * Projection, changes of dimension, variable permutation.
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
#include "uthash.h"
#include "ucons.h"
#include "ucons_fun.h"
#include "ucons_internal.h"
#include "shape_macros.h"
#include "apron2shape.h"

#if defined (UCONS_DCONS_OCT_P21) || defined (UCONS_DCONS_POLY_P21)
ap_dim_t * fold_dim = NULL;
size_t size_fold_dim=0;
size_t first = 0;
#endif


ap_lincons0_t cons_dy0_dy1(size_t intdim, size_t realdim, ap_dim_t dy0, ap_dim_t dy1){

	ap_lincons0_t cons;
	ap_linexpr0_t *linexpr;

	linexpr = ap_linexpr0_alloc ( AP_LINEXPR_DENSE, intdim + 2 * realdim + 4);
	ap_linexpr0_set_coeff_scalar_int (linexpr,intdim + 2 * realdim + 2 , -1);
	ap_linexpr0_set_coeff_scalar_int (linexpr,intdim + 2 * realdim + 3 , 1);
	cons = ap_lincons0_make (AP_CONS_SUPEQ, linexpr, NULL);

	ap_lincons0_fprint(stdout,&cons,NULL);

	return cons;

}

ap_lincons0_t cons_dy0(size_t intdim, size_t realdim, ap_dim_t dy0, ap_dim_t n){

	ap_lincons0_t cons;
	ap_linexpr0_t *linexpr;

	linexpr = ap_linexpr0_alloc ( AP_LINEXPR_DENSE, intdim + 2 * realdim + 2);
	ap_linexpr0_set_coeff_scalar_int (linexpr,intdim + 2 * realdim + 1 , 1);
	ap_linexpr0_set_coeff_scalar_int (linexpr,n , -1);
	cons = ap_lincons0_make (AP_CONS_SUPEQ, linexpr, NULL);

	ap_lincons0_fprint(stdout,&cons,NULL);
	return cons;

}

ap_lincons0_t cons_k_m(size_t intdim, size_t realdim, ap_dim_t k, ap_dim_t m){

	ap_lincons0_t cons;
	ap_linexpr0_t *linexpr;

	linexpr = ap_linexpr0_alloc ( AP_LINEXPR_DENSE, intdim + 2 * realdim );
	ap_linexpr0_set_coeff_scalar_int (linexpr,k , -1);
	ap_linexpr0_set_coeff_scalar_int (linexpr,m , 1);
	cons = ap_lincons0_make (AP_CONS_SUPEQ, linexpr, NULL);

	ap_lincons0_fprint(stdout,&cons,NULL);
	return cons;

}


/* ============================================================ */
/* Projections */
/* ============================================================ */

/* TODO: priority 0 */
/*
 * not used because we suppose that all ptr variables are declared from the
 * beginning
 */
ucons_t *
ucons_forget_array (ap_manager_t * man,
		bool destructive, ucons_t * a,
		ap_dim_t * tdim, size_t size, bool project)
		{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_FORGET_ARRAY, 0);
	
	if (!a) return NULL;
	
	if (!a->econs) return NULL;
	
	ucons_t * r = ucons_copy_internal(pr, a);
	r->econs = ap_abstract0_forget_array(pr->man_dcons, destructive, a->econs, tdim, size, project);
 	return r;
	
	if (destructive)
		ucons_free_internal(pr,a);
	/*	
	ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			"not implemented");
	return a;
	*/
	}


/* ============================================================ */
/* Change and permutation of dimensions */
/* ============================================================ */

/* TODO: priority 0 */
ucons_t *
ucons_add_dimensions (ap_manager_t * man,
		bool destructive, ucons_t * a,
		ap_dimchange_t * dimchange, bool project)
		{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);

	arg_assert (a && dimchange &&
			dimchange->intdim == 0 && dimchange->realdim == 1 &&
			dimchange->dim[0] == (a->datadim + a->segmentdim), return NULL;);

	ucons_t *r = ucons_alloc_internal (pr, a->datadim, a->segmentdim + 1);

	ap_dimchange_t dimadd;
	dimadd.realdim = 0;
	dimadd.intdim = 2;
	dimadd.dim = (ap_dim_t *) malloc (2 * sizeof (ap_dim_t));
	dimadd.dim[0] = a->datadim + a->segmentdim;
	dimadd.dim[1] = a->datadim + 2 * a->segmentdim;
	r->econs =
			ap_abstract0_add_dimensions (pr->man_dcons, false, a->econs, &dimadd,
					project);

	pattern_t *s,*rt=NULL;
	size_t u_seg,intdim, nr_y;
	unsigned keylen;

	for(s=r->udcons; s!=NULL;s=s->hh.next){

		nr_y = pr->PI[s->key.type].nr_y;
		u_seg = pr->PI[s->key.type].u_seg;
		unsigned keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);


		HASH_FIND(hh,a->udcons,&s->key,keylen,rt);
		if(rt){
			s->dcons=ap_abstract0_add_dimensions (pr->man_dcons, false, rt->dcons, &dimadd,
					project);
		}
		else{
			intdim = r->datadim + 2 * r->segmentdim + 2 * nr_y;
			s->dcons=ap_abstract0_top (pr->man_dcons,intdim, 0);
		}

	}

	r->n2p=pattern_key_set_copy(pr, a->n2p, a->segmentdim);

	checked_realloc(r->n2p,pattern_key_set_t, (a->segmentdim+1), sizeof(pattern_key_set_t), return NULL;);

	r->n2p[a->segmentdim].p=NULL;
	r->n2p[a->segmentdim].size=0;

	/* add the patterns corresponding to the new dimension */
	/* no already added by alloc_internal */

	size_t dim = a->segmentdim+1;

#if defined (UCONS_DCONS_OCT_P11) || defined (UCONS_DCONS_POLY_P11)
	r = add_pattern_1( pr, r, a->segmentdim);
#elif defined (UCONS_DCONS_OCT_P12) || defined (UCONS_DCONS_POLY_P12)
	r = add_pattern_1( pr, r, a->segmentdim);
	r = add_pattern_1_2( pr, r, a->segmentdim);
#else
	r = add_pattern_2_1( pr, r, a->segmentdim);
	//	r = add_pattern_1_3( pr, r,a->segmentdim);
#endif

	if (destructive)
		ucons_free_internal (pr, a);

	return r;
		}

/* y1 < y2  : PI[3] */
ucons_t * add_pattern_1_2( ucons_internal_t *pr, ucons_t *r, size_t dim){

	if(!r) return NULL;
	if(!r->n2p) return NULL;
	if(!(dim<r->segmentdim)) return NULL;

	size_t size = r->n2p[dim].size;
	checked_realloc(r->n2p[dim].p, pattern_key_t*,(r->n2p[dim].size+1),sizeof(pattern_key_t *),return NULL;);
	checked_malloc(r->n2p[dim].p[size], pattern_key_t,1,sizeof(pattern_key_t)+1*sizeof(size_t),return NULL;);

	r->n2p[dim].size=r->n2p[dim].size+1;
	r->n2p[dim].p[r->n2p[dim].size-1]->type = 3;
	r->n2p[dim].p[r->n2p[dim].size-1]->segments[0]=dim;

	return r;
}

/* y : PI[0]*/
ucons_t * add_pattern_1( ucons_internal_t *pr, ucons_t *r, size_t dim){

	if(!r) return NULL;
	if(!r->n2p) return NULL;
	if(!(dim<r->segmentdim)) return NULL;

	size_t size = r->n2p[dim].size;
	checked_realloc(r->n2p[dim].p, pattern_key_t*,(r->n2p[dim].size+1),sizeof(pattern_key_t *),return NULL;);
	checked_malloc(r->n2p[dim].p[size], pattern_key_t,1,sizeof(pattern_key_t)+1*sizeof(size_t),return NULL;);

	r->n2p[dim].size=r->n2p[dim].size+1;
	r->n2p[dim].p[r->n2p[dim].size-1]->type = 0;
	r->n2p[dim].p[r->n2p[dim].size-1]->segments[0]=dim;

	return r;
}



/* \forall y1 \in [1] y2 \in [2] y1 = y2 =>... : PI[2] */
ucons_t * add_pattern_2_1( ucons_internal_t *pr, ucons_t *r, size_t dim){
	size_t i,j,size_k,size_j;

	unsigned keylen;
	size_t u_seg,nr_y;
	pattern_t *rn, *rt;

	size_t size;

	u_seg = pr->PI[1].u_seg;
	keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);
	nr_y = pr->PI[1].nr_y;
	for(j=1;j<r->segmentdim-1;j++){
		size = r->n2p[dim].size;
		checked_realloc(r->n2p[dim].p, pattern_key_t*,(r->n2p[dim].size+1),sizeof(pattern_key_t *),return NULL;);
		checked_malloc(r->n2p[dim].p[size], pattern_key_t,1,sizeof(pattern_key_t)+1*sizeof(size_t),return NULL;);

		r->n2p[dim].size=r->n2p[dim].size+1;
		r->n2p[dim].p[r->n2p[dim].size-1]->type = 1;
		r->n2p[dim].p[r->n2p[dim].size-1]->segments[0]=j;
		r->n2p[dim].p[r->n2p[dim].size-1]->segments[1]=dim;

		size_j = r->n2p[j].size;
		checked_realloc(r->n2p[j].p, pattern_key_t*,(r->n2p[j].size+1),sizeof(pattern_key_t *),return NULL;);
		checked_malloc(r->n2p[j].p[size_j], pattern_key_t,1,sizeof(pattern_key_t)+1*sizeof(size_t),return NULL;);

		r->n2p[j].size=r->n2p[j].size+1;
		r->n2p[j].p[r->n2p[j].size-1]->type = 1;
		r->n2p[j].p[r->n2p[j].size-1]->segments[0]=j;
		r->n2p[j].p[r->n2p[j].size-1]->segments[1]=dim;
	}
	return r;
}

ucons_t * add_pattern_3_1( ucons_internal_t *pr, ucons_t *r, size_t dim){

	size_t i,j,k,size_k,size_j,size_l;

	unsigned keylen;
	size_t u_seg;
	pattern_t *rn, *rt;

	u_seg = pr->PI[2].u_seg;
	keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

	for(j=1;j<r->segmentdim;j++)
		for(k=j+1;k<r->segmentdim;k++)
		{

			checked_malloc(rn,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return NULL ;);
			memset(rn, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

			rn->dcons=ap_abstract0_bottom(pr->man_dcons, r->datadim, r->segmentdim+u_seg  );

			rn->key.type=1;

			rn->key.segments[0]=j;
			rn->key.segments[1]=k;
			rn->key.segments[2]=dim;

			HASH_FIND(hh,r->udcons,&rn->key, keylen,rt);

			if(rt==NULL){

				HASH_ADD(hh,r->udcons,key,keylen,rn);

				checked_realloc(r->n2p[j].p, pattern_key_t*,(r->n2p[j].size+1),sizeof(pattern_key_t*),return NULL;);
				checked_realloc(r->n2p[k].p, pattern_key_t*,(r->n2p[k].size+1),sizeof(pattern_key_t*),return NULL;);
				checked_realloc(r->n2p[dim].p, pattern_key_t*,(r->n2p[dim].size+1),sizeof(pattern_key_t*),return NULL;);

				size_k=r->n2p[k].size;
				size_j=r->n2p[j].size;
				size_l=r->n2p[dim].size;

				checked_malloc(r->n2p[j].p[size_j],pattern_key_t,1, sizeof(pattern_key_t)+3*sizeof(size_t),return NULL;);
				//r->n2p[j].p[size_j]=(pattern_key_t *)malloc(((sizeof(pattern_key_t)+2*sizeof(size_t))));
				checked_malloc(r->n2p[k].p[size_k],pattern_key_t,1,sizeof(pattern_key_t)+3*sizeof(size_t),return NULL;);
				//r->n2p[k].p[size_k]=(pattern_key_t *)malloc(((sizeof(pattern_key_t)+2*sizeof(size_t))));
				checked_malloc(r->n2p[dim].p[size_l],pattern_key_t,1,sizeof(pattern_key_t)+3*sizeof(size_t),return NULL;);

				r->n2p[j].size++;
				r->n2p[k].size++;
				r->n2p[dim].size++;

				r->n2p[j].p[r->n2p[j].size-1]->type = 2;

				r->n2p[j].p[r->n2p[j].size-1]->segments[0]=j;
				r->n2p[j].p[r->n2p[j].size-1]->segments[1]=k;
				r->n2p[j].p[r->n2p[j].size-1]->segments[2]=dim;

				r->n2p[k].p[r->n2p[k].size-1]->type = 2;
				r->n2p[k].p[r->n2p[k].size-1]->segments[0]=k;
				r->n2p[k].p[r->n2p[k].size-1]->segments[1]=j;
				r->n2p[k].p[r->n2p[k].size-1]->segments[2]=dim;

				r->n2p[dim].p[r->n2p[dim].size-1]->type = 2;
				r->n2p[dim].p[r->n2p[dim].size-1]->segments[0]=k;
				r->n2p[dim].p[r->n2p[dim].size-1]->segments[1]=j;
				r->n2p[dim].p[r->n2p[dim].size-1]->segments[2]=dim;

			}

		}



	return r;
}

/* TODO: priority 0 */
ucons_t *
ucons_remove_dimensions (ap_manager_t * man,
		bool destructive, ucons_t * a,
		ap_dimchange_t * dimchange)
		{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_REMOVE_DIMENSIONS, 0);
	arg_assert (dimchange && dimchange->intdim == 0, return NULL;);
	if (!a)
		return NULL;
	if(ucons_is_bottom(pr->man,a))
		return a;

#ifndef NDEBUG1
	printf("  \n\t remove: \n");
	for(size_t temp = 0; temp < dimchange->intdim; temp++){
		printf("%zu",dimchange->dim[temp]);
	}
	printf("\n");
	ucons_fprint(stdout,pr->man,a,NULL);
	printf("\n");
#endif

	// for one dimension to remove, remove 2 dimensions
	ucons_t *r =
			ucons_alloc_internal (pr, a->datadim, a->segmentdim - dimchange->realdim);
	size_t i,j;
	ap_dimchange_t dimrm;
	dimrm.realdim = 0;

	dimrm.intdim = 2 * dimchange->realdim;
	dimrm.dim = (ap_dim_t *) malloc (dimrm.intdim * sizeof (ap_dim_t));
	for (i = 0; i < dimchange->realdim; i++)
	{
		dimrm.dim[2 * i + 0] = dimchange->dim[i];
		dimrm.dim[2 * i + 1] = a->segmentdim + dimchange->dim[i];
	}
	// dimrm shall be sorted!
	shape_dimchange_sort(&dimrm);
	r->econs =
			ap_abstract0_remove_dimensions (pr->man_dcons, destructive, a->econs, &dimrm);

	size_t nr_y,u_seg;
	pattern_t * s, *rt, *ra, *new_udcons;
	bool change;
	unsigned keylen;
	for(s=r->udcons;s!=NULL;s=s->hh.next){

		change = false ;

		u_seg=pr->PI[s->key.type].u_seg;
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

		checked_malloc(rt,pattern_t,1,sizeof(pattern_t)+(u_seg)*sizeof(size_t),return NULL;);
		memset(rt, 0, sizeof(pattern_t)+(u_seg)*sizeof(size_t));

		rt->key.type=s->key.type;
		rt->dcons = NULL;
		for(i=0; i<u_seg; i++){
			rt->key.segments[i] = s->key.segments[i];
			j=0;
			while(j<dimchange->realdim){
				if(s->key.segments[i] >= (dimchange->dim[j] - a->datadim))
				{
					rt->key.segments[i]++ ;
					change = true;
					j++;
					while((j<dimchange->realdim) && (dimchange->dim[j-1]+1 == dimchange->dim[j])  ){
						rt->key.segments[i]++ ;
						j++;
					}
				}
				else j++;
			}
		}

		if(change){

			HASH_FIND(hh,a->udcons,&rt->key, keylen,ra);
			if(ra)
				s->dcons = ap_abstract0_remove_dimensions(pr->man_dcons,destructive,ra->dcons,&dimrm);
			else
				s->dcons = ap_abstract0_bottom(pr->man_dcons,r->datadim,r->segmentdim);
			free(rt);
		}
		else {
			HASH_FIND(hh,a->udcons,&s->key, keylen,ra);
			if(ra)
				s->dcons = ap_abstract0_remove_dimensions(pr->man_dcons,destructive,ra->dcons,&dimrm);
			else
				s->dcons = ap_abstract0_bottom(pr->man_dcons,r->datadim,r->segmentdim);
			free(rt);
		}

	} //end modifiyng universals
	/* nodes removed from n2p; already created with the right size
	 * */

#ifndef NDEBUG1
	fprintf(stdout,"\n remove dimensions returns \n");
	ucons_fprint(stdout,man,r,NULL);
	fprintf(stdout,"\n");
#endif

	if (destructive)
		ucons_free_internal (pr, a);

#ifndef NDEBUG1
	printf("  \n\t remove returns : \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif
	return r;
		}

/* TODO: priority 0 */
ucons_t *
ucons_permute_dimensions (ap_manager_t * man,
		bool destructive, ucons_t * a,
		ap_dimperm_t * perm)
		{
	ucons_internal_t *pr =
			ucons_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);
	if (!a)
		return NULL;
	assert (perm && perm->size == (a->datadim + a->segmentdim));

	if(ucons_is_bottom(pr->man,a))
		return a;


#ifndef NDEBUG1
	printf("  \n\t permute: \n");
	printf("\n");
	ucons_fprint(stdout,pr->man,a,NULL);
	printf("\n");
#endif

	ucons_t *r ;
	/*
	 * Remove node dimensions mapped to a->datadim (except a->datadim itself) and build a new permutation
	 */
	ap_dimchange_t dimrm;
	ap_dimperm_t newperm;
	ap_dimperm_t new_node_perm;
	size_t i, j, k, rmsize, newsize;
	dimrm.intdim = 0;
	dimrm.realdim = perm->size;
	dimrm.dim = (ap_dim_t *) malloc (perm->size * sizeof (ap_dim_t));
	newperm.size = perm->size;
	newperm.dim = (ap_dim_t *) malloc (perm->size * sizeof (ap_dim_t));
	/* new_node_perm is the new permutation of nodes */
	new_node_perm.size = perm->size - a->datadim;
	new_node_perm.dim = (ap_dim_t *) malloc ((perm->size - a->datadim) * sizeof (ap_dim_t));

	for (i = 0, j = 0, rmsize = 0; i < perm->size; i++)
		if (i > a->datadim && perm->dim[i] == a->datadim)
		{
			dimrm.dim[rmsize] = i;
			rmsize++;
		}
		else
		{
			newperm.dim[j] = perm->dim[i];
			if(j>=a->datadim) new_node_perm.dim[j - a->datadim] =  perm->dim[i] - a->datadim;
			j++;
		}
	if (rmsize > 0)
	{
		dimrm.intdim = 0;
		dimrm.realdim = rmsize;
		dimrm.dim =
				(ap_dim_t *) realloc (dimrm.dim, rmsize * sizeof (ap_dim_t));

		r = ucons_remove_dimensions (man, false, a, &dimrm);
		pr = ucons_init_from_manager (man, AP_FUNID_PERMUTE_DIMENSIONS, 0);
		newperm.size = perm->size - rmsize;
		newperm.dim =
				(ap_dim_t *) realloc (newperm.dim, newperm.size * sizeof (ap_dim_t));

		new_node_perm.size = new_node_perm.size - rmsize;
		new_node_perm.dim =
				(ap_dim_t *) realloc (new_node_perm.dim, new_node_perm.size * sizeof (ap_dim_t));

	}
	else
		r = ucons_copy_internal (pr, a);
	free (dimrm.dim);


	/*
	 * Build permutations corresponding to each set of constraints.
	 */
	ap_dimperm_t consperm;
	size_t newsegmsize = (newperm.size - a->datadim);

	consperm.size = a->datadim + 2 * newsegmsize;
	consperm.dim = (ap_dim_t *) malloc (consperm.size * sizeof (ap_dim_t));
	ap_dimperm_set_id (&consperm);
	for (i = 0; i < newsegmsize; i++)
	{
		consperm.dim[a->datadim + i] = newperm.dim[a->datadim + i];
		consperm.dim[a->datadim + newsegmsize + i] =
				newsegmsize + newperm.dim[a->datadim + i];
	}

	r->econs = ap_abstract0_permute_dimensions (pr->man_dcons, true, r->econs,
			&consperm);

	free (consperm.dim);


	size_t nr_y, u_seg;
	pattern_t * s, *ra, *rt;
	unsigned keylen;

	pattern_t * perm_r = NULL;

	for(s=r->udcons; s!=NULL; s=s->hh.next){

		ra = NULL;
		u_seg=pr->PI[s->key.type].u_seg;
		checked_malloc(ra,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return NULL;);
		memset(ra, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));


		ra->dcons = (s->dcons) ? ap_abstract0_copy(pr->man_dcons,s->dcons) : NULL;

		ra->key.type=s->key.type;
		/* ra->key.segments[] = perm ( s->key.segments[] ) */
		for(i=0;i<u_seg;i++)
			ra->key.segments[i] = new_node_perm.dim[s->key.segments[i]];


		/*TODO general case: sort segments and apply the resulting permutation on universals */

		bool sort = true;

#if defined (UCONS_DCONS_OCT_P21) || defined (UCONS_DCONS_POLY_P21)
		if(u_seg == 2){
			if (ra->key.segments[0]>ra->key.segments[1]){
				size_t temp = ra->key.segments[0];
				ra->key.segments[0] = ra->key.segments[1];
				ra->key.segments[1] = temp;
				sort = false;
			}
		}
#endif


		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);
		nr_y = pr->PI[s->key.type].nr_y;

		newsegmsize = (newperm.size - a->datadim);
		consperm.size = a->datadim + 2 * newsegmsize + 2 * nr_y;
		consperm.dim = (ap_dim_t *) malloc (consperm.size * sizeof (ap_dim_t));
		ap_dimperm_set_id (&consperm);
		for (i = 0; i < newsegmsize; i++)
		{
			consperm.dim[a->datadim + i] = newperm.dim[a->datadim + i];
			consperm.dim[a->datadim + newsegmsize + i] =
					newsegmsize + newperm.dim[a->datadim + i];
		}

		/*TODO general case: sort segments and apply the resulting permutation on universals */

#if defined (UCONS_DCONS_OCT_P21) || defined (UCONS_DCONS_POLY_P21)
		if (sort == false){
			consperm.dim[a->datadim + 2* newsegmsize] = a->datadim + 2* newsegmsize + 1;
			consperm.dim[a->datadim + 2* newsegmsize + 1] = a->datadim + 2* newsegmsize;
			consperm.dim[a->datadim + 2* newsegmsize + 2] = a->datadim + 2* newsegmsize + 3;
			consperm.dim[a->datadim + 2* newsegmsize + 3] = a->datadim + 2* newsegmsize + 2;
		}
#endif
		ra->dcons = ap_abstract0_permute_dimensions (pr->man_dcons, true, ra->dcons,
				&consperm);
		HASH_ADD(hh,perm_r,key,keylen,ra);

	}


	while(r->udcons) {
		s = r->udcons;
		HASH_DEL(r->udcons,s);
		free(s);
	}
	r->udcons = NULL;


	for(s=perm_r;s!=NULL;s=s->hh.next){

		u_seg=pr->PI[s->key.type].u_seg;
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

		checked_malloc(rt,pattern_t,1,sizeof(pattern_t)+u_seg*sizeof(size_t),return NULL;);
		memset(rt, 0, sizeof(pattern_t)+u_seg*sizeof(size_t));

		rt->key.type=s->key.type;
		for(i=0;i<u_seg;i++)
			rt->key.segments[i] = s->key.segments[i];
		rt->dcons = ap_abstract0_copy(pr->man_dcons,s->dcons);

		HASH_ADD(hh,r->udcons,key,keylen,rt);
	}

	while(perm_r) {
		s = perm_r;
		HASH_DEL(perm_r,s);
		free(s);
	}



	pattern_key_set_t * perm_n2p;
	perm_n2p = pattern_key_set_copy(pr,r->n2p,r->segmentdim);

	size_t i1,i2,i3;
	int jj,kk,ii;
	size_t d;

	for(i=0;i<r->segmentdim;i++){
		i1 = perm_n2p[new_node_perm.dim[i]].size;
		r->n2p[i].size = perm_n2p[new_node_perm.dim[i]].size;
		free (r->n2p[i].p);
		r->n2p[i].p = NULL;
		checked_malloc(r->n2p[i].p, pattern_key_t*,r->n2p[i].size,sizeof(pattern_key_t),return NULL;);
		for(j=0;j<r->n2p[i].size;j++){
			u_seg = pr->PI[perm_n2p[i].p[j]->type].u_seg;
			checked_malloc(r->n2p[i].p[j], pattern_key_t,1,sizeof(pattern_key_t)+u_seg*sizeof(size_t),return NULL ;);

			r->n2p[i].p[j]->type = perm_n2p[i].p[j]->type;
			for(k=0;k<u_seg;k++){
				i2 = r->n2p[i].p[j]->segments[k] ;
				i3 = new_node_perm.dim[perm_n2p[new_node_perm.dim[i]].p[j]->segments[k]];
				r->n2p[i].p[j]->segments[k]=new_node_perm.dim[perm_n2p[new_node_perm.dim[i]].p[j]->segments[k]];
			}

			for (ii = 1; ii < u_seg; ii++)
			{
				jj = 0;
				while (jj != ii && r->n2p[i].p[j]->segments[jj] <= r->n2p[i].p[j]->segments[ii])
					jj++;
				if (jj < ii)
				{
					d = r->n2p[i].p[j]->segments[ii];
					for (kk = ii - 1; kk >= jj; kk--)
						r->n2p[i].p[j]->segments[kk + 1] = r->n2p[i].p[j]->segments[kk];
					r->n2p[i].p[j]->segments[jj] = d;
				}
			}

		}
	}


	if (destructive)
		ucons_free_internal (pr, a);

#ifndef NDEBUG1
	printf("  \n\t permute returns : \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif
	return r;
		}


/* ============================================================ */
/* Expansion and folding of dimensions */
/* ============================================================ */


ucons_t *
ucons_singleton (ucons_internal_t * pr, bool destructive, ucons_t * a,
		ap_dim_t dim)
		{
	ucons_t *r = ucons_alloc_internal (pr, a->datadim, a->segmentdim);
	/*
	 * intersect with constraints l[dim] = 1 and r->udcons[]->dcons = 0
	 */

	ap_lincons0_array_t arr = ap_lincons0_array_make (1);
	// l[dim]-1 == 0
	arr.p[0].constyp = AP_CONS_EQ;
	arr.p[0].linexpr0 =
			ap_linexpr0_alloc (AP_LINEXPR_DENSE, a->datadim + 2 * a->segmentdim);
	arr.p[0].scalar = NULL;
	ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0,
			a->datadim + a->segmentdim + dim, 1);
	ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, -1);

	r->econs =
			ap_abstract0_meet_lincons_array (pr->man_dcons, false, a->econs, &arr);
	//	if (ap_abstract0_is_bottom(pr->man_dcons,r->econs))
	//		return ucons_bottom (pr->man, a->datadim, a->segmentdim);

	/* TODO: add modifications on r->udcons. */

	size_t i,nr_y,u_seg,intdim;
	pattern_t * s, *rt;
	bool found;
	ap_dimchange_t dimadd;
	unsigned keylen;
	for(s = r->udcons; s != NULL; s = s->hh.next){
		u_seg=pr->PI[s->key.type].u_seg;

		found = false;
		for(i = 0; i < u_seg; i++)
			if(s->key.segments[i]==dim) found = true;

		intdim = a->datadim + 2 * a->segmentdim + 2 * pr->PI[s->key.type].nr_y;

		if(found) {
			s->dcons = ap_abstract0_bottom(pr->man_dcons,intdim,0);
		}
		else{
			keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

			HASH_FIND(hh,a->udcons,&s->key,keylen,rt);
			if(rt){
				s->dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, false, rt->dcons, &arr);
			}

			else {
				ap_manager_raise_exception (pr->man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
						"not implemented for different set of patterns ");
			}
		}

	}

	if (destructive)
		ucons_free_internal (pr, a);

#ifndef NDEBUG1
	fprintf(stdout,"\n singleton returns: \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	fprintf(stdout,"\n");
#endif

	ap_lincons0_array_clear (&arr);
	return r;
		}


bool test_pattern_sat(ucons_internal_t * pr, ucons_t *r, pattern_key_t *p, int s ){

	size_t u_seg,nr_yi;
	size_t i,li;
	bool flag;
	ap_lincons0_t cons;
	ap_linexpr0_t *expr;

	u_seg = pr->PI[p->type].u_seg;
	flag = true;
	if(s==0){
		for (i = 0; i < u_seg && flag == true; i++){
			// l(segments[i]) - pr->PI[p->type].uvar[i].size > 0
			li = r->datadim + r->segmentdim + p->segments[i];
			nr_yi = pr->PI[p->type].uvar[i].size;

			expr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
			ap_linexpr0_set_coeff_scalar_int (expr, li, 1);
			ap_linexpr0_set_cst_scalar_int (expr, -nr_yi);
			cons = ap_lincons0_make(AP_CONS_SUP, expr, NULL);

			flag = ap_abstract0_sat_lincons(pr->man_dcons, r->econs,&cons);
			ap_lincons0_clear(&cons);

		}
		return flag;
	}
	else if (s==1){
		li = r->datadim + r->segmentdim + p->segments[0];
		expr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
		ap_linexpr0_set_coeff_scalar_int (expr, li, 1);
		ap_linexpr0_set_cst_scalar_int (expr, -1);
		cons = ap_lincons0_make(AP_CONS_EQ, expr, NULL);
		flag = ap_abstract0_sat_lincons(pr->man_dcons, r->econs,&cons);
		ap_lincons0_clear(&cons);

		return flag;
	}
	else {// s== 2
		li = r->datadim + r->segmentdim + p->segments[0];
		expr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
		ap_linexpr0_set_coeff_scalar_int (expr, li, 1);
		ap_linexpr0_set_cst_scalar_int (expr, -1);
		cons = ap_lincons0_make(AP_CONS_SUP, expr, NULL);
		flag = ap_abstract0_sat_lincons(pr->man_dcons, r->econs,&cons);
		ap_lincons0_clear(&cons);

		return flag;
	}
}





/* TODO: priority 0 */

ucons_t *
ucons_split (ucons_internal_t * pr, bool destructive, ucons_t * a, ap_dim_t dim)
{


	size_t i,ii,j,k;
	pattern_t *aux;
	ap_abstract0_t *n2_aux,*econs_aux;

	unsigned keylen;
	size_t u_seg;

	ap_dim_t * tdim;
	size_t size,contor;
	size_t pos_y=0; /* the dimension in r->udcons of y to be eliminated , data(y) = r->datadim + 2*r->segmentdim + nr_y + pos_y*/
	size_t nr_y; /* number of universally quantified variables */
	ap_dimperm_t perm;
	ap_dim_t pos_i;

	ap_dimchange_t* dimchange;

	bool pattern_corect;
	//size_t ii, nr_y_ii;
	ap_dim_t lii;
	ap_lincons0_t cons;
	bool found_pos_y;

#ifndef NDEBUG1
	fprintf(stdout,"\t split: \n");
	ucons_fprint(stdout,pr->man,a, NULL);
	fprintf(stdout,"\n");
#endif
	/*
	 * The node added by expansion has dimension a->segmdim
	 * Then apply substitutions/assignments:
	 * l[n1] == l[n2] + 1;
	 * l[n1] := 1;
	 * meet l[n2]>=1
	 */
	// Add dimension a->segmentdim
	ap_dimchange_t dimadd;
	ap_dimchange_init (&dimadd, 0, 1);
	dimadd.dim = (ap_dim_t *) malloc (1 * sizeof (ap_dim_t));
	dimadd.dim[0] = a->datadim + a->segmentdim;

	ucons_t *r = ucons_add_dimensions (pr->man, false, a, &dimadd, false);

	ap_dimchange_clear (&dimadd);
	// Build statements and apply them


	ap_dim_t n1,n2,dn2, ln1, ln2,d_y;
	ap_linexpr0_t *expr;

	n1 = dim;
	n2 = a->segmentdim;
	dn2 = a->datadim + n2;

	/*TODO: add precision on universal constraints, when working with sets of patterns */


	/* adding the constraints on dn2 in r->econs from universal formulas */

	pattern_key_set_t n1_pat_set = a->n2p[n1];

	/* l[n2]=l[n1]-1  \land l[n1]=1 */

	econs_aux=NULL;
	/* computing r->econs = add_dimension(a->econs) \meet n2_aux */

	for(j = 0; j < n1_pat_set.size; j++){
		/* for every pattern on the segment n1 do :
		 * 1. compute the data constraint n2_aux to collect information on dn2 from universal formulas
		 * 2. transfer the universal constraint on n2
		 * 3. set to bottom the universal formula on n1
		 * 4. l'[n2] = l[n1] - 1 /\  l'[n1] = 1 /\ define dn2 in r->econs and r->ucons
		 */

		u_seg = pr->PI[n1_pat_set.p[j]->type].u_seg;
		keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

		/* test pattern validity */
		nr_y = pr->PI[n1_pat_set.p[j]->type].nr_y;
		pattern_corect = true;

		HASH_FIND(hh,r->udcons,n1_pat_set.p[j],keylen,aux);
		if(aux){
			if(test_pattern_sat(pr,r,n1_pat_set.p[j],0)){
				pos_y = 0;
				found_pos_y = false;
				/* 1.reinforcing r->econs */
				for(i = 0; i < u_seg && !found_pos_y; i++){
					if(n1_pat_set.p[j]->segments[i]==n1){
						pos_y += 1;
						found_pos_y = true;
					}
					else pos_y += pr->PI[n1_pat_set.p[j]->type].uvar[i].size;
				}

				if(nr_y>1){
					/* if the pattern is different from \forall y */
					checked_malloc(tdim, ap_dim_t, 2*(nr_y-1), sizeof(ap_dim_t), return NULL;);
					/*TO DO does not take into consideration the cases when y1!=1 or y1=y2=1*/
					contor=0;
					for(i = 0; i < 2*nr_y; i++){
						if( i != (pos_y-1) && i != nr_y+(pos_y-1) ){
							tdim[contor] = r->datadim + 2*r->segmentdim + i;
							contor += 1;
						}
					}
					n2_aux = ap_abstract0_copy (pr->man_dcons, aux->dcons);
					//n2_aux = ap_abstract0_forget_array(pr->man_dcons,true,n2_aux,tdim,2*(nr_y-1),true);

					dimchange = ap_dimchange_alloc(2*(nr_y-1), 0);
					for(k = 0; k < 2*(nr_y-1); k++){
						dimchange->dim[k] = tdim[k];
					}
					n2_aux = ap_abstract0_remove_dimensions (pr->man_dcons, true, n2_aux, dimchange);

					free(tdim);
					ap_dimchange_free(dimchange);
				}
				else{
					n2_aux = ap_abstract0_copy (pr->man_dcons, aux->dcons);
				}


				/* n2_aux size is r->datadim + 2*r->segmentdim + 2 */
				pos_y = r->datadim + 2*r->segmentdim;
				/* y=1 */
				ap_lincons0_array_t arr = ap_lincons0_array_make (2);
				// y-1 == 0 where y denotes n2 in universal
				arr.p[0].constyp = AP_CONS_EQ;
				arr.p[0].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
				arr.p[0].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0,pos_y, 1);
				ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, -1);

				//  dn2 already added but unconstrained;
				//  dn2 = pos_y + 1
				arr.p[1].constyp = AP_CONS_EQ;
				arr.p[1].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
				arr.p[1].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, (pos_y+1), -1);
				ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, dn2, 1);
				ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);


				n2_aux = ap_abstract0_meet_lincons_array (pr->man_dcons, false, n2_aux, &arr);
				ap_lincons0_array_clear (&arr);

				/* eliminate y and dy to get constraints on existential */
				checked_malloc(tdim,ap_dim_t,2,sizeof(ap_dim_t),return NULL;);
				tdim[0] = pos_y;
				tdim[1] = pos_y+1;
				n2_aux = ap_abstract0_forget_array(pr->man_dcons, false, n2_aux, tdim, 1, true);
				//  remove dimension pos_y, pos_y+1
				dimchange=ap_dimchange_alloc(2,0);
				dimchange->dim[0] = pos_y;
				dimchange->dim[1] = pos_y+1;
				n2_aux = ap_abstract0_remove_dimensions (pr->man_dcons, true, n2_aux, dimchange);
				ap_dimchange_free(dimchange);

				/* n2_aux size is r->datadim + 2*r->segmentdim */

				/* econs_aux data constraint deduced from patterns  */
				if(econs_aux==NULL)    econs_aux = ap_abstract0_copy (pr->man_dcons, n2_aux);
				else    econs_aux = ap_abstract0_meet(pr->man_dcons, true, econs_aux, n2_aux);

				ap_abstract0_free(pr->man_dcons, n2_aux);
			}//end if pattern_sat

			/* 2. transforming the universal constraints */
			/* case one: searching for the same pattern where n1 replaces with n2
			 * check if condition */

			/* TODO the cases when multiple segments */

			pattern_t *aux_find2;
			ap_dim_t *ydim;
			ap_linexpr0_t ** yexpr;
			pattern_key_t * pat_trasf;

			if(u_seg==1){
				checked_malloc(pat_trasf, pattern_key_t, 1,
						sizeof(pattern_key_t) + u_seg*sizeof(size_t), return NULL;);
				pat_trasf->type = n1_pat_set.p[j]->type;

				pat_trasf->segments[0] = n2;

				keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);
				HASH_FIND(hh,r->udcons, pat_trasf, keylen, aux_find2);
				if(aux_find2){
					/* y substitutes with  y+1  for all y universally quantified */

					checked_malloc(ydim, ap_dim_t, nr_y,sizeof(ap_dim_t), return NULL;);
					checked_malloc(yexpr, ap_linexpr0_t*, nr_y,sizeof(ap_linexpr0_t*), return NULL;);

					aux_find2->dcons = ap_abstract0_copy (pr->man_dcons, aux->dcons);
					for(i = 0 ; i < nr_y; i++){
						ydim[i] = r->datadim + 2 * r->segmentdim + i;

						yexpr[i] = ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2 * nr_y );
						ap_linexpr0_set_coeff_scalar_int (yexpr[i], ydim[i], 1);
						ap_linexpr0_set_cst_scalar_int (yexpr[i], 1);
						aux_find2->dcons= ap_abstract0_substitute_linexpr( pr->man_dcons, true, aux_find2->dcons, ydim[i],
								yexpr[i], NULL );
					}
					for(i = 0 ; i < nr_y; i++){
						ap_linexpr0_free(yexpr[i]);
					}
					free(ydim);
					free(yexpr);
				}
				free(pat_trasf);

				/* \forall y in n1  => \bottom Warning !!!*/
				ap_abstract0_free(pr->man_dcons,aux->dcons);
				aux->dcons = NULL;
				aux->dcons=ap_abstract0_bottom(pr->man_dcons, r->datadim + 2*r->segmentdim + 2*nr_y, 0);
				/** TODO: case two: test to see is sub-patterns needed */
			}else{

				//#if defined (UCONS_DCONS_OCT_P21) || defined (UCONS_DCONS_POLY_P21)

				if (n1_pat_set.p[j]->type == 1){
					checked_malloc(pat_trasf, pattern_key_t, 1, sizeof(pattern_key_t) + u_seg*sizeof(size_t), return NULL;);
					pat_trasf->type = n1_pat_set.p[j]->type;

					if(n1_pat_set.p[j]->segments[0] == n1)
						pat_trasf->segments[0] = n1_pat_set.p[j]->segments[1];
					else pat_trasf->segments[0] = n1_pat_set.p[j]->segments[0];

					pat_trasf->segments[1] = n2;

					keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);
					HASH_FIND(hh, r->udcons, pat_trasf, keylen, aux_find2);
					if(aux_find2){
						if (!test_pattern_sat(pr, r, pat_trasf, 1) || test_pattern_sat(pr, r, pat_trasf,2)){
							ap_dimchange_init (&dimadd, 4, 0);
							dimadd.dim = (ap_dim_t *) malloc (4 * sizeof (ap_dim_t));
							dimadd.dim[0] = r->datadim + 2*r->segmentdim;
							dimadd.dim[1] = r->datadim + 2*r->segmentdim;
							dimadd.dim[2] = r->datadim + 2*r->segmentdim;
							dimadd.dim[3] = r->datadim + 2*r->segmentdim;

							aux_find2->dcons = ap_abstract0_add_dimensions(
									pr->man_dcons,false,r->econs,&dimadd,false);

							ap_dimchange_clear (&dimadd);
						}
						else{
							aux_find2->dcons = ap_abstract0_bottom(pr->man_dcons,r->datadim + 2*r->segmentdim + 2*nr_y,0);
						}
					}
					ap_abstract0_free(pr->man_dcons,aux->dcons);
					aux->dcons = NULL;
					aux->dcons=ap_abstract0_bottom(pr->man_dcons,r->datadim + 2*r->segmentdim + 2*nr_y,0);

					pat_trasf->segments[0] = n1;

					HASH_FIND(hh,r->udcons,pat_trasf,keylen,aux_find2);
					if(aux_find2){
						aux_find2->dcons = ap_abstract0_bottom(pr->man_dcons,r->datadim + 2*r->segmentdim + 2*nr_y,0);

					}
				}

				//#endif
			}
		}//end checking one  pattern
		//		}//end if pattern corect
	}//end checking all patterns


	/*
	 * 4. l'[n2] = l[n1] - 1 /\  l'[n1] = 1 /\ define dn2 in r->econs
	 */

	if (econs_aux) 	r->econs = ap_abstract0_meet(pr->man_dcons, true, econs_aux, r->econs);

	// update the lengths on the existential constraint
	ln1 = r->datadim + r->segmentdim + n1;
	ln2 = r->datadim + r->segmentdim + n2;
	expr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);

	// l[n1] == l[n2] + 1;
	ap_linexpr0_set_coeff_scalar_int (expr, ln2, 1);
	ap_linexpr0_set_cst_scalar_int (expr, 1);
	r->econs =
			ap_abstract0_substitute_linexpr (pr->man_dcons, true, r->econs, ln1, expr,
					NULL);

	// l[n1] := 1;
	ap_linexpr0_set_coeff_scalar_int (expr, ln2, 0);

	r->econs =
			ap_abstract0_assign_linexpr (pr->man_dcons, true, r->econs, ln1, expr,
					NULL);

	// meet l[n2]-1 >= 0
	ap_linexpr0_set_coeff_scalar_int (expr, ln2, 1);
	ap_linexpr0_set_cst_scalar_int (expr, -1);
	ap_lincons0_array_t arr = ap_lincons0_array_make (1);
	arr.p[0] = ap_lincons0_make (AP_CONS_SUPEQ, expr, NULL);
	r->econs=
			ap_abstract0_meet_lincons_array (pr->man_dcons, true, r->econs, &arr);
	ap_lincons0_array_clear (&arr);

	/*
	 * 4. l'[n2] = l[n1] - 1 /\  l'[n1] = 1 /\ define dn2 in r->udcons
	 */

	// update the lengths on the universal constraint

	pattern_t *s;
	for(s = r->udcons; s!=NULL; s = s->hh.next){
		nr_y = pr->PI[s->key.type].nr_y;
		expr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2 * nr_y);
		// l[n1] == l[n2] + 1;
		ap_linexpr0_set_coeff_scalar_int (expr, ln2, 1);
		ap_linexpr0_set_cst_scalar_int (expr, 1);
		s->dcons =
				ap_abstract0_substitute_linexpr (pr->man_dcons, true, s->dcons, ln1, expr,
						NULL);
		// l[n1] := 1;
		ap_linexpr0_set_coeff_scalar_int (expr, ln2, 0);
		s->dcons =
				ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons, ln1, expr,
						NULL);

		//		// d[n2] := d[y1];
		//		ap_linexpr0_set_cst_scalar_int (expr, 0);
		//		ap_linexpr0_set_coeff_scalar_int (expr, r->datadim + 2 * r->segmentdim + nr_y, 1);
		//		s->dcons =
		//				ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons, dn2, expr,
		//						NULL);
		//		ap_linexpr0_set_coeff_scalar_int (expr, r->datadim + 2 * r->segmentdim + nr_y, 0);

		// meet l[n2]-1 >= 0
		ap_linexpr0_set_coeff_scalar_int (expr, ln2, 1);
		ap_linexpr0_set_cst_scalar_int (expr, -1);
		ap_lincons0_array_t arr = ap_lincons0_array_make (1);
		arr.p[0] = ap_lincons0_make (AP_CONS_SUPEQ, expr, NULL);

		s->dcons=
				ap_abstract0_meet_lincons_array (pr->man_dcons, true, s->dcons, &arr);
		ap_lincons0_array_clear (&arr);
	}

#ifndef NDEBUG1
	fprintf(stdout,"\t split returns : \n");
	ucons_fprint(stdout,pr->man,r, NULL);
	fprintf(stdout,"\n");
#endif

	return r;
}


/*
 * Used with n=1 to simulate the singleton operation.
 * Used with n=2 to simulate the split operation.
 */

ucons_t *
ucons_expand (ap_manager_t * man,
		bool destructive, ucons_t * a, ap_dim_t dim, size_t n)
		{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_EXPAND, 0);

	if (!a)
		return NULL;
	arg_assert (n <= 2 && dim > 0 && dim < a->segmentdim, return NULL;);
	ucons_t *r;
	if (n == 1)
		r = ucons_singleton (pr, destructive, a, dim);
	else
		r = ucons_split (pr, destructive, a, dim);
	if (destructive)
		ucons_free_internal (pr, a);

	return r;
		}

/* TODO: priority 0 */



ucons_t *
ucons_fold (ap_manager_t * man,
		bool destructive, ucons_t * a, ap_dim_t * tdim, size_t size)
		{
	ucons_internal_t *pr = ucons_init_from_manager (man, AP_FUNID_FOLD, 0);

	if (!a)
		return NULL;
	arg_assert (tdim && 0 < size && size < a->segmentdim, return NULL;);

#if defined(UCONS_DCONS_OCT_P21) || defined(UCONS_DCONS_POLY_P21)
	if(first==0){
		checked_realloc(fold_dim, ap_dim_t, size_fold_dim + size,sizeof(ap_dim_t), return NULL;);
		for(size_t ii = 0; ii < size; ii++){
			fold_dim[ii+size_fold_dim] = tdim[ii];
		}
		size_fold_dim += size ;
		first = 1;
		/*TODO: make patterns for multiple segments */
		return a;
	}

	ucons_t *r = ucons_copy_internal (pr, a);

#ifndef NDEBUG1
	printf("  \n\t fold : \n");
	ucons_fprint(stdout, pr->man, r, NULL);
	printf("\n");
#endif

	size_t i, j, k;
	ap_dim_t fst_ni, snd_ni;

	ap_dimchange_t dimchange;

	pattern_key_t * look;
	size_t u_seg, nr_y;
	unsigned keylen;
	pattern_t * n_dcons;

	ap_linexpr0_t *expr;
	ap_lincons0_t cons;

	ap_dim_t *snd_node_tdim = NULL; // the nodes to concatenate from dimensions in tdim
	ap_dim_t *fst_node_tdim = NULL; // the nodes to concatenate from dimensions in tdim

	ap_dim_t ddy1, ddy2, l01, l02, ly1, ly2 ,li, lj;

	size_t	size_fst_node_tdim = 0;
	size_t size_snd_node_tdim = 0;

	ap_abstract0_t * aux = NULL;

	for(i = 0; i < size; i++){
		if(tdim[i] >= a->datadim){
			size_snd_node_tdim++;
			checked_realloc(snd_node_tdim, ap_dim_t,
					size_snd_node_tdim, sizeof(ap_dim_t), return NULL;);
			snd_node_tdim[size_snd_node_tdim-1] = tdim[i] - r->datadim;
		}
	}
	for(i = 0; i < size_fold_dim; i++){
		if(fold_dim[i] >= a->datadim){
			size_fst_node_tdim++;
			checked_realloc(fst_node_tdim, ap_dim_t,
					size_fst_node_tdim, sizeof(ap_dim_t), return NULL;);
			fst_node_tdim[size_fst_node_tdim-1] = fold_dim[i] - r->datadim;
		}
	}


	u_seg = pr->PI[1].u_seg;
	nr_y = pr->PI[1].nr_y;
	checked_malloc(look, pattern_key_t, sizeof(pattern_key_t) + u_seg*sizeof(size_t), 1, return NULL;);
	memset(look, 0, sizeof(pattern_key_t) + u_seg*sizeof(size_t));
	look->type = 1;
	keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);

	ap_abstract0_t * val = NULL;
	ap_abstract0_t * val_i = NULL;

	if (size_fold_dim != size){

		if(fst_node_tdim[0] < snd_node_tdim[0]){
			look->segments[0] = fst_node_tdim[0];
			look->segments[1] = snd_node_tdim[0];
		}
		else{
			look->segments[1] = fst_node_tdim[0];
			look->segments[0] = snd_node_tdim[0];
		}

		HASH_FIND(hh, r->udcons, look, keylen, n_dcons);
		if(n_dcons) n_dcons->dcons = ap_abstract0_top(pr->man_dcons, r->datadim + 2*r->segmentdim + 2*nr_y, 0);
	}
	else{
		for(i = 1; i < size; i++){
			fst_ni = fst_node_tdim[i];
			snd_ni = snd_node_tdim[i];

			expr =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim );
			ap_linexpr0_set_cst_scalar_int (expr, 0);
			for(j = 0; j < i; j++){
				lj = r->datadim + r->segmentdim + fst_node_tdim[j] ;
				ap_linexpr0_set_coeff_scalar_int (expr, lj, 1);
				lj = r->datadim + r->segmentdim + snd_node_tdim[j] ;
				ap_linexpr0_set_coeff_scalar_int (expr, lj, -1);
			}
			cons = ap_lincons0_make(AP_CONS_EQ, expr, NULL);

			if(ap_abstract0_sat_lincons(pr->man_dcons,r->econs, &cons)){

				if(fst_node_tdim[i] < snd_node_tdim[i]){
					look->segments[0] = fst_node_tdim[i];
					look->segments[1] = snd_node_tdim[i];
				}
				else{
					look->segments[1]=fst_node_tdim[i];
					look->segments[0]=snd_node_tdim[i];
				}

				HASH_FIND(hh,r->udcons, look, keylen, n_dcons);
				if(n_dcons){
					if (aux) aux = ap_abstract0_join(pr->man_dcons, false, aux, n_dcons->dcons);
					else aux = ap_abstract0_copy(pr->man_dcons, n_dcons->dcons);
				}
				dimchange.intdim = 4;
				dimchange.realdim = 0;
				dimchange.dim = (ap_dim_t *)malloc(4*sizeof(ap_dim_t));
				dimchange.dim[0] = r->datadim + 2*r->segmentdim;
				dimchange.dim[1] = r->datadim + 2*r->segmentdim;
				dimchange.dim[2] = r->datadim + 2*r->segmentdim;
				dimchange.dim[3] = r->datadim + 2*r->segmentdim;

				val_i = ap_abstract0_add_dimensions(pr->man_dcons,false, r->econs, &dimchange, false);
				free(dimchange.dim);


				ap_lincons0_array_t arr = ap_lincons0_array_make (4);
				// l[y1] == l[fst_node_dim[0]] + ... l[fst_node_dim[i-1]]
				arr.p[0].constyp = AP_CONS_EQ;
				arr.p[0].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
				arr.p[0].scalar = NULL;

				// l[y2] == l[snd_node_dim[0]] + ... l[snd_node_dim[i-1]]
				arr.p[1].constyp = AP_CONS_EQ;
				arr.p[1].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
				arr.p[1].scalar = NULL;

				for(j = 0; j < i; j++){
					lj = r->datadim + r->segmentdim + fst_node_tdim[j] ;
					ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, lj, 1);
					lj = r->datadim + r->segmentdim + snd_node_tdim[j] ;
					ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, lj, 1);
				}

				ly1 = r->datadim + 2*r->segmentdim;
				ly2 = r->datadim + 2*r->segmentdim + 1;
				ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0,ly1, -1);
				ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, 0);

				ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,ly2, -1);
				ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);

				// d(y1) - d(fst_node_dim[i]) ==0

				if(fst_node_tdim[i] < snd_node_tdim[i]){
					ddy1 = fst_node_tdim[i];
					ddy2 = snd_node_tdim[i];
				}
				else{
					ddy2 = fst_node_tdim[i];
					ddy1 = snd_node_tdim[i];
				}
				arr.p[2].constyp = AP_CONS_EQ;
				arr.p[2].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
				arr.p[2].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[2].linexpr0,
						r->datadim + ddy1, 1);
				ap_linexpr0_set_coeff_scalar_int (arr.p[2].linexpr0,
						r->datadim + 2*r->segmentdim + 2, -1);

				// d(y2) - d(snd_node_dim[i]) ==0
				arr.p[3].constyp = AP_CONS_EQ;
				arr.p[3].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
				arr.p[3].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[3].linexpr0,
						r->datadim + ddy2, 1);
				ap_linexpr0_set_coeff_scalar_int (arr.p[3].linexpr0,
						r->datadim + 2*r->segmentdim + 3, -1);

				val_i = ap_abstract0_meet_lincons_array (pr->man_dcons,true, val_i,&arr);
				ap_lincons0_array_clear (&arr);

				if(val_i){
					if (aux) aux = ap_abstract0_join(pr->man_dcons,false, aux, val_i);
					else aux = ap_abstract0_copy(pr->man_dcons, val_i);
				}
			}
			else{
				aux = ap_abstract0_top(pr->man_dcons, r->datadim + 2*r->segmentdim + 4, 0);
			}

			ap_linexpr0_free(expr);

		}//end for
		// join with constraints on first
		if(fst_node_tdim[0] < snd_node_tdim[0]){
			look->segments[0] = fst_node_tdim[0];
			look->segments[1] = snd_node_tdim[0];
		}
		else{
			look->segments[1] = fst_node_tdim[0];
			look->segments[0] = snd_node_tdim[0];
		}

		HASH_FIND(hh, r->udcons, look, keylen, n_dcons);
		if(n_dcons){
			if (n_dcons->dcons) n_dcons->dcons = ap_abstract0_join(pr->man_dcons,true, aux, n_dcons->dcons);
			else n_dcons->dcons = ap_abstract0_copy(pr->man_dcons, aux);
		}
	}
	//update lengths


	/* recalculate length of tdim[0] */

	ap_dim_t l0;
	/* update length for the merged segments */
	/* l(cdim[0]) = l(cdim[0]) + l(cdim[1]) + ... + l(cdim[last])*/

	/* for econs */
	expr =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i=0;i<size;i++){
		li = r->datadim + r->segmentdim + fst_node_tdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr,li, 1);
	}

	ap_linexpr0_t *expr2 =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i = 0;i < size; i++){
		li = r->datadim + r->segmentdim + snd_node_tdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr2, li, 1);
	}
	/* for udcons */
	ap_linexpr0_t *expr_21y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i=0;i<size;i++){
		li = r->datadim + r->segmentdim + fst_node_tdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr_21y, li, 1);
	}
	ap_linexpr0_t *expr_22y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i=0;i<size;i++){
		li = r->datadim + r->segmentdim + snd_node_tdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr_22y, li, 1);
	}


	l01 = r->datadim + r->segmentdim + fst_node_tdim[0] ;
	l02 = r->datadim + r->segmentdim + snd_node_tdim[0] ;

	r->econs =  ap_abstract0_assign_linexpr (pr->man_dcons, true, r->econs,l01 , expr, NULL);
	ap_linexpr0_free(expr);

	r->econs =  ap_abstract0_assign_linexpr (pr->man_dcons, true, r->econs,l02 , expr2, NULL);
	ap_linexpr0_free(expr2);

	pattern_t *s;
	for(s=r->udcons; s!=NULL; s=s->hh.next){
		s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons,l01 , expr_21y, NULL);
		s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons,l02 , expr_22y, NULL);
	}
	ap_linexpr0_free(expr_21y);
	ap_linexpr0_free(expr_22y);

#ifndef NDEBUG1
	printf("  \n\t fold returns : \n");
	ucons_fprint(stdout,pr->man, r, NULL);
	printf("\n");
#endif

	if (destructive)
		ucons_free_internal (pr, a);

	free(fold_dim);
	fold_dim = NULL;
	size_fold_dim = 0;
	first = 0;
	return r;

#else

	if(size==1)
		/*TODO: make patterns for multiple segments */
		return a;

	ucons_t *r = ucons_copy_internal (pr, a);

#ifndef NDEBUG1
	printf("  \n\t fold : \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif

	size_t i,j,k,start,size_cdim,size_gdim,size_node_tdim;
	ap_dim_t li,ni;
	ap_linexpr0_t *expr;
	ap_lincons0_t cons;


	ap_dim_t *node_tdim; // the nodes to concatenate from dimensions in tdim

	ap_dim_t *gdim; /* sub-vectors of dimensions (without universals) used to generate data properties */
	ap_dim_t *cdim; /* Dimensions with universals to concatenate */

	size_node_tdim = 0;
	node_tdim = NULL;
	for(i=0; i<size; i++){
		if(tdim[i]>=a->datadim)
			size_node_tdim++;
		checked_realloc(node_tdim,ap_dim_t,size_node_tdim,sizeof(ap_dim_t),return NULL;);
		node_tdim[size_node_tdim-1] = tdim[i] - r->datadim;
	}


	/*tdim splited into sub-vectors gdim such that gdim=ni_start ... ni with l(ni_start)=1 ... l(ni)=1 */
	size_cdim = 0 ;
	cdim = NULL;

	i=0;
	start=i;
	while(i<size_node_tdim){
		ni = node_tdim[i];
		li = r->datadim + r->segmentdim + ni;
		/* li-1=0 */
		expr = ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
		ap_linexpr0_set_coeff_scalar_int (expr, li, 1);
		ap_linexpr0_set_cst_scalar_int (expr, -1);
		cons = ap_lincons0_make(AP_CONS_EQ, expr, NULL);
		if(ap_abstract0_sat_lincons(pr->man_dcons,r->econs,&cons)){
			i++;
		}
		else{

			if (i-start==0){
				checked_realloc(cdim, ap_dim_t, sizeof(ap_dim_t),(size_cdim+1),return NULL;);
				cdim[size_cdim]=node_tdim[start];
				size_cdim++;
				i++;
				start=i;
			}else if (i==start+1){
				checked_realloc(cdim, ap_dim_t, sizeof(ap_dim_t),(size_cdim+2),return NULL;);
				cdim[size_cdim]=node_tdim[start];
				cdim[size_cdim+1]=node_tdim[i];
				size_cdim+=2;
				i++;
				start=i;
			}else{
				size_gdim = i-start;
				checked_malloc(gdim, ap_dim_t, sizeof(ap_dim_t),size_gdim,return NULL;);
				for(j=0;j<size_gdim;j++)
					gdim[j]=node_tdim[j+start+1];

#if defined(UCONS_DCONS_OCT_P11) || defined(UCONS_DCONS_POLY_P11)
				r=merge_succesors_1(pr, r, gdim, size_gdim);

#elif defined(UCONS_DCONS_OCT_P12) || defined(UCONS_DCONS_POLY_P12)
				r=merge_succesors_2(pr, r, gdim, size_gdim);
#endif
				free(gdim);

				checked_realloc(cdim, ap_dim_t, sizeof(ap_dim_t),(size_cdim+2),return NULL;);
				cdim[size_cdim]=node_tdim[start];
				cdim[size_cdim+1]=node_tdim[i];
				size_cdim+=2;

				i++;
				start=i;
			}
		}

		ap_lincons0_clear(&cons);
		expr=NULL;
	}

	if(i!=start){
		if (i==start+1){
			checked_realloc(cdim, ap_dim_t, sizeof(ap_dim_t),(size_cdim+2),return NULL;);
			cdim[size_cdim]=node_tdim[start];
			size_cdim++;
		}else{
			size_gdim = i-start;
			checked_malloc(gdim, ap_dim_t, sizeof(ap_dim_t),size_gdim,return NULL;);
			for(j=0;j<size_gdim;j++)
				gdim[j]=node_tdim[j+start];

#if defined(UCONS_DCONS_OCT_P11) || defined(UCONS_DCONS_POLY_P11)
			r=merge_succesors_1(pr, r, gdim, size_gdim);

#elif defined(UCONS_DCONS_OCT_P12) || defined(UCONS_DCONS_POLY_P12)
			r=merge_succesors_2(pr, r, gdim, size_gdim);
#endif

			free(gdim);

			checked_realloc(cdim, ap_dim_t, sizeof(ap_dim_t),size_cdim+1,return NULL;);
			cdim[size_cdim]=node_tdim[start];;
			size_cdim++;
		}
	}

#if defined(UCONS_DCONS_OCT_P11) || defined(UCONS_DCONS_POLY_P11)
	r=concat_nodes_1(pr,r,cdim,size_cdim);

#elif defined(UCONS_DCONS_OCT_P12) || defined(UCONS_DCONS_POLY_P12)
	r=concat_nodes_2(pr,r,cdim,size_cdim);
#endif

	ap_linexpr0_free (expr);
	free(cdim);

#ifndef NDEBUG1
	printf("  \n\t fold returns : \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif

	if (destructive)
		ucons_free_internal (pr, a);

	return r;
#endif
		}





/*  generates the constraints corresponding to the pattern y1<y2  */
ucons_t * merge_succesors_2(ucons_internal_t *pr, ucons_t * r, ap_dim_t * gdim, size_t size_gdim){

	size_t i,j;
	ap_abstract0_t * udcons, *g_dcons;
	pattern_t * n_dcons, *n0_dcons;
	size_t contor,u_seg;
	ap_dim_t ly,dy;
	ap_dim_t ly1,ly2,dy1,dy2;

	ap_dimchange_t * dimchange;
	pattern_key_t * look;

	bool chainge;
	ly1 = r->datadim + 2*r->segmentdim;
	ly2 = r->datadim + 2*r->segmentdim + 1;
	dy1 = r->datadim + 2*r->segmentdim + 2;
	dy2 = r->datadim + 2*r->segmentdim + 3;
	/* generating data constraint witn the pattern \forall y for the node gdim[0] from gdim[1..size_gdim] */

	u_seg = pr->PI[0].u_seg;
	checked_malloc(look, pattern_key_t, sizeof(pattern_key_t) + u_seg*sizeof(size_t), 1, return NULL;);
	memset(look, 0, sizeof(pattern_key_t) + u_seg*sizeof(size_t));
	look->type = 0;
	look->segments[0] = gdim[0];
	unsigned keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);

	HASH_FIND(hh, r->udcons, look, keylen, n_dcons);

	if(n_dcons){

		n_dcons->dcons = NULL;

		for(i = 1; i < size_gdim; i++){
			dimchange = ap_dimchange_alloc(2,0);
			dimchange->dim[0] = r->datadim + 2*r->segmentdim;
			dimchange->dim[1] = r->datadim + 2*r->segmentdim;
			ly = r->datadim + 2*r->segmentdim;
			dy = r->datadim + 2*r->segmentdim + 1;

			udcons = ap_abstract0_add_dimensions(pr->man_dcons, false, r->econs, dimchange, false);

			ap_dimchange_free(dimchange);

			ap_lincons0_array_t arr = ap_lincons0_array_make (2);
			// l[y]-i == 0
			arr.p[0].constyp = AP_CONS_EQ;
			arr.p[0].linexpr0 =
					ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2 );
			arr.p[0].scalar = NULL;
			ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0,
					r->datadim + 2 * r->segmentdim, 1);
			ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, -i);
			// d(y) - d(i) ==0
			arr.p[1].constyp = AP_CONS_EQ;
			arr.p[1].linexpr0 =
					ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
			arr.p[1].scalar = NULL;
			ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
					r->datadim + gdim[i], 1);
			ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
					r->datadim + 2*r->segmentdim + 1, -1);
			ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);

			udcons =ap_abstract0_meet_lincons_array (pr->man_dcons, true, udcons, &arr);

			ap_lincons0_array_clear (&arr);

			if(n_dcons->dcons==NULL)
				n_dcons->dcons=ap_abstract0_copy(pr->man_dcons, udcons);
			else n_dcons->dcons=ap_abstract0_join(pr->man_dcons, false, n_dcons->dcons, udcons);

			ap_abstract0_free(pr->man_dcons, udcons);
		}
	}
	/*end generation for the first pattern */
	/* TODO function for each pattern type */

	/* generating data constraint with the pattern \forall y1,y2. y1<y2 for the node gdim[0] from gdim[1..size_gdim] */
	/**********************************************************/

#ifndef NDEBUG1
	printf("\n merge_1");
	ucons_fprint(stdout, pr->man, r, NULL);
	printf("\n");
#endif


	pattern_t * n2_dcons;
	ap_abstract0_t *udcons2;
	size_t u_seg2;
	pattern_key_t *look2;
	unsigned keylen2;

	u_seg2 = pr->PI[3].u_seg;
	checked_malloc(look2, pattern_key_t, sizeof(pattern_key_t)+u_seg2*sizeof(size_t), 1, return NULL;);
	memset(look2, 0, sizeof(pattern_key_t) + u_seg2*sizeof(size_t));
	look2->type = 3;
	look2->segments[0] = gdim[0];
	keylen2 = u_seg2*sizeof(size_t) + sizeof(pattern_key_t);

	HASH_FIND(hh, r->udcons, look2, keylen2, n2_dcons);

	if(n2_dcons){
		n2_dcons->dcons = NULL;
		//chainge = false;
		for(i = 1; i < size_gdim; i++){
			for(j = i+1; j < size_gdim; j++){
				dimchange = ap_dimchange_alloc(4,0);
				dimchange->dim[0] = r->datadim + 2*r->segmentdim;
				dimchange->dim[1] = r->datadim + 2*r->segmentdim;
				dimchange->dim[2] = r->datadim + 2*r->segmentdim;
				dimchange->dim[3] = r->datadim + 2*r->segmentdim;
				ly1 = r->datadim + 2*r->segmentdim;
				ly2 = r->datadim + 2*r->segmentdim + 1;
				dy1 = r->datadim + 2*r->segmentdim + 2;
				dy2 = r->datadim + 2*r->segmentdim + 3;

				udcons2 = ap_abstract0_add_dimensions(pr->man_dcons,false, r->econs, dimchange, false);
#ifndef NDEBUG1
				printf("\n merge_2 udcons ");
				ap_abstract0_fprint(stdout,pr->man_dcons,udcons2,NULL);
				printf("\n");
#endif
				ap_dimchange_free(dimchange);

				ap_lincons0_array_t arr = ap_lincons0_array_make (4);

				// l[y1]-i == 0
				arr.p[0].constyp = AP_CONS_EQ;
				arr.p[0].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
				arr.p[0].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, ly1, 1);
				ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, -i);
				//l[y2]-j=0
				arr.p[1].constyp = AP_CONS_EQ;
				arr.p[1].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
				arr.p[1].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, ly2, 1);
				ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, -j);
				// d(y1) - d(i) ==0
				arr.p[2].constyp = AP_CONS_EQ;
				arr.p[2].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
				arr.p[2].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[2].linexpr0,
						r->datadim + gdim[i], 1);
				ap_linexpr0_set_coeff_scalar_int (arr.p[2].linexpr0, dy1, -1);
				ap_linexpr0_set_cst_scalar_int (arr.p[2].linexpr0, 0);
				// d(y2) - d(j) ==0
				arr.p[3].constyp = AP_CONS_EQ;
				arr.p[3].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
				arr.p[3].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[3].linexpr0,
						r->datadim + gdim[j], 1);
				ap_linexpr0_set_coeff_scalar_int (arr.p[3].linexpr0, dy2, -1);
				ap_linexpr0_set_cst_scalar_int (arr.p[3].linexpr0, 0);

				udcons2 = ap_abstract0_meet_lincons_array (pr->man_dcons, true, udcons2, &arr);

#ifndef NDEBUG1
				printf("\n merge_2 udcons ");
				ap_abstract0_fprint(stdout, pr->man_dcons, udcons2, NULL);
				printf("\n");
#endif

				ap_lincons0_array_clear (&arr);

				if(n2_dcons->dcons == NULL){
					n2_dcons->dcons = ap_abstract0_copy(pr->man_dcons, udcons2);
					ap_abstract0_free(pr->man_dcons, udcons2);
				}
				else n2_dcons->dcons = ap_abstract0_join(pr->man_dcons, true, n2_dcons->dcons,udcons2);

#ifndef NDEBUG1
				printf("\n merge_2");
				ucons_fprint(stdout, pr->man, r, NULL);
				printf("\n");
#endif

			}//end for
		}//end for

		if(n2_dcons->dcons == NULL)
			n2_dcons->dcons = ap_abstract0_bottom(pr->man_dcons, r->datadim + 2*r->segmentdim + 4, 0);

	}/* end generation with pattern pattern_2_1 y1<y2 for gdim[0] */


#ifndef NDEBUG1
	printf("\n merge_2");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif

	/* l(gdim[0]) == 1 ; */
	ap_linexpr0_t *expr =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
	ap_linexpr0_set_cst_scalar_int (expr, 1);

	ap_linexpr0_t *expr_y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
	//ap_linexpr0_set_cst_scalar_int (expr_y, 1);

	ap_linexpr0_t *expr_2y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
	//ap_linexpr0_set_cst_scalar_int (expr_2y, 1);

	ap_dim_t n, ln;
	n = gdim[0];
	ln = r->datadim + r->segmentdim + n;
	r->econs = ap_abstract0_substitute_linexpr (pr->man_dcons, true, r->econs, ln , expr, NULL);
	// l(gdim[0]) := size_gdim
	ap_linexpr0_set_cst_scalar_int (expr, size_gdim);
	r->econs = ap_abstract0_assign_linexpr (pr->man_dcons, true, r->econs, ln, expr, NULL);

	ap_linexpr0_free(expr);

#ifndef NDEBUG1
	ap_lincons0_t cons1 = cons_k_m(r->datadim,r->segmentdim,1,2);
	if (ap_abstract0_sat_lincons(pr->man_dcons,r->econs,&cons1))
		printf("\n \t k<m \n");
	else
		printf("\n \t !!!(k<m) \n");
#endif

	pattern_t *s;
	size_t nr_y;
	for(s=r->udcons; s!=NULL; s=s->hh.next){
		nr_y = pr->PI[s->key.type].nr_y;
		if(nr_y == 1){
			ap_linexpr0_set_cst_scalar_int (expr_y, 1);
			s->dcons = ap_abstract0_substitute_linexpr (pr->man_dcons, true, s->dcons,ln , expr_y, NULL);
			// l(gdim[0]) := size_gdim
			ap_linexpr0_set_cst_scalar_int (expr_y, size_gdim);
			s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons, ln , expr_y, NULL);

#ifndef NDEBUG1
			n = s->key.segments[0];
			ap_lincons0_t cons2 = cons_dy0(r->datadim,r->segmentdim,r->datadim+2*r->segmentdim+1,n+r->datadim);
			if (ap_abstract0_sat_lincons(pr->man_dcons,s->dcons,&cons2))
				printf("\n \t d(n)<=dy0 \n");
			else
				printf("\n \t !!!(d(n)<=dy0) \n");
#endif

		}
		else {
			ap_linexpr0_set_cst_scalar_int (expr_2y, 1);
			s->dcons = ap_abstract0_substitute_linexpr (pr->man_dcons, true, s->dcons, ln , expr_2y, NULL);
			// l(gdim[0]) := size_gdim
			ap_linexpr0_set_cst_scalar_int (expr_2y, size_gdim);
			s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons, ln , expr_2y, NULL);

#ifndef NDEBUG1
			ap_lincons0_t cons = cons_dy0_dy1(r->datadim,r->segmentdim,dy1,dy2);
			if (ap_abstract0_sat_lincons(pr->man_dcons,s->dcons,&cons))
				printf("\n \t dy0<dy1 \n");
			else
				printf("\n \t !!!(dy0<dy1) \n");
#endif
		}
	}

	ap_linexpr0_free(expr_y);
	ap_linexpr0_free(expr_2y);

#ifndef NDEBUG1
	printf("\n merge returns");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif
	return r;
}


/* for the first pattern : \forall y generate the constraints */
ucons_t * merge_succesors_1(ucons_internal_t *pr, ucons_t * r, ap_dim_t * gdim, size_t size_gdim){

	size_t i,j;
	ap_abstract0_t * udcons, *g_dcons;
	pattern_t * n_dcons;
	size_t contor,u_seg;
	ap_dim_t ly,dy;
	ap_dim_t ly1,ly2,dy1,dy2;

	ap_dimchange_t * dimchange;
	pattern_key_t * look;

	/* generating data constraint witn the pattern \forall y for the node gdim[0] from gdim[1..size_gdim] */

	u_seg = pr->PI[0].u_seg;
	checked_malloc(look, pattern_key_t, sizeof(pattern_key_t)+u_seg*sizeof(size_t), 1, return NULL;);
	memset(look, 0, sizeof(pattern_key_t) + u_seg*sizeof(size_t));
	look->type = 0;
	look->segments[0]=gdim[0];
	unsigned keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);

	HASH_FIND(hh, r->udcons, look, keylen, n_dcons);

	if(n_dcons){

		n_dcons->dcons = NULL;
		for(i = 1; i < size_gdim; i++){
			dimchange = ap_dimchange_alloc(2,0);
			dimchange->dim[0] = r->datadim + 2*r->segmentdim;
			dimchange->dim[1] = r->datadim + 2*r->segmentdim;
			ly = r->datadim + 2*r->segmentdim;
			dy = r->datadim + 2*r->segmentdim + 1;

			udcons = ap_abstract0_add_dimensions(pr->man_dcons, false, r->econs, dimchange, false);

			ap_dimchange_free(dimchange);

			ap_lincons0_array_t arr = ap_lincons0_array_make (2);
			// l[y]-i == 0
			arr.p[0].constyp = AP_CONS_EQ;
			arr.p[0].linexpr0 =
					ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2 );
			arr.p[0].scalar = NULL;
			ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0,
					r->datadim + 2 * r->segmentdim, 1);
			ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, -i);
			// d(y) - d(i) ==0
			arr.p[1].constyp = AP_CONS_EQ;
			arr.p[1].linexpr0 =
					ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
			arr.p[1].scalar = NULL;
			ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
					r->datadim + gdim[i], 1);
			ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
					r->datadim + 2*r->segmentdim + 1, -1);
			ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);

			udcons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, udcons, &arr);

			ap_lincons0_array_clear (&arr);

			if(n_dcons->dcons == NULL)
				n_dcons->dcons = ap_abstract0_copy(pr->man_dcons, udcons);
			else n_dcons->dcons = ap_abstract0_join(pr->man_dcons, false, n_dcons->dcons, udcons);


			ap_abstract0_free(pr->man_dcons,udcons);
		}
	}
	/*end generation for the first pattern */
	/* TODO function for each pattern type */


	/* l(gdim[0]) == 1 ; */
	ap_linexpr0_t *expr =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
	ap_linexpr0_set_cst_scalar_int (expr, 1);

	ap_linexpr0_t *expr_y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
	ap_linexpr0_set_cst_scalar_int (expr_y, 1);

	ap_dim_t n, ln;
	n = gdim[0];
	ln = r->datadim + r->segmentdim + n;
	r->econs = ap_abstract0_substitute_linexpr (pr->man_dcons, true, r->econs, ln , expr, NULL);
	// l(gdim[0]) := size_gdim
	ap_linexpr0_set_cst_scalar_int (expr, size_gdim);
	r->econs = ap_abstract0_assign_linexpr (pr->man_dcons, true, r->econs, ln, expr, NULL);

	ap_linexpr0_free(expr);


	pattern_t *s;
	size_t nr_y;
	for(s = r->udcons; s != NULL; s = s->hh.next){
		s->dcons = ap_abstract0_substitute_linexpr (pr->man_dcons, true, s->dcons, ln , expr_y, NULL);
		// l(gdim[0]) := size_gdim
		ap_linexpr0_set_cst_scalar_int (expr_y, size_gdim);
		s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons, ln , expr_y, NULL);
		ap_linexpr0_set_cst_scalar_int (expr_y, 1);
	}

	ap_linexpr0_free(expr_y);


	return r;
}



ap_abstract0_t *concat_P11 (ucons_internal_t * pr, ucons_t * r, ap_dim_t * cdim,
		size_t size_cdim){

	ap_abstract0_t * aux_dcons=NULL;
	ap_abstract0_t * fy_dcons;
	size_t u_seg;
	size_t i,j,k;
	ap_abstract0_t * ni_dcons;
	ap_dim_t li,ly,di,dy;

	pattern_t * n_dcons;

	ap_dimchange_t dimchange;
	pattern_key_t * look;

	arg_assert (r, return NULL;);
	arg_assert (cdim && size_cdim > 1, return NULL;);

	u_seg = pr->PI[0].u_seg;
	checked_malloc(look, pattern_key_t, sizeof(pattern_key_t) + u_seg*sizeof(size_t), 1, return NULL;);
	memset(look, 0, sizeof(pattern_key_t) + u_seg*sizeof(size_t));
	look->type = 0;
	unsigned keylen = u_seg*sizeof(size_t)+sizeof(pattern_key_t);

#ifndef NDEBUG1
	printf("  \n\t concat_P11 : \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif

	for(i = 1; i < size_cdim; i++){
		look->segments[0] = cdim[i];
		HASH_FIND(hh, r->udcons, look, keylen, n_dcons);
		if(n_dcons){
			if(n_dcons->dcons){

				/* substitute y with y + l(cdim[0]) + ... + l(cdim[i-1]) + 1*/
				ap_linexpr0_t *expr_y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
				ap_linexpr0_set_cst_scalar_int (expr_y, 0);
				for(j  = 0;j < i; j++){
					li = r->datadim + r->segmentdim + cdim[j] ;
					ap_linexpr0_set_coeff_scalar_int (expr_y,li, -1);
				}
				ly = r->datadim + 2 * r->segmentdim ;
				ap_linexpr0_set_coeff_scalar_int (expr_y, ly, 1);

				fy_dcons = ap_abstract0_copy(pr->man_dcons, n_dcons->dcons);
				fy_dcons = ap_abstract0_substitute_linexpr (pr->man_dcons, true, fy_dcons , ly , expr_y, NULL);
				if(aux_dcons)
					aux_dcons = ap_abstract0_join(pr->man_dcons,true, aux_dcons, fy_dcons);
				else aux_dcons = ap_abstract0_copy(pr->man_dcons, fy_dcons);

				ap_linexpr0_free(expr_y);
				ap_abstract0_free(pr->man_dcons, fy_dcons);
			}
		}

#ifndef NDEBUG1
		if(aux_dcons){
			printf("\n \t aux_dcons \n");
			ap_abstract0_fprint(stdout, pr->man_dcons, aux_dcons, NULL);
			printf("\n");
		}
#endif

		/* ni_dcons  =  econs + y == l[cdim[0]] + ... l[cdim[i]]  + d(y) = d(cdim[i]) */
		dimchange.intdim = 2;
		dimchange.realdim = 0;
		dimchange.dim = (ap_dim_t *)malloc(2*sizeof(ap_dim_t));
		dimchange.dim[0] = r->datadim + 2*r->segmentdim;
		dimchange.dim[1] = r->datadim + 2*r->segmentdim;

		ni_dcons = ap_abstract0_copy(pr->man_dcons, r->econs);
		ni_dcons = ap_abstract0_add_dimensions(pr->man_dcons, false, ni_dcons, &dimchange, false);
		free(dimchange.dim);


		ap_lincons0_array_t arr = ap_lincons0_array_make (2);
		// l[y] == l[cdim[0]] + ... l[cdim[i]]
		arr.p[0].constyp = AP_CONS_EQ;
		arr.p[0].linexpr0 =
				ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2 );
		arr.p[0].scalar = NULL;

		for(j=0;j<i;j++){
			li = r->datadim + r->segmentdim + cdim[j] ;
			ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0,li, 1);
		}

		ly = r->datadim + 2*r->segmentdim;
		ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0,ly, -1);
		//ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, 0);

		// d(y) - d(cdim[i]) ==0
		arr.p[1].constyp = AP_CONS_EQ;
		arr.p[1].linexpr0 =
				ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
		arr.p[1].scalar = NULL;
		ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
				r->datadim + cdim[i], 1);
		ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
				r->datadim + 2*r->segmentdim + 1, -1);

		ni_dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, ni_dcons, &arr);
		ap_lincons0_array_clear (&arr);

#ifndef NDEBUG1
		printf("\n CONCAT_P11 aux_dcons ");
		ap_abstract0_fprint(stdout, pr->man_dcons, ni_dcons, NULL);
		printf("\n");
#endif
		if(aux_dcons)
			aux_dcons = ap_abstract0_join(pr->man_dcons,true,aux_dcons,ni_dcons);
		else {
			aux_dcons = ap_abstract0_copy(pr->man_dcons, ni_dcons);
			ap_abstract0_free(pr->man_dcons, ni_dcons);
		}

#ifndef NDEBUG1
		printf("\n CONCAT_P11 aux_dcons ");
		ap_abstract0_fprint(stdout, pr->man_dcons, aux_dcons, NULL);
		printf("\n");
#endif
	}

#ifndef NDEBUG1
	printf("\n CONCAT_P11 retuns: \t ");
	ap_abstract0_fprint(stdout, pr->man_dcons, aux_dcons, NULL);
	printf("\n");
#endif
	return aux_dcons;
}

ap_abstract0_t *concat_P12 (ucons_internal_t * pr, ucons_t * r, ap_dim_t * cdim,
		size_t size_cdim){

	size_t i, j, k, wr;
	int wl;
	ap_abstract0_t * aux_2y_dcons = NULL; // the abstract attached to cdim[0] with P12
	ap_abstract0_t * fy2_dcons;

	pattern_t  *found2_dcons, *found_right_dcons;
	size_t contor, u_seg, u_seg2;
	ap_dim_t ly1, dy1, ly2, dy2;

	ap_abstract0_t *dy1_econs;
	ap_dim_t li, ly, di, dy;

	ap_dimchange_t dimchange;
	pattern_key_t * look, *look2;

	arg_assert (r, return NULL;);
	arg_assert (cdim && size_cdim > 1, return NULL;);

	u_seg = pr->PI[0].u_seg;
	checked_malloc(look,pattern_key_t, sizeof(pattern_key_t) + u_seg*sizeof(size_t), 1, return NULL;);
	memset(look, 0, sizeof(pattern_key_t) + u_seg*sizeof(size_t));
	look->type = 0;
	unsigned keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);

	/* concat segments w.r.t. the pattern \forall y1,y2. y1<y2 for the node gdim[0] from gdim[1..size_gdim] */
	/* aux_2y_dcons is the resulting data constraint */
	u_seg2 = pr->PI[3].u_seg;
	checked_malloc(look2, pattern_key_t, sizeof(pattern_key_t) + u_seg2*sizeof(size_t), 1, return NULL;);
	memset(look2, 0, sizeof(pattern_key_t) + u_seg2*sizeof(size_t));
	look2->type = 3;
	unsigned keylen2 = u_seg2*sizeof(size_t) + sizeof(pattern_key_t);

#ifndef NDEBUG1
	printf("  \n\t concat_P12 : \n");
	ucons_fprint(stdout, pr->man, r, NULL);
	printf("\n");
#endif
	ap_abstract0_t* border;

	ly1 = r->datadim + 2*r->segmentdim;
	ly2 = r->datadim + 2*r->segmentdim + 1;
	dy1 = r->datadim + 2*r->segmentdim + 2;
	dy2 = r->datadim + 2*r->segmentdim + 3;

	for(i = 1;i < size_cdim; i++){

		/* 1. join universal P12 formulas */
		look2->segments[0] = cdim[i];
		HASH_FIND(hh, r->udcons, look2, keylen2, found2_dcons);
		if(found2_dcons){
			if(found2_dcons->dcons){
				/* y1 = y1 + l(cdim[0]) + ... +l(cdim[i-1]) + 1 */
				/* y2 = y2 + l(cdim[0]) + ... +l(cdim[i-1]) + 1*/
				ap_linexpr0_t *expr_2y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
				ap_linexpr0_set_cst_scalar_int (expr_2y, 0);//!!!
				for(j = 0; j < i; j++){
					li = r->datadim + r->segmentdim + cdim[j] ;
					ap_linexpr0_set_coeff_scalar_int (expr_2y, li, -1);//!!!
				}

				ap_linexpr0_set_coeff_scalar_int (expr_2y, ly1, 1);

				fy2_dcons = ap_abstract0_copy(pr->man_dcons, found2_dcons->dcons);
				fy2_dcons = ap_abstract0_substitute_linexpr (pr->man_dcons, false, fy2_dcons,ly1 , expr_2y, NULL);

				ap_linexpr0_set_coeff_scalar_int (expr_2y, ly1, 0);
				ap_linexpr0_set_coeff_scalar_int (expr_2y, ly2, 1);

				fy2_dcons = ap_abstract0_substitute_linexpr (pr->man_dcons, true, fy2_dcons,ly2 , expr_2y, NULL);

				ap_linexpr0_free(expr_2y);

				if(aux_2y_dcons)
					aux_2y_dcons=ap_abstract0_join(pr->man_dcons,true, aux_2y_dcons, fy2_dcons);
				else {
					aux_2y_dcons=ap_abstract0_copy(pr->man_dcons, fy2_dcons);
					ap_abstract0_free(pr->man_dcons, fy2_dcons);
				}
			}
		}

#ifndef NDEBUG1
		printf("\n concat 1. %zu",i);
		ap_abstract0_fprint(stdout,pr->man_dcons,aux_2y_dcons,NULL);
		printf("\n");
#endif
		/* 2. join with data(cdim[i]) < y , \forall y \in cdim[i] */
		for(wr = i; wr < size_cdim; wr++){
			look->segments[0] = cdim[wr];
			HASH_FIND(hh,r->udcons,look,keylen,found_right_dcons);
			if(found_right_dcons){
				if(found_right_dcons->dcons){
					/* border = add dimensions to  \forall y in cdim[i]*/
					dimchange.intdim = 2;
					dimchange.realdim = 0;
					dimchange.dim = (ap_dim_t*) malloc (2*sizeof(ap_dim_t));
					dimchange.dim[0] = r->datadim + 2*r->segmentdim;
					dimchange.dim[1] = r->datadim + 2*r->segmentdim + 1;

					border = ap_abstract0_copy(pr->man_dcons, found_right_dcons->dcons);
					border = ap_abstract0_add_dimensions(pr->man_dcons, true,
							border, &dimchange, false);
					free(dimchange.dim);

					//ly2 == ly2 + l(cdim[0]) + ... + l(cdim[i-1] + 1)
					ap_linexpr0_t *expr_b =  ap_linexpr0_alloc (AP_LINEXPR_DENSE,
							r->datadim + 2 * r->segmentdim + 4);
					ap_linexpr0_set_cst_scalar_int (expr_b, 0);//!!!
					for(j = 0;j < wr; j++){
						li = r->datadim + r->segmentdim + cdim[j] ;
						ap_linexpr0_set_coeff_scalar_int (expr_b, li, -1);//!!!
					}

					ap_linexpr0_set_coeff_scalar_int (expr_b,ly2, 1);
					border = ap_abstract0_substitute_linexpr (pr->man_dcons, true,
							border, ly2, expr_b, NULL);
					ap_linexpr0_free(expr_b);
					/* DO NOT DELETE old in*/
					/* dy1_econs = add dimensions to econs and constrain dy1 = data(cdim[i])*/
					dimchange.intdim = 4;
					dimchange.realdim = 0;
					dimchange.dim = (ap_dim_t*) malloc (4*sizeof(ap_dim_t));
					dimchange.dim[0] = r->datadim + 2*r->segmentdim;
					dimchange.dim[1] = r->datadim + 2*r->segmentdim;
					dimchange.dim[2] = r->datadim + 2*r->segmentdim;
					dimchange.dim[3] = r->datadim + 2*r->segmentdim;

					dy1_econs = ap_abstract0_add_dimensions(pr->man_dcons,false,
							r->econs, &dimchange,false);
					free(dimchange.dim);

					ap_lincons0_array_t arr = ap_lincons0_array_make (2);

					// l[y1] - l(cdim[0]) + ... + l(cdim[i-1]) ==0
					arr.p[0].constyp = AP_CONS_EQ;
					arr.p[0].linexpr0 =
							ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
					arr.p[0].scalar = NULL;
					for(j=0;j<i;j++){
						li = r->datadim + r->segmentdim + cdim[j] ;
						ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, li, 1);
					}
					ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, ly1, -1);
					//ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, 0);

					// d(y1) - d(cdim[i]) ==0
					arr.p[1].constyp = AP_CONS_EQ;
					arr.p[1].linexpr0 =
							ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
					arr.p[1].scalar = NULL;
					ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
							r->datadim + cdim[i], 1);
					ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, dy1, -1);
					//ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);
					/* DO NOT DELETE old in*/
					dy1_econs = ap_abstract0_meet_lincons_array (pr->man_dcons, true, dy1_econs, &arr);
					border = ap_abstract0_meet(pr->man_dcons, true, dy1_econs, border);

					/* Old out */
					//					border = ap_abstract0_meet_lincons_array (pr->man_dcons, true, border, &arr);
					/**/
					ap_lincons0_array_clear (&arr);

#ifndef NDEBUG1
					printf("\n border_right: %zu",i);
					ap_abstract0_fprint(stdout,pr->man_dcons,border,NULL);
					printf("\n");
#endif

					if(aux_2y_dcons)
						aux_2y_dcons = ap_abstract0_join(pr->man_dcons, true, border, aux_2y_dcons);
					else{
						aux_2y_dcons = ap_abstract0_copy(pr->man_dcons, border);
						ap_abstract0_free(pr->man_dcons, border);
					}
					border = NULL;
				}// end found->dcons!=NULL
			}//end found!=NULL i.e. 2. join with (cdim[i],y) with y \in cdim[i]

		}//end for
#ifndef NDEBUG1
		printf("\n concat 2. %zu",i);
		ap_abstract0_fprint(stdout, pr->man_dcons, aux_2y_dcons, NULL);
		printf("\n");
#endif
		/* 3. join with  (y,cdim[i]) with y\in cdim[i-1]*/
		pattern_t * found_left_dcons;
		ap_abstract0_t* dy2_econs;

		for(wl = i-1; wl >= 0; wl--){

			look->segments[0] = cdim[wl];
			HASH_FIND(hh, r->udcons, look, keylen, found_left_dcons);
			if(found_left_dcons){
				if(found_left_dcons->dcons){

					/* border = add dimensions to  \forall y in cdim[i]*/
					dimchange.intdim = 2;
					dimchange.realdim = 0;
					dimchange.dim = (ap_dim_t*) malloc (2*sizeof(ap_dim_t));
					dimchange.dim[0] = r->datadim + 2*r->segmentdim + 1;
					dimchange.dim[1] = r->datadim + 2*r->segmentdim + 2;

					border = ap_abstract0_copy(pr->man_dcons, found_left_dcons->dcons);
					border = ap_abstract0_add_dimensions(pr->man_dcons, true, border, &dimchange, false);
					free(dimchange.dim);

#ifndef NDEBUG1
					printf("\n border : %zu",i);
					ap_abstract0_fprint(stdout,pr->man_dcons,border,NULL);
					printf("\n");
#endif
					/*DO NOT DELETE old in*/
					/* ly1 = ly1 */
					/* dy2_econs = add dimensions to econs and constrain dy2 = data(cdim[i])*/
					dimchange.intdim = 4;
					dimchange.realdim = 0;
					dimchange.dim = (ap_dim_t*) malloc (4*sizeof(ap_dim_t));
					dimchange.dim[0] = r->datadim + 2*r->segmentdim;
					dimchange.dim[1] = r->datadim + 2*r->segmentdim;
					dimchange.dim[2] = r->datadim + 2*r->segmentdim;
					dimchange.dim[3] = r->datadim + 2*r->segmentdim;

					dy2_econs = ap_abstract0_add_dimensions(pr->man_dcons, false, r->econs, &dimchange, false);
					free(dimchange.dim);
					ap_lincons0_array_t arr = ap_lincons0_array_make (2);

					// l[y2] - l(cdim[0]) + ... + l(cdim[i-1]) ==0
					arr.p[0].constyp = AP_CONS_EQ;
					arr.p[0].linexpr0 =
							ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
					arr.p[0].scalar = NULL;
					for(j=0;j<i;j++){
						li = r->datadim + r->segmentdim + cdim[j] ;
						ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, li, 1);
					}
					ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, ly2, -1);
					//ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, 0);

					// d(y2) - d(cdim[i]) ==0
					arr.p[1].constyp = AP_CONS_EQ;
					arr.p[1].linexpr0 =
							ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
					arr.p[1].scalar = NULL;
					ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
							r->datadim + cdim[i], 1);
					ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, dy2, -1);
					//ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);

					/*Old in*/
					dy2_econs = ap_abstract0_meet_lincons_array (pr->man_dcons,
							true, dy2_econs, &arr);
					border = ap_abstract0_meet(pr->man_dcons, true, dy2_econs, border);
					/*Old out*/
					//					border = ap_abstract0_meet_lincons_array (pr->man_dcons, true, border, &arr);
					ap_lincons0_array_clear (&arr);
					/**/
#ifndef NDEBUG1
					printf("\n border_left: %zu",i);
					ap_abstract0_fprint(stdout, pr->man_dcons, border, NULL);
					printf("\n");
#endif
					if(aux_2y_dcons)
						aux_2y_dcons = ap_abstract0_join(pr->man_dcons, true, border, aux_2y_dcons);
					else{
						aux_2y_dcons = ap_abstract0_copy(pr->man_dcons, border);
						ap_abstract0_free(pr->man_dcons, border);
					}
					border = NULL;
				} // end found_left_dcons->dcons!=NULL
			}// end found_left_dcons!=NULL, i.e. end join with (y,cdim[i]) with y\in cdim[i-1]
		}// end for wl<=i-1


#ifndef NDEBUG1
		printf("\n concat 3: %zu",i);
		ap_abstract0_fprint(stdout,pr->man_dcons,aux_2y_dcons,NULL);
		printf("\n");
#endif
		/* 4. join with (cdim[i],cdim[j]) 0<i<j<size_cdim */
		if(i+1 < size_cdim){
			ap_abstract0_t* dy12_econs, *dy12_ucons;

			dimchange.intdim = 4;
			dimchange.realdim = 0;
			dimchange.dim = (ap_dim_t*) malloc (4*sizeof(ap_dim_t));
			dimchange.dim[0] = r->datadim + 2*r->segmentdim;
			dimchange.dim[1] = r->datadim + 2*r->segmentdim;
			dimchange.dim[2] = r->datadim + 2*r->segmentdim;
			dimchange.dim[3] = r->datadim + 2*r->segmentdim;

			dy12_econs = ap_abstract0_copy(pr->man_dcons, r->econs);
			dy12_econs = ap_abstract0_add_dimensions(pr->man_dcons, true,
					dy12_econs, &dimchange, false);
			free(dimchange.dim);
			ap_lincons0_array_t arr = ap_lincons0_array_make (2);

			// l[y1] - l(cdim[0]) + ... + l(cdim[i-1]) ==0
			arr.p[0].constyp = AP_CONS_EQ;
			arr.p[0].linexpr0 =
					ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
			arr.p[0].scalar = NULL;
			for(j = 0; j < i; j++){
				li = r->datadim + r->segmentdim + cdim[j] ;
				ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, li, 1);
			}
			ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, ly1, -1);
			ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, 0);

			// d(y1) - d(cdim[i]) ==0
			arr.p[1].constyp = AP_CONS_EQ;
			arr.p[1].linexpr0 =
					ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
			arr.p[1].scalar = NULL;
			ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
					r->datadim + cdim[i], 1);
			ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, dy1, -1);
			ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);

			dy12_econs = ap_abstract0_meet_lincons_array (pr->man_dcons, true, dy12_econs, &arr);
			ap_lincons0_array_clear (&arr);

			for(j = i+1; j < size_cdim; j++){
				dy12_ucons = ap_abstract0_copy(pr->man_dcons, dy12_econs);
				ap_lincons0_array_t arr = ap_lincons0_array_make (2);

				// l[y2] - l(cdim[0]) + ... + l(cdim[j-1]) ==0
				arr.p[0].constyp = AP_CONS_EQ;
				arr.p[0].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4 );
				arr.p[0].scalar = NULL;
				for(k=0;k<j;k++){
					li = r->datadim + r->segmentdim + cdim[k] ;
					ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, li, 1);
				}
				ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, ly2, -1);
				ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, 0);

				// d(y2) - d(cdim[j]) ==0
				arr.p[1].constyp = AP_CONS_EQ;
				arr.p[1].linexpr0 =
						ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
				arr.p[1].scalar = NULL;
				ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
						r->datadim + cdim[j], 1);
				ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0, dy2, -1);
				ap_linexpr0_set_cst_scalar_int (arr.p[1].linexpr0, 0);

				dy12_ucons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, dy12_ucons, &arr);
				ap_lincons0_array_clear (&arr);

				if(aux_2y_dcons)
					aux_2y_dcons = ap_abstract0_join(pr->man_dcons, true,
							dy12_ucons, aux_2y_dcons);
				else{
					aux_2y_dcons = ap_abstract0_copy(pr->man_dcons, dy12_ucons);
					ap_abstract0_free(pr->man_dcons, dy12_ucons);
				}
			}// end for
		}// end 4. join with (cdim[i],cdim[j])


#ifndef NDEBUG1
		printf("\n concat 4.%zu",i);
		ap_abstract0_fprint(stdout, pr->man_dcons, aux_2y_dcons, NULL);
		printf("\n");
#endif
	}//end for
	// end concat with pattern succ_1
#ifndef NDEBUG1
	printf("\n \t concat P12 returns \n ");
	ap_abstract0_fprint(stdout, pr->man_dcons, aux_2y_dcons, NULL);
	printf("\n");
#endif
	return aux_2y_dcons;
}


/* the function concatentas the segments from cdim preserving information w.r.t the patterns \forall y and \forall y1,y2. y1 < y2 */

ucons_t* concat_nodes_2(ucons_internal_t *pr, ucons_t * r, ap_dim_t* cdim, size_t size_cdim){

	size_t i,j,k;
	ap_abstract0_t * aux_dcons = NULL; // the abstract attached to cdim[0] with P1
	ap_abstract0_t * aux_2y_dcons = NULL; // the abstract attached to cdim[0] with P12
	ap_abstract0_t * dy1_econs;
	ap_abstract0_t * fy_dcons;
	ap_abstract0_t * fy2_dcons;

	pattern_t * n_dcons;
	pattern_t  *found2_dcons, *found_dcons;
	size_t contor, u_seg, u_seg2;
	ap_dim_t ly1, dy1, ly2, dy2;

	ap_abstract0_t * ni_dcons;
	ap_dim_t li, ly, di, dy;

	ap_dimchange_t dimchange;
	pattern_key_t * look, *look2;

	arg_assert (r, return NULL;);
	arg_assert (cdim && size_cdim > 1, return r;);

	dy1 = r->datadim + 2*r->segmentdim + 2;
	dy2 = r->datadim + 2*r->segmentdim + 3 ;

#ifndef NDEBUG1
	printf("\n CONCAT: ");
	ucons_fprint(stdout, pr->man, r, NULL);
	printf("\n");
#endif
	/* concat segments w.r.t. the pattern \forall y for the node cdim[0] from cdim[1..size_cdim]
	 * aux_dcons is the resulting data constraint */
	aux_dcons = concat_P11(pr, r, cdim, size_cdim);

	aux_2y_dcons = concat_P12(pr, r, cdim, size_cdim);

	u_seg = pr->PI[0].u_seg;
	checked_malloc(look, pattern_key_t,
			sizeof(pattern_key_t) + u_seg*sizeof(size_t), 1, return NULL;);
	memset(look, 0, sizeof(pattern_key_t) + u_seg*sizeof(size_t));
	look->type = 0;
	unsigned keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);

	/* concat segments w.r.t. the pattern \forall y1,y2. y1<y2 for the node gdim[0] from gdim[1..size_gdim] */
	/* aux_2y_dcons is the resulting data constraint */
	u_seg2 = pr->PI[3].u_seg;
	checked_malloc(look2, pattern_key_t,
			sizeof(pattern_key_t) + u_seg2*sizeof(size_t), 1, return NULL;);
	memset(look2, 0, sizeof(pattern_key_t) + u_seg2*sizeof(size_t));
	look2->type = 3;
	unsigned keylen2 = u_seg2*sizeof(size_t) + sizeof(pattern_key_t);

	/* pt \forall y inlocuirea vechii constrangeri n_dcons->dcons cu cea nou calculata aux_dcons */
	look->segments[0]=cdim[0];
	HASH_FIND(hh,r->udcons, look, keylen, n_dcons);
	if(n_dcons){
		if(n_dcons->dcons){
			if(aux_dcons)
				n_dcons->dcons = ap_abstract0_join(pr->man_dcons, true, aux_dcons, n_dcons->dcons);
		}
		else n_dcons->dcons= (aux_dcons) ? ap_abstract0_copy(pr->man_dcons, aux_dcons) : NULL;
	}
	else {
		return NULL;
	}

	/* pt \forall y1,y2. y1<y2 inlocuirea vechii constrangeri found2_dcons->dcons cu cea nou calculata aux_2y_dcons */
	look2->segments[0] = cdim[0];
	HASH_FIND(hh,r->udcons, look2, keylen2, found2_dcons);
	if(found2_dcons){
		if(found2_dcons->dcons){
			if(aux_2y_dcons)
				found2_dcons->dcons = ap_abstract0_join(pr->man_dcons, true, aux_2y_dcons, found2_dcons->dcons);
		}
		else
			found2_dcons->dcons= (aux_2y_dcons) ? ap_abstract0_copy(pr->man_dcons, aux_2y_dcons) : NULL;
	}
	//else return NULL;
	//end concat for pattern with succ_1

#ifndef NDEBUG1
	printf("\n concat_nodes_2 before recalc lengths \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif

	/* recalculate length of tdim[0] */

	ap_dim_t l0;
	/* update length for the merged segments */
	/* l(cdim[0]) = l(cdim[0]) + l(cdim[1]) + ... + l(cdim[last])*/

	/* for econs */
	ap_linexpr0_t *expr =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i = 0; i < size_cdim; i++){
		li = r->datadim + r->segmentdim + cdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr, li, 1);
	}
	/* for udcons */
	ap_linexpr0_t *expr_1y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i = 0;i < size_cdim; i++){
		li = r->datadim + r->segmentdim + cdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr_1y, li, 1);
	}
	ap_linexpr0_t *expr_2y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i=0;i<size_cdim;i++){
		li = r->datadim + r->segmentdim + cdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr_2y, li, 1);
	}
	/* TODO allocate linexpr for each pattern type */

	l0 = r->datadim + r->segmentdim + cdim[0] ;
	r->econs =  ap_abstract0_assign_linexpr (pr->man_dcons, true,
			r->econs, l0 , expr, NULL);
	ap_linexpr0_free(expr);

#ifndef NDEBUG1
	ap_lincons0_t cons1 = cons_k_m(r->datadim,r->segmentdim,1,2);
	if (ap_abstract0_sat_lincons(pr->man_dcons,r->econs,&cons1))
		printf("\n \t k<m \n");
	else
		printf("\n \t !!!(k<m) \n");
#endif

	pattern_t *s;
	size_t nr_y;
	for(s = r->udcons; s != NULL; s = s->hh.next){
		nr_y = pr->PI[s->key.type].nr_y;
		if(nr_y == 1){
			s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true,
					s->dcons, l0 , expr_1y, NULL);

#ifndef NDEBUG1
			ap_dim_t n = s->key.segments[0];
			ap_lincons0_t cons2 = cons_dy0(r->datadim, r->segmentdim, r->datadim + 2*r->segmentdim+1,n+r->datadim);
			if (ap_abstract0_sat_lincons(pr->man_dcons, s->dcons, &cons2))
				printf("\n \t d(n)<=dy0 \n");
			else
				printf("\n \t !!!(d(n)<=dy0) \n");
#endif
		}
		if(nr_y == 2){
			s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons, l0 , expr_2y, NULL);

#ifndef NDEBUG1
			ap_lincons0_t cons = cons_dy0_dy1(r->datadim, r->segmentdim, dy1, dy2);
			if (ap_abstract0_sat_lincons(pr->man_dcons, s->dcons, &cons))
				printf("\n \t dy0<dy1 \n");
			else
				printf("\n \t !!!(dy0<dy1) \n");
#endif
		}
	}
	ap_linexpr0_free(expr_1y);
	ap_linexpr0_free(expr_2y);

#ifndef NDEBUG1
	printf("\n concat_nodes_2 returs \n");
	ucons_fprint(stdout,pr->man,r,NULL);
	printf("\n");
#endif
	return r;

}



ucons_t* concat_nodes_1(ucons_internal_t *pr, ucons_t * r, ap_dim_t* cdim, size_t size_cdim){

	size_t i, j;
	ap_abstract0_t * aux_dcons = NULL;
	ap_abstract0_t * aux_2y_dcons = NULL;
	ap_abstract0_t * dy1_econs;
	pattern_t * n_dcons;
	pattern_t  *found2_dcons, *found_dcons;
	size_t contor, u_seg, u_seg2;
	ap_dim_t ly1, dy1;

	ap_abstract0_t * ni_dcons;
	ap_dim_t li, ly, di, dy;

	ap_dimchange_t dimchange;
	pattern_key_t * look, *look2;

	arg_assert (r, return NULL;);
	arg_assert (cdim && size_cdim > 1, return r;);


	/* concat segments w.r.t. the pattern \forall y for the node cdim[0] from cdim[1..size_cdim]
	 * aux_dcons is the resulting data constraint */
	u_seg = pr->PI[0].u_seg;
	checked_malloc(look, pattern_key_t, sizeof(pattern_key_t) + u_seg*sizeof(size_t), 1, return NULL;);
	memset(look, 0, sizeof(pattern_key_t) + u_seg*sizeof(size_t));
	look->type = 0;
	unsigned keylen = u_seg*sizeof(size_t) + sizeof(pattern_key_t);
	//new

	//aux_dcons=ap_abstract0_copy(pr->man_dcons,n_dcons->dcons);
	for(i = 1; i < size_cdim; i++){
		look->segments[0] = cdim[i];
		HASH_FIND(hh, r->udcons, look, keylen, n_dcons);
		if(n_dcons){
			/* TODO add element corresponding to the existential */
			/* join with the universal from cdim[i] not needed for this pattern */
			if(n_dcons->dcons){

				/* */
				ap_linexpr0_t *expr_y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
				ap_linexpr0_set_cst_scalar_int (expr_y, 1);
				for(j = 0; j < i; j++){
					li = r->datadim + r->segmentdim + cdim[j] ;
					ap_linexpr0_set_coeff_scalar_int (expr_y, li, 1);
				}
				ly = r->datadim + 2 * r->segmentdim ;
				ap_linexpr0_set_coeff_scalar_int (expr_y, ly, 1);
				n_dcons->dcons = ap_abstract0_substitute_linexpr (pr->man_dcons, true, n_dcons->dcons,ly , expr_y, NULL);
				if(aux_dcons)
					aux_dcons = ap_abstract0_join(pr->man_dcons, true, aux_dcons, n_dcons->dcons);
				else aux_dcons = ap_abstract0_copy(pr->man_dcons, n_dcons->dcons);

				ap_linexpr0_free(expr_y);
			}
		}
		/* ni_dcons  =  econs + y == l[cdim[0]] + ... l[cdim[i]]  + d(y) = d(cdim[i]) */

		dimchange.intdim = 2;
		dimchange.realdim = 0;
		dimchange.dim = (ap_dim_t *) malloc (2*sizeof(ap_dim_t));
		dimchange.dim[0] = r->datadim + 2*r->segmentdim;
		dimchange.dim[1] = r->datadim + 2*r->segmentdim;

		ni_dcons = ap_abstract0_add_dimensions(pr->man_dcons, false, r->econs, &dimchange,false);
		free(dimchange.dim);


		ap_lincons0_array_t arr = ap_lincons0_array_make (2);
		// l[y] == l[cdim[0]] + ... l[cdim[i]]
		arr.p[0].constyp = AP_CONS_EQ;
		arr.p[0].linexpr0 =
				ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2 );
		arr.p[0].scalar = NULL;

		for(j = 0; j < i; j++){
			li = r->datadim + r->segmentdim + cdim[j] ;
			ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, li, 1);
		}

		ly = r->datadim + 2*r->segmentdim;
		ap_linexpr0_set_coeff_scalar_int (arr.p[0].linexpr0, ly, -1);
		ap_linexpr0_set_cst_scalar_int (arr.p[0].linexpr0, 0);

		// d(y) - d(cdim[i]) ==0
		arr.p[1].constyp = AP_CONS_EQ;
		arr.p[1].linexpr0 =
				ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 2);
		arr.p[1].scalar = NULL;
		ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
				r->datadim + cdim[i], 1);
		ap_linexpr0_set_coeff_scalar_int (arr.p[1].linexpr0,
				r->datadim + 2*r->segmentdim + 1, -1);

		ni_dcons = ap_abstract0_meet_lincons_array (pr->man_dcons, true, ni_dcons, &arr);

		ap_lincons0_array_clear (&arr);

		if(aux_dcons)
			aux_dcons=ap_abstract0_join(pr->man_dcons, true, aux_dcons, ni_dcons);
		else aux_dcons=ap_abstract0_copy(pr->man_dcons, ni_dcons);


	}

	/*end computation for the first pattern */


	/* pt \forall y inlocuirea vechii constrangeri n_dcons->dcons cu cea nou calculata aux_dcons */
	look->segments[0] = cdim[0];
	HASH_FIND(hh,r->udcons, look, keylen, n_dcons);
	if(n_dcons){
		if(n_dcons->dcons){
			if(aux_dcons)
				n_dcons->dcons = ap_abstract0_join(pr->man_dcons, true, aux_dcons, n_dcons->dcons);
		}
		else n_dcons->dcons= (aux_dcons) ? ap_abstract0_copy(pr->man_dcons, aux_dcons) : NULL;
	}
	else {
		return NULL;
	}



	/* recalculate length of tdim[0] */

	ap_dim_t l0;
	/* update length for the merged segments */
	/* l(cdim[0]) = l(cdim[0]) + l(cdim[1]) + ... + l(cdim[last])*/

	/* for econs */
	ap_linexpr0_t *expr =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i = 0; i < size_cdim; i++){
		li = r->datadim + r->segmentdim + cdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr,li, 1);
	}
	/* for udcons */
	ap_linexpr0_t *expr_1y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim+2);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i = 0; i < size_cdim; i++){
		li = r->datadim + r->segmentdim + cdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr_1y, li, 1);
	}
	ap_linexpr0_t *expr_2y =  ap_linexpr0_alloc (AP_LINEXPR_DENSE, r->datadim + 2 * r->segmentdim + 4);
	ap_linexpr0_set_cst_scalar_int (expr, 0);
	for(i = 0; i < size_cdim; i++){
		li = r->datadim + r->segmentdim + cdim[i] ;
		ap_linexpr0_set_coeff_scalar_int (expr_2y, li, 1);
	}
	/* TODO allocate linexpr for each pattern type */

	l0 = r->datadim + r->segmentdim + cdim[0] ;
	r->econs =  ap_abstract0_assign_linexpr (pr->man_dcons, true, r->econs,l0 , expr, NULL);

	ap_linexpr0_free(expr);

	pattern_t *s;
	size_t nr_y;
	for(s=r->udcons; s!=NULL; s=s->hh.next){
		nr_y = pr->PI[s->key.type].nr_y;
		if(nr_y == 1)
			s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons,l0 , expr_1y, NULL);
		if(nr_y == 2)
			s->dcons = ap_abstract0_assign_linexpr (pr->man_dcons, true, s->dcons,l0 , expr_2y, NULL);
	}

	return r;
}




