#include "util.h"
#include <math.h>
#include <assert.h>
#include <stdio.h> // Added for debugging

/**
 * Calculate the Euclidean distance between two points `x` and `y` in `d` dimensions.
 * Formula: sqrt( sum (x[i] - y[i])^2 )
 */
double distance(int d, const double *x, const double *y) {
    double sum = 0.0; // initialize sum, to save the squared differences between the dimension of x and y
    for (int i = 0; i < d; i++) { // a loop, starts at i=0, and ends when not(i<d). i increments for each 
        double diff = x[i] - y[i]; // calculate the diff between the two points
        sum += diff * diff; //  square the diff AND add it to the sum 
    }
    double result = sqrt(sum); //return the square root of the sum
    printf("Distance calculated: %f\n", result); // Debugging
    return result;
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
    double candidate_dist = distance(d, query, &points[candidate * d]);//calculate the distance between the query point and the candidate point
    double farthest_dist = 0.0; // the greatest dist  curryently
    int farthest_index = -1; // the index of the point with the gratest distance

    // Find the farthest point in `closest` or the first empty slot.
    for (int i = 0; i < k; i++) { // loop for while i <k
        if (closest[i] == -1) { // if Empty slot in array: 
            closest[i] = candidate;//then insert candidate
            printf("Inserted candidate %d into empty slot\n", candidate); // Debugging
            return 1;
        }
        double dist = distance(d, query, &points[closest[i] * d]);// calculate the distance from the query point to each point in closest
        if (dist > farthest_dist) {// if the distance is smaller than the farthest point in closest
            farthest_dist = dist; // then replace farthest dist with dist
            farthest_index = i; //  set the farthest index to i
        }
    }

    if (candidate_dist < farthest_dist) {  // if candidate distance is closer than farthest dist
        closest[farthest_index] = candidate; // then replace 
        printf("Replaced farthest index %d with candidate %d\n", farthest_index, candidate); // Debugging
        return 1; // return 1 for succes
    }

    return 0; // Candidate is not closer than the farthest point.
}
