#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "util.h"

// Figure out which child octant a particle belongs to. Returns a
// number from 0 to 7, inclusive.
//
// 'l' is the edge length of space.
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

// You do not need to modify this definition.
struct bh_node {
  bool internal; // False when external.

  struct vec3 corner;
  double l; // Edge length of space.

  // Fields for external nodes.
  int particle; // Index of particle in particle array; -1 if none.

  // Fields for internal nodes. Only have sensible values when
  // 'internal' is true.
  struct vec3 com; // Center of mass.
  double mass; // Total mass.
  struct bh_node* children[8];
};

// Turn an external node into an internal node containing no
// particles, and with 8 external node children.
void bh_mk_internal(struct bh_node* bh) {
    assert(!bh->internal);
    bh->internal = true;
    bh->mass = 0;
    bh->com = (struct vec3){0, 0, 0};

    for (int i = 0; i < 8; i++) {
        bh->children[i] = malloc(sizeof(struct bh_node));
        if (!bh->children[i]) {
            fprintf(stderr, "Memory allocation failed for child %d\n", i);
            exit(1);
        }
        bh->children[i]->internal = false;
        bh->children[i]->particle = -1;
        bh->children[i]->l = bh->l / 2;
        double ox, oy, oz;
        octant_offset(i, &ox, &oy, &oz);
        bh->children[i]->corner.x = bh->corner.x + ox * bh->l ;
        bh->children[i]->corner.y = bh->corner.y + oy * bh->l ;
        bh->children[i]->corner.z = bh->corner.z + oz * bh->l ;
    }
}


// Insert particle 'p' (which must be a valid index in 'ps') into our octree.
void bh_insert(struct bh_node* bh, struct particle* ps, int p) {
    if (bh->internal) {
        // Internal node: insert particle into the correct child.
        int child_index = octant(bh->corner, bh->l, &ps[p]);  // ps[p] correctly references the particle p in the list

        // Ensure the child node exists before inserting.
        if (bh->children[child_index] == NULL) {
            fprintf(stderr, "Child %d is not allocated in bh_insert\n", child_index);
            exit(1);
        }

        bh_insert(bh->children[child_index], ps, p);  // Correctly using the ps array and particle index

        // Update the center of mass and total mass
        double total_mass = bh->mass + ps[p].mass;
        bh->com.x = (bh->com.x * bh->mass + ps[p].pos.x * ps[p].mass) / total_mass;
        bh->com.y = (bh->com.y * bh->mass + ps[p].pos.y * ps[p].mass) / total_mass;
        bh->com.z = (bh->com.z * bh->mass + ps[p].pos.z * ps[p].mass) / total_mass;
        bh->mass = total_mass;
    } else {
        if (bh->particle == -1) {
            // External node: store the particle.
            bh->particle = p;
        } else {
            // External node already contains a particle.
            int existing_particle = bh->particle;
            bh->particle = -1;
            bh_mk_internal(bh);  // Convert to internal node

            // Ensure all child nodes are properly allocated
            for (int i = 0; i < 8; i++) {
                if (bh->children[i] == NULL) {
                    fprintf(stderr, "Child %d allocation failed during conversion to internal node\n", i);
                    exit(1);
                }
            }

            // Reinsert both particles
            bh_insert(bh, ps, existing_particle);  // Using ps correctly as an array of particles
            bh_insert(bh, ps, p);
        }
    }
}


// Free all memory used for the tree.
void bh_free(struct bh_node* bh) {
    if (bh->internal) {
        for (int i = 0; i < 8; i++) {
            bh_free(bh->children[i]);
            free(bh->children[i]);
        }
    }
}

// Compute the accel acting on particle 'p'. Increments *a.
void bh_accel(double theta, struct bh_node* bh,
              struct particle* ps, int p,
              struct vec3 *a) {
    if (bh->internal) {
        double d = dist(bh->com, ps[p].pos);
        if (bh->l / d < theta) {
            struct vec3 f = force(ps[p].pos, bh->com, bh->mass);
            a->x += f.x;
            a->y += f.y;
            a->z += f.z;
        } else {
            for (int i = 0; i < 8; i++) {
                if (bh->children[i]) {
                    bh_accel(theta, bh->children[i], ps, p, a);
                }
            }
        }
    } else if (bh->particle != -1 && bh->particle != p) {
        struct vec3 f = force(ps[p].pos, ps[bh->particle].pos, ps[bh->particle].mass);
        a->x += f.x;
        a->y += f.y;
        a->z += f.z;
    }
}

// Create a new octree that spans a space with the provided minimum and maximum coordinates.
struct bh_node* bh_new(double min_coord, double max_coord) {
    struct bh_node* bh = malloc(sizeof(struct bh_node));
    if (!bh) {
        fprintf(stderr, "Memory allocation failed for bh_node\n");
        exit(1);
    }
    bh->corner.x = min_coord;
    bh->corner.y = min_coord;
    bh->corner.z = min_coord;
    bh->l = max_coord - min_coord;
    bh->internal = false;
    bh->particle = -1;

    for (int i = 0; i < 8; i++) {
        bh->children[i] = NULL;
    }

    return bh;
}

static const double WARNING_DISTANCE = 0.01;

// Barnes-Hut N-body simulation.
void nbody(int n, struct particle *ps, int steps, int* tc, struct warning** ts, double theta) {
    for (int s = 0; s < steps; s++) {
        // Determine min and max coordinates.
        double min_coord = INFINITY, max_coord = -INFINITY;
        for (int i = 0; i < n; i++) {
            if (ps[i].pos.x < min_coord) min_coord = ps[i].pos.x;
            if (ps[i].pos.y < min_coord) min_coord = ps[i].pos.y;
            if (ps[i].pos.z < min_coord) min_coord = ps[i].pos.z;
            if (ps[i].pos.x > max_coord) max_coord = ps[i].pos.x;
            if (ps[i].pos.y > max_coord) max_coord = ps[i].pos.y;
            if (ps[i].pos.z > max_coord) max_coord = ps[i].pos.z;
        }

        struct bh_node* tree = bh_new(min_coord, max_coord);
        for (int i = 0; i < n; i++) {
            bh_insert(tree, ps, i);
        }

        // Compute accelerations and update velocities.
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            struct vec3 a = {0, 0, 0};
            bh_accel(theta, tree, ps, i, &a);
            ps[i].vel.x += a.x;
            ps[i].vel.y += a.y;
            ps[i].vel.z += a.z;
        }

        // Update positions and check warnings.
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
