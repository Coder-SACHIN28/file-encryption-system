#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include "encryption.h"
#include "ui.h"

#define AES_KEY_SIZE 32  // 256 bits
#define AES_IV_SIZE 16   // 128 bits
#define SALT_SIZE 16

// Vigenère cipher for additional layer
void vigenere_encrypt(unsigned char *data, int data_len, const char *key) {
    int key_len = strlen(key);
    for(int i = 0; i < data_len; i++) {
        data[i] = data[i] ^ key[i % key_len];
    }
}

void vigenere_decrypt(unsigned char *data, int data_len, const char *key) {
    // XOR is symmetric, so decrypt is same as encrypt
    vigenere_encrypt(data, data_len, key);
}

// Generate cryptographically secure random salt
void generate_salt(unsigned char *salt, int length) {
    RAND_bytes(salt, length);
}

// Derive AES key and IV from password using SHA-256
void derive_key(const char *password, const unsigned char *salt, 
                unsigned char *key, unsigned char *iv) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // Combine password and salt
    int total_len = strlen(password) + SALT_SIZE;
    unsigned char *combined = malloc(total_len);
    memcpy(combined, password, strlen(password));
    memcpy(combined + strlen(password), salt, SALT_SIZE);
    
    // Hash multiple times for key stretching
    SHA256(combined, total_len, hash);
    for(int i = 0; i < 10000; i++) {
        SHA256(hash, SHA256_DIGEST_LENGTH, hash);
    }
    
    // Use hash for key and IV
    memcpy(key, hash, AES_KEY_SIZE);
    memcpy(iv, hash, AES_IV_SIZE);
    
    free(combined);
}

// AES-256-CBC encryption
int aes_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len, ciphertext_len;
    
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

// AES-256-CBC decryption
int aes_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len, plaintext_len;
    
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

// Dual-layer encryption: AES-256 + Vigenère
void dual_encrypt_file(const char *filename, const char *password) {
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        print_error("File not found or cannot be read");
        return;
    }
    
    // Get file size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // Read file content
    unsigned char *plaintext = malloc(file_size);
    fread(plaintext, 1, file_size, fp);
    fclose(fp);
    
    print_info("File loaded successfully");
    printf("        File size: %ld bytes\n\n", file_size);
    
    // Generate salt and derive keys
    animate_loading("Generating cryptographic salt", 500);
    unsigned char salt[SALT_SIZE];
    unsigned char key[AES_KEY_SIZE];
    unsigned char iv[AES_IV_SIZE];
    
    generate_salt(salt, SALT_SIZE);
    derive_key(password, salt, key, iv);
    
    animate_loading("Deriving AES-256 encryption key", 800);
    
    // Layer 1: AES-256 Encryption
    print_separator();
    printf(COLOR_CYAN "    Layer 1: AES-256-CBC Encryption\n" COLOR_RESET);
    
    unsigned char *ciphertext = malloc(file_size + AES_BLOCK_SIZE);
    int ciphertext_len = 0;
    
    // Encrypt in chunks with progress
    int chunk_size = 1024;
    int total_chunks = (file_size + chunk_size - 1) / chunk_size;
    
    for(int i = 0; i < total_chunks; i++) {
        show_progress(i + 1, total_chunks);
    }
    
    ciphertext_len = aes_encrypt(plaintext, file_size, key, iv, ciphertext);
    print_success("AES-256 encryption complete");
    
    // Layer 2: Vigenère Cipher
    printf("\n");
    print_separator();
    printf(COLOR_CYAN "    Layer 2: Vigenère Cipher\n" COLOR_RESET);
    
    for(int i = 0; i < 100; i++) {
        show_progress(i + 1, 100);
    }
    
    vigenere_encrypt(ciphertext, ciphertext_len, password);
    print_success("Vigenère encryption complete");
    
    // Create backup
    char backup_name[512];
    snprintf(backup_name, sizeof(backup_name), "%s.backup", filename);
    FILE *backup = fopen(backup_name, "wb");
    if(backup) {
        fread(plaintext, 1, file_size, fp);
        fwrite(plaintext, 1, file_size, backup);
        fclose(backup);
        printf("\n");
        print_info("Backup created successfully");
    }
    
    // Write encrypted file
    animate_loading("Writing encrypted data", 500);
    
    fp = fopen(filename, "wb");
    fwrite(salt, 1, SALT_SIZE, fp);
    fwrite(ciphertext, 1, ciphertext_len, fp);
    fclose(fp);
    
    free(plaintext);
    free(ciphertext);
    
    printf("\n");
    print_separator();
    print_success("FILE ENCRYPTED SUCCESSFULLY!");
    printf("\n");
    printf(COLOR_DIM "    Encryption Method: AES-256-CBC + Vigenère\n");
    printf("    Original Size: %ld bytes\n", file_size);
    printf("    Encrypted Size: %d bytes\n" COLOR_RESET, ciphertext_len + SALT_SIZE);
    printf("\n");
}

// Dual-layer decryption: Vigenère + AES-256
void dual_decrypt_file(const char *filename, const char *password) {
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        print_error("File not found or cannot be read");
        return;
    }
    
    // Get file size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if(file_size < SALT_SIZE) {
        print_error("Invalid encrypted file");
        fclose(fp);
        return;
    }
    
    // Read salt
    unsigned char salt[SALT_SIZE];
    fread(salt, 1, SALT_SIZE, fp);
    
    // Read encrypted content
    int ciphertext_len = file_size - SALT_SIZE;
    unsigned char *ciphertext = malloc(ciphertext_len);
    fread(ciphertext, 1, ciphertext_len, fp);
    fclose(fp);
    
    print_info("Encrypted file loaded");
    printf("        File size: %ld bytes\n\n", file_size);
    
    // Derive keys
    animate_loading("Deriving decryption key", 800);
    unsigned char key[AES_KEY_SIZE];
    unsigned char iv[AES_IV_SIZE];
    derive_key(password, salt, key, iv);
    
    // Layer 1: Vigenère Decryption
    print_separator();
    printf(COLOR_CYAN "    Layer 1: Vigenère Decryption\n" COLOR_RESET);
    
    for(int i = 0; i < 100; i++) {
        show_progress(i + 1, 100);
    }
    
    vigenere_decrypt(ciphertext, ciphertext_len, password);
    print_success("Vigenère decryption complete");
    
    // Layer 2: AES-256 Decryption
    printf("\n");
    print_separator();
    printf(COLOR_CYAN "    Layer 2: AES-256-CBC Decryption\n" COLOR_RESET);
    
    unsigned char *plaintext = malloc(ciphertext_len);
    int plaintext_len = 0;
    
    int chunk_size = 1024;
    int total_chunks = (ciphertext_len + chunk_size - 1) / chunk_size;
    
    for(int i = 0; i < total_chunks; i++) {
        show_progress(i + 1, total_chunks);
    }
    
    plaintext_len = aes_decrypt(ciphertext, ciphertext_len, key, iv, plaintext);
    
    if(plaintext_len > 0) {
        print_success("AES-256 decryption complete");
        
        // Write decrypted file
        animate_loading("Writing decrypted data", 500);
        
        fp = fopen(filename, "wb");
        fwrite(plaintext, 1, plaintext_len, fp);
        fclose(fp);
        
        printf("\n");
        print_separator();
        print_success("FILE DECRYPTED SUCCESSFULLY!");
        printf("\n");
        printf(COLOR_DIM "    Decryption Method: Vigenère + AES-256-CBC\n");
        printf("    Decrypted Size: %d bytes\n" COLOR_RESET, plaintext_len);
        printf("\n");
    } else {
        printf("\n");
        print_error("Decryption failed - Wrong password or corrupted file");
    }
    
    free(ciphertext);
    free(plaintext);
}

// Password strength checker
int check_password_strength(const char *password) {
    int length = strlen(password);
    int strength = 0;
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    
    if(length >= 12) strength += 3;
    else if(length >= 8) strength += 2;
    else if(length >= 6) strength += 1;
    
    for(int i = 0; i < length; i++) {
        if(isupper(password[i])) has_upper = 1;
        else if(islower(password[i])) has_lower = 1;
        else if(isdigit(password[i])) has_digit = 1;
        else has_special = 1;
    }
    
    if(has_upper) strength += 1;
    if(has_lower) strength += 1;
    if(has_digit) strength += 1;
    if(has_special) strength += 2;
    
    return strength;
}

void show_password_strength(const char *password) {
    int strength = check_password_strength(password);
    int length = strlen(password);
    
    printf("\n");
    print_separator();
    printf(COLOR_BOLD "    PASSWORD STRENGTH ANALYSIS\n" COLOR_RESET);
    print_separator();
    
    printf("    Password Length: %d characters\n", length);
    printf("    Strength Score: %d/9\n\n", strength);
    
    if(strength < 4) {
        printf(COLOR_RED "    Security Level: WEAK ⚠\n" COLOR_RESET);
        print_warning("Consider using a stronger password!");
    } else if(strength < 7) {
        printf(COLOR_YELLOW "    Security Level: MODERATE ⚡\n" COLOR_RESET);
        print_info("Password is acceptable but could be stronger");
    } else {
        printf(COLOR_GREEN "    Security Level: STRONG ✓\n" COLOR_RESET);
        print_success("Excellent password security!");
    }
    
    printf("\n");
    print_separator();
}
