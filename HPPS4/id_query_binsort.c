#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

// Structure to store an index record: holds ID and a pointer to the record
struct index_record {
    int64_t osm_id;            // Record ID
    const struct record *record; // Pointer to the actual record
};

// Structure to hold the sorted index
struct binsort_data {
    struct index_record *irs; // Array of sorted index records
    int n;                    // Number of records
};

// Comparison function for qsort
// Input: Two pointers to index_record structures
// Output: Comparison result for sorting
int compare_index_record(const void *a, const void *b) {
    int64_t id1 = ((struct index_record *)a)->osm_id;
    int64_t id2 = ((struct index_record *)b)->osm_id;
    return (id1 > id2) - (id1 < id2); // Returns -1, 0, or 1
}

// Function to create and sort the index
// Input: Array of records (rs) and number of records (n)
// Output: Pointer to binsort_data structure
struct binsort_data* mk_binsort(const struct record* rs, int n) {
    struct binsort_data* data = malloc(sizeof(struct binsort_data));
    if (!data) {
        fprintf(stderr, "Error: Failed to allocate memory for binsort_data.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for index records
    data->irs = malloc(n * sizeof(struct index_record));
    if (!data->irs) {
        fprintf(stderr, "Error: Failed to allocate memory for index_record array.\n");
        free(data);
        exit(EXIT_FAILURE);
    }

    // Populate the index with IDs and corresponding record pointers
    for (int i = 0; i < n; i++) {
        data->irs[i].osm_id = rs[i].osm_id;
        data->irs[i].record = &rs[i];
    }

    // Sort the index array using qsort
    qsort(data->irs, n, sizeof(struct index_record), compare_index_record);
    data->n = n;

    return data;
}

// Function to free the binsort_data structure
void free_binsort(struct binsort_data* data) {
    if (data) {
        free(data->irs); // Free the sorted index array
        free(data);      // Free the binsort_data structure
    }
}

// Perform a binary search to find a record by ID
// Input: Pointer to binsort_data structure, ID to search for (needle)
// Output: Pointer to the matching record, or NULL if not found
const struct record* lookup_binsort(struct binsort_data *data, int64_t needle) {
    struct index_record key = { .osm_id = needle, .record = NULL };

    // Perform binary search on the sorted index
    struct index_record *result = bsearch(&key, data->irs, data->n,
                                          sizeof(struct index_record), compare_index_record);
    if (result) {
        return result->record; // Return the matching record
    }
    return NULL; // Return NULL if no match is found
}

// Main function to run the query loop with the sorted index
int main(int argc, char** argv) {
    return id_query_loop(argc, argv,
                        (mk_index_fn)mk_binsort,    // Create sorted index
                        (free_index_fn)free_binsort, // Free index
                        (lookup_fn)lookup_binsort); // Lookup function
}
