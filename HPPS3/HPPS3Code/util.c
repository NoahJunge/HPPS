#include "util.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>




/**
 * Calculate the Euclidean distance between two points `x` and `y` in `d` dimensions.
 * Formula: sqrt( sum (x[i] - y[i])^2 )
 */
double distance(int d, const double *x, const double *y) {
    double sum = 0.0; // Initialize sum, to save the squared differences between the dimension of x and y
    for (int i = 0; i < d; i++) { // Loop over each dimension
        double diff = x[i] - y[i]; // Calculate the difference between the two points
        sum += diff * diff; // Square the difference and add to the sum
    }
    return sqrt(sum); // Return the square root of the sum
}


//chatgbt was used to very syntax and for debugging purposes

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
    double candidate_dist = distance(d, query, &points[candidate * d]); // Distance to candidate
    double farthest_dist = 0.0;  // Distance to the current farthest point in 'closest'
    int farthest_index = -1;     // Index of the farthest point

    printf("Candidate %d, Distance: %f\n", candidate, candidate_dist);

    // Find the farthest point in `closest` or the first empty slot.
    for (int i = 0; i < k; i++) {
        if (closest[i] == -1) { // If an empty slot is found
            printf("Empty slot at %d, inserting candidate %d\n", i, candidate);
            closest[i] = candidate;
            
            // Sort the array after adding the new candidate
            for (int m = 0; m < k - 1; m++) {
                for (int n = m + 1; n < k && closest[n] != -1; n++) {
                    double dist_m = distance(d, query, &points[closest[m] * d]);
                    double dist_n = distance(d, query, &points[closest[n] * d]);
                    if (dist_m > dist_n) {
                        int temp = closest[m];
                        closest[m] = closest[n];
                        closest[n] = temp;
                    }
                }
            }
            return 1;
        }

        double dist = distance(d, query, &points[closest[i] * d]);
        printf("Checking closest[%d] = %d, Distance: %f\n", i, closest[i], dist);

        if (dist > farthest_dist) { // Track the farthest point
            farthest_dist = dist;
            farthest_index = i;
        }
    }

    // Replace the farthest point if the candidate is closer
    if (candidate_dist < farthest_dist) {
        printf("Replacing farthest point %d (Distance: %f) with candidate %d\n",
               closest[farthest_index], farthest_dist, candidate);
        closest[farthest_index] = candidate;

        // Sort the array after replacing the farthest point
        for (int m = 0; m < k - 1; m++) {
            for (int n = m + 1; n < k && closest[n] != -1; n++) {
                double dist_m = distance(d, query, &points[closest[m] * d]);
                double dist_n = distance(d, query, &points[closest[n] * d]);
                if (dist_m > dist_n) {
                    int temp = closest[m];
                    closest[m] = closest[n];
                    closest[n] = temp;
                }
            }
        }
        return 1;
    }

    printf("Candidate %d not closer than the farthest point\n", candidate);
    return 0; // Candidate is not closer than the farthest point
}
