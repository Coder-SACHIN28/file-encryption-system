#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdint.h>

// AES-256 + Vigenère Dual-Layer Encryption
void dual_encrypt_file(const char *filename, const char *password);
void dual_decrypt_file(const char *filename, const char *password);

// Password strength checker
int check_password_strength(const char *password);
void show_password_strength(const char *password);

// Utility functions
void generate_salt(unsigned char *salt, int length);
void derive_key(const char *password, const unsigned char *salt, 
                unsigned char *key, unsigned char *iv);

#endif
