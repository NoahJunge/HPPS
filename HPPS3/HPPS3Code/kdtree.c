#include "kdtree.h"
#include "sort.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct node {
    int point_index;    // Index of this node's point in the 'indexes' array
    int axis;           // Axis along which this node is split
    struct node *left;  // Left child
    struct node *right; // Right child
};

struct kdtree {
    int d;              // Number of dimensions
    const double *points; // Reference points
    struct node* root;  // Root of the k-d tree
};

// Helper function to compare two indices based on the points along a given axis
struct sort_context {
    const double *points;
    int d;
    int axis;
};

int sort_on_axis(const void *a, const void *b, void *arg) {
    struct sort_context *ctx = (struct sort_context *)arg;
    const double *points = ctx->points;
    int d = ctx->d;
    int axis = ctx->axis;

    int index_a = *(const int *)a;
    int index_b = *(const int *)b;

    double coord_a = points[index_a * d + axis];
    double coord_b = points[index_b * d + axis];

    if (coord_a < coord_b) return -1;
    if (coord_a > coord_b) return 1;
    return 0;
}

// Sort indices based on the given axis
void sort_on_index(const double *points, int *indexes, int n, int d, int axis) {
    struct sort_context ctx = { points, d, axis };
    hpps_quicksort(indexes, n, sizeof(int), sort_on_axis, &ctx);
}

// Recursive function to create a k-d tree node
struct node* kdtree_create_node(int d, const double *points, int depth, int n, int *indexes) {
    if (n <= 0) return NULL;

    int axis = depth % d;

    // Sort indices based on the current axis
    sort_on_index(points, indexes, n, d, axis);

    // Find the median index
    int median_idx = n / 2;

    struct node *node = malloc(sizeof(struct node));
    node->point_index = indexes[median_idx];
    node->axis = axis;

    // Recursively build left and right subtrees
    node->left = kdtree_create_node(d, points, depth + 1, median_idx, indexes);
    node->right = kdtree_create_node(d, points, depth + 1, n - median_idx - 1, &indexes[median_idx + 1]);

    return node;
}

// Create a k-d tree
struct kdtree *kdtree_create(int d, int n, const double *points) {
    struct kdtree *tree = malloc(sizeof(struct kdtree));
    tree->d = d;
    tree->points = points;

    int *indexes = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        indexes[i] = i;
    }

    tree->root = kdtree_create_node(d, points, 0, n, indexes);
    free(indexes);

    return tree;
}

// Free memory of a k-d tree node recursively
void kdtree_free_node(struct node *node) {
    if (node == NULL) return;

    kdtree_free_node(node->left);
    kdtree_free_node(node->right);
    free(node);
}

// Free the entire k-d tree
void kdtree_free(struct kdtree *tree) {
    if (tree == NULL) return;

    kdtree_free_node(tree->root);
    free(tree);
}

// Recursive function to find k nearest neighbors in the k-d tree
void kdtree_knn_node(const struct kdtree *tree, int k, const double* query,
                     int *closest, double *radius, const struct node *node) {
    if (node == NULL) return;

    const double *point = &tree->points[node->point_index * tree->d];

    // Try to insert the current point into the closest list
    if (insert_if_closer(k, tree->d, tree->points, closest, query, node->point_index)) {
        // Update radius to the distance of the furthest point in the closest list
        *radius = distance(tree->d, query, &tree->points[closest[k - 1] * tree->d]);
    }

    // Determine which child to visit first
    double diff = query[node->axis] - point[node->axis];
    struct node *first = diff < 0 ? node->left : node->right;
    struct node *second = diff < 0 ? node->right : node->left;

    // Visit the first child
    kdtree_knn_node(tree, k, query, closest, radius, first);

    // Visit the second child only if it could contain closer points
    if (fabs(diff) < *radius) {
        kdtree_knn_node(tree, k, query, closest, radius, second);
    }
}

// Wrapper function for k-NN search using the k-d tree
int* kdtree_knn(const struct kdtree *tree, int k, const double* query) {
    int* closest = malloc(k * sizeof(int));
    double radius = INFINITY;

    for (int i = 0; i < k; i++) {
        closest[i] = -1; // Initialize closest list with invalid indices
    }

    kdtree_knn_node(tree, k, query, closest, &radius, tree->root);

    return closest;
}

// Generate SVG visualization of the k-d tree
void kdtree_svg_node(double scale, FILE *f, const struct kdtree *tree,
                     double x1, double y1, double x2, double y2,
                     const struct node *node) {
    if (node == NULL) return;

    double coord = tree->points[node->point_index * 2 + node->axis];
    if (node->axis == 0) {
        // Vertical line splitting X axis
        fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke-width=\"1\" stroke=\"black\" />\n",
                coord * scale, y1 * scale, coord * scale, y2 * scale);
        kdtree_svg_node(scale, f, tree, x1, y1, coord, y2, node->left);
        kdtree_svg_node(scale, f, tree, coord, y1, x2, y2, node->right);
    } else {
        // Horizontal line splitting Y axis
        fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke-width=\"1\" stroke=\"black\" />\n",
                x1 * scale, coord * scale, x2 * scale, coord * scale);
        kdtree_svg_node(scale, f, tree, x1, y1, x2, coord, node->left);
        kdtree_svg_node(scale, f, tree, x1, coord, x2, y2, node->right);
    }
}

void kdtree_svg(double scale, FILE* f, const struct kdtree *tree) {
    assert(tree->d == 2); // Visualization is only for 2D spaces
    kdtree_svg_node(scale, f, tree, 0, 0, 1, 1, tree->root);
}
