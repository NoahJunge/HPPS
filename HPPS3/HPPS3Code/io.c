#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// Read points from a points data file.
double* read_points(FILE *f, int *n_out, int *d_out) {
    if (!f || !n_out || !d_out) { // check if f, n_out, d_out are valid, eg. not Null
        return NULL;
    }

    if (fread(n_out, sizeof(int32_t), 1, f) != 1 || // checks if the starting ints are read(number of rows and coloums), if not return null
        fread(d_out, sizeof(int32_t), 1, f) != 1) { // (fread, return 1 if succesfull)
        return NULL;
    }

    int n = *n_out; // extract value from n_out: n number of rows
    int d = *d_out; // extract value from d_out: d number of columns(dimensions)
    if (!n || !d){
        return NULL;
    }

    printf("Reading %d points with %d dimensions\n", n, d); // Debugging

    double *data = (double*)malloc(n * d * sizeof(double)); // allocate memory to hold n*d doubles, meaning an array
    if (!data) { // if no data, then failure
        return NULL; 
    }

    if (fread(data, sizeof(double), n * d, f) != (size_t)(n * d)) {// save n*d doubles to data from the file f
        free(data); // fread  returns the number of correctly read doubles, it this isnt n*d, then free up the space at data again.
        return NULL; 
    }

    printf("Successfully read points from file\n"); // Debugging
    return data;
}

// Read indexes from an indexes data file.
//for explanation of each line of code, please look at the function above, which is quite similar
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
    if (!n || !k){
        return NULL;
    }


    printf("Reading %d queries with %d neighbors each\n", n, k); // Debugging

    int *data = (int*)malloc(n * k * sizeof(int));
    if (!data) {
        return NULL;
    }

    if (fread(data, sizeof(int), n * k, f) != (size_t)(n * k)) {
        free(data);
        return NULL;
    }

    printf("Successfully read indexes from file\n"); // Debugging
    return data;
}
// Write a points data file.
int write_points(FILE *f, int32_t n, int32_t d, double *data) {
    if (!f || !data) { // if the file, or the data, isnt valid then
        return 1;  //return error code 1
    }

    if (fwrite(&n, sizeof(int32_t), 1, f) != 1 || // if not able to write the data n to file
        fwrite(&d, sizeof(int32_t), 1, f) != 1) { // if not able to write the data n to file
        return 1; //then return error code 1
    }

    if (fwrite(data, sizeof(double), n * d, f) != (size_t)(n * d)) { // write n*d doubles from the data array to the file
        return 1; // if not then return error code 1
    }

    printf("Successfully wrote points to file\n"); // Debugging
    return 0; // return 0 for success
}

// Write an indexes data file.
int write_indexes(FILE *f, int32_t n, int32_t k, int *data) {
    if (!f || !data) { // check if file and data is valid
        return 1; // if not, return error code 1
    }

    if (fwrite(&n, sizeof(int32_t), 1, f) != 1 || // writes n to file f
        fwrite(&k, sizeof(int32_t), 1, f) != 1) { // writes k to file f
        return 1; // if not, return error code 1
    }

    if (fwrite(data, sizeof(int), n * k, f) != (size_t)(n * k)) { // writes n*k integers to file f
        return 1; // if not, return error code 1
    }

    printf("Successfully wrote indexes to file\n"); // Debugging
    return 0; // return 0 for success
}