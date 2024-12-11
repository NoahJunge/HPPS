#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// Read points from a points data file.
double* read_points(FILE *f, int *n_out, int *d_out) {
    if (!f || !n_out || !d_out) {
        return NULL;
    }

    if (fread(n_out, sizeof(int32_t), 1, f) != 1 || 
        fread(d_out, sizeof(int32_t), 1, f) != 1) {
        return NULL;
    }

    int n = *n_out;
    int d = *d_out;

    double *data = (double*)malloc(n * d * sizeof(double));
    if (!data) {
        return NULL;
    }

    if (fread(data, sizeof(double), n * d, f) != (size_t)(n * d)) {
        free(data);
        return NULL;
    }

    return data;
}




// Read indexes from an indexes data file.
int* read_indexes(FILE *f, int *n_out, int *k_out) {
    if (!f || !n_out || !k_out) {
        return NULL;
    }

    if (fread(n_out, sizeof(int32_t), 1, f) != 1 || 
        fread(k_out, sizeof(int32_t), 1, f) != 1) {
        return NULL;
    }

    int n = *n_out;
    int k = *k_out;

    int *data = (int*)malloc(n * k * sizeof(int));
    if (!data) {
        return NULL;
    }

    if (fread(data, sizeof(int), n * k, f) != (size_t)(n * k)) {
        free(data);
        return NULL;
    }

    return data;
}





// Write a points data file.
int write_points(FILE *f, int32_t n, int32_t d, double *data) {
    if (!f || !data) {
        return 1;
    }

    if (fwrite(&n, sizeof(int32_t), 1, f) != 1 || 
        fwrite(&d, sizeof(int32_t), 1, f) != 1) {
        return 1;
    }

    if (fwrite(data, sizeof(double), n * d, f) != (size_t)(n * d)) {
        return 1;
    }

    return 0;
}





// Write an indexes data file.
int write_indexes(FILE *f, int32_t n, int32_t k, int *data) {
    if (!f || !data) {
        return 1;
    }

    if (fwrite(&n, sizeof(int32_t), 1, f) != 1 || 
        fwrite(&k, sizeof(int32_t), 1, f) != 1) {
        return 1;
    }

    if (fwrite(data, sizeof(int), n * k, f) != (size_t)(n * k)) {
        return 1;
    }

    return 0;
}
