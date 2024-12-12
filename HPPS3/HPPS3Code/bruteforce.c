#include "bruteforce.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h> // Added for debugging output

int* knn(int k, int d, int n, const double *points, const double* query) {
    int* closest = malloc(k * sizeof(int));
    for (int i = 0; i < k; i++) {
        closest[i] = -1;  // Initialize with invalid indices
    }

    for (int i = 0; i < n; i++) {
        printf("Checking point %d\n", i); // Debugging: Log point being checked
        insert_if_closer(k, d, points, closest, query, i);
    }

    printf("Closest neighbors: ");
    for (int i = 0; i < k; i++) {
        printf("%d ", closest[i]); // Debugging: Print resulting closest indices
    }
    printf("\n");

    return closest;
}
