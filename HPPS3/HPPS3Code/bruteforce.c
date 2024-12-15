#include "bruteforce.h"
#include "util.h"
#include <stdlib.h>
#include <float.h>

/**
 * Helper function to sort indices by distance.
 * Sorting is done using a simple selection sort for small arrays.
 */
void sort_by_distance(int k, const double *distances, int *indices) {
    for (int i = 0; i < k - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < k; j++) {
            if (distances[indices[j]] < distances[indices[min_idx]]) {
                min_idx = j;
            }
        }
        // Swap indices
        int temp = indices[i];
        indices[i] = indices[min_idx];
        indices[min_idx] = temp;
    }
}

/**
 * knn: Find the k-nearest neighbors of a query point using brute-force.
 * @param k        Number of neighbors to find.
 * @param d        Dimensionality of the points.
 * @param n        Number of reference points.
 * @param points   Array of reference points (flattened row-major format).
 * @param query    The query point to find neighbors for.
 * @return         Array of k indices corresponding to the nearest neighbors.
 */
int* knn(int k, int d, int n, const double *points, const double *query) {
    int* closest = malloc(k * sizeof(int));
    double* distances = malloc(n * sizeof(double));
    if (!closest || !distances) {
        free(closest);
        free(distances);
        return NULL; // Memory allocation failed
    }

    // Initialize closest array with invalid indices
    for (int i = 0; i < k; i++) {
        closest[i] = -1;
    }

    // Calculate distances for all points
    for (int i = 0; i < n; i++) {
        distances[i] = distance(d, query, &points[i * d]);
        insert_if_closer(k, d, points, closest, query, i);
    }

    // Sort closest indices based on distances
    sort_by_distance(k, distances, closest);

    free(distances);
    return closest;
}
