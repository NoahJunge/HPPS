#include "kdtree.h"
#include "sort.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

// Structure to represent a k-d tree node
struct node {
    int point_index;  // Index of this node's point in the corresponding 'indexes' array
    int axis;         // Axis along which this node has been split
    struct node *left;  // Left child
    struct node *right; // Right child
};

// Structure to represent a k-d tree
struct kdtree {
    int d;              // Number of dimensions
    const double *points; // Reference points
    struct node *root;   // Root of the tree
};

// Context for sorting points along a specific axis
struct sort_context {
    const double *points;  // Reference points
    int d;                 // Number of dimensions
    int axis;              // Sorting axis
};

// Global sort context for qsort compatibility
static struct sort_context global_sort_context;

// Comparator function for qsort
int sort_on_axis(const void *a, const void *b) {
    const double *points = global_sort_context.points;
    int d = global_sort_context.d;
    int axis = global_sort_context.axis;

    int index_a = *(const int *)a;
    int index_b = *(const int *)b;

    double coord_a = points[index_a * d + axis];
    double coord_b = points[index_b * d + axis];

    if (coord_a < coord_b) return -1;
    if (coord_a > coord_b) return 1;
    return 0;
}

// Sort points along a specific axis using qsort
void sort_on_index(const double *points, int *indexes, int n, int d, int axis) {
    global_sort_context.points = points;
    global_sort_context.d = d;
    global_sort_context.axis = axis;

    qsort(indexes, n, sizeof(int), sort_on_axis);
}

// Create a node of the k-d tree
struct node* kdtree_create_node(int d, const double *points, int depth, int n, int *indexes) {
    if (n <= 0) return NULL;

    int axis = depth % d;  // Determine which axis to split
    sort_on_index(points, indexes, n, d, axis);  // Sort points along axis

    int median_idx = n / 2;  // Median split
    struct node *node = malloc(sizeof(struct node));
    node->point_index = indexes[median_idx];
    node->axis = axis;

    // Recursively create left and right children
    node->left = kdtree_create_node(d, points, depth + 1, median_idx, indexes);
    node->right = kdtree_create_node(d, points, depth + 1, n - median_idx - 1, &indexes[median_idx + 1]);

    return node;
}

// Create a k-d tree
//d:dimensions, n:datapoints, points: pointer to points array
struct kdtree *kdtree_create(int d, int n, const double *points) {
    struct kdtree *tree = malloc(sizeof(struct kdtree));// allocate memory for the tree structure
    tree->d = d; // access d in the tree structure
    tree->points = points; // access points in the tree structue

    int *indexes = malloc(sizeof(int) * n); // allocate memory for an array of size n
    for (int i = 0; i < n; i++) { // initialise the indexes array, so that it has the values 0,1,2,3,4,...,n-1.  (less than n)
        indexes[i] = i;// -||-
    }

    // access the root from the tree structure, and build the tree recursively using the function below
    tree->root = kdtree_create_node(d, points, 0, n, indexes);
    free(indexes); // free memory after creating the tree.

    return tree; //return the finished tree structure
}

// Free a k-d tree node
void kdtree_free_node(struct node *node) {
    if (node == NULL) return;

    kdtree_free_node(node->left);
    kdtree_free_node(node->right);
    free(node);
}

// Free a k-d tree
void kdtree_free(struct kdtree *tree) {
    if (tree == NULL) return;

    kdtree_free_node(tree->root);
    free(tree);
}

// Find k nearest neighbors in the k-d tree recursively
void kdtree_knn_node(const struct kdtree *tree, int k, const double* query,
                     int *closest, double *radius, const struct node *node) {
    if (node == NULL) return;

    const double *point = &tree->points[node->point_index * tree->d];
    double dist = distance(tree->d, query, point);

    if (insert_if_closer(k, tree->d, tree->points, closest, query, node->point_index)) {
        *radius = distance(tree->d, query, &tree->points[closest[k - 1] * tree->d]);
    }

    double diff = query[node->axis] - point[node->axis];
    struct node *first = diff < 0 ? node->left : node->right;
    struct node *second = diff < 0 ? node->right : node->left;

    kdtree_knn_node(tree, k, query, closest, radius, first);

    if (fabs(diff) < *radius) {
        kdtree_knn_node(tree, k, query, closest, radius, second);
    }
}

// Find k nearest neighbors in the k-d tree
//tree: the kdtree, k: the amount of neighbours we are searching for, q: the point for which we are looking for neighbours.
int* kdtree_knn(const struct kdtree *tree, int k, const double* query) {
    int *closest = malloc(k * sizeof(int));// an array that will store the k nearest neighbours
    double radius = INFINITY;// the distance to the furthest point in the closest array, initialize to infinity because no points have been found yet

    for (int i = 0; i < k; i++) {
        closest[i] = -1; // Initialize closest array with invalid indices
    }
    // the function that performs the actual search through the kdtree to find the nearest neighbours
    kdtree_knn_node(tree, k, query, closest, &radius, tree->root);

    //returns an array of the closest points(their indices)
    return closest;
}

// Generate an SVG representation of the k-d tree
void kdtree_svg_node(double scale, FILE *f, const struct kdtree *tree,
                     double x1, double y1, double x2, double y2,
                     const struct node *node) {
    if (node == NULL) return;

    double coord = tree->points[node->point_index * tree->d + node->axis];
    if (node->axis == 0) {
        fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"black\" stroke-width=\"1\" />\n",
                coord * scale, y1 * scale, coord * scale, y2 * scale);
        kdtree_svg_node(scale, f, tree, x1, y1, coord, y2, node->left);
        kdtree_svg_node(scale, f, tree, coord, y1, x2, y2, node->right);
    } else {
        fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"black\" stroke-width=\"1\" />\n",
                x1 * scale, coord * scale, x2 * scale, coord * scale);
        kdtree_svg_node(scale, f, tree, x1, y1, x2, coord, node->left);
        kdtree_svg_node(scale, f, tree, x1, coord, x2, y2, node->right);
    }
}

void kdtree_svg(double scale, FILE *f, const struct kdtree *tree) {
    if (tree == NULL || tree->d != 2) return;

    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1000\" height=\"1000\" viewBox=\"0 0 1000 1000\">\n");
    kdtree_svg_node(scale, f, tree, 0, 0, 1, 1, tree->root);
    fprintf(f, "</svg>\n");
}
