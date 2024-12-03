#include <stdlib.h>
#include "numbers.h"

void test_bits8_conversion() {
    // Test bits8_from_int and bits8_to_int
    for (unsigned int i = 0; i < 256; i++) { // Test all 8-bit numbers
        struct bits8 b = bits8_from_int(i);
        unsigned int result = bits8_to_int(b);
        if (result != i) {
            printf("Error: bits8_from_int(%u) -> bits8_to_int() = %u\n", i, result);
            exit(1);
        }
    }
    printf("bits8_from_int() and bits8_to_int() passed.\n");
}

void test_bits8_print() {
    struct bits8 b = bits8_from_int(123); // using hint from assingment: 123 = 01111011 in binary
    printf("Expected: 01111011, Got: ");
    bits8_print(b);
    printf("\n");
}


void test_bits8_add() {
    struct bits8 x = bits8_from_int(123); // 01111011
    struct bits8 y = bits8_from_int(45);  // 00101101
    struct bits8 sum = bits8_add(x, y);

    unsigned int result = bits8_to_int(sum);
    unsigned int expected = (123 + 45) & 0xFF; // Mask to 8 bits

    if (result != expected) {
        printf("Error: bits8_add failed for 123 + 45\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    // Edge cases
    struct bits8 a = bits8_from_int(255); // Max value (11111111)
    struct bits8 b = bits8_from_int(1);   // Smallest overflow case
    struct bits8 overflow = bits8_add(a, b);

    result = bits8_to_int(overflow);
    expected = (255 + 1) & 0xFF; // Mask to 8 bits
    if (result != expected) {
        printf("Error: bits8_add failed for 255 + 1\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    printf("bits8_add passed all tests.\n");
}

void test_bits8_negate() {
    struct bits8 x = bits8_from_int(5);  // 5 in binary: 00000101
    struct bits8 neg_x = bits8_negate(x);

    unsigned int result = bits8_to_int(neg_x);
    unsigned int expected = (-5 + 256) & 0xFF; // Convert -5 to unsigned 8-bit representation

    if (result != expected) {
        printf("Error: bits8_negate failed for 5\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    // Edge case: Negating 0
    struct bits8 zero = bits8_from_int(0); // 0 in binary: 00000000
    struct bits8 neg_zero = bits8_negate(zero);

    result = bits8_to_int(neg_zero);
    expected = 0;

    if (result != expected) {
        printf("Error: bits8_negate failed for 0\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    // Edge case: Negating -1 (Two's complement edge case)
    struct bits8 minus_one = bits8_from_int(255); // -1 in 8-bit two's complement: 11111111
    struct bits8 neg_minus_one = bits8_negate(minus_one);

    result = bits8_to_int(neg_minus_one);
    expected = 1; // -(-1) = 1

    if (result != expected) {
        printf("Error: bits8_negate failed for -1\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    printf("bits8_negate passed all tests.\n");
}


void test_bits8_mul() {
    // Test Case 1: Normal case
    struct bits8 x = bits8_from_int(3);  // 3 in binary: 00000011
    struct bits8 y = bits8_from_int(5);  // 5 in binary: 00000101
    struct bits8 product = bits8_mul(x, y);

    unsigned int result = bits8_to_int(product);
    unsigned int expected = (3 * 5) & 0xFF; // Mask to 8 bits
    if (result != expected) {
        printf("Error: bits8_mul failed for 3 * 5\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    // Test Case 2: Multiplying by 0
    struct bits8 zero = bits8_from_int(0);
    struct bits8 prod_zero = bits8_mul(x, zero);

    result = bits8_to_int(prod_zero);
    expected = 0;
    if (result != expected) {
        printf("Error: bits8_mul failed for 3 * 0\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    // Test Case 3: Multiplying maximum values
    struct bits8 max = bits8_from_int(255); // 255 in binary: 11111111
    struct bits8 prod_max = bits8_mul(max, y);

    result = bits8_to_int(prod_max);
    expected = (255 * 5) & 0xFF; // Mask to 8 bits
    if (result != expected) {
        printf("Error: bits8_mul failed for 255 * 5\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    // Test Case 4: Multiplying by 1
    struct bits8 one = bits8_from_int(1);  // Binary: 00000001
    struct bits8 prod_one = bits8_mul(x, one);

    result = bits8_to_int(prod_one);
    expected = 3; // x * 1 should equal x
    if (result != expected) {
        printf("Error: bits8_mul failed for 3 * 1\n");
        printf("Got: %u, Expected: %u\n", result, expected);
        exit(1);
    }

    printf("bits8_mul passed all tests.\n");
}

int main() {
    test_bits8_conversion();
    test_bits8_print();
    test_bits8_add();
    test_bits8_negate();
    test_bits8_mul();
    return 0;
}
