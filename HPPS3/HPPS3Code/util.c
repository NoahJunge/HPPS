#include "util.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

/**
 * Calculate the Euclidean distance between two points `x` and `y` in `d` dimensions.
 * Formula: √( ∑ (x[i] - y[i])² )
 */
double distance(int d, const double *x, const double *y) {
    double sum = 0.0;
    for (int i = 0; i < d; i++) {
        double diff = x[i] - y[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

/**
 * Insert a candidate point into the array of k closest points if it is closer
 * to the query than the farthest point in the current closest set.
 *
 * @param k        Maximum number of closest points to track.
 * @param d        Dimensionality of the space.
 * @param points   Array of all reference points.
 * @param closest  Array of length `k` holding indexes of closest points or -1 for empty slots.
 * @param query    The query point for distance calculation.
 * @param candidate Index of the candidate point in the `points` array.
 * @return         1 if the candidate is added to `closest`, 0 otherwise.
 */
int insert_if_closer(int k, int d,
                     const double *points, int *closest, const double *query,
                     int candidate) {
    double candidate_dist = distance(d, query, &points[candidate * d]);
    double farthest_dist = 0.0;
    int farthest_index = -1;

    // Find the farthest point in `closest` or the first empty slot.
    for (int i = 0; i < k; i++) {
        if (closest[i] == -1) {
            // Empty slot: insert candidate directly.
            closest[i] = candidate;
            return 1;
        }
        double dist = distance(d, query, &points[closest[i] * d]);
        if (dist > farthest_dist) {
            farthest_dist = dist;
            farthest_index = i;
        }
    }

    // Replace the farthest point if the candidate is closer.
    if (candidate_dist < farthest_dist) {
        closest[farthest_index] = candidate;
        return 1;
    }

    return 0; // Candidate is not closer than the farthest point.
}
