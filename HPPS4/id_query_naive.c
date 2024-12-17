#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

// Structure to hold the dataset for naive searching
struct naive_data {
    struct record *rs; // Pointer to array of records
    int n;             // Number of records
};

// Function to create and initialize the naive_data structure
// Input: Array of records (rs) and the number of records (n)
// Output: Pointer to initialized naive_data structure
struct naive_data* mk_naive(const struct record* rs, int n) {
    struct naive_data* data = malloc(sizeof(struct naive_data));
    if (!data) {
        fprintf(stderr, "Error: Failed to allocate memory for naive_data.\n");
        exit(EXIT_FAILURE); // Exit if allocation fails
    }

    // Allocate memory for the records array and copy the input records
    data->rs = malloc(n * sizeof(struct record));
    if (!data->rs) {
        fprintf(stderr, "Error: Failed to allocate memory for records.\n");
        free(data); // Free the structure on failure
        exit(EXIT_FAILURE);
    }
    memcpy(data->rs, rs, n * sizeof(struct record));
    data->n = n; // Store the number of records

    return data;
}

// Function to free the naive_data structure
// Input: Pointer to naive_data structure
void free_naive(struct naive_data* data) {
    if (data) {
        free(data->rs); // Free the records array
        free(data);     // Free the data structure
    }
}

// Perform a linear search to find a record with the specified ID
// Input: Pointer to naive_data structure, ID to search for (needle)
// Output: Pointer to the matching record, or NULL if not found
const struct record* lookup_naive(struct naive_data *data, int64_t needle) {
    for (int i = 0; i < data->n; i++) {
        if (data->rs[i].osm_id == needle) {
            return &data->rs[i]; // Return the matching record
        }
    }
    return NULL; // Return NULL if no record matches the ID
}

// Main function to run the query loop with the naive implementation
int main(int argc, char** argv) {
    return id_query_loop(argc, argv,
                        (mk_index_fn)mk_naive,    // Create index
                        (free_index_fn)free_naive, // Free index
                        (lookup_fn)lookup_naive); // Lookup function
}
