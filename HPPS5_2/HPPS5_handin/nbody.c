#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "util.h"
#include <omp.h> // Include OpenMP header

static const double WARNING_DISTANCE = 0.01;


//This function was developed using ChatGbt.
// Naive n-body simulation (parallelized with OpenMP).
//
// *tc must be set to the number of warnings.
// *ts must point to an array of warnings with at least *tc elements.
void nbody(int n, struct particle *ps, int steps, int* tc, struct warning** ts) {
  for (int s = 0; s < steps; s++) {
    // Compute forces and update velocities
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
      double fx = 0, fy = 0, fz = 0;
      for (int j = 0; j < n; j++) {
        if (j == i) continue;
        struct vec3 a = force(ps[i].pos, ps[j].pos, ps[j].mass);
        fx += a.x;
        fy += a.y;
        fz += a.z;
      }
      #pragma omp critical
      {
        ps[i].vel.x += fx;
        ps[i].vel.y += fy;
        ps[i].vel.z += fz;
      }
    }

    // Update positions and check warnings
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
      ps[i].pos.x += ps[i].vel.x;
      ps[i].pos.y += ps[i].vel.y;
      ps[i].pos.z += ps[i].vel.z;

      double distance = dist_centre(ps[i].pos);
      if (distance < WARNING_DISTANCE) {
        #pragma omp critical
        {
          (*tc)++; // Increase warning count
          *ts = realloc(*ts, (*tc) * sizeof(struct warning)); // Resize array
          assert(*ts != NULL); // Ensure allocation succeeded

          (*ts)[*tc - 1].s = s;  // Record the current step
          (*ts)[*tc - 1].i = i;  // Record the particle index
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  int steps = 1;
  if (argc < 4) {
    printf("Usage: \n");
    printf("%s <input> <particle output> <warnings output> [steps]\n", argv[0]);
    return 1;
  } else if (argc > 4) {
    steps = atoi(argv[4]);
  }

  int32_t n;
  struct particle *ps = read_particles(argv[1], &n);

  int tc = 0;                 // int to store size of warning array
  struct warning* ts = NULL;  // array to store warnings

  double bef = seconds();
  nbody(n, ps, steps, &tc, &ts);
  double aft = seconds();
  printf("Simulation time: %f seconds\n", aft - bef);
  write_particles(argv[2], n, ps);
  write_warnings(argv[3], tc, ts);

  free(ts);
  free(ps);
}
