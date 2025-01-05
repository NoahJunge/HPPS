#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "util.h"

// Figure out which child octant a particle belongs to. Returns a
// number from 0 to 7, inclusive.
int octant(struct vec3 corner, double l, const struct particle* p) {
  if (p->pos.x >= corner.x + l / 2) {
    if (p->pos.y >= corner.y + l / 2) {
      return (p->pos.z >= corner.z + l / 2) ? 0 : 1;
    } else {
      return (p->pos.z >= corner.z + l / 2) ? 2 : 3;
    }
  } else {
    if (p->pos.y >= corner.y + l / 2) {
      return (p->pos.z >= corner.z + l / 2) ? 4 : 5;
    } else {
      return (p->pos.z >= corner.z + l / 2) ? 6 : 7;
    }
  }
}

// Given the index of a child octant (0-7), place in *ox/*oy/*oz the
// normalized corner coordinate.
void octant_offset(int j, double* ox, double* oy, double* oz) {
  switch (j) {
    case 0: *ox = 0.5; *oy = 0.5; *oz = 0.5; break;
    case 1: *ox = 0.5; *oy = 0.5; *oz = 0.0; break;
    case 2: *ox = 0.5; *oy = 0.0; *oz = 0.5; break;
    case 3: *ox = 0.5; *oy = 0.0; *oz = 0.0; break;
    case 4: *ox = 0.0; *oy = 0.5; *oz = 0.5; break;
    case 5: *ox = 0.0; *oy = 0.5; *oz = 0.0; break;
    case 6: *ox = 0.0; *oy = 0.0; *oz = 0.5; break;
    case 7: *ox = 0.0; *oy = 0.0; *oz = 0.0; break;
  }
}

// Node definition for Barnes-Hut tree
struct bh_node {
  bool internal; // False when external.
  struct vec3 corner;
  double l; // Edge length of space.
  int particle; // Index of particle in particle array; -1 if none.
  struct vec3 com; // Center of mass (only for internal nodes).
  double mass; // Total mass (only for internal nodes).
  struct bh_node* children[8]; // Children of internal nodes
};

// Convert an external node to an internal one
void bh_mk_internal(struct bh_node* bh) {
    assert(!bh->internal);  // Ensures the node is external
    bh->internal = true;
    bh->mass = 0;
    bh->com = (struct vec3){0, 0, 0};

    // Allocate memory for the 8 children and set their properties
    for (int i = 0; i < 8; i++) {
        bh->children[i] = malloc(sizeof(struct bh_node));
        if (!bh->children[i]) {
            fprintf(stderr, "Memory allocation failed for child %d\n", i);
            exit(1);
        }
        bh->children[i]->internal = false;  // External nodes initially
        bh->children[i]->particle = -1;
        bh->children[i]->l = bh->l / 2; // Half the size of the parent
        double ox, oy, oz;
        octant_offset(i, &ox, &oy, &oz); // Calculate child corner offset
        // Calculate new corner based on parent's corner and size
        bh->children[i]->corner.x = bh->corner.x + ox * bh->l;
        bh->children[i]->corner.y = bh->corner.y + oy * bh->l;
        bh->children[i]->corner.z = bh->corner.z + oz * bh->l;
    }
}

// Insert a particle into the octree
void bh_insert(struct bh_node* bh, struct particle* ps, int p) {
    if (bh->internal) {
        // Internal node: Insert the particle into the appropriate child
        int child_index = octant(bh->corner, bh->l, &ps[p]);
        bh_insert(bh->children[child_index], ps, p);  // Recurse into the child node

        // Update the center of mass and total mass for this internal node
        double new_mass = bh->mass + ps[p].mass;
        bh->com.x = (bh->mass * bh->com.x + ps[p].mass * ps[p].pos.x) / new_mass;
        bh->com.y = (bh->mass * bh->com.y + ps[p].mass * ps[p].pos.y) / new_mass;
        bh->com.z = (bh->mass * bh->com.z + ps[p].mass * ps[p].pos.z) / new_mass;
        bh->mass = new_mass;
    } else {
        if (bh->particle == -1) {
            // External node: No particle, just insert
            bh->particle = p;
        } else {
            // External node already contains a particle: Convert to internal node
            int existing_particle = bh->particle;
            bh->particle = -1;
            bh_mk_internal(bh);  // Convert to internal node

            // Insert both particles into the internal node
            bh_insert(bh, ps, existing_particle);  // Reinsert existing particle
            bh_insert(bh, ps, p);  // Insert the new particle
        }
    }
}

// Free all memory used for the tree (recursive)
void bh_free(struct bh_node* bh) {
    if (bh->internal) {
        for (int i = 0; i < 8; i++) {
            bh_free(bh->children[i]); // Recursively free children
            free(bh->children[i]);    // Free the child node itself
        }
    }
}

// Compute the acceleration on a particle due to the tree structure
void bh_accel(double theta, struct bh_node* bh, struct particle* ps, int p, struct vec3 *a) {
    if (bh->internal) {
        double d = dist(bh->com, ps[p].pos);
        if (bh->l / d < theta) {
            // Use the center of mass for distant particles (Barnes-Hut approximation)
            struct vec3 f = force(ps[p].pos, bh->com, bh->mass);
            a->x += f.x;
            a->y += f.y;
            a->z += f.z;
        } else {
            // Recursively calculate the force from the children
            for (int i = 0; i < 8; i++) {
                if (bh->children[i]) {
                    bh_accel(theta, bh->children[i], ps, p, a);
                }
            }
        }
    } else if (bh->particle != -1 && bh->particle != p) {
        // Calculate force from another particle in the external node
        struct vec3 f = force(ps[p].pos, ps[bh->particle].pos, ps[bh->particle].mass);
        a->x += f.x;
        a->y += f.y;
        a->z += f.z;
    }
}

// Create a new octree node spanning the given space
struct bh_node* bh_new(double min_coord, double max_coord) {
    struct bh_node* bh = malloc(sizeof(struct bh_node));
    if (!bh) {
        fprintf(stderr, "Memory allocation failed for bh_node\n");
        exit(1);
    }
    bh->corner.x = min_coord;
    bh->corner.y = min_coord;
    bh->corner.z = min_coord;
    bh->l = max_coord - min_coord;  // Length of the node space
    bh->internal = false;
    bh->particle = -1;

    for (int i = 0; i < 8; i++) {
        bh->children[i] = NULL;
    }

    return bh;
}

static const double WARNING_DISTANCE = 0.01;

// Barnes-Hut N-body simulation function
void nbody(int n, struct particle *ps, int steps, int* tc, struct warning** ts, double theta) {
    for (int s = 0; s < steps; s++) {
        // Find min and max coordinates to determine the bounds of the octree
        double min_coord = INFINITY, max_coord = -INFINITY;
        for (int i = 0; i < n; i++) {
            if (ps[i].pos.x < min_coord) min_coord = ps[i].pos.x;
            if (ps[i].pos.y < min_coord) min_coord = ps[i].pos.y;
            if (ps[i].pos.z < min_coord) min_coord = ps[i].pos.z;
            if (ps[i].pos.x > max_coord) max_coord = ps[i].pos.x;
            if (ps[i].pos.y > max_coord) max_coord = ps[i].pos.y;
            if (ps[i].pos.z > max_coord) max_coord = ps[i].pos.z;
        }

        // Create the octree and insert particles
        struct bh_node* tree = bh_new(min_coord, max_coord);
        for (int i = 0; i < n; i++) {
            bh_insert(tree, ps, i);
        }

        // Compute accelerations and update velocities
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            struct vec3 a = {0, 0, 0};
            bh_accel(theta, tree, ps, i, &a);
            ps[i].vel.x += a.x;
            ps[i].vel.y += a.y;
            ps[i].vel.z += a.z;
        }

        // Update positions and check warnings
        for (int i = 0; i < n; i++) {
            ps[i].pos.x += ps[i].vel.x;
            ps[i].pos.y += ps[i].vel.y;
            ps[i].pos.z += ps[i].vel.z;

            double distance = dist_centre(ps[i].pos);
            if (distance < WARNING_DISTANCE) {
                #pragma omp critical
                {
                    (*tc)++;
                    *ts = realloc(*ts, (*tc) * sizeof(struct warning));
                    assert(*ts != NULL);
                    (*ts)[*tc - 1].s = s;
                    (*ts)[*tc - 1].i = i;
                }
            }
        }

        // Free the tree
        bh_free(tree);
        free(tree);
    }
}

int main(int argc, char** argv) {
    int steps = 1;
    double theta = 0.5;
    if (argc < 4) {
        printf("Usage: \n%s <input> <particle output> <warnings output> [steps]\n", argv[0]);
        return 1;
    }
    if (argc > 4) {
        steps = atoi(argv[4]);
    }
    if (argc > 5) {
        theta = atof(argv[5]);
    }

    int32_t n;
    struct particle *ps = read_particles(argv[1], &n);

    int tc = 0;
    struct warning* ts = NULL;

    double bef = seconds();
    nbody(n, ps, steps, &tc, &ts, theta);
    double aft = seconds();
    printf("%f\n", aft - bef);
    write_particles(argv[2], n, ps);
    write_warnings(argv[3], tc, ts);

    free(ts);
    free(ps);
}

