/*
 * spmat.c
 *
 *  Created on: 2 ���� 2020
 *      Author: irist
 */

#include "spmat.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct linked_list{

	/*value*/
	double val;

	/*column index*/
	int col;

	/*next pointer*/
	struct linked_list *next;

}linked_list;

typedef struct spmat_arr{

	/*matrix size*/
	int n;

	/*number of non zero elements*/
	int nnz;

	/*values array*/
	double *values;

	/*column index array*/
	int *colind;

	/*row index array*/
	int *rowptr;
}spmat_arr;


void add_row_in_list(struct _spmat *A, const double *row, int i){
	int j;
	linked_list *currlist;
	currlist = (linked_list)(A -> private[i]);
	for(j = 0; j < A->n; j++){
		if(row[j] != 0){
			currlist->val = row[j];
			currlist->col = j;
			currlist = currlist->next;
		}
	}
}

void add_row_in_array(struct _spmat *A, const double *row, int i){
	int nnz_ind,j;
	spmat_arr *arr_imp;
	arr_imp = A->private;
	nnz_ind = arr_imp->rowptr[i];
	for(j=0; j<arr_imp->n; j++){
		if(row[j]!= 0){
			arr_imp->values[nnz_ind] = row[j];
			arr_imp->colind[nnz_ind] = j;
			nnz_ind++;
		}
	}
	arr_imp->rowptr[i+1] = nnz_ind;

}

void free_in_list(struct _spmat *A){
	int i;
	linked_list *currlist;
	for(i = 0; i < A->n; i++){
		currlist = A->private[i];
		while(currlist != NULL){
			free(currlist);
			currlist = currlist->next;
		}
	}
	free(A->private);
}

void free_in_array(struct _spmat *A){
	spmat_arr *arr_imp;
	arr_imp = A->private;
	free(arr_imp->colind);
	free(arr_imp->rowptr);
	free(arr_imp->values);
	free(arr_imp);
}

void mult_in_list(const struct _spmat *A, const double *v, double *result){
	int i, j, n;
	double dotproduct;
	linked_list *currlist;
	n = A->n;
	for(i = 0 ; i < n; i++){
		dotproduct = 0.0;
		currlist = A->private[i];
		while(currlist != NULL){
			j = currlist->col;
			dotproduct += currlist->val * v[j];
			currlist = currlist->next;
		}
		result[i] = dotproduct;
	}
}

void mult_in_array(const struct _spmat *A, const double *v, double *result){
	spmat_arr *arr_imp;
	double sum;
	int i,ind,diff,j;
	sum=0.0;
	arr_imp = A->private;
	for(i=0;i<arr_imp->n;i++){
		ind = arr_imp->rowptr[i];
		diff = arr_imp->rowptr[i+1] - arr_imp->rowptr[i];
		for(j=ind; j<(ind+diff);j++){
			sum += arr_imp->values[j]*v[arr_imp->colind[j]];
		}
		result[i] = sum;
		sum=0.0;
	}
}

/* Allocates a new linked-lists sparse matrix of size n */
spmat* spmat_allocate_list(int n){
	spmat matrix;
	matrix.private = (linked_list*)malloc(n*sizeof(linked_list));
	matrix.add_row = add_row_in_list;
	matrix.free = free_in_list;
	matrix.mult = mult_in_list;
	return matrix;

}

/* Allocates a new arrays sparse matrix of size n with nnz non-zero elements */
spmat* spmat_allocate_array(int n, int nnz){
	spmat matrix;
	spmat_arr arr_imp;
	arr_imp.n = n;
	arr_imp.nnz = nnz;
	arr_imp.values = (double*)calloc(nnz, sizeof(double));
	arr_imp.colind = (int*)calloc(nnz, sizeof(int));
	arr_imp.rowptr = (int*)calloc(n + 1, sizeof(int));
	matrix.private = arr_imp;
	matrix.add_row = add_row_in_array;
	matrix.free = free_in_array;
	matrix.mult = mult_in_array;
	return matrix;
}

