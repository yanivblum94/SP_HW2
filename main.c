/*
 * main.c
 *
 *  Created on: 2 May 2020
 *      Author: Yaniv Blum
 */
#include "spmat.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "SPBufferset.h"
#include <math.h>

static const double EPSILON = 0.00001;

void initialize_random_vector(double *currvector, int n){
	int i;
	srand(time(NULL));
	for(i = 0; i < n; ++i){
		currvector[i] = (double)rand();
	}
}

void read_sparse_matrix(spmat matrix, int n, char flag, FILE input){
	if(flag == 'l'){
		read_in_list_format(matrix, n, input);
	}
	else{
		read_in_array_format(matrix, n, input);
	}
}

void read_in_list_format(spmat matrix, int n, FILE input){
	int i, d;
	double *temp_row;
	matrix = spmat_allocate_list(n);
	temp_row = (double*)calloc(n, sizeof(double));
	for(i = 0; i < n; i++){
		d = fread(&temp_row, sizeof(double), n, input);
		assert(d == n);
		add_row(matrix, temp_row, i);
	}
	free(temp_row);
}

void read_in_array_format(spmat matrix, int n, FILE input){
	int nnz, d, i;
	double num;
	double *temp_row;
	nnz = 0;
	d = fread(&num, sizeof(double), 1, input);
	while(d == 1){
		if(num != 0){/*non zero element of the matrix*/
			nnz++;
		}
		d = fread(&num, sizeof(double), 1, input);
	}
	matrix = spmat_allocate_array(n, nnz);
	rewind(input);
	d = fread(&n, sizeof(int), 1, input);
	assert(d == 1);
	for(i = 0; i < n; i++){
		d = fread(&temp_row, sizeof(double), n, input);
		assert(d == n);
		add_row(matrix, temp_row, i);
	}
	free(temp_row);
}

double clacnorm(double *nextvector, int n){
	int i;
	double sum;
	sum = 0.0;
	for(i = 0; i < n; i++){
		sum += pow(nextvector[i],2.0);
	}
	return sqrt(sum);
}


void poweriteration(spmat matrix, double *currvector, double *nextvector, int n){
	int norm, i;
	mult(matrix, currvector, nextvector);
	norm = calcnorm(nextvector, n);
	for(i = 0; i < n; i++){
		nextvector[i] = (double)nextvector[i]/norm;
	}
}

int check(double *currvector, double *nextvector, int n){
	int i;
	for(i = 0; i < n; ++i){
		if(fabs(currvector[i] - nextvector[i]) >= EPSILON){
			return 0;
		}
	}
	return 1;
}

void calc_eigen(spmat matrix, double *currvector, double *nextvector, int n){
	int i;
	poweriteration(matrix, currvector, nextvector, n);
	while(check(currvector, nextvector, n) != 1){
		for(i = 0; i < n; i++){
			currvector[i] = nextvector[i];
		}
		poweriteration(matrix, currvector, nextvector, n);
	}
}

void write_eigen_to_outf(double nextvector, int n, FILE output){
	int d;
	d = fwrite(&n, sizeof(int), 1, output);
	assert(d == 1);
	d = fwrite(nextvector, sizeof(double), n, output);
	assert(d == n);
}

int main(int argc, char* argv[]){
	FILE* input, output, initvector;
	double *currvector, *nextvector;
	spmat *matrix;
	int n, d;
	char flag;
	clock_t start,end;
	double time1;
	SP_BUFF_SET();
	start = clock();
	input = fopen(argv[1], "r");/*get the input matrix*/
	d = fread(&n, sizeof(int), 1, input);
	assert(d == 1);
	currvector = (double*)calloc(n, sizeof(double));
	nextvector = (double*)calloc(n, sizeof(double));
	if(argc == 4){/*the initial vector is not given*/
		initialize_random_vector(currvector);
		output = fopen(argv[2], "w");
		if(argv[3] == "-list"){
			flag = 'l';
		}
		else{
			flag = 'a';
		}
	}
	else{
		assert(argc == 5); /*the initial vector is given*/
		initvector = fopen(argv[2], "r");
		d = fread(&currvector, sizeof(double), n, initvector);
		assert(d == n);
		output = fopen(argv[3], "w");
		if(argv[4] == "-list"){
			flag = 'l';
		}
		else{
			flag = 'a';
		}
	}
	read_sparse_matrix(matrix, n, flag, input);
	calc_eigen(matrix, currvector, nextvector, n);
	write_eigen_to_outf(nextvector, n, output);
	end = clock();
	/*time1 = ((double) (end - start) / CLOCKS_PER_SEC);*/
	fclose(input);
	fclose(output);
	free(currvector);
	free(nextvector);
	free(matrix);
	return 0;

}

