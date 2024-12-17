#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <float.h>

#include "coord_query.h"
#include "record.h"

// Structure to hold the dataset for naive querying
struct naive_data {
    const struct record *rs; // Pointer to the array of records
    int n;                   // Number of records in the dataset
};

// Function to create and initialize the naive_data structure
// Input: Pointer to the array of records (rs), number of records (n)
// Output: Pointer to an initialized naive_data structure
struct naive_data* mk_naive(const struct record *rs, int n) {
    // Allocate memory for the naive_data structure
    struct naive_data *data = malloc(sizeof(struct naive_data));
    if (!data) {
        fprintf(stderr, "Error: Failed to allocate memory for naive_data.\n");
        exit(EXIT_FAILURE); // Exit program if memory allocation fails
    }

    data->rs = rs; // Assign the dataset pointer to naive_data
    data->n = n;   // Store the number of records
    return data;
}

// Function to free the naive_data structure
// Input: Pointer to the naive_data structure
void free_naive(struct naive_data *data) {
    free(data); // Free the naive_data structure itself
    // Note: The records are managed and freed elsewhere
}

// Function to calculate the Euclidean distance between two points
// Input: Coordinates (lon1, lat1) and (lon2, lat2)
// Output: Euclidean distance between the two points
static double euclidean_distance(double lon1, double lat1, double lon2, double lat2) {
    // Apply the Euclidean distance formula
    return sqrt((lon1 - lon2) * (lon1 - lon2) + (lat1 - lat2) * (lat1 - lat2));
}

// Function to find the closest record to a given longitude and latitude
// Input: Pointer to naive_data, target longitude (lon), and target latitude (lat)
// Output: Pointer to the closest record, or NULL if no records exist
const struct record* lookup_naive(struct naive_data *data, double lon, double lat) {
    const struct record *closest = NULL; // Pointer to the closest record
    double min_distance = DBL_MAX;       // Initialize minimum distance to a large value

    // Iterate through all records to find the closest one
    for (int i = 0; i < data->n; i++) {
        // Calculate the distance between the target coordinates and the current record
        double distance = euclidean_distance(lon, lat, data->rs[i].lon, data->rs[i].lat);

        // Update the closest record if this one is closer
        if (distance < min_distance) {
            min_distance = distance;   // Update the minimum distance
            closest = &data->rs[i];    // Update the pointer to the closest record
        }
    }

    return closest; // Return the closest record, or NULL if no records exist
}

// Main function to run the coordinate query loop
// Input: Command-line arguments
// Output: Exit status
int main(int argc, char **argv) {
    // Call the generic coordinate query loop with the naive implementation functions
    return coord_query_loop(argc, argv,
                            (mk_index_fn)mk_naive,    // Function to create the index
                            (free_index_fn)free_naive, // Function to free the index
                            (lookup_fn)lookup_naive);  // Function to perform a lookup
}
