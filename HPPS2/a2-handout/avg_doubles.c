#include "numlib.h"
#include <stdio.h>

int main(void) {
    double value, sum = 0.0;
    int count = 0;

    // Read doubles from standard input until EOF
    while (read_double_bin(stdin, &value) == 0) {
        sum += value;
        count++;
    }

    // Handle the case where no numbers are read
    if (count == 0) {
        fprintf(stderr, "Error: No numbers provided in input.\n");
        return 1;
    }

    // Compute the mean
    double mean = sum / count;

    // Output the mean in ASCII format
    if (write_double_ascii(stdout, mean) != 0) {
        fprintf(stderr, "Error: Failed to write the output.\n");
        return 1;
    }

    // Print a newline for clarity
    printf("\n");

    return 0;
}

