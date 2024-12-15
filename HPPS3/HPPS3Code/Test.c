#include "io.h"
#include "util.h"
#include "bruteforce.h"
#include "kdtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

//this test file was developed using chatGbt


// Helper function to print points
void print_points(double *data, int n, int d) {
    for (int i = 0; i < n; i++) {
        printf("Point %d: ", i);
        for (int j = 0; j < d; j++) {
            printf("%f ", data[i * d + j]);
        }
        printf("\n");
    }
}

// Test the distance function
void test_distance() {
    double point1[] = {1.0, 2.0, 3.0};
    double point2[] = {4.0, 5.0, 6.0};
    int d = 3;

    double dist = distance(d, point1, point2);
    printf("\nTesting distance function:\n");
    printf("Point 1: (1.0, 2.0, 3.0)\n");
    printf("Point 2: (4.0, 5.0, 6.0)\n");
    printf("Expected distance: 5.196152, Calculated distance: %f\n", dist);

    if (fabs(dist - 5.196152) < 1e-6) {
        printf("Distance function passed!\n");
    } else {
        printf("Distance function failed!\n");
    }
}

// Test the insert_if_closer function
void test_insert_if_closer() {
    int k = 3;          // Number of closest neighbors to track
    int d = 2;          // Number of dimensions
    int n = 5;          // Number of points
    int closest[3] = {-1, -1, -1};

    // Reference points
    double points[] = {
        0.1, 0.2,   // Point 0
        0.3, 0.4,   // Point 1
        0.5, 0.6,   // Point 2
        0.7, 0.8,   // Point 3
        0.9, 1.0    // Point 4
    };

    double query[] = {0.4, 0.5}; // Query point

    printf("\nTesting insert_if_closer function:\n");
    printf("Query point: (%f, %f)\n", query[0], query[1]);
    printf("Reference points:\n");
    print_points(points, n, d);

    for (int i = 0; i < n; i++) {
        insert_if_closer(k, d, points, closest, query, i);
    }

    printf("Indexes of the 3 closest points: ");
    for (int i = 0; i < k; i++) {
        printf("%d ", closest[i]);
    }
    printf("\n");
}

// Test the brute-force knn function
void test_bruteforce_knn() {
    int k = 3;          // Number of neighbors
    int d = 2;          // Dimensions
    int n = 5;          // Number of points

    double points[] = {
        0.1, 0.2,   // Point 0
        0.3, 0.4,   // Point 1
        0.5, 0.6,   // Point 2
        0.7, 0.8,   // Point 3
        0.9, 1.0    // Point 4
    };

    double query[] = {0.4, 0.5};

    printf("\nTesting brute-force k-NN function:\n");
    printf("Query point: (%f, %f)\n", query[0], query[1]);
    printf("Points dataset:\n");
    print_points(points, n, d);

    int *closest = knn(k, d, n, points, query);

    printf("Indexes of the %d nearest neighbors: ", k);
    for (int i = 0; i < k; i++) {
        printf("%d ", closest[i]);
    }
    printf("\n");

    free(closest);
}

// Test the k-d tree knn function
void test_kdtree_knn() {
    int k = 3;          // Number of neighbors
    int d = 2;          // Dimensions
    int n = 5;          // Number of points

    double points[] = {
        0.1, 0.2,   // Point 0
        0.3, 0.4,   // Point 1
        0.5, 0.6,   // Point 2
        0.7, 0.8,   // Point 3
        0.9, 1.0    // Point 4
    };

    double query[] = {0.4, 0.5};

    printf("\nTesting k-d tree k-NN function:\n");
    printf("Query point: (%f, %f)\n", query[0], query[1]);
    printf("Reference points:\n");
    print_points(points, n, d);

    struct kdtree *tree = kdtree_create(d, n, points);
    if (!tree) {
        printf("Error: k-d tree creation failed.\n");
        return;
    }

    int *closest = kdtree_knn(tree, k, query);

    printf("Indexes of the %d nearest neighbors using k-d tree: ", k);
    for (int i = 0; i < k; i++) {
        printf("%d ", closest[i]);
    }
    printf("\n");

    free(closest);
    kdtree_free(tree);
}

int main() {
    const char *filename = "test_points.dat";
    int32_t n = 5; // Number of points
    int32_t d = 2; // Number of dimensions

    // Step 1: Create test points
    double points[] = {
        0.1, 0.2,
        0.3, 0.4,
        0.5, 0.6,
        0.7, 0.8,
        0.9, 1.0
    };

    printf("Original points:\n");
    print_points(points, n, d);

    // Step 2: Write points to a file
    FILE *file_out = fopen(filename, "wb");
    if (!file_out) {
        perror("Failed to open file for writing");
        return 1;
    }

    if (write_points(file_out, n, d, points) != 0) {
        fprintf(stderr, "Error writing points to file\n");
        fclose(file_out);
        return 1;
    }
    fclose(file_out);
    printf("Points written successfully to %s\n", filename);

    // Step 3: Read points back from the file
    FILE *file_in = fopen(filename, "rb");
    if (!file_in) {
        perror("Failed to open file for reading");
        return 1;
    }

    int n_read, d_read;
    double *read_data = read_points(file_in, &n_read, &d_read);
    fclose(file_in);

    if (!read_data) {
        fprintf(stderr, "Error reading points from file\n");
        return 1;
    }

    printf("Read points (n=%d, d=%d):\n", n_read, d_read);
    print_points(read_data, n_read, d_read);

    // Step 4: Test all components
    test_distance();
    test_insert_if_closer();
    test_bruteforce_knn();
    test_kdtree_knn();

    // Clean up
    free(read_data);

    return 0;
}
