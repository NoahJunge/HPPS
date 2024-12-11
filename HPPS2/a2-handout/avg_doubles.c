#include "numlib.h"
#include <stdio.h>


//the code was developed using chatgbt, to check for correct syntax and to handle errors
int main(void) {
    double value, sum = 0.0;//initializing values
    int count = 0;// -||-

    // Read doubles from standard input until EOF
    while (read_double_bin(stdin, &value) == 0) { //while it succesfully reads double(returns 0), read input from stdin, and stores it at the address of 'value'
        sum += value; //adds the value to the sum
        count++; //increments the count by 1
    }

    // Handle the case where no numbers are read
    if (count == 0) {// ie. no numbers have been read
        fprintf(stderr, "Error: No numbers provided in input.\n");
        return 1;
    }

    // Compute the mean
    double mean = sum / count;//mean is equal to the sum divided by the count...

    // Output the mean in ASCII format
    if (write_double_ascii(stdout, mean) != 0) {//writes the mean in ASCII format to stdout. 
        fprintf(stderr, "Error: Failed to write the output.\n"); //if it doesnt work, ie. it doesnt return 0(for succes), it prints an error message
        return 1;//return 1 for failure
    }

    // Print a newline for clarity
    printf("\n");

    return 0;//return 0 for succes
}

