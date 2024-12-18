#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

//the functions in this program are mostly based on the functions in id_query_naive.c.
//chatgbt was also used to verify syntax and for error handling

// Structure to store an index record: holds ID and a pointer to the record
struct index_record {
    int64_t osm_id;            // Record ID
    const struct record *record; // Pointer to the actual record
};

// Structure to hold the indexed data
struct indexed_data {
    struct index_record *irs; // Array of index records
    int n;                    // Number of records
};

// Function to create an indexed_data structure
// Input: Array of records (rs) and number of records (n)
// Output: Pointer to indexed_data structure
struct indexed_data* mk_indexed(const struct record* rs, int n) {
    struct indexed_data* data = malloc(sizeof(struct indexed_data));
    if (!data) {
        fprintf(stderr, "Error: Failed to allocate memory for indexed_data.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the index array
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

    data->n = n; // Store the number of records
    return data;
}

// Function to free the indexed_data structure
// Input: Pointer to indexed_data structure
void free_indexed(struct indexed_data* data) {
    if (data) {
        free(data->irs); // Free the index array
        free(data);      // Free the indexed_data structure
    }
}

// Perform a linear search on the index to find a record by ID
// Input: Pointer to indexed_data structure, ID to search for (needle)
// Output: Pointer to the matching record, or NULL if not found
const struct record* lookup_indexed(struct indexed_data *data, int64_t needle) {
    for (int i = 0; i < data->n; i++) {
        if (data->irs[i].osm_id == needle) {
            return data->irs[i].record; // Return the matching record
        }
    }
    return NULL; // Return NULL if no record matches the ID
}

// Main function to run the query loop with the indexed implementation
int main(int argc, char** argv) {
    return id_query_loop(argc, argv,
                        (mk_index_fn)mk_indexed,    // Create index
                        (free_index_fn)free_indexed, // Free index
                        (lookup_fn)lookup_indexed); // Lookup function
}
