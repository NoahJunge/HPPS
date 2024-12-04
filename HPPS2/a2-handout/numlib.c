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

int read_double_ascii(FILE *f, double *out) {
    // Skip whitespace
    skipspaces(f);

    // Attempt to read a double using fscanf
    int result = fscanf(f, "%lf", out);

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


int read_double_bin(FILE *f, double *out) {
    // Attempt to read 8 bytes into the output double
    if (fread(out, sizeof(double), 1, f) != 1) {
        return (feof(f) ? EOF : 1); // EOF or incomplete read
    }
    return 0; // Success
}

int write_uint_ascii(FILE *f, uint32_t x) {
  if (fprintf(f, "%u", x) < 0) {
    return 1;
  } else {
    return 0;
  }
}

int write_double_ascii(FILE *f, double x) {
    // Use fprintf to write the double to the file in ASCII format
    if (fprintf(f, "%g", x) < 0) {
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


int write_double_bin(FILE *f, double x) {
    // Attempt to write 8 bytes from the double
    if (fwrite(&x, sizeof(double), 1, f) != 1) {
        return 1; // Write error
    }
    return 0; // Success
}

