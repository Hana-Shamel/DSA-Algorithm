#include <stdio.h>
#include <limits.h>

void print_bin_ull(unsigned long long value, int bits) {
    for (int i = bits-1; i >= 0; i--) {
        putchar('0' + ((value >> i) & 1));
    }
}

int perm_choice1_table[] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

int perm_choice2_table[] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

int initial_perm[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

int inverse_initial_perm[] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

int expansion_perm[] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

int perm[] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25
};

// first argument: input value, second argument: number of shifts, third argument: number of bits in input
unsigned long long left_shift(unsigned long long input, int shifts, int bits) {
    int mask = (1 << bits) - 1;
    for (int i = 0; i < shifts; i++) {
        unsigned long long msb = (input >> (bits - 1)) & 0x01; // Extract the msb
        input = input << 1;
        input = input | msb;
    }
    input = input & mask;
    return input;
}

unsigned long long permutation(unsigned long long input, int input_bits , int perm_table[], int output_bits) {     // Input 64 bits, Output 56 bits
    unsigned long long output = 0x0;
    for (int i = 0; i < output_bits; i++) {
        int position = input_bits - perm_table[i];
        unsigned long long bit = (input >> position) & 0x01;    // Extract the bit (0 & 1 = 0, 1 & 1 = 1)
        output = output << 1;   // Shift left to make space for the next bit
        output = output | bit;  // Set the bit (0 | 1 = 1, 0 | 0 = 0)
    }
    return output;
}

unsigned long long concatenate(unsigned long long left, unsigned long long right, int right_bits) {
    unsigned long long output = left << right_bits; 
    output = output | right;
    return output;
}

void generate_subkeys(unsigned long long key_after_perm, unsigned long long subkeys[16]) {
    unsigned long long left = (key_after_perm >> 28) & 0x0FFFFFFF;
    unsigned long long right = key_after_perm & 0x0FFFFFFF;

    for(int i = 0; i < 16; i++) {
        int shift = 1; 
        if (i == 0 || i == 1 || i == 8 || i == 15) {
            shift = 1;
        } else {
            shift = 2;
        }
        left = left_shift(left, shift, 28);
        right = left_shift(right, shift, 28);

        unsigned long long combined_key = concatenate(left, right, 28);

        subkeys[i] = permutation(combined_key, 56, perm_choice2_table, 48);
    }
}

int main() {
    unsigned long long key = 0x133457799BBCDFF1;
    unsigned long long key_after_perm = permutation(key, 64, perm_choice1_table, 56);
    unsigned long long subkeys[16];
    generate_subkeys(key_after_perm , subkeys);
    print_bin_ull(key_after_perm, 56);
    for(int i = 0; i < 16; i++) {
        
        printf("Subkey %d: ", i+1);
        print_bin_ull(subkeys[i], 48);
        printf("\n");
    }
}