int main(int argc, char *argv[]) {
    
    // 1. Command-line Argument Validation
    if (argc != 5) {
        fprintf(stderr, "Usage (Encrypt): %s \"e\" key_filename plaintext_filename ciphertext_filename\n", argv[0]);
        fprintf(stderr, "Usage (Decrypt): %s \"d\" key_filename ciphertext_filename plaintext_filename\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    char *key_filename = argv[2];
    char *input_filename = argv[3];
    char *output_filename = argv[4];

    if (strcmp(mode, "e") != 0 && strcmp(mode, "d") != 0) {
        fprintf(stderr, "Invalid mode. Use 'e' for encrypt or 'd' for decrypt.\n");
        return 1;
    }

    // 2. Read Key File
    unsigned char key[DES_KEY_SIZE];
    if (read_key_file(key_filename, key) != 0) {
        return 1;
    }

    // 3. Open Input and Output Files
    // Use "rb" and "wb" for raw binary data as DES operates on raw bytes.
    FILE *input_file = fopen(input_filename, "rb");
    if (input_file == NULL) {
        perror("Error opening input file");
        return 1;
    }

    FILE *output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(input_file);
        return 1;
    }

    // 4. Process File in Chunks (Read -> Transform -> Write)
    //PUT HERE DES LOGIC: ENC AND DEC

    // 5. Close Files
    fclose(input_file);
    fclose(output_file);

    return 0;
}

/**
 * Reads the 8-byte DES key from the specified file.
 * Returns 0 on success, 1 on error.
 */
int read_key_file(const char *key_filename, unsigned char *key_buffer) {
    FILE *key_file = fopen(key_filename, "rb");
    if (key_file == NULL) {
        perror("Error opening key file");
        return 1;
    }

    // Read exactly DES_KEY_SIZE bytes (8 bytes)
    size_t bytes_read = fread(key_buffer, 1, DES_KEY_SIZE, key_file);
    
    // Check if the file contains the correct number of bytes for a DES key
    if (bytes_read != DES_KEY_SIZE) {
        fprintf(stderr, "Error: Key file '%s' must contain exactly %d bytes.\n", key_filename, DES_KEY_SIZE);
        fclose(key_file);
        return 1;
    }

    fclose(key_file);
    return 0;
}
void concatenate(const unsigned char *left_half_ptr, 
                 const unsigned char *right_half_ptr, 
                 unsigned char *result_ptr) {
    
    int i;

    // 1. Copy the Left Half (First 4 bytes)
    for (i = 0; i < HALF_BLOCK_SIZE; i++) {
        // Copy byte from left_half_ptr[i] to result_ptr[i]
        result_ptr[i] = left_half_ptr[i];
    }

    // 2. Copy the Right Half (Next 4 bytes)
    // The loop counter 'i' continues from HALF_BLOCK_SIZE (which is 4).
    for (i = 0; i < HALF_BLOCK_SIZE; i++) {
        // Copy byte from right_half_ptr[i] to result_ptr[HALF_BLOCK_SIZE + i]
        // Example: 
        // i=0: result_ptr[4] = right_half_ptr[0]
        // i=1: result_ptr[5] = right_half_ptr[1]
        // i=3: result_ptr[7] = right_half_ptr[3]
        result_ptr[HALF_BLOCK_SIZE + i] = right_half_ptr[i];
    }
}
