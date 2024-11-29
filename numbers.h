#include <stdint.h>
#include "bits.h"

struct bits8 { //data structure of 8 bits
  struct bit b0; // Least significant bit
  struct bit b1;
  struct bit b2;
  struct bit b3;
  struct bit b4;
  struct bit b5;
  struct bit b6;
  struct bit b7; // Most significant bit
};

struct bits8 bits8_from_int(unsigned int x) { // return type must be bits8, x is expected to be unsigned(positive)
    struct bits8 result;
    result.b0 = bit_from_int(x & 1); // Extract least significant bit
    result.b1 = bit_from_int((x >> 1) & 1); // Extract next bit
    result.b2 = bit_from_int((x >> 2) & 1);
    result.b3 = bit_from_int((x >> 3) & 1);
    result.b4 = bit_from_int((x >> 4) & 1);
    result.b5 = bit_from_int((x >> 5) & 1);
    result.b6 = bit_from_int((x >> 6) & 1);
    result.b7 = bit_from_int((x >> 7) & 1); // Extract most significant bit
    return result;
}

unsigned int bits8_to_int(struct bits8 x) {
    return bit_to_int(x.b0) |
           (bit_to_int(x.b1) << 1) |
           (bit_to_int(x.b2) << 2) |
           (bit_to_int(x.b3) << 3) |
           (bit_to_int(x.b4) << 4) |
           (bit_to_int(x.b5) << 5) |
           (bit_to_int(x.b6) << 6) |
           (bit_to_int(x.b7) << 7);
}

void bits8_print(struct bits8 v) {
    printf("%d", bit_to_int(v.b7));
    printf("%d", bit_to_int(v.b6));
    printf("%d", bit_to_int(v.b5));
    printf("%d", bit_to_int(v.b4));
    printf("%d", bit_to_int(v.b3));
    printf("%d", bit_to_int(v.b2));
    printf("%d", bit_to_int(v.b1));
    printf("%d", bit_to_int(v.b0));
}


struct add_result {
    struct bit s; 
    struct bit c; 
};

struct add_result bit_add(struct bit x, struct bit y, struct bit c_in){
    struct add_result result;
    result.s = bit_xor(bit_xor(x, y), c_in); // s = x ⊕ y ⊕ c_in
    struct bit carry1 = bit_and(x,y); // Carry from x and y
    struct  bit carry2 = bit_and(bit_or(x,y),c_in); // Carry from (x ⊕ y) and c_in
    result.c = bit_or(carry1,carry2); // c_out = (x and y) or ((x or y) and c_in)
    return result;
}

struct bits8 bits8_add(struct bits8 x, struct bits8 y) {
  
    struct bits8 result;
    struct bit carry = bit_from_int(0);

    // Add each bit starting from the least significant bit
    struct add_result r0 = bit_add(x.b0, y.b0, carry);
    result.b0 = r0.s;
    carry = r0.c; 

    struct add_result r1 = bit_add(x.b1, y.b1, carry);
    result.b1 = r1.s;
    carry = r1.c;

    struct add_result r2 = bit_add(x.b2, y.b2, carry);
    result.b2 = r2.s;
    carry = r2.c;

    struct add_result r3 = bit_add(x.b3, y.b3, carry);
    result.b3 = r3.s;
    carry = r3.c;

    struct add_result r4 = bit_add(x.b4, y.b4, carry);
    result.b4 = r4.s;
    carry = r4.c;

    struct add_result r5 = bit_add(x.b5, y.b5, carry);
    result.b5 = r5.s;
    carry = r5.c;

    struct add_result r6 = bit_add(x.b6, y.b6, carry);
    result.b6 = r6.s;
    carry = r6.c;

    struct add_result r7 = bit_add(x.b7, y.b7, carry);
    result.b7 = r7.s;
    carry = r7.c;
    
    
    return result;
    
}

struct bits8 bits8_negate(struct bits8 x) {
    // Step 1: Invert all bits
    struct bits8 inverted;
    inverted.b0 = bit_not(x.b0);
    inverted.b1 = bit_not(x.b1);
    inverted.b2 = bit_not(x.b2);
    inverted.b3 = bit_not(x.b3);
    inverted.b4 = bit_not(x.b4);
    inverted.b5 = bit_not(x.b5);
    inverted.b6 = bit_not(x.b6);
    inverted.b7 = bit_not(x.b7);

    // Step 2: Add 1 to the inverted number
    struct bits8 one = bits8_from_int(1); // Represents 00000001
    struct bits8 negated = bits8_add(inverted, one);

    return negated;
}



struct bits8 bits8_mul(struct bits8 x, struct bits8 y) {
    struct bits8 result = bits8_from_int(0); // Initialize result to 0
    struct bits8 shifted = x;

    // Add x for bit 0 of y
    result = bits8_add(result, bits8_from_int(bit_to_int(y.b0) * bits8_to_int(shifted)));

    // Add x shifted left by 1 for bit 1 of y
    shifted.b7 = shifted.b6; shifted.b6 = shifted.b5; shifted.b5 = shifted.b4; 
    shifted.b4 = shifted.b3; shifted.b3 = shifted.b2; shifted.b2 = shifted.b1; 
    shifted.b1 = shifted.b0; shifted.b0 = bit_from_int(0); // Shift left by 1
    result = bits8_add(result, bits8_from_int(bit_to_int(y.b1) * bits8_to_int(shifted)));

    // Repeat this process for all bits of y
    shifted.b7 = shifted.b6; shifted.b6 = shifted.b5; shifted.b5 = shifted.b4; 
    shifted.b4 = shifted.b3; shifted.b3 = shifted.b2; shifted.b2 = shifted.b1; 
    shifted.b1 = shifted.b0; shifted.b0 = bit_from_int(0); // Shift left by 1
    result = bits8_add(result, bits8_from_int(bit_to_int(y.b2) * bits8_to_int(shifted)));

    // Continue for bits y.b3, y.b4, y.b5, y.b6, and y.b7 in the same way.

    return result;
}

