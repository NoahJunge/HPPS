#include "numlib.h"
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

void skipspaces(FILE *f) {
  while (1) {
    int c = fgetc(f);
    if (!isspace(c)) {
      if (c != EOF) {
        ungetc(c, f);
      }
      return;
    }
  }
}

int read_uint_ascii(FILE *f, uint32_t *out) {
  int read = 0;
  uint32_t num = 0;

  while (1) {
    int c = fgetc(f);

    if (c >= '0' && c <= '9') {
      num = num * 10 + (c - '0');
    } else {
      if (c != EOF) {
        ungetc(c, f);
      }
      if (read == 0) {
        if (c == EOF) {
          return EOF;
        } else {
          return 1;
        }
      } else {
        *out = num;
        return 0;
      }
    }
    read++;
  }
}

//chatgbt was used to check for edge cases and for verifying correct syntax
int read_double_ascii(FILE *f, double *out) {//*f is a pointer to the file we wish to read from, *out is a pointer to the double variable, where the function will store the read double
    skipspaces(f); // Skips whitespace

    // Attempt to read a double using fscanf
    int result = fscanf(f, "%lf", out);// reads input from the file f, %lf:only reads doubles, out is a ppinter to where the location where the read double will be stored

    if (result == EOF) {
        return EOF; // End of file
    } else if (result == 0) {
        return 1; // Invalid input
    }
    return 0; // Success
}


int read_uint_le(FILE *f, uint32_t *out) {
  int b0, b1, b2, b3;
  b0 = fgetc(f);

  if (b0 == EOF) {
    return EOF;
  }

  b1 = fgetc(f);
  b2 = fgetc(f);
  b3 = fgetc(f);

  if (b1 == EOF || b2 == EOF || b3 == EOF) {
    return 1;
  }

  *out =
    ((uint32_t)b0)
    | ((uint32_t)b1 << 8)
    | ((uint32_t)b2 << 16)
    | ((uint32_t)b3 << 24);
  return 0;
}

//code was based on read_uint_le() and syntax was checked using chatgbt
int read_uint_be(FILE *f, uint32_t *out) {
    int b3, b2, b1, b0;

    // Read 4 bytes in big endian order
    b3 = fgetc(f);
    if (b3 == EOF) return EOF;
    b2 = fgetc(f);
    b1 = fgetc(f);
    b0 = fgetc(f);

    // Check if any byte was not read completely
    if (b2 == EOF || b1 == EOF || b0 == EOF) {
        return 1; // Incomplete data
    }

    // Combine bytes in big-endian order
    *out = ((uint32_t)b3 << 24) |
           ((uint32_t)b2 << 16) | 
           ((uint32_t)b1 << 8) | 
           (uint32_t)b0;

    return 0; // Success
}


int read_double_bin(FILE *f, double *out) {// f is a pointer to the file to read from, out is a pointer to the address where we will store the read value
    // Attempt to read 8 bytes into the output double
    if (fread(out, sizeof(double), 1, f) != 1) { // out:stored data, sizeof(double): size of each element in bytes(8), 1: count of elements to be read, f: pointer to files stream, fread returns the number of elements succesfully read. If it doesnt read 1 it 
        return (feof(f) ? EOF : 1); // feof checks if end of file has been reached in which case it returns EOF, if not 1 is returned indicating some error.
    }
    return 0; // return 0 for success
}

int write_uint_ascii(FILE *f, uint32_t x) {
  if (fprintf(f, "%u", x) < 0) {
    return 1;
  } else {
    return 0;
  }
}

int write_double_ascii(FILE *f, double x) {  // *f a pointer to the file f, where the double x will be written in the humanly readable ASCII format
    // Use fprintf to write the double to the file in ASCII format
    if (fprintf(f, "%g", x) < 0) { //%g best of both worlds;) fprintf already writes in ASCII fortunately
        return 1; // Write error
    }
    return 0; // Success
}


int write_uint_le(FILE *f, uint32_t x) {
  fputc(x>>0,  f);
  fputc(x>>8,  f);
  fputc(x>>16, f);
  fputc(x>>24, f);
  return 0;
}

//code was based on write_uint_le and developed using chatgbt
int write_uint_be(FILE *f, uint32_t x) {
    // Write bytes in big-endian order
    if (fputc((x >> 24) & 0xFF, f) == EOF ||  // MSB
        fputc((x >> 16) & 0xFF, f) == EOF ||
        fputc((x >> 8) & 0xFF, f) == EOF ||
        fputc(x & 0xFF, f) == EOF) {         // LSB
        return 1; // Error writing
    }

    return 0; // Success
}


int write_double_bin(FILE *f, double x) { //*f is a pointer to where the, 'x' the double will be written
    // Attempt to write 8 bytes from the double
    if (fwrite(&x, sizeof(double), 1, f) != 1) { // &x: the address of x(where it should write), sizeof(double): the size of each element in bytes(8), 1:the number of elements to write, f: the file where the double is written
        return 1; // if no double is written it returns 1
    }
    return 0; // if 1 double is written it returns 0 to indicate success.
}

