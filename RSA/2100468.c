#include <stdio.h>
#include <stdint.h>

// extended euclidean algorithm for modular inverse
uint32_t mod_inverse(uint32_t a, uint32_t m) {
    int32_t m0 = m;
    int32_t x0 = 0, x1 = 1;
    
    if (m == 1) return 0;
    
    while (a > 1) {
        int32_t q = a / m;
        int32_t t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    
    if (x1 < 0) x1 += m0;
    return (uint32_t)x1;
}

// compute N^-1 mod R where R = 2^32
uint32_t mod_inverse_R(uint32_t n) {
    uint64_t R = 0x100000000ULL;
    int64_t r0 = R, r1 = n;
    int64_t t0 = 0, t1 = 1;
    
    while (r1 != 0) {
        int64_t q = r0 / r1;
        int64_t temp = r0 - q * r1;
        r0 = r1;
        r1 = temp;
        temp = t0 - q * t1;
        t0 = t1;
        t1 = temp;
    }
    
    if (t0 < 0) t0 += R;
    return (uint32_t)t0;
}

// montgomery reduction
uint32_t montgomery_redc(uint64_t x, uint32_t N, uint32_t invN) {
    uint64_t t1 = x;
    uint64_t t2 = (uint64_t)((uint32_t)x * invN) * N;
    uint32_t result = (uint32_t)((t1 - t2) >> 32);
    result += N & -(t1 < t2);
    return result;
}

// montgomery multiplication
uint32_t montgomery_mul(uint32_t a, uint32_t b, uint32_t N, uint32_t invN) {
    uint64_t product = (uint64_t)a * (uint64_t)b;
    return montgomery_redc(product, N, invN);
}

// convert to montgomery space
uint32_t to_montgomery(uint32_t a, uint32_t N, uint32_t invN, uint32_t R2) {
    return montgomery_mul(a, R2, N, invN);
}

// convert from montgomery space
uint32_t from_montgomery(uint32_t a, uint32_t N, uint32_t invN) {
    return montgomery_redc(a, N, invN);
}

// modular exponentiation with montgomery multiplication
uint32_t mod_exp_montgomery(uint32_t base, uint32_t exp, uint32_t N) {
    if (N == 1) return 0;
    if (exp == 0) return 1;
    
    uint32_t invN = mod_inverse_R(N);
    
    // compute R^2 mod N
    uint64_t R = 0x100000000ULL;
    uint64_t R2 = R % N;
    R2 = (R2 * R) % N;
    uint32_t R2_mod_N = (uint32_t)R2;
    
    uint32_t mont_base = to_montgomery(base, N, invN, R2_mod_N);
    uint32_t mont_result = to_montgomery(1, N, invN, R2_mod_N);
    
    while (exp > 0) {
        if (exp & 1) {
            mont_result = montgomery_mul(mont_result, mont_base, N, invN);
        }
        mont_base = montgomery_mul(mont_base, mont_base, N, invN);
        exp >>= 1;
    }
    
    return from_montgomery(mont_result, N, invN);
}

// read hex file
int read_hex_file(const char *filename, uint32_t *data, int max_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }
    
    int count = 0;
    while (count < max_count && fscanf(file, "%x", &data[count]) == 1) {
        count++;
    }
    
    fclose(file);
    return count;
}

// write hex file
void write_hex_file(const char *filename, uint32_t *data, int count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Cannot open file %s for writing\n", filename);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%08X", data[i]);
        if (i < count - 1) fprintf(file, " ");
    }
    
    fclose(file);
}

int main(int argc, char *argv[]) {
    
    char mode = argv[1][0];
    
    // encryption mode
    if (mode == 'e' || mode == 'E') {

        uint32_t public_key[2];
        uint32_t plaintext;
        
        if (read_hex_file(argv[2], public_key, 2) != 2) {
            printf("Error reading public key\n");
            return 1;
        }
        
        uint32_t e = public_key[0];
        uint32_t N = public_key[1];
        
        if (read_hex_file(argv[3], &plaintext, 1) != 1) {
            printf("Error reading plaintext\n");
            return 1;
        }

        printf("Plaintext P: %08X\n", plaintext);
        
        uint32_t ciphertext = mod_exp_montgomery(plaintext, e, N);
        
        write_hex_file(argv[4], &ciphertext, 1);
        printf("Ciphertext C: %08X\n", ciphertext);
        printf("Ciphertext written to %s\n", argv[4]);
    }
    
    // decryption mode
    else if (mode == 'd' || mode == 'D') {
        
        uint32_t private_key[2];
        uint32_t ciphertext;
        
        if (read_hex_file(argv[2], private_key, 2) != 2) {
            printf("Error reading private key\n");
            return 1;
        }
        
        uint32_t d = private_key[0];
        uint32_t N = private_key[1];
        
        if (read_hex_file(argv[3], &ciphertext, 1) != 1) {
            printf("Error reading ciphertext\n");
            return 1;
        }
        
        printf("Ciphertext C: %08X\n", ciphertext);
        
        uint32_t plaintext = mod_exp_montgomery(ciphertext, d, N);
        
        write_hex_file(argv[4], &plaintext, 1);
        printf("Plaintext P: %08X\n", plaintext);
        printf("Plaintext written to %s\n", argv[4]);
    }
    
    return 0;

}
